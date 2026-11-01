// EPublisher service: what the Channel/Game servers call through KDEBAuth/KDEBBilling (SimLayer's "EPublisher"
// requests, enabled by _DEB_AUTH). SOAP over HTTP, one thread per connection.
//
// A reimplementation of the project owner's AuthServer (C++/gSOAP/SOCI): same messages, same tables, same
// results, on the servers' own KODBC. Nothing in Common depends on it (docs/porting/server-epublisher.md).
#pragma once

#include "OdbcAccountStore.h"
#include "PublisherService.h"

#include "x2/platform/net.h"

#include <atomic>
#include <string>

struct lua_State;

namespace x2::epublisher {

class EPublisherServer
{
public:
    explicit EPublisherServer(IAccountStore& store) : m_service(store) {}
    ~EPublisherServer();

    // Lua configuration (EPublisher:SetPort(8000) ...)
    void SetPort_LUA(int port) { m_port = static_cast<unsigned short>(port); }
    void SetMaintenance_LUA(bool maintenance) { m_service.SetMaintenance(maintenance); }
    void SetBindAddress_LUA(const char* address) { m_bindAddress = address ? address : ""; }
    void SetDSN_LUA(const char* path) { m_dsnPath = path ? path : ""; }
    void SetODBCDriver_LUA(const char* driver);
    void SetODBCOptions_LUA(const char* options);
    unsigned short GetPort() const { return m_port; }
    const std::string& GetDSN() const { return m_dsnPath; }

    bool Listen();          // binds the port (0: any free port, see GetPort)
    void Run();             // accepts until Stop()
    void Stop() { m_stop = true; }

    // One HTTP body in, the SOAP answer out (exposed for the tests).
    std::string HandleSoap(const std::string& body, int& httpStatus);

private:
    void Serve(SOCKET sock, std::string peer);

    PublisherService       m_service;
    unsigned short         m_port = 8000;
    std::string            m_bindAddress = "0.0.0.0";
    std::string            m_dsnPath = "Account.dsn";
    SOCKET                 m_listen = INVALID_SOCKET;
    std::atomic<bool>      m_stop{ false };
    std::atomic<int>       m_connections{ 0 };
};

// Registers the global EPublisher (this server) and log() in `L`.
void RegisterLua(lua_State* L, EPublisherServer& server);

} // namespace x2::epublisher
