#include "x2/lua/bytecode.h"

#include "reader.h"

#include <array>

namespace x2::lua {

namespace {

// 0x82 opcode -> 0x81 (stock LuaJIT 2.0) opcode, from kom.py (KlTranslate.cs); the other opcodes are unchanged.
constexpr std::array<std::uint8_t, 256> MakeOp82To81()
{
    std::array<std::uint8_t, 256> t{};
    for (int i = 0; i < 256; ++i)
        t[i] = static_cast<std::uint8_t>(i);
    constexpr int kRuns[][3] = {{0x0C, 0x10, 4}, {0x10, 0x0C, 4},  {0x14, 0x25, 6}, {0x1A, 0x14, 17},
                                {0x2B, 0x3D, 8}, {0x33, 0x2B, 6}, {0x39, 0x31, 12}};
    for (const auto& run : kRuns)
        for (int i = 0; i < run[2]; ++i)
            t[run[0] + i] = static_cast<std::uint8_t>(run[1] + i);
    return t;
}

constexpr std::array<std::uint8_t, 256> Invert(const std::array<std::uint8_t, 256>& t)
{
    std::array<std::uint8_t, 256> inv{};
    for (int i = 0; i < 256; ++i)
        inv[t[i]] = static_cast<std::uint8_t>(i);
    return inv;
}

constexpr auto kOp82To81 = MakeOp82To81();
constexpr auto kOp81To82 = Invert(kOp82To81);

bool Remap(std::span<const std::uint8_t> data, const std::array<std::uint8_t, 256>& table, std::uint8_t version,
           std::vector<std::uint8_t>& out, std::string& error)
{
    out.assign(data.begin(), data.end());
    out[3] = version;
    detail::Reader r(data);
    try
    {
        r.Skip(4);
        if ((r.Uleb() & 2) == 0)
        {
            error = "only stripped dumps can be remapped";
            return false;
        }
        for (;;)
        {
            const std::uint32_t length = r.Uleb();
            if (length == 0)
                break;
            const std::size_t end = r.Pos() + length;
            r.Skip(4);
            r.Uleb();
            r.Uleb();
            const std::uint32_t count = r.Uleb();
            const std::size_t code = r.Pos();
            r.Skip(4 * static_cast<std::size_t>(count));
            for (std::size_t i = 0; i < count; ++i)
                out[code + 4 * i] = table[out[code + 4 * i]];
            if (end > data.size())
                throw detail::Truncated();
            r.Seek(end);
        }
    }
    catch (const detail::Truncated&)
    {
        error = "truncated bytecode";
        return false;
    }
    return true;
}

} // namespace

bool IsBytecode(std::span<const std::uint8_t> data)
{
    if (data.size() < 4 || data[0] != 0x1B)
        return false;
    if (data[1] == 'K' && data[2] == 'L')
        return data[3] == 0x81 || data[3] == 0x82;
    return data[1] == 'L' && data[2] == 'J' && data[3] == 0x01;
}

bool ToLayout(std::span<const std::uint8_t> data, std::uint8_t version, std::vector<std::uint8_t>& out,
              std::string& error)
{
    const bool kl = data.size() >= 4 && data[0] == 0x1B && data[1] == 'K' && data[2] == 'L' &&
                    (data[3] == 0x81 || data[3] == 0x82);
    if (!kl || data[3] == version)
    {
        out.assign(data.begin(), data.end());
        return true;
    }
    if (version == 0x81)
        return Remap(data, kOp82To81, 0x81, out, error);
    if (version == 0x82)
        return Remap(data, kOp81To82, 0x82, out, error);
    error = "unknown bytecode layout";
    return false;
}

} // namespace x2::lua
