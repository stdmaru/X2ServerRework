#include "EPublisherServer.h"

#include "Http.h"
#include "Soap.h"

#include "NetError.h"
#include "x2/platform/net_io.h"
#include "x2/platform/unicode.h"

#include <KncLua.h>
#include <dbg/dbg.hpp>
#include <lua_tinker.h>

#include <chrono>
#include <cstdlib>
#include <thread>

namespace x2::epublisher {

namespace {

constexpr unsigned kRequestTimeoutMs = 10 * 1000;  // as the original: 10 s per socket operation
constexpr int kMaxConnections = 100;

std::wstring Wide(const std::string& s) { return x2::platform::Utf8ToWide(s); }

unsigned long ToULong(const std::map<std::string, std::string>& fields, const char* name)
{
    const auto it = fields.find(name);
    return it == fields.end() ? 0 : std::strtoul(it->second.c_str(), nullptr, 10);
}

const std::string& Field(const std::map<std::string, std::string>& fields, const char* name)
{
    static const std::string empty;
    const auto it = fields.find(name);
    return it == fields.end() ? empty : it->second;
}

} // namespace

EPublisherServer::~EPublisherServer()
{
    Stop();
    if (m_listen != INVALID_SOCKET)
        ::closesocket(m_listen);
    // request threads are detached and only touch the store (it outlives the server in main)
    while (m_connections > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void EPublisherServer::SetODBCDriver_LUA(const char* driver)
{
    KODBC::SetODBCDriver(Wide(driver ? driver : ""));
}

void EPublisherServer::SetODBCOptions_LUA(const char* options)
{
    KODBC::SetODBCOptions(Wide(options ? options : ""));
}

bool EPublisherServer::Listen()
{
    m_listen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listen == INVALID_SOCKET)
        return false;
    int reuse = 1;
    ::setsockopt(m_listen, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    SOCKADDR_IN addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = inet_addr(m_bindAddress.c_str());
    if (::bind(m_listen, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR
        || ::listen(m_listen, SOMAXCONN) == SOCKET_ERROR) {
        START_LOG(cerr, L"Cannot listen on the EPublisher port.")
            << BUILD_LOG(m_port)
            << BUILD_LOG(x2::platform::net::SocketErrorMessage(x2::platform::net::LastSocketError()).c_str())
            << END_LOG;
        ::closesocket(m_listen);
        m_listen = INVALID_SOCKET;
        return false;
    }
    x2_socklen_t len = sizeof(addr);
    ::getsockname(m_listen, reinterpret_cast<SOCKADDR*>(&addr), &len);
    m_port = ntohs(addr.sin_port);
    START_LOG(cout, L"EPublisher service listening.") << BUILD_LOG(m_port);
    return true;
}

void EPublisherServer::Run()
{
    while (!m_stop) {
        if (!x2::platform::net::WaitReadable(m_listen, 200))
            continue;
        SOCKADDR_IN peer{};
        x2_socklen_t len = sizeof(peer);
        const SOCKET sock = ::accept(m_listen, reinterpret_cast<SOCKADDR*>(&peer), &len);
        if (sock == INVALID_SOCKET)
            continue;
        if (m_connections >= kMaxConnections) {
            ::closesocket(sock);
            continue;
        }
        ++m_connections;
        std::thread(&EPublisherServer::Serve, this, sock, std::string(inet_ntoa(peer.sin_addr))).detach();
    }
}

void EPublisherServer::Serve(SOCKET sock, std::string peer)
{
    std::string pending;
    for (;;) {
        HttpRequest request;
        const auto read = ReadHttpRequest(sock, kRequestTimeoutMs, pending, request);
        if (read != HttpReadResult::Ok) {
            if (read == HttpReadResult::Invalid || read == HttpReadResult::TooLarge)
                WriteHttpResponse(sock, 400, "Bad Request", "text/plain", "bad request\n", false);
            break;
        }
        if (request.method != "POST") {
            WriteHttpResponse(sock, 405, "Method Not Allowed", "text/plain", "POST a SOAP request\n", false);
            break;
        }
        int status = 200;
        const std::string answer = HandleSoap(request.body, status);
        if (!WriteHttpResponse(sock, status, status == 200 ? "OK" : "Internal Server Error",
                               "text/xml; charset=utf-8", answer, request.keepAlive)
            || !request.keepAlive)
            break;
    }
    ::shutdown(sock, SD_BOTH);
    ::closesocket(sock);
    const std::wstring wstrPeer = Wide(peer);
    START_LOG(clog, L"EPublisher connection closed.") << BUILD_LOG(wstrPeer);
    --m_connections;
}

std::string EPublisherServer::HandleSoap(const std::string& body, int& httpStatus)
{
    httpStatus = 200;
    const auto request = ParseSoapRequest(body);
    if (!request) {
        httpStatus = 500;
        return BuildSoapFault("SOAP-ENV:Client", "Malformed SOAP request");
    }
    const auto& f = request->fields;

    if (request->operation == "LoginRequest") {
        const auto result = m_service.Login(Field(f, "strUsername"), Field(f, "strPassword"));
        const std::wstring wstrUsername = Wide(Field(f, "strUsername"));
        const std::wstring wstrIP = Wide(Field(f, "strIPAddress"));
        const int iResult = result.result;
        START_LOG(cout, L"[EPublisher] Login")
            << BUILD_LOG(wstrUsername)
            << BUILD_LOG(wstrIP)
            << BUILD_LOG(iResult)
            << BUILD_LOG(NetError::GetErrStr(iResult));
        SoapFields out{ { "iResult", std::to_string(result.result) } };
        if (result.result == NetError::NET_OK) {
            out.push_back({ "strID", std::to_string(result.account.uid) });
            out.push_back({ "iPublisherSN", std::to_string(result.account.uid) });
            out.push_back({ "strPurchaseToken", result.account.purchaseToken });
            out.push_back({ "iChannelingCode", "0" });
        } else {
            out.push_back({ "strID", "" });
            out.push_back({ "iPublisherSN", "0" });
            out.push_back({ "strPurchaseToken", "" });
            out.push_back({ "iChannelingCode", "0" });
        }
        return BuildSoapResponse("LoginResponse", out);
    }
    if (request->operation == "GetUserCashRequest") {
        const auto uid = static_cast<unsigned int>(ToULong(f, "uiPublisherUID"));
        const auto result = m_service.GetUserCash(uid);
        const long long iBalance = result.balance;
        const int iOK = result.result;
        START_LOG(cout, L"[EPublisher] GetUserCash")
            << BUILD_LOG(uid)
            << BUILD_LOG(iBalance)
            << BUILD_LOG(iOK)
            << BUILD_LOG(NetError::GetErrStr(iOK));
        return BuildSoapResponse("GetUserCashResponse",
            { { "iOK", std::to_string(result.result) }, { "ulBalance", std::to_string(result.balance) } });
    }
    if (request->operation == "PurchaseProductRequest") {
        const auto uid = static_cast<unsigned int>(ToULong(f, "uiPublisherUID"));
        const auto result = m_service.PurchaseProduct(uid, Field(f, "strTotalList"));
        const std::wstring wstrNickName = Wide(Field(f, "strNickName"));
        const std::wstring wstrOrderID = Wide(Field(f, "strTotalOrderID"));
        const std::wstring wstrTotalList = Wide(Field(f, "strTotalList"));
        const long long iBalance = result.balance;
        const int iOK = result.result;
        START_LOG(cout, L"[EPublisher] PurchaseProduct")
            << BUILD_LOG(uid)
            << BUILD_LOG(wstrNickName)
            << BUILD_LOG(wstrOrderID)
            << BUILD_LOG(wstrTotalList)
            << BUILD_LOG(iBalance)
            << BUILD_LOG(iOK)
            << BUILD_LOG(NetError::GetErrStr(iOK));
        return BuildSoapResponse("PurchaseProductResponse",
            { { "iOK", std::to_string(result.result) }, { "iTotalCash", std::to_string(result.balance) } });
    }

    httpStatus = 500;
    return BuildSoapFault("SOAP-ENV:Client", "Unknown operation " + request->operation);
}

void RegisterLua(lua_State* L, EPublisherServer& server)
{
    lua_tinker::def(L, "log", &dbg::logfile::SetLogLevel);
    lua_tinker::class_add<EPublisherServer>(L, "EPublisherServer");
    lua_tinker::class_def<EPublisherServer>(L, "SetPort", &EPublisherServer::SetPort_LUA);
    lua_tinker::class_def<EPublisherServer>(L, "SetBindAddress", &EPublisherServer::SetBindAddress_LUA);
    lua_tinker::class_def<EPublisherServer>(L, "SetMaintenance", &EPublisherServer::SetMaintenance_LUA);
    lua_tinker::class_def<EPublisherServer>(L, "SetDSN", &EPublisherServer::SetDSN_LUA);
    lua_tinker::class_def<EPublisherServer>(L, "SetODBCDriver", &EPublisherServer::SetODBCDriver_LUA);
    lua_tinker::class_def<EPublisherServer>(L, "SetODBCOptions", &EPublisherServer::SetODBCOptions_LUA);
    lua_tinker::set(L, "EPublisher", &server);
}

} // namespace x2::epublisher
