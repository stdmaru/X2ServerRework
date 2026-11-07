// D3DX's image files on KTDXLIB's device (docs/porting/client-d3d9.md#image-files): .dds/.tga/.png/.bmp built here,
// texels converted, resized and BC-encoded, the detail-level loads of CKTDXDeviceTexture, and, with
// X2_MASSFILE_CLIENT_DATA set to the client's data folder, every .dds/.tga of the client's archives.

#include "D3DXTypes.h"
#include "Device.h"
#include "Formats.h"
#include "Image.h"

#include "x2/kom/mount.h"
#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace {

namespace image = x2::d3d9::image;
using Bytes = std::vector<std::byte>;

std::unique_ptr<x2::render::Device> NullRender()
{
    std::string error;
    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::Null;
    auto device = x2::render::Device::Create(options, error);
    REQUIRE_MESSAGE(device, error);
    return device;
}

void Put32(Bytes& out, std::uint32_t v)
{
    for (int i = 0; i < 4; ++i) {
        out.push_back(static_cast<std::byte>((v >> (8 * i)) & 0xFF));
    }
}

void Put16(Bytes& out, unsigned v)
{
    out.push_back(static_cast<std::byte>(v & 0xFF));
    out.push_back(static_cast<std::byte>((v >> 8) & 0xFF));
}

void PutBytes(Bytes& out, std::initializer_list<unsigned> bytes)
{
    for (unsigned b : bytes) {
        out.push_back(static_cast<std::byte>(b));
    }
}

struct PixelFormat {
    std::uint32_t flags = 0, fourCC = 0, bits = 0, r = 0, g = 0, b = 0, a = 0;
};

constexpr PixelFormat kDxt1{0x4, D3DFMT_DXT1};
constexpr PixelFormat kDxt3{0x4, D3DFMT_DXT3};
constexpr PixelFormat kArgb{0x41, 0, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000};
constexpr PixelFormat kXrgb{0x40, 0, 32, 0xFF0000, 0xFF00, 0xFF, 0};
constexpr PixelFormat kAbgr{0x41, 0, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000};
constexpr PixelFormat kRgb24{0x40, 0, 24, 0xFF0000, 0xFF00, 0xFF, 0};
constexpr PixelFormat kRgb565{0x40, 0, 16, 0xF800, 0x7E0, 0x1F, 0};
constexpr PixelFormat kArgb1555{0x41, 0, 16, 0x7C00, 0x3E0, 0x1F, 0x8000};
constexpr PixelFormat kXrgb1555{0x40, 0, 16, 0x7C00, 0x3E0, 0x1F, 0};
constexpr PixelFormat kArgb4444{0x41, 0, 16, 0xF00, 0xF0, 0xF, 0xF000};
constexpr PixelFormat kL8{0x20000, 0, 8, 0xFF, 0, 0, 0};
constexpr PixelFormat kA8L8{0x20001, 0, 16, 0xFF, 0, 0, 0xFF00};
constexpr PixelFormat kA8{0x2, 0, 8, 0, 0, 0, 0xFF};

// A DDS file; mips 0 leaves the mip count out, depth 0 makes a plain texture. caps2 0x200 makes a cube map.
Bytes Dds(const PixelFormat& pf, UINT width, UINT height, UINT mips, const Bytes& data, UINT depth = 0,
          std::uint32_t caps2 = 0)
{
    Bytes out;
    PutBytes(out, {'D', 'D', 'S', ' '});
    Put32(out, 124);
    Put32(out, 0x1007 | (mips ? 0x20000u : 0u) | (depth ? 0x800000u : 0u));
    Put32(out, height);
    Put32(out, width);
    Put32(out, 0);                  // pitch or linear size (ignored)
    Put32(out, depth);
    Put32(out, mips);
    for (int i = 0; i < 11; ++i) {
        Put32(out, 0);
    }
    Put32(out, 32);
    Put32(out, pf.flags);
    Put32(out, pf.fourCC);
    Put32(out, pf.bits);
    Put32(out, pf.r);
    Put32(out, pf.g);
    Put32(out, pf.b);
    Put32(out, pf.a);
    Put32(out, 0x1000);
    Put32(out, depth ? caps2 | 0x200000u : caps2);
    for (int i = 0; i < 3; ++i) {
        Put32(out, 0);
    }
    REQUIRE(out.size() == 128);
    out.insert(out.end(), data.begin(), data.end());
    return out;
}

Bytes Tga(unsigned type, unsigned bits, unsigned descriptor, unsigned width, unsigned height, const Bytes& data,
          const Bytes& colorMap = {})
{
    Bytes out;
    PutBytes(out, {0, colorMap.empty() ? 0u : 1u, type});
    Put16(out, 0);
    Put16(out, colorMap.empty() ? 0 : static_cast<unsigned>(colorMap.size() / 3));
    PutBytes(out, {colorMap.empty() ? 0u : 24u});
    Put16(out, 0);
    Put16(out, 0);
    Put16(out, width);
    Put16(out, height);
    PutBytes(out, {bits, descriptor});
    out.insert(out.end(), colorMap.begin(), colorMap.end());
    out.insert(out.end(), data.begin(), data.end());
    return out;
}

Bytes Of(std::initializer_list<unsigned> bytes)
{
    Bytes out;
    PutBytes(out, bytes);
    return out;
}

Bytes Sequence(std::size_t size, unsigned seed = 0)
{
    Bytes out(size);
    for (std::size_t i = 0; i < size; ++i) {
        out[i] = static_cast<std::byte>((i * 7 + seed) & 0xFF);
    }
    return out;
}

D3DXIMAGE_INFO InfoOf(const Bytes& file)
{
    D3DXIMAGE_INFO info{};
    CHECK(D3DXGetImageInfoFromFileInMemory(file.data(), static_cast<UINT>(file.size()), &info) == D3D_OK);
    return info;
}

bool Rejected(const Bytes& file)
{
    D3DXIMAGE_INFO info{};
    return D3DXGetImageInfoFromFileInMemory(file.data(), static_cast<UINT>(file.size()), &info) ==
               D3DXERR_INVALIDDATA &&
           !image::Decode(file);
}

// The bytes of a texture level (its CPU copy).
Bytes LevelBytes(x2::d3d9::Texture& texture, UINT level)
{
    D3DSURFACE_DESC desc{};
    D3DLOCKED_RECT locked{};
    REQUIRE(texture.GetLevelDesc(level, &desc));
    REQUIRE(texture.LockRect(level, &locked, nullptr, D3DLOCK_READONLY));
    const auto* bits = static_cast<const std::byte*>(locked.pBits);
    Bytes out(bits, bits + x2::d3d9::LevelBytes(desc.Format, desc.Width, desc.Height));
    texture.UnlockRect(level);
    return out;
}

std::vector<std::uint8_t> Bgra(x2::d3d9::Texture& texture, UINT level)
{
    D3DSURFACE_DESC desc{};
    REQUIRE(texture.GetLevelDesc(level, &desc));
    std::vector<std::uint8_t> out(static_cast<std::size_t>(desc.Width) * desc.Height * 4);
    image::ToBgra8(desc.Format, LevelBytes(texture, level), desc.Width, desc.Height, out.data());
    return out;
}

std::unique_ptr<x2::d3d9::Texture> Load(x2::d3d9::Device& device, const Bytes& file, UINT width = 0, UINT height = 0,
                                        UINT mips = 1, D3DFORMAT format = D3DFMT_UNKNOWN,
                                        DWORD filter = D3DX_FILTER_NONE, D3DCOLOR colorKey = 0)
{
    x2::d3d9::Texture* texture = nullptr;
    D3DXIMAGE_INFO info{};
    const HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(&device, file.data(), static_cast<UINT>(file.size()), width,
                                                           height, mips, 0, format, D3DPOOL_MANAGED, filter,
                                                           D3DX_FILTER_NONE, colorKey, &info, nullptr, &texture);
    CHECK(hr == D3D_OK);
    REQUIRE(texture != nullptr);
    return std::unique_ptr<x2::d3d9::Texture>(texture);
}

// One BGRA texel.
std::vector<std::uint8_t> Texel(const std::vector<std::uint8_t>& bgra, UINT width, UINT x, UINT y)
{
    const std::size_t at = (static_cast<std::size_t>(y) * width + x) * 4;
    return {bgra[at], bgra[at + 1], bgra[at + 2], bgra[at + 3]};
}

using V = std::vector<std::uint8_t>;

}  // namespace

TEST_CASE("d3dx: image info of .dds files")
{
    D3DXIMAGE_INFO info = InfoOf(Dds(kDxt1, 16, 8, 5, Sequence(64 + 16 + 8 + 8 + 8)));
    CHECK(info.Width == 16);
    CHECK(info.Height == 8);
    CHECK(info.Depth == 1);
    CHECK(info.MipLevels == 5);
    CHECK(info.Format == D3DFMT_DXT1);
    CHECK(info.ResourceType == D3DRTYPE_TEXTURE);
    CHECK(info.ImageFileFormat == D3DXIFF_DDS);

    CHECK(InfoOf(Dds(kDxt1, 4, 4, 0, Sequence(8))).MipLevels == 1);       // no mip count
    CHECK(InfoOf(Dds(kDxt1, 4, 4, 9, Sequence(8))).MipLevels == 3);       // bounded by the chain
    CHECK(InfoOf(Dds(kDxt3, 4, 4, 1, Sequence(16))).Format == D3DFMT_DXT3);
    const std::pair<PixelFormat, D3DFORMAT> formats[] = {
        {kArgb, D3DFMT_A8R8G8B8},       {kXrgb, D3DFMT_X8R8G8B8},       {kAbgr, D3DFMT_A8B8G8R8},
        {kRgb24, D3DFMT_R8G8B8},        {kRgb565, D3DFMT_R5G6B5},       {kArgb1555, D3DFMT_A1R5G5B5},
        {kXrgb1555, D3DFMT_X1R5G5B5},   {kArgb4444, D3DFMT_A4R4G4B4},   {kL8, D3DFMT_L8},
        {kA8L8, D3DFMT_A8L8},           {kA8, D3DFMT_A8},
        {PixelFormat{0x4, D3DFMT_A8R8G8B8}, D3DFMT_A8R8G8B8},   // a D3DFORMAT number as the FourCC
    };
    for (const auto& [pf, format] : formats) {
        CAPTURE(format);
        CHECK(InfoOf(Dds(pf, 2, 2, 1, Sequence(16))).Format == format);
    }
    // The alpha mask without DDPF_ALPHAPIXELS does not count.
    CHECK(InfoOf(Dds(PixelFormat{0x40, 0, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000}, 2, 2, 1, Sequence(16))).Format ==
          D3DFMT_X8R8G8B8);

    info = InfoOf(Dds(kDxt1, 4, 4, 1, Sequence(8 * 3), 3));
    CHECK(info.ResourceType == D3DRTYPE_VOLUMETEXTURE);
    CHECK(info.Depth == 3);
    CHECK(InfoOf(Dds(kDxt1, 4, 4, 1, Sequence(8 * 6), 0, 0xFE00)).ResourceType == D3DRTYPE_CUBETEXTURE);

    CHECK(Rejected(Dds(PixelFormat{0x4, 63}, 4, 4, 1, Sequence(64))));     // Q8W8V8U8: a bump map
    CHECK(Rejected(Dds(PixelFormat{0x4, 0x30315844}, 4, 4, 1, Sequence(64))));  // "DX10"
    CHECK(Rejected(Dds(PixelFormat{0x40, 0, 32, 0xFF, 0xFF, 0xFF, 0}, 2, 2, 1, Sequence(16))));
    CHECK(Rejected(Dds(kDxt1, 0, 4, 1, {})));
    CHECK(InfoOf(Dds(kDxt1, 4, 4, 1, {})).Width == 4);   // header only: the info, but no level to decode
    CHECK_FALSE(image::Decode(Dds(kDxt1, 4, 4, 1, {})));
    Bytes header = Dds(kDxt1, 4, 4, 1, Sequence(8));
    header.resize(100);
    CHECK(Rejected(header));
    CHECK(Rejected(Of({'D', 'D', 'S', ' '})));

    CHECK(D3DXGetImageInfoFromFileInMemory(nullptr, 10, &info) == D3DERR_INVALIDCALL);
    CHECK(D3DXGetImageInfoFromFileInMemory("x", 0, &info) == D3DERR_INVALIDCALL);
    const Bytes dxt = Dds(kDxt1, 4, 4, 1, Sequence(8));
    CHECK(D3DXGetImageInfoFromFileInMemory(dxt.data(), static_cast<UINT>(dxt.size()), nullptr) == D3D_OK);
}

TEST_CASE("d3dx: image info of .tga, .png, .jpg and .bmp files")
{
    const std::pair<Bytes, D3DFORMAT> tgas[] = {
        {Tga(2, 24, 0, 1, 1, Sequence(3)), D3DFMT_R8G8B8},
        {Tga(2, 32, 8, 1, 1, Sequence(4)), D3DFMT_A8R8G8B8},
        {Tga(2, 32, 0, 1, 1, Sequence(4)), D3DFMT_A8R8G8B8},  // no attribute bits: still A8R8G8B8 (assumed)
        {Tga(2, 16, 1, 1, 1, Sequence(2)), D3DFMT_A1R5G5B5},
        {Tga(2, 16, 0, 1, 1, Sequence(2)), D3DFMT_X1R5G5B5},
        {Tga(10, 24, 0, 1, 1, Of({0x80, 1, 2, 3})), D3DFMT_R8G8B8},
        {Tga(3, 8, 0, 1, 1, Sequence(1)), D3DFMT_L8},
        {Tga(1, 8, 0, 1, 1, Of({0}), Sequence(3)), D3DFMT_P8},
    };
    for (const auto& [file, format] : tgas) {
        CAPTURE(format);
        const D3DXIMAGE_INFO info = InfoOf(file);
        CHECK(info.Format == format);
        CHECK(info.ImageFileFormat == D3DXIFF_TGA);
        CHECK(info.Width == 1);
        CHECK(info.MipLevels == 1);
    }
    CHECK(Rejected(Of({'B', 'M', 'F', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})));  // "BM", no BMP header
    CHECK(Rejected(Tga(2, 8, 0, 1, 1, Sequence(1))));
    CHECK(Rejected(Tga(2, 24, 0, 0, 1, {})));

    const Bytes png = Of({0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44,
                          0x52, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00, 0x00, 0xF4,
                          0x22, 0x7F, 0x8A, 0x00, 0x00, 0x00, 0x11, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0x63, 0xF8,
                          0xCF, 0xC0, 0xF0, 0x9F, 0xE1, 0x3F, 0x43, 0x03, 0x00, 0x10, 0x79, 0x03, 0x7E, 0x21, 0xC0,
                          0xFD, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82});
    D3DXIMAGE_INFO info = InfoOf(png);
    CHECK(info.ImageFileFormat == D3DXIFF_PNG);
    CHECK(info.Format == D3DFMT_A8R8G8B8);
    CHECK(info.Width == 2);
    const std::optional<image::Image> decoded = image::Decode(png);
    REQUIRE(decoded);
    CHECK(decoded->format == D3DFMT_A8R8G8B8);
    CHECK(decoded->levels[0].bytes == Of({0, 0, 255, 255, 0, 255, 0, 128}));     // red, then half-transparent green

    // A 2x2 24-bit BMP (bottom-up rows padded to 4 bytes).
    Bytes bmp = Of({'B', 'M'});
    Put32(bmp, 54 + 16);
    Put32(bmp, 0);
    Put32(bmp, 54);
    Put32(bmp, 40);
    Put32(bmp, 2);
    Put32(bmp, 2);
    Put16(bmp, 1);
    Put16(bmp, 24);
    for (int i = 0; i < 6; ++i) {
        Put32(bmp, 0);
    }
    PutBytes(bmp, {1, 2, 3, 4, 5, 6, 0, 0, 7, 8, 9, 10, 11, 12, 0, 0});
    info = InfoOf(bmp);
    CHECK(info.ImageFileFormat == D3DXIFF_BMP);
    CHECK(info.Format == D3DFMT_R8G8B8);
    const std::optional<image::Image> fromBmp = image::Decode(bmp);
    REQUIRE(fromBmp);
    CHECK(fromBmp->format == D3DFMT_X8R8G8B8);
    CHECK(fromBmp->levels[0].bytes == Of({7, 8, 9, 255, 10, 11, 12, 255, 1, 2, 3, 255, 4, 5, 6, 255}));

    // A JPEG is recognized by its signature, whatever the name says (some of the client's .dds and .tga are).
    CHECK(Rejected(Of({0xFF, 0xD8, 0xFF, 0xE0, 0, 0})));
}

TEST_CASE("d3dx: .tga texels")
{
    // 24-bit, bottom-up: the first row in the file is the bottom one.
    std::optional<image::Image> img = image::Decode(Tga(2, 24, 0, 1, 2, Of({1, 2, 3, 4, 5, 6})));
    REQUIRE(img);
    CHECK(img->format == D3DFMT_X8R8G8B8);
    CHECK(img->levels[0].bytes == Of({4, 5, 6, 255, 1, 2, 3, 255}));
    // 32-bit, top-down (descriptor bit 5).
    img = image::Decode(Tga(2, 32, 0x28, 2, 1, Of({1, 2, 3, 4, 5, 6, 7, 8})));
    REQUIRE(img);
    CHECK(img->format == D3DFMT_A8R8G8B8);
    CHECK(img->levels[0].bytes == Of({1, 2, 3, 4, 5, 6, 7, 8}));
    // RLE: a run of 3 and a raw packet of 1.
    img = image::Decode(Tga(10, 24, 0x20, 4, 1, Of({0x82, 9, 8, 7, 0x00, 1, 2, 3})));
    REQUIRE(img);
    CHECK(img->levels[0].bytes == Of({9, 8, 7, 255, 9, 8, 7, 255, 9, 8, 7, 255, 1, 2, 3, 255}));
    // 16-bit keeps the alpha bit: A1R5G5B5's own words, right to left (descriptor bit 4), bottom-up.
    img = image::Decode(Tga(2, 16, 0x11, 2, 2, Of({0x00, 0x80, 0x1F, 0x00, 0xE0, 0x83, 0x00, 0x7C})));
    REQUIRE(img);
    CHECK(img->format == D3DFMT_A1R5G5B5);
    CHECK(img->levels[0].bytes == Of({0x00, 0x7C, 0xE0, 0x83, 0x1F, 0x00, 0x00, 0x80}));
    // 16-bit RLE without attribute bits: X1R5G5B5 with the bit set.
    img = image::Decode(Tga(10, 16, 0x20, 3, 1, Of({0x82, 0x1F, 0x00})));
    REQUIRE(img);
    CHECK(img->format == D3DFMT_X1R5G5B5);
    CHECK(img->levels[0].bytes == Of({0x1F, 0x80, 0x1F, 0x80, 0x1F, 0x80}));
    CHECK_FALSE(image::Decode(Tga(10, 16, 0x20, 3, 1, Of({0x82}))));            // cut short
    // Grayscale and color-mapped.
    img = image::Decode(Tga(3, 8, 0x20, 2, 1, Of({10, 20})));
    REQUIRE(img);
    CHECK(img->format == D3DFMT_L8);
    CHECK(img->levels[0].bytes == Of({10, 20}));
    img = image::Decode(Tga(1, 8, 0x20, 2, 1, Of({1, 0}), Of({1, 2, 3, 4, 5, 6})));
    REQUIRE(img);
    CHECK(img->format == D3DFMT_A8R8G8B8);
    CHECK(img->levels[0].bytes == Of({4, 5, 6, 255, 1, 2, 3, 255}));
}

TEST_CASE("d3dx: BC1-3 decoding and encoding")
{
    // Red and blue end points, four colors (c0 > c1); indices 0, 1, 2, 3 on the first row.
    std::vector<std::uint8_t> out(16 * 4);
    image::ToBgra8(D3DFMT_DXT1, Of({0x00, 0xF8, 0x1F, 0x00, 0xE4, 0, 0, 0}), 4, 4, out.data());
    CHECK(Texel(out, 4, 0, 0) == V{0, 0, 255, 255});
    CHECK(Texel(out, 4, 1, 0) == V{255, 0, 0, 255});
    CHECK(Texel(out, 4, 2, 0) == V{85, 0, 170, 255});
    CHECK(Texel(out, 4, 3, 0) == V{170, 0, 85, 255});
    CHECK(Texel(out, 4, 0, 1) == V{0, 0, 255, 255});
    // c0 <= c1: three colors and transparent black.
    image::ToBgra8(D3DFMT_DXT1, Of({0x1F, 0x00, 0x00, 0xF8, 0xE4, 0, 0, 0}), 4, 4, out.data());
    CHECK(Texel(out, 4, 2, 0) == V{127, 0, 127, 255});
    CHECK(Texel(out, 4, 3, 0) == V{0, 0, 0, 0});
    // DXT3's explicit alpha and DXT5's interpolated alpha, with a 2x2 level (one partial block).
    std::vector<std::uint8_t> small(2 * 2 * 4);
    image::ToBgra8(D3DFMT_DXT3, Of({0x0F, 0x80, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0}), 2, 2,
                   small.data());
    CHECK(Texel(small, 2, 0, 0) == V{255, 255, 255, 255});
    CHECK(Texel(small, 2, 1, 0) == V{255, 255, 255, 0});
    CHECK(Texel(small, 2, 0, 1)[3] == 0);
    // a0 = 255, a1 = 0: index 1 is 0, index 2 is (6 * 255 + 0) / 7 = 219.
    image::ToBgra8(D3DFMT_DXT5, Of({255, 0, 0x01 | (2 << 3), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), 2, 2,
                   small.data());
    CHECK(Texel(small, 2, 0, 0)[3] == 0);
    CHECK(Texel(small, 2, 1, 0)[3] == 219);
    CHECK(Texel(small, 2, 0, 1)[3] == 255);

    // Round trips of a block of two colors and two alphas.
    std::vector<std::uint8_t> bgra(8 * 4 * 4);
    for (UINT y = 0; y < 4; ++y) {
        for (UINT x = 0; x < 8; ++x) {
            const bool first = x < 4;
            std::uint8_t* t = &bgra[(y * 8 + x) * 4];
            t[0] = first ? 255 : 0;
            t[1] = first ? 0 : 255;
            t[2] = 0;
            t[3] = first ? 255 : 0x44;
        }
    }
    for (D3DFORMAT format : {D3DFMT_DXT1, D3DFMT_DXT3, D3DFMT_DXT5}) {
        CAPTURE(format);
        const Bytes encoded = image::FromBgra8(format, bgra.data(), 8, 4);
        CHECK(encoded.size() == x2::d3d9::LevelBytes(format, 8, 4));
        std::vector<std::uint8_t> back(bgra.size());
        image::ToBgra8(format, encoded, 8, 4, back.data());
        CHECK(Texel(back, 8, 0, 0) == V{255, 0, 0, 255});
        CHECK(Texel(back, 8, 7, 3) == V{0, 255, 0, format == D3DFMT_DXT1 ? std::uint8_t{255} : std::uint8_t{0x44}});
    }
    // A level smaller than a block repeats its edge.
    const Bytes tiny = image::FromBgra8(D3DFMT_DXT5, bgra.data(), 1, 1);
    CHECK(tiny.size() == 16);
}

TEST_CASE("d3dx: texel formats to and from BGRA8")
{
    const std::uint8_t texel[4] = {0x10, 0x80, 0xF0, 0x80};     // B, G, R, A
    const std::pair<D3DFORMAT, Bytes> encodings[] = {
        {D3DFMT_A8R8G8B8, Of({0x10, 0x80, 0xF0, 0x80})},
        {D3DFMT_X8R8G8B8, Of({0x10, 0x80, 0xF0, 0xFF})},
        {D3DFMT_R8G8B8, Of({0x10, 0x80, 0xF0})},
        {D3DFMT_R5G6B5, Of({0x02, 0xEC})},          // 11101 100000 00010
        {D3DFMT_A1R5G5B5, Of({0x02, 0xF6})},        // 1 11101 10000 00010
        {D3DFMT_X1R5G5B5, Of({0x02, 0xF6})},
        {D3DFMT_A4R4G4B4, Of({0x81, 0x8E})},
        {D3DFMT_A8, Of({0x80})},
        {D3DFMT_L8, Of({0x90})},                    // 0.2125 * 240 + 0.7154 * 128 + 0.0721 * 16
        {D3DFMT_A8L8, Of({0x90, 0x80})},
    };
    for (const auto& [format, bytes] : encodings) {
        CAPTURE(format);
        CHECK(image::FromBgra8(format, texel, 1, 1) == bytes);
    }
    std::uint8_t out[4];
    image::ToBgra8(D3DFMT_A4R4G4B4, Of({0x81, 0x8E}), 1, 1, out);
    CHECK(V(out, out + 4) == V{0x11, 0x88, 0xEE, 0x88});
    image::ToBgra8(D3DFMT_A1R5G5B5, Of({0x02, 0x7A}), 1, 1, out);
    CHECK(out[3] == 0);
}

TEST_CASE("d3dx: resize filters")
{
    // A 4x2 image whose texels are their x * 40 in every channel.
    std::vector<std::uint8_t> src(4 * 2 * 4);
    for (UINT i = 0; i < 8; ++i) {
        std::fill_n(&src[i * 4], 4, static_cast<std::uint8_t>((i % 4) * 40));
    }
    auto at = [](const std::vector<std::uint8_t>& v, UINT w, UINT x, UINT y) { return v[(y * w + x) * 4]; };
    std::vector<std::uint8_t> out = image::Resize(src.data(), 4, 2, 2, 1, image::Filter::Box);
    CHECK(at(out, 2, 0, 0) == 20);
    CHECK(at(out, 2, 1, 0) == 100);
    out = image::Resize(src.data(), 4, 2, 2, 1, image::Filter::Linear);
    CHECK(at(out, 2, 0, 0) == 20);
    CHECK(at(out, 2, 1, 0) == 100);
    out = image::Resize(src.data(), 4, 2, 1, 1, image::Filter::Linear);     // the four nearest, not all eight
    CHECK(at(out, 1, 0, 0) == 60);
    out = image::Resize(src.data(), 4, 2, 2, 1, image::Filter::Point);
    CHECK(at(out, 2, 0, 0) == 0);
    CHECK(at(out, 2, 1, 0) == 80);
    out = image::Resize(src.data(), 4, 2, 2, 3, image::Filter::None);       // cropped, transparent black outside
    CHECK(at(out, 2, 1, 0) == 40);
    CHECK(out[(2 * 2 + 1) * 4 + 3] == 0);
    out = image::Resize(src.data(), 4, 2, 8, 2, image::Filter::Box);        // enlarging: linear
    CHECK(at(out, 8, 0, 0) == 0);
    CHECK(at(out, 8, 2, 0) == 30);

    CHECK(image::FilterFromD3DX(D3DX_FILTER_NONE) == image::Filter::None);
    CHECK(image::FilterFromD3DX(D3DX_FILTER_POINT) == image::Filter::Point);
    CHECK(image::FilterFromD3DX(D3DX_FILTER_LINEAR | (1 << 16)) == image::Filter::Linear);
    CHECK(image::FilterFromD3DX(D3DX_DEFAULT) == image::Filter::Box);
}

TEST_CASE("d3dx: textures from .dds files")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());

    // DXT1 8x8 with its chain: copied as it is, block for block.
    const Bytes l0 = Sequence(32, 1), l1 = Sequence(8, 2), l2 = Sequence(8, 3), l3 = Sequence(8, 4);
    Bytes data = l0;
    for (const Bytes* l : {&l1, &l2, &l3}) {
        data.insert(data.end(), l->begin(), l->end());
    }
    const Bytes dxt = Dds(kDxt1, 8, 8, 4, data);
    auto texture = Load(device, dxt, 0, 0, D3DX_FROM_FILE);
    CHECK(texture->Format() == D3DFMT_DXT1);
    REQUIRE(texture->GetLevelCount() == 4);
    CHECK(LevelBytes(*texture, 0) == l0);
    CHECK(LevelBytes(*texture, 3) == l3);
    // CKTDXDeviceTexture asks for one level.
    texture = Load(device, dxt, 8, 8, 1);
    CHECK(texture->GetLevelCount() == 1);
    CHECK(LevelBytes(*texture, 0) == l0);

    // A file cut short keeps its whole levels; the full chain generates the rest from them.
    Bytes cut = dxt;
    cut.resize(128 + 32 + 8 + 4);
    const std::optional<image::Image> partial = image::Decode(cut);
    REQUIRE(partial);
    CHECK(partial->info.MipLevels == 4);
    CHECK(partial->levels.size() == 2);
    texture = Load(device, cut, 0, 0, D3DX_DEFAULT);
    CHECK(texture->GetLevelCount() == 4);
    CHECK(LevelBytes(*texture, 1) == l1);

    // A8R8G8B8 4x4 with no chain: the full chain is generated by a box filter.
    Bytes argb;
    for (int i = 0; i < 16; ++i) {
        PutBytes(argb, {static_cast<unsigned>(i * 16), 0, 0, 255});
    }
    texture = Load(device, Dds(kArgb, 4, 4, 1, argb), 0, 0, 0);
    REQUIRE(texture->GetLevelCount() == 3);
    CHECK(Bgra(*texture, 1)[0] == (0 + 16 + 64 + 80 + 2) / 4);
    CHECK(Bgra(*texture, 2)[0] == 120);

    // R8G8B8 becomes X8R8G8B8, A8B8G8R8 swaps to A8R8G8B8.
    texture = Load(device, Dds(kRgb24, 2, 1, 1, Of({1, 2, 3, 4, 5, 6})));
    CHECK(texture->Format() == D3DFMT_X8R8G8B8);
    CHECK(LevelBytes(*texture, 0) == Of({1, 2, 3, 255, 4, 5, 6, 255}));
    texture = Load(device, Dds(kAbgr, 1, 1, 1, Of({1, 2, 3, 4})));
    CHECK(texture->Format() == D3DFMT_A8R8G8B8);
    CHECK(LevelBytes(*texture, 0) == Of({3, 2, 1, 4}));
    // 16-bit and luminance formats stay as they are.
    texture = Load(device, Dds(kArgb4444, 1, 1, 1, Of({0x34, 0x12})));
    CHECK(texture->Format() == D3DFMT_A4R4G4B4);
    CHECK(LevelBytes(*texture, 0) == Of({0x34, 0x12}));
    texture = Load(device, Dds(kA8L8, 1, 1, 1, Of({7, 9})));
    CHECK(texture->Format() == D3DFMT_A8L8);

    // A volume texture keeps its first slice of each level; a cube map its first face.
    texture = Load(device, Dds(kArgb, 1, 1, 1, Of({1, 2, 3, 4, 5, 6, 7, 8}), 2));
    CHECK(LevelBytes(*texture, 0) == Of({1, 2, 3, 4}));
    texture = Load(device, Dds(kArgb, 2, 2, 2, Sequence(16 * 2 + 4 * 2), 2), 0, 0, D3DX_FROM_FILE);
    REQUIRE(texture->GetLevelCount() == 2);
    const Bytes volume = Sequence(40);
    CHECK(LevelBytes(*texture, 1) == Bytes(volume.begin() + 32, volume.begin() + 36));
    texture = Load(device, Dds(kArgb, 1, 1, 1, Sequence(24), 0, 0xFE00));
    CHECK(LevelBytes(*texture, 0) == Sequence(4));

    // Asked for another format: converted (a format the device does not take becomes A8R8G8B8).
    texture = Load(device, Dds(kRgb24, 2, 1, 1, Of({1, 2, 3, 4, 5, 6})), 0, 0, 1, D3DFMT_A4R4G4B4);
    CHECK(texture->Format() == D3DFMT_A4R4G4B4);
    texture = Load(device, Dds(kArgb, 1, 1, 1, Of({1, 2, 3, 4})), 0, 0, 1, D3DFMT_A8B8G8R8);
    CHECK(texture->Format() == D3DFMT_A8R8G8B8);

    // The color key turns its exact color into transparent black.
    texture = Load(device, Dds(kArgb, 2, 1, 1, Of({0, 0, 255, 255, 1, 0, 255, 255})), 0, 0, 1, D3DFMT_UNKNOWN,
                   D3DX_FILTER_NONE, 0xFFFF0000);
    CHECK(LevelBytes(*texture, 0) == Of({0, 0, 0, 0, 1, 0, 255, 255}));

    // Failures leave no texture.
    x2::d3d9::Texture* none = reinterpret_cast<x2::d3d9::Texture*>(&device);
    const Bytes bump = Dds(PixelFormat{0x4, 63}, 4, 4, 1, Sequence(64));
    CHECK(D3DXCreateTextureFromFileInMemoryEx(&device, bump.data(), static_cast<UINT>(bump.size()), 0, 0, 1, 0,
                                              D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0,
                                              nullptr, nullptr, &none) == D3DXERR_INVALIDDATA);
    CHECK(none == nullptr);
    CHECK(D3DXCreateTextureFromFileInMemoryEx(nullptr, dxt.data(), static_cast<UINT>(dxt.size()), 0, 0, 1, 0,
                                              D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0,
                                              nullptr, nullptr, &none) == D3DERR_INVALIDCALL);
    CHECK(D3DXCreateTextureFromFileInMemoryEx(&device, dxt.data(), static_cast<UINT>(dxt.size()), 0, 0, 1, 0,
                                              D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0,
                                              nullptr, nullptr, nullptr) == D3DERR_INVALIDCALL);
}

// CKTDXDeviceTexture's loads: DL_HIGH keeps the file; DL_MEDIUM halves textures of 128 and up, and makes R8G8B8 and
// R5G6B5 DXT1; DL_LOW quarters them, and makes everything else DXT3.
TEST_CASE("d3dx: the detail-level loads of CKTDXDeviceTexture")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());

    // A 128x128 24-bit .tga of two halves, red on the left, green on the right.
    Bytes pixels;
    for (int y = 0; y < 128; ++y) {
        for (int x = 0; x < 128; ++x) {
            PutBytes(pixels, x < 64 ? std::initializer_list<unsigned>{0, 0, 255} : std::initializer_list<unsigned>{0, 255, 0});
        }
    }
    const Bytes tga = Tga(2, 24, 0, 128, 128, pixels);
    const D3DXIMAGE_INFO info = InfoOf(tga);
    CHECK(info.Format == D3DFMT_R8G8B8);

    auto high = Load(device, tga, info.Width, info.Height, 1, D3DFMT_UNKNOWN, D3DX_FILTER_NONE);
    CHECK(high->Format() == D3DFMT_X8R8G8B8);
    CHECK(high->Width() == 128);

    auto medium = Load(device, tga, 64, 64, 1, D3DFMT_DXT1, D3DX_FILTER_LINEAR);
    CHECK(medium->Format() == D3DFMT_DXT1);
    CHECK(medium->Width() == 64);
    CHECK(medium->GetLevelCount() == 1);
    std::vector<std::uint8_t> bgra = Bgra(*medium, 0);
    CHECK(Texel(bgra, 64, 0, 0) == V{0, 0, 255, 255});
    CHECK(Texel(bgra, 64, 63, 63) == V{0, 255, 0, 255});

    auto low = Load(device, tga, 32, 32, 1, D3DFMT_DXT3, D3DX_FILTER_LINEAR);
    CHECK(low->Format() == D3DFMT_DXT3);
    CHECK(low->Width() == 32);
    bgra = Bgra(*low, 0);
    CHECK(Texel(bgra, 32, 31, 0) == V{0, 255, 0, 255});

    // A DXT1 .dds at low detail: decoded, quartered and encoded as DXT3.
    const Bytes dxtFile = Dds(kDxt1, 8, 8, 1, image::FromBgra8(D3DFMT_DXT1, Bgra(*high, 0).data(), 8, 8));
    auto dxtLow = Load(device, dxtFile, 2, 2, 1, D3DFMT_DXT3, D3DX_FILTER_LINEAR);
    CHECK(dxtLow->Format() == D3DFMT_DXT3);
    CHECK(dxtLow->Width() == 2);
    CHECK(LevelBytes(*dxtLow, 0).size() == 16);
}

TEST_CASE("d3dx: the client's textures (X2_MASSFILE_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_MASSFILE_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0') {
        MESSAGE("X2_MASSFILE_CLIENT_DATA not set: skipped");
        return;
    }
    std::string error;
    x2::kom::Mount mount([](const std::string&) {});
    REQUIRE_MESSAGE(mount.AddFolder(folder, error), error);
    std::vector<std::string> names;
    std::set<std::string> seen;
    for (const auto& archive : mount.Archives()) {
        for (const x2::kom::Entry& entry : archive->Entries()) {
            std::string lower = entry.name;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                           [](char c) { return static_cast<char>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c); });
            if ((lower.ends_with(".dds") || lower.ends_with(".tga")) && seen.insert(lower).second) {
                names.push_back(entry.name);
            }
        }
    }
    MESSAGE(names.size() << " textures");

    // Every texture decodes but three, which D3DX could not have read either.
    const auto start = std::chrono::steady_clock::now();
    std::atomic<std::size_t> next{0};
    std::mutex mutex;
    std::map<std::string, int> formats;
    std::vector<std::string> failed;
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&] {
            std::vector<std::uint8_t> bytes;
            std::string readError;
            for (std::size_t i = next++; i < names.size(); i = next++) {
                if (!mount.Read(names[i], bytes, readError)) {
                    const std::lock_guard lock(mutex);
                    failed.push_back(names[i] + " (" + readError + ")");
                    continue;
                }
                const auto decoded = image::Decode(std::as_bytes(std::span(bytes)));
                const std::lock_guard lock(mutex);
                if (!decoded) {
                    failed.push_back(names[i]);
                    continue;
                }
                const D3DXIMAGE_INFO& info = decoded->info;
                ++formats[std::to_string(info.ImageFileFormat) + "/" + std::to_string(info.Format)];
                if (decoded->levels.size() != info.MipLevels) {
                    ++formats["cut short"];
                }
            }
        });
    }
    for (std::thread& t : threads) {
        t.join();
    }
    MESSAGE("decoded in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start)
                                 .count()
                          << " s");
    for (const auto& [format, count] : formats) {
        MESSAGE("file format/D3DFORMAT " << format << ": " << count);
    }
    std::sort(failed.begin(), failed.end());
    for (const std::string& name : failed) {
        MESSAGE("not read: " << name);
    }
    // Two empty files and a Q8W8V8U8 bump map (2026-11-07).
    CHECK(failed == std::vector<std::string>{"Velder_Field_001.dds", "Velder_Field_003.dds",
                                             "field_Peita_stage1_bluegate.dds"});

    // A few of them through the device, at every detail level.
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    for (const char* name : {"Black_Drain.dds", "Explosion_Sphere_01.dds", "Nasod_King_Sanding.dds",
                             "DragonNest_DragonBone.tga", "Apple_Monkey.tga", "Mesh_Arme_Cash_Foot20A.tga"}) {
        CAPTURE(name);
        std::vector<std::uint8_t> bytes;
        if (!mount.Read(name, bytes, error)) {
            MESSAGE("missing: " << name);
            continue;
        }
        const Bytes file(reinterpret_cast<const std::byte*>(bytes.data()),
                         reinterpret_cast<const std::byte*>(bytes.data()) + bytes.size());
        const D3DXIMAGE_INFO info = InfoOf(file);
        auto high = Load(device, file, info.Width, info.Height, 1);
        CHECK(high->Width() == info.Width);
        auto low = Load(device, file, std::max(1u, info.Width / 4), std::max(1u, info.Height / 4), 1, D3DFMT_DXT3,
                        D3DX_FILTER_LINEAR);
        CHECK(low->Format() == D3DFMT_DXT3);
    }
}
