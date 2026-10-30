#include "OdbcAccountStore.h"

#include "x2/platform/unicode.h"

#include <stdexcept>

namespace x2::epublisher {

namespace {

// N'...' literal: the text goes in as written, quotes doubled (T-SQL), so the account name cannot end the string.
std::wstring SqlString(const std::string& utf8)
{
    std::wstring out = L"N'";
    for (const wchar_t c : x2::platform::Utf8ToWide(utf8)) {
        if (c == L'\'')
            out += L'\'';
        out += c;
    }
    out += L'\'';
    return out;
}

} // namespace

bool OdbcAccountStore::Connect(const std::wstring& dsnPath)
{
    std::lock_guard lock(m_lock);
    return m_kODBC.DriverConnect(dsnPath.c_str(), false);
}

void OdbcAccountStore::Query(const std::wstring& query)
{
    if (!m_kODBC.Query(query))
        throw std::runtime_error("query failed");
}

std::optional<Account> OdbcAccountStore::FindAccount(const std::string& accountID)
{
    std::lock_guard lock(m_lock);
    Query(L"SELECT UID, AccountID, Password, PurchaseToken FROM dbo.DEBAccounts WHERE AccountID = " + SqlString(accountID));
    if (!m_kODBC.BeginFetch())
        return std::nullopt;
    Account account;
    std::wstring id, hash, token;
    m_kODBC >> account.uid >> id >> hash >> token;
    m_kODBC.EndFetch();
    account.accountID = x2::platform::WideToUtf8(id);
    account.passwordHash = x2::platform::WideToUtf8(hash);
    account.purchaseToken = x2::platform::WideToUtf8(token);
    return account;
}

std::optional<std::int64_t> OdbcAccountStore::FindCash(unsigned int userUID)
{
    std::lock_guard lock(m_lock);
    Query(L"SELECT Cash FROM dbo.DEBUserCash WHERE UserUID = " + std::to_wstring(userUID));
    if (!m_kODBC.BeginFetch())
        return std::nullopt;
    int cash = 0;
    m_kODBC >> cash;
    m_kODBC.EndFetch();
    return cash;
}

std::optional<std::int64_t> OdbcAccountStore::SpendCash(unsigned int userUID, std::int64_t amount)
{
    std::lock_guard lock(m_lock);
    // one statement: the check and the update cannot be split by another purchase
    Query(L"UPDATE dbo.DEBUserCash SET Cash = Cash - " + std::to_wstring(amount) + L" OUTPUT inserted.Cash"
          L" WHERE UserUID = " + std::to_wstring(userUID) + L" AND Cash >= " + std::to_wstring(amount));
    if (!m_kODBC.BeginFetch())
        return std::nullopt;
    int cash = 0;
    m_kODBC >> cash;
    m_kODBC.EndFetch();
    return cash;
}

} // namespace x2::epublisher
