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
