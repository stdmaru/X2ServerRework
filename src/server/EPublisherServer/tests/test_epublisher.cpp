#include "EPublisherServer.h"
#include "Soap.h"

#include "NetError.h"
#include "OnlyGlobal/AuthAndBilling/DEB/soapH.h"  // the servers' gSOAP client (namespaces[] is in DEBAuth.cpp)

#include <doctest/doctest.h>

#include <map>
#include <string>
#include <thread>

using namespace x2::epublisher;

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
    const auto r = ParseSoapRequest(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\""
        " xmlns:ns2=\"urn:AuthService\"><SOAP-ENV:Body><ns2:LoginRequest><ns2:strUsername>a&lt;b&amp;c&#233;</ns2:strUsername>"
        "<ns2:strPassword></ns2:strPassword><ns2:strIPAddress/></ns2:LoginRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>");
    REQUIRE(r);
    CHECK(r->operation == "LoginRequest");
    CHECK(r->fields.at("strUsername") == "a<b&c\xC3\xA9");
    CHECK(r->fields.at("strPassword").empty());
    CHECK(r->fields.at("strIPAddress").empty());
    CHECK_FALSE(ParseSoapRequest("not xml"));
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

TEST_CASE("the servers' gSOAP client talks to the service")
{
    FakeStore store = MakeStore();
    EPublisherServer server(store);
    server.SetBindAddress_LUA("127.0.0.1");
    server.SetPort_LUA(0);
    REQUIRE(server.Listen());
    std::thread accepter([&server] { server.Run(); });
    const std::string endpoint = "http://127.0.0.1:" + std::to_string(server.GetPort()) + "/";

    soap* client = soap_new();

    SUBCASE("Login, as KDEBAuth calls it")
    {
        _ns2__LoginRequest request;
        request.strUsername = "test";
        request.strPassword = "test";
        request.strIPAddress = "192.168.71.45";
        _ns2__LoginResponse response;
        REQUIRE(soap_call___ns1__Login(client, endpoint.c_str(), NULL, &request, &response) == SOAP_OK);
        CHECK(response.iResult == NetError::NET_OK);
        CHECK(response.strID == "20004");
        CHECK(response.iPublisherSN == 20004);
        CHECK(response.strPurchaseToken == "tok<&>");
        CHECK(response.iChannelingCode == 0);

        request.strPassword = "nope";
        REQUIRE(soap_call___ns1__Login(client, endpoint.c_str(), NULL, &request, &response) == SOAP_OK);
        CHECK(response.iResult == NetError::ERR_VERIFY_20);
    }
    SUBCASE("GetUserCash and PurchaseProduct, as KDEBBilling calls them")
    {
        _ns2__GetUserCashRequest cashRequest;
        cashRequest.uiPublisherUID = 20004;
        _ns2__GetUserCashResponse cashResponse;
        REQUIRE(soap_call___ns1__GetUserCash(client, endpoint.c_str(), NULL, &cashRequest, &cashResponse) == SOAP_OK);
        CHECK(cashResponse.iOK == NetError::NET_OK);
        CHECK(cashResponse.ulBalance == 7700);

        _ns2__PurchaseProductRequest buy;
        buy.uiPublisherUID = 20004;
        buy.iChannelingCode = 0;
        buy.iUnitUID = 1;
        buy.strUserIP = "127.0.0.1";
        buy.strNickName = "nick";
        buy.strTotalOrderID = "order";
        buy.strTotalList = "1_100_700";
        _ns2__PurchaseProductResponse bought;
        REQUIRE(soap_call___ns1__PurchaseProduct(client, endpoint.c_str(), NULL, &buy, &bought) == SOAP_OK);
        CHECK(bought.iOK == NetError::NET_OK);
        CHECK(bought.iTotalCash == 7000);
    }

    soap_destroy(client);
    soap_end(client);
    soap_free(client);
    server.Stop();
    accepter.join();
}
