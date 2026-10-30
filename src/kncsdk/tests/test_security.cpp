#include "KncSecurity/KncSecurity.h"

#include <doctest/doctest.h>

#include <cstring>
#include <string>

using namespace KncSecurity;

namespace {

KByteStream Bytes(const std::string& s)
{
    return KByteStream(reinterpret_cast<const unsigned char*>(s.data()), s.size());
}

struct SecurityFixture
{
    SecurityFixture() { InitSecurity(); }
    ~SecurityFixture() { ReleaseSecurity(); }
};

} // namespace

TEST_CASE_FIXTURE(SecurityFixture, "secure buffer round trip with a fresh SA")
{
    SpiType spi = 0;
    REQUIRE(GetSADB().CreateNewSA(spi) != nullptr);
    CHECK(spi != 0);

    KSecureBuffer send(spi);
    REQUIRE(send.Create(Bytes("hello elsword")));

    // header: SPI (2 bytes) + sequence number (4 bytes), raw little-endian
    REQUIRE(send.GetSize() > 6);
    SpiType spiOnWire = 0;
    std::memcpy(&spiOnWire, send.GetDataPtr(), sizeof(spiOnWire));
    CHECK(spiOnWire == spi);

    KSecureBuffer recv(spi, send.GetDataPtr(), send.GetSize());
    REQUIRE(recv.IsAuthentic());
    KByteStream payload;
    REQUIRE(recv.GetPayload(payload));
    CHECK(payload == Bytes("hello elsword"));

    GetSADB().Delete(spi);
}

TEST_CASE_FIXTURE(SecurityFixture, "tampered data is rejected")
{
    SpiType spi = 0;
    REQUIRE(GetSADB().CreateNewSA(spi) != nullptr);

    KSecureBuffer send(spi);
    REQUIRE(send.Create(Bytes("payload that will be altered")));
    std::string wire(send.GetDataPtr(), send.GetSize());
    wire[wire.size() / 2] ^= 0x5A;

    KSecureBuffer recv(spi, wire.data(), static_cast<unsigned long>(wire.size()));
    CHECK_FALSE(recv.IsAuthentic());
    KByteStream payload;
    CHECK_FALSE(recv.GetPayload(payload)); // the original went on decrypting here (_JIF without return)

    GetSADB().Delete(spi);
}

TEST_CASE_FIXTURE(SecurityFixture, "SA keys are not the same on every run")
{
    // the original generator was never seeded; at least two SAs in a row must differ
    SpiType a = 0, b = 0;
    REQUIRE(GetSADB().CreateNewSA(a));
    REQUIRE(GetSADB().CreateNewSA(b));
    CHECK(a != b);
    CHECK_FALSE(GetSADB().GetSA(a).GetCipherKey() == GetSADB().GetSA(b).GetCipherKey());
    GetSADB().Delete(a);
    GetSADB().Delete(b);
}

TEST_CASE("default SA (SPI 0) uses the original client's key bytes")
{
    // The original keys were CP949 string literals; the client hashes with their first 8 bytes. A wrong key makes
    // the client reject the server's first packet (IsValidICV_Local fails with SPI 0).
    const KSecurityAssociation sa;
    const unsigned char auth[] = { 0xC0, 0xD3, 0xBD, 0xC3, 0xB7, 0xCE, 0xB8, 0xB8 };
    CHECK(sa.GetAuthKey() == KByteStream(auth, sizeof(auth)));

    // HMAC-MD5 truncated to 10 bytes, reference computed outside (Python hmac): f50c12d659a268b33f18
    const unsigned char expected[] = { 0xf5, 0x0c, 0x12, 0xd6, 0x59, 0xa2, 0x68, 0xb3, 0x3f, 0x18 };
    CHECK(sa.GenerateICV(Bytes("Elsword")) == KByteStream(expected, sizeof(expected)));
}
