// x2::crypto against the published test vectors (RFC 1321, FIPS 180-4 examples, RFC 2202, FIPS 81 / SP 800-17).
#include "Crypto/Crypto.h"

#include <doctest/doctest.h>

#include <string>
#include <vector>

using namespace x2::crypto;

namespace {

std::vector<Byte> FromHex(const std::string& hex)
{
    std::vector<Byte> out;
    for (std::size_t i = 0; i + 1 < hex.size(); i += 2)
        out.push_back(static_cast<Byte>(std::stoi(hex.substr(i, 2), nullptr, 16)));
    return out;
}

template <class Digest>
std::string Hex(const Digest& d) { return ToHex(d.data(), d.size()); }

std::string Md5Hex(const std::string& s) { return Hex(Md5::Hash(s.data(), s.size())); }
std::string Sha256Hex(const std::string& s) { return Hex(Sha256::Hash(s.data(), s.size())); }

} // namespace

TEST_CASE("MD5: RFC 1321 test suite")
{
    CHECK(Md5Hex("") == "d41d8cd98f00b204e9800998ecf8427e");
    CHECK(Md5Hex("a") == "0cc175b9c0f1b6a831c399e269772661");
    CHECK(Md5Hex("abc") == "900150983cd24fb0d6963f7d28e17f72");
    CHECK(Md5Hex("message digest") == "f96b697d7cb7938d525a2f31aaf161d0");
    CHECK(Md5Hex("abcdefghijklmnopqrstuvwxyz") == "c3fcd3d76192e4007dfb496cca67e13b");
    CHECK(Md5Hex("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") == "d174ab98d277d9f5a5611c2c9f419d9f");
    CHECK(Md5Hex("12345678901234567890123456789012345678901234567890123456789012345678901234567890") ==
          "57edf4a22be3c955ac49da2e2107b67a");
}

TEST_CASE("MD5 and SHA-256: input split in pieces gives the same digest")
{
    const std::string text(1000, 'x');
    for (std::size_t piece : {1u, 7u, 55u, 56u, 63u, 64u, 65u, 333u}) {
        Md5 md5;
        Sha256 sha;
        for (std::size_t i = 0; i < text.size(); i += piece) {
            const std::size_t n = text.size() - i < piece ? text.size() - i : piece;
            md5.Update(text.data() + i, n);
            sha.Update(text.data() + i, n);
        }
        CHECK(Hex(md5.Final()) == Md5Hex(text));
        CHECK(Hex(sha.Final()) == Sha256Hex(text));
    }
}

TEST_CASE("SHA-256: FIPS 180-4 examples")
{
    CHECK(Sha256Hex("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CHECK(Sha256Hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    CHECK(Sha256Hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ==
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    CHECK(Sha256Hex(std::string(1000000, 'a')) == "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST_CASE("HMAC-MD5: RFC 2202 test cases")
{
    auto hmac = [](const std::vector<Byte>& key, const std::string& data) {
        HmacMd5 mac(key.data(), key.size());
        mac.Update(data.data(), data.size());
        return Hex(mac.Final());
    };
    CHECK(hmac(std::vector<Byte>(16, 0x0b), "Hi There") == "9294727a3638bb1c13f48ef8158bfc9d");
    CHECK(hmac(FromHex("4a656665"), "what do ya want for nothing?") == "750c783e6ab0b503eaa86e310a5db738");
    CHECK(hmac(std::vector<Byte>(16, 0xaa), std::string(50, '\xdd')) == "56be34521d144c88dbb8c733f0e8b3f6");
    CHECK(hmac(std::vector<Byte>(80, 0xaa), "Test Using Larger Than Block-Size Key - Hash Key First") ==
          "6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd");

    // Final() restarts with the same key
    HmacMd5 mac(FromHex("4a656665").data(), 4);
    const std::string data = "what do ya want for nothing?";
    mac.Update(data.data(), data.size());
    mac.Final();
    mac.Update(data.data(), data.size());
    CHECK(Hex(mac.Final()) == "750c783e6ab0b503eaa86e310a5db738");
}

TEST_CASE("DES: known answers")
{
    auto encrypt = [](const std::string& key, const std::string& plain) {
        const auto k = FromHex(key), p = FromHex(plain);
        Byte out[8];
        Des(k.data()).EncryptBlock(p.data(), out);
        Byte back[8];
        Des(k.data()).DecryptBlock(out, back);
        CHECK(ToHex(back, 8) == plain);
        return ToHex(out, 8);
    };
    CHECK(encrypt("133457799bbcdff1", "0123456789abcdef") == "85e813540f0ab405");
    CHECK(encrypt("0e329232ea6d0d73", "8787878787878787") == "0000000000000000");
    CHECK(encrypt("0123456789abcdef", "4e6f772069732074") == "3fa40e8a984d4815");     // FIPS 81
    CHECK(encrypt("0101010101010101", "8000000000000000") == "95f8a5e5dd31d900");     // SP 800-17 variable plaintext
    CHECK(encrypt("8001010101010101", "0000000000000000") == "95a8d72813daa94d");     // SP 800-17 variable key
}

TEST_CASE("DES-CBC: FIPS 81 example, in place too")
{
    const auto key = FromHex("0123456789abcdef");
    const auto iv = FromHex("1234567890abcdef");
    const std::string plain = "Now is the time for all ";
    std::vector<Byte> buf(plain.begin(), plain.end());
    DesCbcEncrypt(key.data(), iv.data(), buf.data(), buf.data(), buf.size());
    CHECK(ToHex(buf.data(), buf.size()) == "e5c7cdde872bf27c43e934008c389c0f683788499a7c05f6");
    DesCbcDecrypt(key.data(), iv.data(), buf.data(), buf.data(), buf.size());
    CHECK(std::string(buf.begin(), buf.end()) == plain);
}
