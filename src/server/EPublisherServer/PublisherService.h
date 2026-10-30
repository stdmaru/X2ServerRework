// The operations of the EPublisher service (the DEB "AuthService": login and cash), independent of SOAP and of
// the database. Result codes are the servers' NetError values: the Channel/Game servers forward them to the client.
#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace x2::epublisher {

struct Account
{
    int         uid = 0;            // publisher UID (DEBAccounts.UID)
    std::string accountID;          // login name
    std::string passwordHash;       // lower-case hex sha256( password + salt )
    std::string purchaseToken;
};

// Storage of accounts and cash (Account database: DEBAccounts, DEBUserCash). Errors throw.
class IAccountStore
{
public:
    virtual ~IAccountStore() = default;
    virtual std::optional<Account> FindAccount(const std::string& accountID) = 0;
    virtual std::optional<std::int64_t> FindCash(unsigned int userUID) = 0;
    // Takes `amount` if the balance covers it; returns the new balance, or nullopt when it does not.
    virtual std::optional<std::int64_t> SpendCash(unsigned int userUID, std::int64_t amount) = 0;
};

struct LoginResult
{
    int         result = 0;
    Account     account;
};

struct CashResult
{
    int           result = 0;
    std::int64_t  balance = 0;
};

class PublisherService
{
public:
    explicit PublisherService(IAccountStore& store) : m_store(store) {}

    void SetMaintenance(bool maintenance) { m_maintenance = maintenance; }
    bool IsMaintenance() const { return m_maintenance; }

    LoginResult Login(const std::string& username, const std::string& password);
    CashResult GetUserCash(unsigned int publisherUID);
    // totalList: "<productNo>_<productID>_<price>|..." (as KDEBBilling sends it)
    CashResult PurchaseProduct(unsigned int publisherUID, const std::string& totalList);

    static std::string HashPassword(const std::string& password);

private:
    IAccountStore& m_store;
    bool           m_maintenance = false;
};

} // namespace x2::epublisher
