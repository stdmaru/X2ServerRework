// Little-endian reader for LuaJIT dumps (uleb128, uleb128_33), throwing Truncated past the end.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace x2::lua::detail {

struct Truncated
{
};

class Reader
{
public:
    explicit Reader(std::span<const std::uint8_t> data) : data_(data) {}

    std::size_t Pos() const { return pos_; }
    std::size_t Size() const { return data_.size(); }
    void Seek(std::size_t pos) { pos_ = pos; }
    void Skip(std::size_t n)
    {
        if (n > data_.size() - pos_)
            throw Truncated();
        pos_ += n;
    }
    std::uint8_t Peek() const
    {
        if (pos_ >= data_.size())
            throw Truncated();
        return data_[pos_];
    }
    std::uint8_t Byte()
    {
        const std::uint8_t v = Peek();
        ++pos_;
        return v;
    }
    // uleb128, truncated to 32 bits as LuaJIT reads it.
    std::uint32_t Uleb()
    {
        std::uint64_t v = 0;
        unsigned shift = 0;
        for (;;)
        {
            const std::uint8_t c = Byte();
            if (shift < 64)
                v |= static_cast<std::uint64_t>(c & 0x7F) << shift;
            shift += 7;
            if (c < 0x80)
                return static_cast<std::uint32_t>(v);
        }
    }
    // The 33-bit uleb128 of numeric constants (the low bit tags integer/double).
    std::uint32_t Uleb33()
    {
        std::uint8_t c = Byte();
        std::uint64_t v = c >> 1;
        unsigned shift = 6;
        if (c & 0x80)
        {
            v &= 0x3F;
            for (;;)
            {
                c = Byte();
                if (shift < 64)
                    v |= static_cast<std::uint64_t>(c & 0x7F) << shift;
                shift += 7;
                if (c < 0x80)
                    break;
            }
        }
        return static_cast<std::uint32_t>(v);
    }
    std::span<const std::uint8_t> Bytes(std::size_t n)
    {
        const std::size_t at = pos_;
        Skip(n);
        return data_.subspan(at, n);
    }
    std::uint32_t U32()
    {
        const auto b = Bytes(4);
        return b[0] | (b[1] << 8) | (b[2] << 16) | (static_cast<std::uint32_t>(b[3]) << 24);
    }
    std::uint16_t U16()
    {
        const auto b = Bytes(2);
        return static_cast<std::uint16_t>(b[0] | (b[1] << 8));
    }

private:
    std::span<const std::uint8_t> data_;
    std::size_t pos_ = 0;
};

} // namespace x2::lua::detail
