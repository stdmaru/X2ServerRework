// Texel conversions for the image loaders and the device's uploads: D3D9 formats to and from 8-bit BGRA, BC1-3 decoding
// (software, for resizing and re-encoding) and encoding (stb_dxt), and D3DX's resize filters.
// docs/porting/client-d3d9.md#image-files

#include "Formats.h"
#include "Image.h"

#include <stb_dxt.h>

#include <cmath>
#include <cstring>

namespace x2::d3d9::image {
namespace {

std::uint8_t Expand(unsigned value, unsigned bits)
{
    return static_cast<std::uint8_t>((value * 255 + ((1u << bits) - 1) / 2) / ((1u << bits) - 1));
}

unsigned Quantize(unsigned value, unsigned bits)
{
    return (value * ((1u << bits) - 1) + 127) / 255;
}

// --- BC1-3 decoding -------------------------------------------------------------------------------------------------

// A 4x4 color block into 16 BGRA texels. `fourColors`: BC2/BC3 always use four colors.
void DecodeColorBlock(const std::uint8_t* block, bool fourColors, std::uint8_t (*out)[4])
{
    const unsigned c0 = block[0] | (block[1] << 8), c1 = block[2] | (block[3] << 8);
    std::uint8_t palette[4][4];
    auto set = [&](int i, unsigned c) {
        palette[i][0] = Expand(c & 0x1F, 5);
        palette[i][1] = Expand((c >> 5) & 0x3F, 6);
        palette[i][2] = Expand(c >> 11, 5);
        palette[i][3] = 255;
    };
    set(0, c0);
    set(1, c1);
    for (int ch = 0; ch < 3; ++ch) {
        const unsigned a = palette[0][ch], b = palette[1][ch];
        if (fourColors || c0 > c1) {
            palette[2][ch] = static_cast<std::uint8_t>((2 * a + b + 1) / 3);
            palette[3][ch] = static_cast<std::uint8_t>((a + 2 * b + 1) / 3);
        } else {
            palette[2][ch] = static_cast<std::uint8_t>((a + b) / 2);
            palette[3][ch] = 0;
        }
    }
    palette[2][3] = 255;
    palette[3][3] = fourColors || c0 > c1 ? 255 : 0;
    const std::uint32_t indices = block[4] | (block[5] << 8) | (block[6] << 16) |
                                  (static_cast<std::uint32_t>(block[7]) << 24);
    for (int i = 0; i < 16; ++i) {
        std::memcpy(out[i], palette[(indices >> (2 * i)) & 3], 4);
    }
}

void DecodeExplicitAlpha(const std::uint8_t* block, std::uint8_t (*out)[4])
{
    for (int i = 0; i < 16; ++i) {
        out[i][3] = static_cast<std::uint8_t>(((block[i / 2] >> (4 * (i % 2))) & 0xF) * 17);
    }
}

void DecodeInterpolatedAlpha(const std::uint8_t* block, std::uint8_t (*out)[4])
{
    const unsigned a0 = block[0], a1 = block[1];
    unsigned palette[8] = {a0, a1};
    if (a0 > a1) {
        for (unsigned i = 1; i < 7; ++i) {
            palette[i + 1] = ((7 - i) * a0 + i * a1 + 3) / 7;
        }
    } else {
        for (unsigned i = 1; i < 5; ++i) {
            palette[i + 1] = ((5 - i) * a0 + i * a1 + 2) / 5;
        }
        palette[6] = 0;
        palette[7] = 255;
    }
    std::uint64_t indices = 0;
    for (int i = 0; i < 6; ++i) {
        indices |= static_cast<std::uint64_t>(block[2 + i]) << (8 * i);
    }
    for (int i = 0; i < 16; ++i) {
        out[i][3] = static_cast<std::uint8_t>(palette[(indices >> (3 * i)) & 7]);
    }
}

void DecodeBlocks(D3DFORMAT format, const std::uint8_t* in, UINT width, UINT height, std::uint8_t* out)
{
    const UINT blocksX = std::max(1u, (width + 3) / 4), blocksY = std::max(1u, (height + 3) / 4);
    const UINT blockBytes = Info(format).blockBytes;
    std::uint8_t texels[16][4];
    for (UINT by = 0; by < blocksY; ++by) {
        for (UINT bx = 0; bx < blocksX; ++bx) {
            const std::uint8_t* block = in + (static_cast<std::size_t>(by) * blocksX + bx) * blockBytes;
            switch (format) {
            case D3DFMT_DXT1: DecodeColorBlock(block, false, texels); break;
            case D3DFMT_DXT2:
            case D3DFMT_DXT3:
                DecodeColorBlock(block + 8, true, texels);
                DecodeExplicitAlpha(block, texels);
                break;
            default:
                DecodeColorBlock(block + 8, true, texels);
                DecodeInterpolatedAlpha(block, texels);
                break;
            }
            for (UINT y = 0; y < 4 && by * 4 + y < height; ++y) {
                for (UINT x = 0; x < 4 && bx * 4 + x < width; ++x) {
                    std::memcpy(out + ((static_cast<std::size_t>(by) * 4 + y) * width + bx * 4 + x) * 4,
                                texels[y * 4 + x], 4);
                }
            }
        }
    }
}

// --- BC1-3 encoding -------------------------------------------------------------------------------------------------

void EncodeBlocks(D3DFORMAT format, const std::uint8_t* bgra, UINT width, UINT height, std::byte* out)
{
    const UINT blocksX = std::max(1u, (width + 3) / 4), blocksY = std::max(1u, (height + 3) / 4);
    const UINT blockBytes = Info(format).blockBytes;
    std::uint8_t rgba[16 * 4];
    for (UINT by = 0; by < blocksY; ++by) {
        for (UINT bx = 0; bx < blocksX; ++bx) {
            // Texels past the edge repeat the last row and column.
            for (UINT y = 0; y < 4; ++y) {
                for (UINT x = 0; x < 4; ++x) {
                    const UINT sx = std::min(bx * 4 + x, width - 1), sy = std::min(by * 4 + y, height - 1);
                    const std::uint8_t* t = bgra + (static_cast<std::size_t>(sy) * width + sx) * 4;
                    std::uint8_t* d = rgba + (y * 4 + x) * 4;
                    d[0] = t[2];
                    d[1] = t[1];
                    d[2] = t[0];
                    d[3] = t[3];
                }
            }
            auto* block = reinterpret_cast<unsigned char*>(out + (static_cast<std::size_t>(by) * blocksX + bx) * blockBytes);
            switch (format) {
            case D3DFMT_DXT1: stb_compress_dxt_block(block, rgba, 0, STB_DXT_NORMAL); break;
            case D3DFMT_DXT2:
            case D3DFMT_DXT3:
                for (int i = 0; i < 8; ++i) {
                    block[i] = static_cast<unsigned char>(Quantize(rgba[(2 * i) * 4 + 3], 4) |
                                                          (Quantize(rgba[(2 * i + 1) * 4 + 3], 4) << 4));
                }
                stb_compress_dxt_block(block + 8, rgba, 0, STB_DXT_NORMAL);
                break;
            default: stb_compress_dxt_block(block, rgba, 1, STB_DXT_NORMAL); break;
            }
        }
    }
}

// --- Resize ---------------------------------------------------------------------------------------------------------

void Bilinear(const std::uint8_t* in, UINT width, UINT height, UINT newWidth, UINT newHeight, std::uint8_t* out)
{
    const float sx = static_cast<float>(width) / static_cast<float>(newWidth);
    const float sy = static_cast<float>(height) / static_cast<float>(newHeight);
    for (UINT y = 0; y < newHeight; ++y) {
        const float fy = std::clamp((static_cast<float>(y) + 0.5f) * sy - 0.5f, 0.0f, static_cast<float>(height - 1));
        const UINT y0 = static_cast<UINT>(fy), y1 = std::min(y0 + 1, height - 1);
        const float wy = fy - static_cast<float>(y0);
        for (UINT x = 0; x < newWidth; ++x) {
            const float fx = std::clamp((static_cast<float>(x) + 0.5f) * sx - 0.5f, 0.0f,
                                        static_cast<float>(width - 1));
            const UINT x0 = static_cast<UINT>(fx), x1 = std::min(x0 + 1, width - 1);
            const float wx = fx - static_cast<float>(x0);
            for (int c = 0; c < 4; ++c) {
                auto at = [&](UINT px, UINT py) {
                    return static_cast<float>(in[(static_cast<std::size_t>(py) * width + px) * 4 + c]);
                };
                const float top = at(x0, y0) + (at(x1, y0) - at(x0, y0)) * wx;
                const float bottom = at(x0, y1) + (at(x1, y1) - at(x0, y1)) * wx;
                out[(static_cast<std::size_t>(y) * newWidth + x) * 4 + c] =
                    static_cast<std::uint8_t>(std::lround(top + (bottom - top) * wy));
            }
        }
    }
}

void Box(const std::uint8_t* in, UINT width, UINT height, UINT newWidth, UINT newHeight, std::uint8_t* out)
{
    for (UINT y = 0; y < newHeight; ++y) {
        const UINT y0 = static_cast<UINT>(static_cast<std::uint64_t>(y) * height / newHeight);
        const UINT y1 = std::max(y0 + 1, static_cast<UINT>(static_cast<std::uint64_t>(y + 1) * height / newHeight));
        for (UINT x = 0; x < newWidth; ++x) {
            const UINT x0 = static_cast<UINT>(static_cast<std::uint64_t>(x) * width / newWidth);
            const UINT x1 = std::max(x0 + 1, static_cast<UINT>(static_cast<std::uint64_t>(x + 1) * width / newWidth));
            unsigned sum[4] = {};
            for (UINT py = y0; py < y1; ++py) {
                for (UINT px = x0; px < x1; ++px) {
                    for (int c = 0; c < 4; ++c) {
                        sum[c] += in[(static_cast<std::size_t>(py) * width + px) * 4 + c];
                    }
                }
            }
            const unsigned count = (y1 - y0) * (x1 - x0);
            for (int c = 0; c < 4; ++c) {
                out[(static_cast<std::size_t>(y) * newWidth + x) * 4 + c] =
                    static_cast<std::uint8_t>((sum[c] + count / 2) / count);
            }
        }
    }
}

}  // namespace

void ToBgra8(D3DFORMAT format, std::span<const std::byte> level, UINT width, UINT height, std::uint8_t* out)
{
    const auto* in = reinterpret_cast<const std::uint8_t*>(level.data());
    if (Info(format).compressed) {
        DecodeBlocks(format, in, width, height, out);
        return;
    }
    const std::size_t texels = static_cast<std::size_t>(width) * height;
    auto word = [&](std::size_t i) { return static_cast<unsigned>(in[i * 2] | (in[i * 2 + 1] << 8)); };
    for (std::size_t i = 0; i < texels; ++i) {
        std::uint8_t b = 0, g = 0, r = 0, a = 255;
        switch (format) {
        case D3DFMT_A8R8G8B8: b = in[i * 4]; g = in[i * 4 + 1]; r = in[i * 4 + 2]; a = in[i * 4 + 3]; break;
        case D3DFMT_X8R8G8B8: b = in[i * 4]; g = in[i * 4 + 1]; r = in[i * 4 + 2]; break;
        case D3DFMT_R8G8B8: b = in[i * 3]; g = in[i * 3 + 1]; r = in[i * 3 + 2]; break;
        case D3DFMT_R5G6B5: {
            const unsigned v = word(i);
            r = Expand(v >> 11, 5); g = Expand((v >> 5) & 0x3F, 6); b = Expand(v & 0x1F, 5);
            break;
        }
        case D3DFMT_A1R5G5B5:
        case D3DFMT_X1R5G5B5: {
            const unsigned v = word(i);
            r = Expand((v >> 10) & 0x1F, 5); g = Expand((v >> 5) & 0x1F, 5); b = Expand(v & 0x1F, 5);
            a = format == D3DFMT_A1R5G5B5 && !(v & 0x8000) ? 0 : 255;
            break;
        }
        case D3DFMT_A4R4G4B4:
        case D3DFMT_X4R4G4B4: {
            const unsigned v = word(i);
            r = Expand((v >> 8) & 0xF, 4); g = Expand((v >> 4) & 0xF, 4); b = Expand(v & 0xF, 4);
            a = format == D3DFMT_A4R4G4B4 ? Expand(v >> 12, 4) : 255;
            break;
        }
        case D3DFMT_A8: a = in[i]; break;
        case D3DFMT_L8: r = g = b = in[i]; break;
        case D3DFMT_A8L8: r = g = b = in[i * 2]; a = in[i * 2 + 1]; break;
        default: break;
        }
        out[i * 4] = b;
        out[i * 4 + 1] = g;
        out[i * 4 + 2] = r;
        out[i * 4 + 3] = a;
    }
}

std::vector<std::byte> FromBgra8(D3DFORMAT format, const std::uint8_t* bgra, UINT width, UINT height)
{
    std::vector<std::byte> out(LevelBytes(format, width, height));
    if (Info(format).compressed) {
        EncodeBlocks(format, bgra, width, height, out.data());
        return out;
    }
    auto* o = reinterpret_cast<std::uint8_t*>(out.data());
    auto word = [&](std::size_t i, unsigned v) {
        o[i * 2] = static_cast<std::uint8_t>(v & 0xFF);
        o[i * 2 + 1] = static_cast<std::uint8_t>(v >> 8);
    };
    const std::size_t texels = static_cast<std::size_t>(width) * height;
    for (std::size_t i = 0; i < texels; ++i) {
        const unsigned b = bgra[i * 4], g = bgra[i * 4 + 1], r = bgra[i * 4 + 2], a = bgra[i * 4 + 3];
        // D3DX's luminance weights.
        const auto luminance = static_cast<std::uint8_t>(std::lround(0.2125 * r + 0.7154 * g + 0.0721 * b));
        switch (format) {
        case D3DFMT_A8R8G8B8: std::memcpy(o + i * 4, bgra + i * 4, 4); break;
        case D3DFMT_X8R8G8B8:
            std::memcpy(o + i * 4, bgra + i * 4, 3);
            o[i * 4 + 3] = 255;
            break;
        case D3DFMT_R8G8B8: std::memcpy(o + i * 3, bgra + i * 4, 3); break;
        case D3DFMT_R5G6B5: word(i, (Quantize(r, 5) << 11) | (Quantize(g, 6) << 5) | Quantize(b, 5)); break;
        case D3DFMT_A1R5G5B5:
        case D3DFMT_X1R5G5B5:
            word(i, (format == D3DFMT_X1R5G5B5 || a >= 128 ? 0x8000u : 0u) | (Quantize(r, 5) << 10) |
                        (Quantize(g, 5) << 5) | Quantize(b, 5));
            break;
        case D3DFMT_A4R4G4B4:
        case D3DFMT_X4R4G4B4:
            word(i, ((format == D3DFMT_X4R4G4B4 ? 15u : Quantize(a, 4)) << 12) | (Quantize(r, 4) << 8) |
                        (Quantize(g, 4) << 4) | Quantize(b, 4));
            break;
        case D3DFMT_A8: o[i] = static_cast<std::uint8_t>(a); break;
        case D3DFMT_L8: o[i] = luminance; break;
        case D3DFMT_A8L8:
            o[i * 2] = luminance;
            o[i * 2 + 1] = static_cast<std::uint8_t>(a);
            break;
        default: break;
        }
    }
    return out;
}

Filter FilterFromD3DX(DWORD filter)
{
    switch (filter == static_cast<DWORD>(D3DX_DEFAULT) ? D3DX_FILTER_TRIANGLE : filter & 0xFF) {
    case D3DX_FILTER_NONE: return Filter::None;
    case D3DX_FILTER_POINT: return Filter::Point;
    case D3DX_FILTER_LINEAR: return Filter::Linear;
    default: return Filter::Box;    // TRIANGLE, BOX
    }
}

std::vector<std::uint8_t> Resize(const std::uint8_t* bgra, UINT width, UINT height, UINT newWidth, UINT newHeight,
                                 Filter filter)
{
    std::vector<std::uint8_t> out(static_cast<std::size_t>(newWidth) * newHeight * 4);
    if (width == newWidth && height == newHeight) {
        std::memcpy(out.data(), bgra, out.size());
        return out;
    }
    switch (filter) {
    case Filter::None:
        for (UINT y = 0; y < std::min(height, newHeight); ++y) {
            std::memcpy(out.data() + static_cast<std::size_t>(y) * newWidth * 4,
                        bgra + static_cast<std::size_t>(y) * width * 4, static_cast<std::size_t>(std::min(width, newWidth)) * 4);
        }
        break;
    case Filter::Point:
        for (UINT y = 0; y < newHeight; ++y) {
            const UINT sy = static_cast<UINT>(static_cast<std::uint64_t>(y) * height / newHeight);
            for (UINT x = 0; x < newWidth; ++x) {
                const UINT sx = static_cast<UINT>(static_cast<std::uint64_t>(x) * width / newWidth);
                std::memcpy(out.data() + (static_cast<std::size_t>(y) * newWidth + x) * 4,
                            bgra + (static_cast<std::size_t>(sy) * width + sx) * 4, 4);
            }
        }
        break;
    case Filter::Linear: Bilinear(bgra, width, height, newWidth, newHeight, out.data()); break;
    case Filter::Box:
        if (newWidth <= width && newHeight <= height) {
            Box(bgra, width, height, newWidth, newHeight, out.data());
        } else {
            Bilinear(bgra, width, height, newWidth, newHeight, out.data());
        }
        break;
    }
    return out;
}

}  // namespace x2::d3d9::image
