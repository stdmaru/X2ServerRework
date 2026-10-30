// Exact bytes of the first packets of the login, derived by hand from the field order in ClientPacket.cpp/
// CommonPacket.cpp and the wire format of docs/original/protocolo.md (numbers big-endian, strings with a
// big-endian byte count; std::wstring as UTF-16LE). Not yet checked against captured traffic.
#include "packet_test.h"

#include "ClientPacket.h"
#include "CommonPacket.h"

namespace {

// big-endian 32-bit
Bytes U32(unsigned v) { return { Bytes::value_type(v >> 24), Bytes::value_type(v >> 16), Bytes::value_type(v >> 8), Bytes::value_type(v) }; }

Bytes Cat(std::initializer_list<Bytes> parts)
{
    Bytes out;
    for (const Bytes& p : parts)
        out.insert(out.end(), p.begin(), p.end());
    return out;
}

} // namespace

TEST_CASE("ECH_VERIFY_ACCOUNT_REQ: client -> channel server")
{
    KECH_VERIFY_ACCOUNT_REQ req;
    req.m_bLogin = true;
    req.m_wstrID = L"ab";
    req.m_wstrPassword = L"가"; // a Hangul syllable: one UTF-16 unit
    req.m_iChannelingCode = 0x0102;
    req.m_strMachineID = "M";
    req.m_bIDVerified = false;
    req.m_bSteamClient = false;
    req.m_bAeriaClient = true;

    CHECK(Serialize(req) == Cat({
        { 0x01 },                                   // m_bLogin
        U32(4), { 'a', 0, 'b', 0 },                 // m_wstrID: byte count + UTF-16LE
        U32(2), { 0x00, 0xAC },                     // m_wstrPassword
        U32(0x0102),                                // m_iChannelingCode
        U32(1), { 'M' },                            // m_strMachineID
        { 0x00, 0x00, 0x01 },                       // m_bIDVerified, m_bSteamClient, m_bAeriaClient
    }));
}

TEST_CASE("ECH_VERIFY_ACCOUNT_ACK: channel server -> client")
{
    KECH_VERIFY_ACCOUNT_ACK ack;
    ack.m_iOK = 0;
    ack.m_iUserUID = 0x0102030405060708LL;
    ack.m_wstrPassport = L"p";
    ack.m_iChannelingCode = 3;
    ack.m_wstrCurrentTime = L"";
    ack.m_wstrUserID = L"u";
    ack.m_wstrPurchaseTok = L"";
    ack.m_strAgreementURL = "";

    CHECK(Serialize(ack) == Cat({
        U32(0),                                     // m_iOK
        { 1, 2, 3, 4, 5, 6, 7, 8 },                 // m_iUserUID (UidType, 64-bit)
        U32(2), { 'p', 0 },                         // m_wstrPassport
        U32(3),                                     // m_iChannelingCode
        U32(0),                                     // m_wstrCurrentTime
        U32(2), { 'u', 0 },                         // m_wstrUserID
        U32(0),                                     // m_wstrPurchaseTok
        U32(0),                                     // m_strAgreementURL
    }));
}

TEST_CASE("EGS_CONNECT_REQ / EGS_CONNECT_ACK: client <-> game server")
{
    KEGS_CONNECT_REQ req;
    req.m_wstrVersion = L"가나"; // the deployed config_gs.ini has Korean version strings
    CHECK(Serialize(req) == Cat({ U32(4), { 0x00, 0xAC, 0x98, 0xB0 } }));

    KEGS_CONNECT_ACK ack;
    ack.m_iOK = 0;
    ack.m_UDPPort = 9400;
    ack.m_iChannelID = 1;
    CHECK(Serialize(ack) == Cat({ U32(0), U32(9400), U32(1) }));
}
