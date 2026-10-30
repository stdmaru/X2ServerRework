// DES (FIPS 46-3) and CBC mode. Bit tables are numbered from 1 at the most significant bit, as in the standard.
#include "Crypto/Crypto.h"

namespace x2::crypto {

namespace {

constexpr unsigned char kInitialPermutation[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7,
};
constexpr unsigned char kFinalPermutation[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9,  49, 17, 57, 25,
};
constexpr unsigned char kExpansion[48] = {
    32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9,  10, 11,
    12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
    22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1,
};
constexpr unsigned char kPermutation[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25,
};
constexpr unsigned char kPermutedChoice1[56] = {
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4,
};
constexpr unsigned char kPermutedChoice2[48] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
    23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32,
};
constexpr int kKeyShifts[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// S-boxes S1..S8, row-major: row (outer bits) * 16 + column (inner 4 bits).
constexpr unsigned char kSBoxes[8][64] = {
    {14, 4,  13, 1, 2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0, 7,
     0,  15, 7,  4, 14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3, 8,
     4,  1,  14, 8, 13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5, 0,
     15, 12, 8,  2, 4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6, 13},
    {15, 1,  8,  14, 6,  11, 3,  4,  9,  7, 2,  13, 12, 0, 5,  10,
     3,  13, 4,  7,  15, 2,  8,  14, 12, 0, 1,  10, 6,  9, 11, 5,
     0,  14, 7,  11, 10, 4,  13, 1,  5,  8, 12, 6,  9,  3, 2,  15,
     13, 8,  10, 1,  3,  15, 4,  2,  11, 6, 7,  12, 0,  5, 14, 9},
    {10, 0,  9,  14, 6, 3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8,
     13, 7,  0,  9,  3, 4,  6,  10, 2,  8,  5,  14, 12, 11, 15, 1,
     13, 6,  4,  9,  8, 15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7,
     1,  10, 13, 0,  6, 9,  8,  7,  4,  15, 14, 3,  11, 5,  2,  12},
    {7,  13, 14, 3, 0,  6,  9,  10, 1,  2, 8, 5,  11, 12, 4,  15,
     13, 8,  11, 5, 6,  15, 0,  3,  4,  7, 2, 12, 1,  10, 14, 9,
     10, 6,  9,  0, 12, 11, 7,  13, 15, 1, 3, 14, 5,  2,  8,  4,
     3,  15, 0,  6, 10, 1,  13, 8,  9,  4, 5, 11, 12, 7,  2,  14},
    {2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0, 14, 9,
     14, 11, 2,  12, 4,  7,  13, 1,  5,  0,  15, 10, 3,  9, 8,  6,
     4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3, 0,  14,
     11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3},
    {12, 1,  10, 15, 9, 2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11,
     10, 15, 4,  2,  7, 12, 9,  5,  6,  1,  13, 14, 0,  11, 3,  8,
     9,  14, 15, 5,  2, 8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6,
     4,  3,  2,  12, 9, 5,  15, 10, 11, 14, 1,  7,  6,  0,  8,  13},
    {4,  11, 2,  14, 15, 0, 8,  13, 3,  12, 9, 7,  5,  10, 6, 1,
     13, 0,  11, 7,  4,  9, 1,  10, 14, 3,  5, 12, 2,  15, 8, 6,
     1,  4,  11, 13, 12, 3, 7,  14, 10, 15, 6, 8,  0,  5,  9, 2,
     6,  11, 13, 8,  1,  4, 10, 7,  9,  5,  0, 15, 14, 2,  3, 12},
    {13, 2,  8,  4, 6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7,
     1,  15, 13, 8, 10, 3,  7,  4,  12, 5,  6,  11, 0,  14, 9,  2,
     7,  11, 4,  1, 9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8,
     2,  1,  14, 7, 4,  10, 8,  13, 15, 12, 9,  0,  3,  5,  6,  11},
};

// Output bit i is input bit table[i] (1 = most significant of inputBits).
template <std::size_t N>
std::uint64_t Permute(std::uint64_t in, int inputBits, const unsigned char (&table)[N])
{
    std::uint64_t out = 0;
    for (std::size_t i = 0; i < N; ++i)
        out = (out << 1) | ((in >> (inputBits - table[i])) & 1);
    return out;
}

std::uint32_t Feistel(std::uint32_t right, std::uint64_t subkey)
{
    const std::uint64_t mixed = Permute(right, 32, kExpansion) ^ subkey;
    std::uint32_t out = 0;
    for (int box = 0; box < 8; ++box) {
        const unsigned six = static_cast<unsigned>(mixed >> (42 - 6 * box)) & 0x3F;
        const unsigned row = ((six & 0x20) >> 4) | (six & 1);
        const unsigned column = (six >> 1) & 0xF;
        out = (out << 4) | kSBoxes[box][row * 16 + column];
    }
    return static_cast<std::uint32_t>(Permute(out, 32, kPermutation));
}

std::uint64_t Load(const Byte* p)
{
    std::uint64_t v = 0;
    for (int i = 0; i < 8; ++i)
        v = (v << 8) | p[i];
    return v;
}

void Store(std::uint64_t v, Byte* p)
{
    for (int i = 7; i >= 0; --i, v >>= 8)
        p[i] = static_cast<Byte>(v);
}

} // namespace

Des::Des(const Byte* key)
{
    const std::uint64_t permuted = Permute(Load(key), 64, kPermutedChoice1);
    std::uint32_t c = static_cast<std::uint32_t>(permuted >> 28) & 0x0FFFFFFF;
    std::uint32_t d = static_cast<std::uint32_t>(permuted) & 0x0FFFFFFF;
    for (int round = 0; round < 16; ++round) {
        const int shift = kKeyShifts[round];
        c = ((c << shift) | (c >> (28 - shift))) & 0x0FFFFFFF;
        d = ((d << shift) | (d >> (28 - shift))) & 0x0FFFFFFF;
        m_subkeys[round] = Permute((std::uint64_t(c) << 28) | d, 56, kPermutedChoice2);
    }
}

std::uint64_t Des::Crypt(std::uint64_t block, bool decrypt) const
{
    const std::uint64_t permuted = Permute(block, 64, kInitialPermutation);
    std::uint32_t left = static_cast<std::uint32_t>(permuted >> 32);
    std::uint32_t right = static_cast<std::uint32_t>(permuted);
    for (int round = 0; round < 16; ++round) {
        const std::uint32_t next = left ^ Feistel(right, m_subkeys[decrypt ? 15 - round : round]);
        left = right;
        right = next;
    }
    // the halves are swapped before the final permutation
    return Permute((std::uint64_t(right) << 32) | left, 64, kFinalPermutation);
}

void Des::EncryptBlock(const Byte* in, Byte* out) const { Store(Crypt(Load(in), false), out); }
void Des::DecryptBlock(const Byte* in, Byte* out) const { Store(Crypt(Load(in), true), out); }

void DesCbcEncrypt(const Byte* key, const Byte* iv, const Byte* in, Byte* out, std::size_t size)
{
    const Des des(key);
    std::uint64_t chain = Load(iv);
    for (std::size_t i = 0; i + Des::kBlockSize <= size; i += Des::kBlockSize) {
        Byte block[Des::kBlockSize];
        Store(Load(in + i) ^ chain, block);
        des.EncryptBlock(block, out + i);
        chain = Load(out + i);
    }
}

void DesCbcDecrypt(const Byte* key, const Byte* iv, const Byte* in, Byte* out, std::size_t size)
{
    const Des des(key);
    std::uint64_t chain = Load(iv);
    for (std::size_t i = 0; i + Des::kBlockSize <= size; i += Des::kBlockSize) {
        const std::uint64_t cipher = Load(in + i); // before out overwrites it (in == out)
        Byte block[Des::kBlockSize];
        des.DecryptBlock(in + i, block);
        Store(Load(block) ^ chain, out + i);
        chain = cipher;
    }
}

} // namespace x2::crypto
