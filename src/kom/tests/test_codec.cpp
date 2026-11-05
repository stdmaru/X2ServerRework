#include "helpers.h"

#include "x2/kom/codec.h"

using namespace x2::kom;
using namespace kom_test;

namespace {

std::string Hex(const Bytes& data)
{
    static const char digits[] = "0123456789abcdef";
    std::string out;
    for (const std::uint8_t b : data)
    {
        out += digits[b >> 4];
        out += digits[b & 15];
    }
    return out;
}

Bytes Iota(std::size_t n)
{
    Bytes out(n);
    for (std::size_t i = 0; i < n; ++i)
        out[i] = static_cast<std::uint8_t>(i);
    return out;
}

} // namespace

TEST_CASE("kom: XORCRC matches kom.py")
{
    // Vectors from tools/kom/kom.py xorcrc_encrypt(bytes(range(40)), name, algorithm), 2026-11-05.
    Bytes a = Iota(40);
    XorCrcEncrypt(a, "Test_Script.lua", kXorCrc);
    CHECK(Hex(a) == "bcaff297e4e8b1279eb762ded49d1fd11e98b7148210548a81716f96cdff9196c45f7b24b45b0950");

    Bytes b = Iota(40);
    XorCrcEncrypt(b, "Test_Script.lua", kXorCrc3);
    CHECK(Hex(b) == "8f001af7dcc4bc3733c89ee46663277e9ddbf65c7a6405a957ceec94384d259b5808e80a931aa2de");

    // Non-ASCII name: UTF-16 code units, the high byte only when non-zero; only ASCII is lower-cased.
    Bytes c = Iota(40);
    XorCrcEncrypt(c, "Ab\xC3\xA9\xE4\xB8\xAD.TXT", kXorCrc);
    CHECK(Hex(c) == "be477a6bccb4293e3c0857e0257ceed1825a9b3a9b81b8c414ac9e322107e71ce80309922aaccf13");
}

TEST_CASE("kom: XORCRC round trip, keyed by the case-insensitive name and the length")
{
    for (const std::uint32_t algorithm : {kXorCrc, kXorCrc3})
    {
        for (const std::size_t size : {std::size_t{1}, std::size_t{255}, std::size_t{256}, std::size_t{70000}})
        {
            const Bytes plain = Pattern(size, 3);
            Bytes x = plain;
            XorCrcEncrypt(x, "Some_File.LUA", algorithm);
            CHECK(x != plain);
            Bytes y = x;
            XorCrcDecrypt(y, "some_file.lua", algorithm);
            CHECK(y == plain);
            Bytes z = x;
            XorCrcDecrypt(z, "other.lua", algorithm);
            CHECK(z != plain);
        }
    }
}

TEST_CASE("kom: encode and decode every decodable algorithm")
{
    for (const std::uint32_t algorithm : {kZlib, kXorCrc, kXorCrc3})
    {
        for (const std::size_t size : {std::size_t{0}, std::size_t{1}, std::size_t{4096}, std::size_t{300000}})
        {
            CAPTURE(algorithm);
            CAPTURE(size);
            const Bytes data = Pattern(size, static_cast<unsigned>(algorithm));
            const Bytes blob = Encoded("Entry.lua", algorithm, data);
            Bytes out;
            std::string error;
            REQUIRE_MESSAGE(Decode("ENTRY.LUA", algorithm, blob, static_cast<std::uint32_t>(size), out, error), error);
            CHECK(out == data);

            // A wrong size in the header is an error, never silently cut or padded data.
            CHECK_FALSE(Decode("Entry.lua", algorithm, blob, static_cast<std::uint32_t>(size + 1), out, error));
            if (size > 0)
                CHECK_FALSE(Decode("Entry.lua", algorithm, blob, static_cast<std::uint32_t>(size - 1), out, error));
        }
    }
}

TEST_CASE("kom: corrupted blobs and unsupported algorithms are rejected")
{
    const Bytes data = Pattern(5000, 1);
    std::string error;
    Bytes out;
    for (const std::uint32_t algorithm : {kZlib, kXorCrc, kXorCrc3})
    {
        // Algorithm 3 has no adler32 trailer, so only the entry checksum (Archive::ReadStored) catches every change.
        if (algorithm != kXorCrc3)
        {
            Bytes blob = Encoded("x.lua", algorithm, data);
            blob[blob.size() / 2] ^= 0x5A;
            CHECK_FALSE(Decode("x.lua", algorithm, blob, 5000, out, error));
        }
        // The wrong name gives garbage for the encrypted algorithms.
        if (algorithm != kZlib)
            CHECK_FALSE(Decode("y.lua", algorithm, Encoded("x.lua", algorithm, data), 5000, out, error));
    }
    CHECK_FALSE(Decode("x.lua", kXorCrc3, Bytes{1, 2, 3}, 0, out, error));
    CHECK_FALSE(Decode("x.lua", kBlowfish, Encoded("x.lua", kZlib, data), 5000, out, error));
    CHECK(error.find("not supported") != std::string::npos);
    Bytes blob;
    CHECK_FALSE(Encode("x.lua", kBlowfish, data, blob, error));
    CHECK_FALSE(IsDecodable(kBlowfish));
}

TEST_CASE("kom: adler32 and ASCII lower-casing")
{
    CHECK(Adler32(ToBytes("")) == 1u);
    CHECK(Adler32(ToBytes("Wikipedia")) == 0x11E60398u);
    CHECK(AsciiLower("DLG_Map.LUA") == "dlg_map.lua");
    CHECK(AsciiLower("\xC3\x89") == "\xC3\x89");
}
