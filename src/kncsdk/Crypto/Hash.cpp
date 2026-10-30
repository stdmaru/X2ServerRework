// MD5 (RFC 1321) and SHA-256 (FIPS 180-4).
#include "Crypto/Crypto.h"

#include <cstring>

namespace x2::crypto {

namespace {

constexpr std::uint32_t RotateLeft(std::uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }
constexpr std::uint32_t RotateRight(std::uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

// Shared by both hashes: buffer the input and run Transform on each full block.
template <class Self>
void Absorb(Self& self, Byte* buffer, std::uint64_t& length, const void* data, std::size_t size)
{
    const Byte* in = static_cast<const Byte*>(data);
    std::size_t used = static_cast<std::size_t>(length % 64);
    length += size;
    if (used) {
        const std::size_t take = size < 64 - used ? size : 64 - used;
        std::memcpy(buffer + used, in, take);
        in += take;
        size -= take;
        used += take;
        if (used < 64)
            return;
        self.TransformBlock(buffer);
    }
    for (; size >= 64; in += 64, size -= 64)
        self.TransformBlock(in);
    std::memcpy(buffer, in, size);
}

// 0x80, zeros, then the bit length as 8 bytes (little-endian for MD5, big-endian for SHA-256).
template <class Self>
void Pad(Self& self, std::uint64_t length, bool bigEndian)
{
    const std::uint64_t bits = length * 8;
    Byte tail[72] = {0x80};
    const std::size_t used = static_cast<std::size_t>(length % 64);
    const std::size_t padSize = (used < 56 ? 56 : 120) - used;
    for (int i = 0; i < 8; ++i)
        tail[padSize + i] = static_cast<Byte>(bits >> (bigEndian ? 56 - 8 * i : 8 * i));
    self.Update(tail, padSize + 8);
}

} // namespace

// --- MD5 -------------------------------------------------------------------------------------------

namespace {

constexpr std::uint32_t kMd5K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};
constexpr int kMd5Shift[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
};

} // namespace

void Md5::Reset()
{
    m_state[0] = 0x67452301;
    m_state[1] = 0xefcdab89;
    m_state[2] = 0x98badcfe;
    m_state[3] = 0x10325476;
    m_length = 0;
}

void Md5::Transform(const Byte* block)
{
    std::uint32_t m[16];
    for (int i = 0; i < 16; ++i)
        m[i] = std::uint32_t(block[i * 4]) | std::uint32_t(block[i * 4 + 1]) << 8 |
               std::uint32_t(block[i * 4 + 2]) << 16 | std::uint32_t(block[i * 4 + 3]) << 24;

    std::uint32_t a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];
    for (int i = 0; i < 64; ++i) {
        std::uint32_t f;
        int g;
        if (i < 16) {
            f = (b & c) | (~b & d);
            g = i;
        } else if (i < 32) {
            f = (d & b) | (~d & c);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        } else {
            f = c ^ (b | ~d);
            g = (7 * i) % 16;
        }
        const std::uint32_t next = d;
        d = c;
        c = b;
        b = b + RotateLeft(a + f + kMd5K[i] + m[g], kMd5Shift[i]);
        a = next;
    }
    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;
}

void Md5::Update(const void* data, std::size_t size)
{
    struct Self
    {
        Md5* md5;
        void TransformBlock(const Byte* block) { md5->Transform(block); }
    } self{this};
    Absorb(self, m_buffer, m_length, data, size);
}

Md5::Digest Md5::Final()
{
    struct Self
    {
        Md5* md5;
        void Update(const void* data, std::size_t size) { md5->Update(data, size); }
    } self{this};
    Pad(self, m_length, false);

    Digest digest;
    for (int i = 0; i < 16; ++i)
        digest[i] = static_cast<Byte>(m_state[i / 4] >> (8 * (i % 4)));
    Reset();
    return digest;
}

Md5::Digest Md5::Hash(const void* data, std::size_t size)
{
    Md5 md5;
    md5.Update(data, size);
    return md5.Final();
}

// --- SHA-256 ---------------------------------------------------------------------------------------

namespace {

constexpr std::uint32_t kSha256K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

} // namespace

void Sha256::Reset()
{
    static constexpr std::uint32_t kInit[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                               0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    std::memcpy(m_state, kInit, sizeof(m_state));
    m_length = 0;
}

void Sha256::Transform(const Byte* block)
{
    std::uint32_t w[64];
    for (int i = 0; i < 16; ++i)
        w[i] = std::uint32_t(block[i * 4]) << 24 | std::uint32_t(block[i * 4 + 1]) << 16 |
               std::uint32_t(block[i * 4 + 2]) << 8 | std::uint32_t(block[i * 4 + 3]);
    for (int i = 16; i < 64; ++i) {
        const std::uint32_t s0 = RotateRight(w[i - 15], 7) ^ RotateRight(w[i - 15], 18) ^ (w[i - 15] >> 3);
        const std::uint32_t s1 = RotateRight(w[i - 2], 17) ^ RotateRight(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    std::uint32_t a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];
    std::uint32_t e = m_state[4], f = m_state[5], g = m_state[6], h = m_state[7];
    for (int i = 0; i < 64; ++i) {
        const std::uint32_t s1 = RotateRight(e, 6) ^ RotateRight(e, 11) ^ RotateRight(e, 25);
        const std::uint32_t ch = (e & f) ^ (~e & g);
        const std::uint32_t t1 = h + s1 + ch + kSha256K[i] + w[i];
        const std::uint32_t s0 = RotateRight(a, 2) ^ RotateRight(a, 13) ^ RotateRight(a, 22);
        const std::uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        const std::uint32_t t2 = s0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;
    m_state[4] += e;
    m_state[5] += f;
    m_state[6] += g;
    m_state[7] += h;
}

void Sha256::Update(const void* data, std::size_t size)
{
    struct Self
    {
        Sha256* sha;
        void TransformBlock(const Byte* block) { sha->Transform(block); }
    } self{this};
    Absorb(self, m_buffer, m_length, data, size);
}

Sha256::Digest Sha256::Final()
{
    struct Self
    {
        Sha256* sha;
        void Update(const void* data, std::size_t size) { sha->Update(data, size); }
    } self{this};
    Pad(self, m_length, true);

    Digest digest;
    for (int i = 0; i < 32; ++i)
        digest[i] = static_cast<Byte>(m_state[i / 4] >> (24 - 8 * (i % 4)));
    Reset();
    return digest;
}

Sha256::Digest Sha256::Hash(const void* data, std::size_t size)
{
    Sha256 sha;
    sha.Update(data, size);
    return sha.Final();
}

// --- Hex -------------------------------------------------------------------------------------------

std::string ToHex(const void* data, std::size_t size)
{
    static constexpr char kHex[] = "0123456789abcdef";
    const Byte* bytes = static_cast<const Byte*>(data);
    std::string out;
    out.reserve(size * 2);
    for (std::size_t i = 0; i < size; ++i) {
        out.push_back(kHex[bytes[i] >> 4]);
        out.push_back(kHex[bytes[i] & 0xF]);
    }
    return out;
}

} // namespace x2::crypto
