// Hashes and ciphers the protocol and the databases of the original need (decision T27): DES-CBC and HMAC-MD5
// (KncSecurity packets, DB connection strings), MD5 (KncUtil::GetMD5) and SHA-256 (EPublisherServer passwords).
// Written from the specifications (FIPS 46-3, RFC 1321, FIPS 180-4, RFC 2104) in place of Crypto++, and checked
// against their test vectors (tests/test_crypto.cpp). DES and MD5 are broken: they are here only because the
// original client speaks them.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace x2::crypto {

using Byte = unsigned char;

// MD5 (RFC 1321).
class Md5
{
public:
    static constexpr std::size_t kDigestSize = 16;
    static constexpr std::size_t kBlockSize = 64;
    using Digest = std::array<Byte, kDigestSize>;

    Md5() { Reset(); }
    void Reset();
    void Update(const void* data, std::size_t size);
    Digest Final(); // also resets

    static Digest Hash(const void* data, std::size_t size);

private:
    void Transform(const Byte* block);

    std::uint32_t m_state[4];
    std::uint64_t m_length; // bytes hashed so far
    Byte m_buffer[kBlockSize];
};

// SHA-256 (FIPS 180-4).
class Sha256
{
public:
    static constexpr std::size_t kDigestSize = 32;
    static constexpr std::size_t kBlockSize = 64;
    using Digest = std::array<Byte, kDigestSize>;

    Sha256() { Reset(); }
    void Reset();
    void Update(const void* data, std::size_t size);
    Digest Final(); // also resets

    static Digest Hash(const void* data, std::size_t size);

private:
    void Transform(const Byte* block);

    std::uint32_t m_state[8];
    std::uint64_t m_length;
    Byte m_buffer[kBlockSize];
};

// HMAC (RFC 2104) over Md5 or Sha256.
template <class Hash>
class Hmac
{
public:
    using Digest = typename Hash::Digest;

    Hmac(const void* key, std::size_t keySize)
    {
        Byte block[Hash::kBlockSize] = {};
        if (keySize > Hash::kBlockSize) {
            const Digest digest = Hash::Hash(key, keySize);
            for (std::size_t i = 0; i < digest.size(); ++i)
                block[i] = digest[i];
        } else {
            const Byte* bytes = static_cast<const Byte*>(key);
            for (std::size_t i = 0; i < keySize; ++i)
                block[i] = bytes[i];
        }
        for (std::size_t i = 0; i < Hash::kBlockSize; ++i) {
            m_innerPad[i] = static_cast<Byte>(block[i] ^ 0x36);
            m_outerPad[i] = static_cast<Byte>(block[i] ^ 0x5C);
        }
        m_inner.Update(m_innerPad, Hash::kBlockSize);
    }

    void Update(const void* data, std::size_t size) { m_inner.Update(data, size); }

    Digest Final() // also restarts with the same key
    {
        const Digest innerDigest = m_inner.Final();
        Hash outer;
        outer.Update(m_outerPad, Hash::kBlockSize);
        outer.Update(innerDigest.data(), innerDigest.size());
        m_inner.Update(m_innerPad, Hash::kBlockSize);
        return outer.Final();
    }

private:
    Hash m_inner;
    Byte m_innerPad[Hash::kBlockSize];
    Byte m_outerPad[Hash::kBlockSize];
};

using HmacMd5 = Hmac<Md5>;

// DES (FIPS 46-3), one 8-byte block at a time. The key's parity bits are ignored.
class Des
{
public:
    static constexpr std::size_t kBlockSize = 8;
    static constexpr std::size_t kKeySize = 8;

    explicit Des(const Byte* key);
    void EncryptBlock(const Byte* in, Byte* out) const;
    void DecryptBlock(const Byte* in, Byte* out) const;

private:
    std::uint64_t Crypt(std::uint64_t block, bool decrypt) const;

    std::uint64_t m_subkeys[16]; // 48 bits each
};

// DES in CBC mode without padding: size must be a multiple of 8 (Crypto++'s CBC_Mode<DES> as the original used it).
// in and out may be the same buffer.
void DesCbcEncrypt(const Byte* key, const Byte* iv, const Byte* in, Byte* out, std::size_t size);
void DesCbcDecrypt(const Byte* key, const Byte* iv, const Byte* in, Byte* out, std::size_t size);

// Lower-case hex of the bytes.
std::string ToHex(const void* data, std::size_t size);

} // namespace x2::crypto
