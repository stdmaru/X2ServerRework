#include "EPublisherServer.h"

#include "NetError.h"
#include "OnlyGlobal/AuthAndBilling/DEB/Soap.h"
#include "OnlyGlobal/AuthAndBilling/DEB/SoapClient.h"  // the servers' client (KDEBAuth, KDEBBilling)

#include <doctest/doctest.h>

#include <map>
#include <string>
#include <thread>

using namespace x2::epublisher;
using x2::deb::ParseSoapMessage;
using x2::deb::SoapClient;

namespace {

class FakeStore final : public IAccountStore
{
public:
    std::map<std::string, Account> accounts;
    std::map<unsigned int, std::int64_t> cash;
    bool fail = false;

    std::optional<Account> FindAccount(const std::string& id) override
    {
        if (fail) throw std::runtime_error("db down");
        const auto it = accounts.find(id);
        return it == accounts.end() ? std::nullopt : std::optional<Account>(it->second);
    }
    std::optional<std::int64_t> FindCash(unsigned int uid) override
    {
        if (fail) throw std::runtime_error("db down");
        const auto it = cash.find(uid);
        return it == cash.end() ? std::nullopt : std::optional<std::int64_t>(it->second);
    }
    std::optional<std::int64_t> SpendCash(unsigned int uid, std::int64_t amount) override
    {
        auto& balance = cash.at(uid);
        if (balance < amount) return std::nullopt;
        return balance -= amount;
    }
};

FakeStore MakeStore()
{
    FakeStore store;
    // the DB keeps sha256( password + salt ): "test" is the account of the servers' DEB self-test
    store.accounts["test"] = { 20004, "test", "3df9919207da009d4df29850bc743fbcf7aeefd83507e4c4074f1a9b25295214", "tok<&>" };
    store.cash[20004] = 7700;
    return store;
}

} // namespace

TEST_CASE("password hash is sha256( password + salt ) in lower-case hex")
{
    CHECK(PublisherService::HashPassword("test") == "3df9919207da009d4df29850bc743fbcf7aeefd83507e4c4074f1a9b25295214");
}

TEST_CASE("SOAP request: operation and fields, entities decoded")
{
    const auto r = ParseSoapMessage(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\""
        " xmlns:ns2=\"urn:AuthService\"><SOAP-ENV:Body><ns2:LoginRequest><ns2:strUsername>a&lt;b&amp;c&#233;</ns2:strUsername>"
        "<ns2:strPassword></ns2:strPassword><ns2:strIPAddress/></ns2:LoginRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>");
    REQUIRE(r);
    CHECK(r->element == "LoginRequest");
    CHECK(r->Text("strUsername") == "a<b&c\xC3\xA9");
    CHECK(r->fields.at("strPassword").empty());
    CHECK(r->fields.at("strIPAddress").empty());
    CHECK(r->Text("missing").empty());
    CHECK(r->Int("missing") == 0);
    CHECK_FALSE(ParseSoapMessage("not xml"));
}

// What the gSOAP 2.7 client of the original servers wrote (captured on 2026-10-31, before it was removed, decision T31).
constexpr const char* kGsoapLoginRequest =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\""
    " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""
    " xmlns:ns1=\"http://192.168.1.100:8000/AuthService.wsdl\" xmlns:ns2=\"urn:AuthService\"><SOAP-ENV:Body><ns2:LoginRequest>"
    "<ns2:strUsername>us&lt;er&amp;</ns2:strUsername><ns2:strPassword>pw</ns2:strPassword><ns2:strIPAddress>1.2.3.4</ns2:strIPAddress>"
    "</ns2:LoginRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>";

TEST_CASE("SOAP request: written as the gSOAP client wrote it")
{
    CHECK(x2::deb::BuildSoapRequest("LoginRequest",
              { { "strUsername", "us<er&" }, { "strPassword", "pw" }, { "strIPAddress", "1.2.3.4" } }) == kGsoapLoginRequest);

    FakeStore store = MakeStore();
    EPublisherServer server(store);
    int status = 0;
    const auto answer = ParseSoapMessage(server.HandleSoap(kGsoapLoginRequest, status));
    CHECK(status == 200);
    REQUIRE(answer);
    CHECK(answer->element == "LoginResponse");
    CHECK(answer->Int("iResult") == NetError::ERR_VERIFY_20);
}

TEST_CASE("operations: results are the servers' NetError codes")
{
    FakeStore store = MakeStore();
    PublisherService service(store);

    auto login = service.Login("test", "test");
    CHECK(login.result == NetError::NET_OK);
    CHECK(login.account.uid == 20004);
    CHECK(service.Login("test", "wrong").result == NetError::ERR_VERIFY_20);
    CHECK(service.Login("nobody", "test").result == NetError::ERR_VERIFY_20);
    service.SetMaintenance(true);
    CHECK(service.Login("test", "test").result == NetError::ERR_VERIFY_12);
    service.SetMaintenance(false);

    CHECK(service.GetUserCash(20004).balance == 7700);
    CHECK(service.GetUserCash(1).result == NetError::ERR_GASH_02);

    auto buy = service.PurchaseProduct(20004, "1_100_700|2_200_1000");
    CHECK(buy.result == NetError::NET_OK);
    CHECK(buy.balance == 6000);
    CHECK(service.PurchaseProduct(20004, "1_100_99999").result == NetError::ERR_PAYLETTER_BILLING_08);
    CHECK(service.PurchaseProduct(1, "1_100_1").result == NetError::ERR_PAYLETTER_BILLING_09);
    CHECK(service.PurchaseProduct(20004, "garbage").result == NetError::ERR_PAYLETTER_BILLING_30);
    CHECK(service.PurchaseProduct(20004, "1_100_-5").result == NetError::ERR_PAYLETTER_BILLING_30);
    CHECK(store.cash[20004] == 6000);

    store.fail = true;
    CHECK(service.Login("test", "test").result == NetError::ERR_VERIFY_21);
    CHECK(service.GetUserCash(20004).result == NetError::ERR_GASH_00);
}

TEST_CASE("the servers' SOAP client talks to the service")
{
    FakeStore store = MakeStore();
    store.accounts["\xC3\xA9\xEA\xB0\x80"] = store.accounts["test"];  // non-ASCII ID (gSOAP encoded it twice)
    EPublisherServer server(store);
    server.SetBindAddress_LUA("127.0.0.1");
    server.SetPort_LUA(0);
    REQUIRE(server.Listen());
    std::thread accepter([&server] { server.Run(); });
    const SoapClient client("http://127.0.0.1:" + std::to_string(server.GetPort()) + "/");
    std::string error;

    SUBCASE("Login, as KDEBAuth calls it")
    {
        auto response = client.Call("LoginRequest",
            { { "strUsername", "test" }, { "strPassword", "test" }, { "strIPAddress", "192.168.71.45" } }, error);
        REQUIRE_MESSAGE(response, error);
        CHECK(response->element == "LoginResponse");
        CHECK(response->Int("iResult") == NetError::NET_OK);
        CHECK(response->Text("strID") == "20004");
        CHECK(response->Int("iPublisherSN") == 20004);
        CHECK(response->Text("strPurchaseToken") == "tok<&>");
        CHECK(response->Int("iChannelingCode") == 0);

        response = client.Call("LoginRequest",
            { { "strUsername", "test" }, { "strPassword", "nope" }, { "strIPAddress", "192.168.71.45" } }, error);
        REQUIRE_MESSAGE(response, error);
        CHECK(response->Int("iResult") == NetError::ERR_VERIFY_20);

        response = client.Call("LoginRequest",
            { { "strUsername", "\xC3\xA9\xEA\xB0\x80" }, { "strPassword", "test" }, { "strIPAddress", "" } }, error);
        REQUIRE_MESSAGE(response, error);
        CHECK(response->Int("iResult") == NetError::NET_OK);
    }
    SUBCASE("GetUserCash and PurchaseProduct, as KDEBBilling calls them")
    {
        auto cash = client.Call("GetUserCashRequest", { { "uiPublisherUID", "20004" } }, error);
        REQUIRE_MESSAGE(cash, error);
        CHECK(cash->Int("iOK") == NetError::NET_OK);
        CHECK(cash->UInt("ulBalance") == 7700);

        auto bought = client.Call("PurchaseProductRequest",
            { { "uiPublisherUID", "20004" }, { "iChannelingCode", "0" }, { "iUnitUID", "1" }, { "strUserIP", "127.0.0.1" },
              { "strNickName", "nick" }, { "strTotalOrderID", "order" }, { "strTotalList", "1_100_700" } }, error);
        REQUIRE_MESSAGE(bought, error);
        CHECK(bought->Int("iOK") == NetError::NET_OK);
        CHECK(bought->Int("iTotalCash") == 7000);
    }
    SUBCASE("a SOAP Fault is an error")
    {
        CHECK_FALSE(client.Call("NoSuchRequest", {}, error));
        CHECK(error.find("Unknown operation NoSuchRequest") != std::string::npos);
    }

    server.Stop();
    accepter.join();
}

TEST_CASE("the servers' SOAP client: endpoint errors")
{
    std::string error;
    CHECK_FALSE(SoapClient("https://127.0.0.1/").Call("LoginRequest", {}, error));
    CHECK(error.find("invalid endpoint") == 0);
    CHECK_FALSE(SoapClient("http://127.0.0.1:99999/").Call("LoginRequest", {}, error));
    CHECK(error.find("invalid endpoint") == 0);

    // a port nobody listens on: the one of a socket just closed
    FakeStore store = MakeStore();
    unsigned short port = 0;
    {
        EPublisherServer server(store);
        server.SetBindAddress_LUA("127.0.0.1");
        server.SetPort_LUA(0);
        REQUIRE(server.Listen());
        port = server.GetPort();
        server.Stop();
    }
    CHECK_FALSE(SoapClient("http://127.0.0.1:" + std::to_string(port) + "/AuthService").Call("LoginRequest", {}, error));
    CHECK(error.find("connect 127.0.0.1:") == 0);
}
