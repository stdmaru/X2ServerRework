// IAccountStore over the servers' KODBC (Common/ODBC): Account database, tables DEBAccounts and DEBUserCash.
#pragma once

#include "PublisherService.h"

#include "ODBC/Odbc.h"

#include <mutex>
#include <string>

namespace x2::epublisher {

class OdbcAccountStore final : public IAccountStore
{
public:
    // dsnPath: a file DSN as the servers use them (DBLayer:SetODBCDriver/SetODBCOptions apply through KODBC).
    bool Connect(const std::wstring& dsnPath);

    std::optional<Account> FindAccount(const std::string& accountID) override;
    std::optional<std::int64_t> FindCash(unsigned int userUID) override;
    std::optional<std::int64_t> SpendCash(unsigned int userUID, std::int64_t amount) override;

private:
    void Query(const std::wstring& query);  // throws on failure

    std::mutex m_lock;  // one connection, shared by the request threads
    KODBC      m_kODBC;
};

} // namespace x2::epublisher
