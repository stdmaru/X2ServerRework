#include "x2/kom/codec.h"

#include "codec_internal.h"

#include <array>
#include <zlib.h>

namespace x2::kom {

namespace {

constexpr std::array<std::uint32_t, 256> MakeCrcTable()
{
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t i = 0; i < 256; ++i)
    {
        std::uint32_t c = i;
        for (int k = 0; k < 8; ++k)
            c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
        table[i] = c;
    }
    return table;
}

constexpr std::array<std::uint32_t, 256> kCrc = MakeCrcTable();

// The low bytes of the table are a permutation of 0..255; encryption needs its inverse.
constexpr std::array<std::uint8_t, 256> MakeCrcLowInverse()
{
    std::array<std::uint8_t, 256> inverse{};
    for (std::uint32_t i = 0; i < 256; ++i)
        inverse[kCrc[i] & 0xFF] = static_cast<std::uint8_t>(i);
    return inverse;
}

constexpr std::array<std::uint8_t, 256> kCrcLowInverse = MakeCrcLowInverse();

// Algorithm 2: five dwords, each XOR 0x38d2dfa4, little endian (KTDXCommonFunc.h XORCRCDecrypt).
constexpr std::array<std::uint8_t, 20> MakeKey2()
{
    constexpr std::uint32_t words[5] = {0xcc3d4ffb, 0x593cdeaf, 0xcf4235de, 0xde34bd78, 0x52b34a75};
    std::array<std::uint8_t, 20> key{};
    for (int i = 0; i < 5; ++i)
    {
        const std::uint32_t w = words[i] ^ 0x38d2dfa4u;
        for (int b = 0; b < 4; ++b)
            key[static_cast<std::size_t>(i * 4 + b)] = static_cast<std::uint8_t>(w >> (8 * b));
    }
    return key;
}

constexpr std::array<std::uint8_t, 20> kKey2 = MakeKey2();
constexpr std::array<std::uint8_t, 20> kKey3 = {0xFD, 0xE1, 0x08, 0xEC, 0x0D, 0x09, 0x02, 0xDC, 0xDE, 0x4C,
                                                0x8C, 0x64, 0x16, 0x62, 0x7D, 0xE4, 0x97, 0x16, 0xFF, 0xDC};

struct CipherState
{
    std::uint32_t crc = 0xFFFFFFFFu;
    std::array<std::uint8_t, 20> key{};
};

// One step of the decrypt/encrypt update: `s` is the table index derived from the ciphertext byte.
inline void Advance(std::uint32_t& crc, std::uint8_t s)
{
    crc = (crc >> 8) ^ ((kCrc[s] & 0xFFFFFF00u) | s);
}

CipherState Setup(std::string_view name, std::size_t size, std::uint32_t algorithm)
{
    // Prefix: the lower-cased name as UTF-16 code units (high byte only when non-zero), then the blob length in little
    // endian, stopping at the first zero byte of what is left.
    std::vector<std::uint8_t> prefix;
    for (const char16_t unit : Utf8ToUtf16(AsciiLower(name)))
    {
        prefix.push_back(static_cast<std::uint8_t>(unit & 0xFF));
        if (unit >> 8)
            prefix.push_back(static_cast<std::uint8_t>(unit >> 8));
    }
    auto length = static_cast<std::uint32_t>(size);
    for (int i = 0; i < 4 && length != 0; ++i, length >>= 8)
        prefix.push_back(static_cast<std::uint8_t>(length & 0xFF));

    const auto& base = algorithm == kXorCrc3 ? kKey3 : kKey2;
    CipherState state;
    for (std::size_t i = 0; i < prefix.size(); ++i)
    {
        const std::uint8_t k = base[i % 20];
        if (algorithm == kXorCrc3)
        {
            // XORCRC3 runs the prefix through the decrypt update, discarding the output.
            const auto comp = static_cast<std::uint8_t>((state.crc & 0xFF) ^ k);
            Advance(state.crc, static_cast<std::uint8_t>(prefix[i] ^ 0xFF ^ comp));
        }
        else
        {
            state.crc = (state.crc >> 8) ^ kCrc[(state.crc ^ prefix[i] ^ k) & 0xFF];
        }
    }
    const std::size_t r = prefix.size() % 20;
    for (std::size_t i = 0; i < 20; ++i)
        state.key[i] = base[(i + r) % 20];
    return state;
}

} // namespace

bool IsDecodable(std::uint32_t algorithm)
{
    return algorithm == kZlib || algorithm == kXorCrc || algorithm == kXorCrc3;
}

std::uint32_t Adler32(std::span<const std::uint8_t> data)
{
    uLong adler = ::adler32(0L, Z_NULL, 0);
    const std::uint8_t* p = data.data();
    std::size_t left = data.size();
    while (left > 0)
    {
        const auto chunk = static_cast<uInt>(left > 0x40000000u ? 0x40000000u : left);
        adler = ::adler32(adler, p, chunk);
        p += chunk;
        left -= chunk;
    }
    return static_cast<std::uint32_t>(adler);
}

std::string AsciiLower(std::string_view name)
{
    std::string out(name);
    for (char& c : out)
        if (c >= 'A' && c <= 'Z')
            c = static_cast<char>(c - 'A' + 'a');
    return out;
}

void XorCrcDecrypt(std::span<std::uint8_t> blob, std::string_view name, std::uint32_t algorithm)
{
    CipherState st = Setup(name, blob.size(), algorithm);
    for (std::size_t i = 0; i < blob.size(); ++i)
    {
        const auto comp = static_cast<std::uint8_t>((st.crc & 0xFF) ^ st.key[i % 20]);
        const auto s = static_cast<std::uint8_t>(blob[i] ^ 0xFF ^ comp);
        blob[i] = static_cast<std::uint8_t>((kCrc[s] & 0xFF) ^ comp);
        Advance(st.crc, s);
    }
}

void XorCrcEncrypt(std::span<std::uint8_t> blob, std::string_view name, std::uint32_t algorithm)
{
    CipherState st = Setup(name, blob.size(), algorithm);
    for (std::size_t i = 0; i < blob.size(); ++i)
    {
        const auto comp = static_cast<std::uint8_t>((st.crc & 0xFF) ^ st.key[i % 20]);
        const std::uint8_t s = kCrcLowInverse[static_cast<std::uint8_t>(blob[i] ^ comp)];
        blob[i] = static_cast<std::uint8_t>(s ^ 0xFF ^ comp);
        Advance(st.crc, s);
    }
}

namespace {

// Inflates `in` (zlib stream when windowBits is 15, raw deflate when -15) into exactly `size` bytes.
bool Inflate(std::span<const std::uint8_t> in, int windowBits, std::uint32_t size, std::vector<std::uint8_t>& out,
             std::string& error)
{
    out.assign(size, 0);
    z_stream zs{};
    if (inflateInit2(&zs, windowBits) != Z_OK)
    {
        error = "zlib initialization failed";
        return false;
    }
    zs.next_in = const_cast<Bytef*>(in.data());
    zs.avail_in = static_cast<uInt>(in.size());
    // One spare byte so that data longer than `size` is detected instead of silently cut.
    std::uint8_t spare = 0;
    zs.next_out = size ? out.data() : &spare;
    zs.avail_out = size ? size : 1;
    int rc = inflate(&zs, Z_FINISH);
    if (rc == Z_BUF_ERROR && size != 0 && zs.avail_out == 0)
    {
        zs.next_out = &spare;
        zs.avail_out = 1;
        rc = inflate(&zs, Z_FINISH);
    }
    const uLong produced = zs.total_out;
    inflateEnd(&zs);
    if (rc != Z_STREAM_END)
    {
        error = "corrupted compressed data";
        return false;
    }
    if (produced != size)
    {
        error = "decoded size " + std::to_string(produced) + ", the header says " + std::to_string(size);
        return false;
    }
    return true;
}

} // namespace

bool Decode(std::string_view name, std::uint32_t algorithm, std::span<const std::uint8_t> blob, std::uint32_t size,
            std::vector<std::uint8_t>& out, std::string& error)
{
    switch (algorithm)
    {
    case kZlib:
        return Inflate(blob, 15, size, out, error);
    case kXorCrc:
    {
        std::vector<std::uint8_t> plain(blob.begin(), blob.end());
        XorCrcDecrypt(plain, name, algorithm);
        return Inflate(plain, 15, size, out, error);
    }
    case kXorCrc3:
    {
        // The client inflates from byte 6: 4 prefix bytes of unknown meaning, then the 2-byte zlib header.
        if (blob.size() < 6)
        {
            error = "encrypted blob too short";
            return false;
        }
        std::vector<std::uint8_t> plain(blob.begin(), blob.end());
        XorCrcDecrypt(plain, name, algorithm);
        return Inflate(std::span<const std::uint8_t>(plain).subspan(6), -15, size, out, error);
    }
    default:
        error = "algorithm " + std::to_string(algorithm) + " is not supported";
        return false;
    }
}

} // namespace x2::kom
