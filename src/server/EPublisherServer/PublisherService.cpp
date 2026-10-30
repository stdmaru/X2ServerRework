#include "PublisherService.h"

#include "NetError.h"

#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <charconv>
#include <exception>
#include <string_view>

namespace x2::epublisher {

namespace {

// Salt of the stored hashes (DEBAccounts.Password = sha256( password + salt ), lower-case hex).
constexpr std::string_view kPasswordSalt = "ZKlr49cxNAVt5BJH";

bool ParseInt(std::string_view s, std::int64_t& value)
{
    const auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    return ec == std::errc() && end == s.data() + s.size();
}

// Sum of the prices of "<productNo>_<productID>_<price>|..."; nullopt when the list is malformed or empty.
std::optional<std::int64_t> TotalPrice(std::string_view list)
{
    std::int64_t total = 0;
    int count = 0;
    while (!list.empty()) {
        const auto bar = list.find('|');
        std::string_view item = list.substr(0, bar);
        list = bar == std::string_view::npos ? std::string_view() : list.substr(bar + 1);
        if (item.empty())
            continue;
        std::int64_t parts[3];
        for (int i = 0; i < 3; ++i) {
            const auto sep = item.find('_');
            if ((i < 2) == (sep == std::string_view::npos) || !ParseInt(item.substr(0, sep), parts[i]) || parts[i] < 0)
                return std::nullopt;
            item = sep == std::string_view::npos ? std::string_view() : item.substr(sep + 1);
        }
        total += parts[2];
        ++count;
    }
    if (count == 0)
        return std::nullopt;
    return total;
}

} // namespace

std::string PublisherService::HashPassword(const std::string& password)
{
    const std::string salted = password + std::string(kPasswordSalt);
    std::string hex;
    CryptoPP::SHA256 sha;
    CryptoPP::StringSource(salted, true,
        new CryptoPP::HashFilter(sha, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex), false)));
    return hex;
}

LoginResult PublisherService::Login(const std::string& username, const std::string& password)
{
    LoginResult result;
    if (m_maintenance) {
        result.result = NetError::ERR_VERIFY_12;
        return result;
    }
    try {
        const auto account = m_store.FindAccount(username);
        if (!account || account->passwordHash != HashPassword(password)) {
            result.result = NetError::ERR_VERIFY_20;
            return result;
        }
        result.result = NetError::NET_OK;
        result.account = *account;
    } catch (const std::exception&) {
        result.result = NetError::ERR_VERIFY_21;
    }
    return result;
}

CashResult PublisherService::GetUserCash(unsigned int publisherUID)
{
    CashResult result;
    try {
        const auto cash = m_store.FindCash(publisherUID);
        if (!cash) {
            result.result = NetError::ERR_GASH_02;
            return result;
        }
        result.result = NetError::NET_OK;
        result.balance = *cash;
    } catch (const std::exception&) {
        result.result = NetError::ERR_GASH_00;
    }
    return result;
}

CashResult PublisherService::PurchaseProduct(unsigned int publisherUID, const std::string& totalList)
{
    CashResult result;
    result.result = NetError::ERR_PAYLETTER_BILLING_30;
    const auto price = TotalPrice(totalList);
    if (!price)
        return result;
    try {
        const auto cash = m_store.FindCash(publisherUID);
        if (!cash) {
            result.result = NetError::ERR_PAYLETTER_BILLING_09;
            return result;
        }
        const auto balance = m_store.SpendCash(publisherUID, *price);
        if (!balance) {
            result.result = NetError::ERR_PAYLETTER_BILLING_08;
            result.balance = *cash;
            return result;
        }
        result.result = NetError::NET_OK;
        result.balance = *balance;
    } catch (const std::exception&) {
        result.result = NetError::ERR_PAYLETTER_BILLING_30;
    }
    return result;
}

} // namespace x2::epublisher
