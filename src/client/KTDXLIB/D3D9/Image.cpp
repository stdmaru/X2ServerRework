// D3DX's image files: .dds read here, .tga/.png/.jpg/.bmp through stb_image (16-bit .tga read here, so that their
// alpha bit survives as D3DX's A1R5G5B5). docs/porting/client-d3d9.md#image-files

#include "Image.h"
#include "Formats.h"

#include <stb_image.h>

#include <bit>
#include <climits>
#include <cstring>
#include <memory>

namespace x2::d3d9::image {
namespace {

enum class Kind { Dds, Tga, Stb };

unsigned U8(std::span<const std::byte> file, std::size_t offset) { return std::to_integer<unsigned>(file[offset]); }

std::uint32_t U32(std::span<const std::byte> file, std::size_t offset)
{
    return U8(file, offset) | (U8(file, offset + 1) << 8) | (U8(file, offset + 2) << 16) |
           (static_cast<std::uint32_t>(U8(file, offset + 3)) << 24);
}

unsigned U16(std::span<const std::byte> file, std::size_t offset)
{
    return U8(file, offset) | (U8(file, offset + 1) << 8);
}

UINT FullChain(UINT width, UINT height) { return static_cast<UINT>(std::bit_width(std::max(width, height))); }

// The format a texture of the file's format is created in (Image::format).
D3DFORMAT CreatedAs(D3DFORMAT format)
{
    switch (format) {
    case D3DFMT_R8G8B8:
    case D3DFMT_X8B8G8R8: return D3DFMT_X8R8G8B8;
    case D3DFMT_P8:
    case D3DFMT_A8B8G8R8: return D3DFMT_A8R8G8B8;
    default: return format;
    }
}

// --- DDS ------------------------------------------------------------------------------------------------------------

constexpr std::size_t kDdsHeader = 128;            // "DDS " + DDS_HEADER
constexpr std::uint32_t kDdsdDepth = 0x800000, kDdsdMipMapCount = 0x20000;
constexpr std::uint32_t kDdpfAlphaPixels = 0x1, kDdpfAlpha = 0x2, kDdpfFourCC = 0x4, kDdpfRgb = 0x40,
                        kDdpfLuminance = 0x20000;
constexpr std::uint32_t kDdsCaps2Cubemap = 0x200, kDdsCaps2Volume = 0x200000;

D3DFORMAT DdsFormat(std::span<const std::byte> file)
{
    const std::uint32_t flags = U32(file, 80), fourCC = U32(file, 84), bits = U32(file, 88);
    const std::uint32_t r = U32(file, 92), g = U32(file, 96), b = U32(file, 100);
    const std::uint32_t a = (flags & (kDdpfAlphaPixels | kDdpfAlpha)) ? U32(file, 104) : 0;
    if (flags & kDdpfFourCC) {
        switch (fourCC) {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT3:
        case D3DFMT_DXT4:
        case D3DFMT_DXT5: return static_cast<D3DFORMAT>(fourCC);
        default:
            // Some writers store a D3DFORMAT number as the FourCC.
            return Info(static_cast<D3DFORMAT>(fourCC)).blockBytes != 0 ? static_cast<D3DFORMAT>(fourCC)
                                                                        : D3DFMT_UNKNOWN;
        }
    }
    if (flags & kDdpfRgb) {
        if (bits == 32 && r == 0xFF0000 && g == 0xFF00 && b == 0xFF) {
            return a == 0xFF000000 ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
        }
        if (bits == 32 && r == 0xFF && g == 0xFF00 && b == 0xFF0000) {
            return a == 0xFF000000 ? D3DFMT_A8B8G8R8 : D3DFMT_X8B8G8R8;
        }
        if (bits == 24 && r == 0xFF0000 && g == 0xFF00 && b == 0xFF) {
            return D3DFMT_R8G8B8;
        }
        if (bits == 16 && r == 0xF800 && g == 0x7E0 && b == 0x1F) {
            return D3DFMT_R5G6B5;
        }
        if (bits == 16 && r == 0x7C00 && g == 0x3E0 && b == 0x1F) {
            return a == 0x8000 ? D3DFMT_A1R5G5B5 : D3DFMT_X1R5G5B5;
        }
        if (bits == 16 && r == 0xF00 && g == 0xF0 && b == 0xF) {
            return a == 0xF000 ? D3DFMT_A4R4G4B4 : D3DFMT_X4R4G4B4;
        }
        return D3DFMT_UNKNOWN;
    }
    if ((flags & kDdpfLuminance) && r == 0xFF) {
        return bits == 8 ? D3DFMT_L8 : bits == 16 && a == 0xFF00 ? D3DFMT_A8L8 : D3DFMT_UNKNOWN;
    }
    if ((flags & kDdpfAlpha) && bits == 8 && a == 0xFF) {
        return D3DFMT_A8;
    }
    return D3DFMT_UNKNOWN;
}

bool DdsInfo(std::span<const std::byte> file, D3DXIMAGE_INFO& info)
{
    if (file.size() < kDdsHeader || U32(file, 4) != 124) {
        return false;
    }
    const std::uint32_t flags = U32(file, 8), caps2 = U32(file, 112);
    info = {};
    info.Height = U32(file, 12);
    info.Width = U32(file, 16);
    info.Format = DdsFormat(file);
    if (info.Width == 0 || info.Height == 0 || info.Width > 16384 || info.Height > 16384 ||
        info.Format == D3DFMT_UNKNOWN) {
        return false;
    }
    const std::uint32_t mips = U32(file, 28);
    info.MipLevels = (flags & kDdsdMipMapCount) && mips > 0 ? std::min<UINT>(mips, FullChain(info.Width, info.Height))
                                                             : 1;
    info.Depth = 1;
    info.ResourceType = D3DRTYPE_TEXTURE;
    if ((caps2 & kDdsCaps2Volume) && (flags & kDdsdDepth)) {
        info.Depth = std::clamp<UINT>(U32(file, 24), 1, 2048);
        info.ResourceType = D3DRTYPE_VOLUMETEXTURE;
    } else if (caps2 & kDdsCaps2Cubemap) {
        info.ResourceType = D3DRTYPE_CUBETEXTURE;
    }
    info.ImageFileFormat = D3DXIFF_DDS;
    return true;
}

// Swaps R and B of 32-bit texels in place (A8B8G8R8 -> A8R8G8B8), setting alpha for the X format.
void SwapRedBlue(std::vector<std::byte>& bytes, bool opaque)
{
    for (std::size_t i = 0; i + 3 < bytes.size(); i += 4) {
        std::swap(bytes[i], bytes[i + 2]);
        if (opaque) {
            bytes[i + 3] = std::byte{0xFF};
        }
    }
}

std::optional<Image> DecodeDds(std::span<const std::byte> file, const D3DXIMAGE_INFO& info)
{
    Image image;
    image.info = info;
    image.format = CreatedAs(info.Format);
    // A8B8G8R8 and X8B8G8R8 have A8R8G8B8's size.
    const D3DFORMAT layout = Info(info.Format).blockBytes != 0 ? info.Format : D3DFMT_A8R8G8B8;
    std::size_t offset = kDdsHeader;
    for (UINT i = 0; i < info.MipLevels; ++i) {
        Level level;
        level.width = std::max(1u, info.Width >> i);
        level.height = std::max(1u, info.Height >> i);
        const UINT depth = std::max(1u, info.Depth >> i);
        const std::size_t bytes = LevelBytes(layout, level.width, level.height);
        if (offset + bytes > file.size()) {
            break;      // cut short: keep the levels that are whole
        }
        const auto* in = file.data() + offset;
        if (info.Format == D3DFMT_R8G8B8) {
            level.bytes.resize(static_cast<std::size_t>(level.width) * level.height * 4);
            for (std::size_t t = 0; t < static_cast<std::size_t>(level.width) * level.height; ++t) {
                std::memcpy(&level.bytes[t * 4], in + t * 3, 3);
                level.bytes[t * 4 + 3] = std::byte{0xFF};
            }
        } else {
            level.bytes.assign(in, in + bytes);
            if (info.Format == D3DFMT_A8B8G8R8 || info.Format == D3DFMT_X8B8G8R8) {
                SwapRedBlue(level.bytes, info.Format == D3DFMT_X8B8G8R8);
            }
        }
        image.levels.push_back(std::move(level));
        offset += bytes * depth;
    }
    if (image.levels.empty()) {
        return std::nullopt;
    }
    return image;
}

// --- TGA ------------------------------------------------------------------------------------------------------------

constexpr std::size_t kTgaHeader = 18;

bool TgaInfo(std::span<const std::byte> file, D3DXIMAGE_INFO& info)
{
    if (file.size() < kTgaHeader) {
        return false;
    }
    const unsigned colorMapType = U8(file, 1), type = U8(file, 2), bits = U8(file, 16), descriptor = U8(file, 17);
    const unsigned width = U16(file, 12), height = U16(file, 14);
    if (colorMapType > 1 || width == 0 || height == 0) {
        return false;
    }
    info = {};
    switch (type) {
    case 1:
    case 9:     // color-mapped
        if (colorMapType != 1 || bits != 8) {
            return false;
        }
        info.Format = D3DFMT_P8;
        break;
    case 2:
    case 10:    // true color
        switch (bits) {
        case 15:
        case 16: info.Format = (descriptor & 0xF) != 0 ? D3DFMT_A1R5G5B5 : D3DFMT_X1R5G5B5; break;
        case 24: info.Format = D3DFMT_R8G8B8; break;
        case 32: info.Format = D3DFMT_A8R8G8B8; break;
        default: return false;
        }
        break;
    case 3:
    case 11:    // grayscale
        if (bits == 8) {
            info.Format = D3DFMT_L8;
        } else if (bits == 16) {
            info.Format = D3DFMT_A8L8;
        } else {
            return false;
        }
        break;
    default: return false;
    }
    info.Width = width;
    info.Height = height;
    info.Depth = 1;
    info.MipLevels = 1;
    info.ResourceType = D3DRTYPE_TEXTURE;
    info.ImageFileFormat = D3DXIFF_TGA;
    return true;
}

// 15/16-bit true-color TGA (types 2 and 10), whose texels are A1R5G5B5's own bytes.
std::optional<Image> DecodeTga16(std::span<const std::byte> file, const D3DXIMAGE_INFO& info)
{
    const unsigned descriptor = U8(file, 17);
    std::size_t offset = kTgaHeader + U8(file, 0);
    if (U8(file, 1) == 1) {
        offset += static_cast<std::size_t>(U16(file, 5)) * ((U8(file, 7) + 7) / 8);
    }
    const bool rle = U8(file, 2) == 10;
    const std::size_t texels = static_cast<std::size_t>(info.Width) * info.Height;
    std::vector<std::uint16_t> pixels(texels);
    auto read = [&](std::uint16_t& out) {
        if (offset + 2 > file.size()) {
            return false;
        }
        out = static_cast<std::uint16_t>(U16(file, offset));
        offset += 2;
        return true;
    };
    for (std::size_t i = 0; i < texels;) {
        unsigned count = 1;
        bool repeat = false;
        if (rle) {
            if (offset >= file.size()) {
                return std::nullopt;
            }
            const unsigned packet = U8(file, offset++);
            count = (packet & 0x7F) + 1;
            repeat = (packet & 0x80) != 0;
        }
        std::uint16_t value = 0;
        for (unsigned n = 0; n < count && i < texels; ++n, ++i) {
            if ((n == 0 || !repeat) && !read(value)) {
                return std::nullopt;
            }
            pixels[i] = value;
        }
    }
    Image image;
    image.info = info;
    image.format = info.Format;
    Level level{info.Width, info.Height, std::vector<std::byte>(texels * 2)};
    const bool rightToLeft = (descriptor & 0x10) != 0, topDown = (descriptor & 0x20) != 0;
    const std::uint16_t alpha = info.Format == D3DFMT_X1R5G5B5 ? 0x8000 : 0;
    for (UINT y = 0; y < info.Height; ++y) {
        const UINT sy = topDown ? y : info.Height - 1 - y;
        for (UINT x = 0; x < info.Width; ++x) {
            const UINT sx = rightToLeft ? info.Width - 1 - x : x;
            const std::uint16_t v = static_cast<std::uint16_t>(pixels[static_cast<std::size_t>(sy) * info.Width + sx] |
                                                               alpha);
            const std::size_t at = (static_cast<std::size_t>(y) * info.Width + x) * 2;
            level.bytes[at] = static_cast<std::byte>(v & 0xFF);
            level.bytes[at + 1] = static_cast<std::byte>(v >> 8);
        }
    }
    image.levels.push_back(std::move(level));
    return image;
}

// --- stb_image (TGA, PNG, JPEG, BMP) --------------------------------------------------------------------------------

// PNG, JPEG and BMP, recognized by their signatures.
bool StbInfo(std::span<const std::byte> file, D3DXIMAGE_INFO& info)
{
    D3DXIMAGE_FILEFORMAT fileFormat;
    if (file.size() >= 8 && U32(file, 0) == 0x474E5089) {
        fileFormat = D3DXIFF_PNG;
    } else if (file.size() >= 3 && U8(file, 0) == 0xFF && U8(file, 1) == 0xD8 && U8(file, 2) == 0xFF) {
        fileFormat = D3DXIFF_JPG;
    } else if (file.size() >= 18 && U8(file, 0) == 'B' && U8(file, 1) == 'M') {
        // stb_image tries every decoder: a file that is not a BMP after all must not reach its lenient TGA reader.
        const std::uint32_t header = U32(file, 14);
        if (header != 12 && header != 40 && header != 56 && header != 108 && header != 124) {
            return false;
        }
        fileFormat = D3DXIFF_BMP;
    } else {
        return false;
    }
    int width = 0, height = 0, components = 0;
    if (!stbi_info_from_memory(reinterpret_cast<const stbi_uc*>(file.data()), static_cast<int>(file.size()), &width,
                               &height, &components) ||
        width <= 0 || height <= 0) {
        return false;
    }
    info = {};
    info.ImageFileFormat = fileFormat;
    switch (fileFormat) {
    case D3DXIFF_PNG: info.Format = components == 2 || components == 4 ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8; break;
    case D3DXIFF_BMP: info.Format = components == 4 ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8; break;
    default: info.Format = D3DFMT_X8R8G8B8; break;
    }
    info.Width = static_cast<UINT>(width);
    info.Height = static_cast<UINT>(height);
    info.Depth = 1;
    info.MipLevels = 1;
    info.ResourceType = D3DRTYPE_TEXTURE;
    return true;
}

std::optional<Image> DecodeStb(std::span<const std::byte> file, const D3DXIMAGE_INFO& info)
{
    Image image;
    image.info = info;
    image.format = CreatedAs(info.Format);
    const int channels = info.Format == D3DFMT_L8 ? 1 : info.Format == D3DFMT_A8L8 ? 2 : 4;
    int width = 0, height = 0, components = 0;
    std::unique_ptr<stbi_uc, void (*)(void*)> pixels(
        stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(file.data()), static_cast<int>(file.size()), &width,
                              &height, &components, channels),
        stbi_image_free);
    if (!pixels || static_cast<UINT>(width) != info.Width || static_cast<UINT>(height) != info.Height) {
        return std::nullopt;
    }
    const std::size_t bytes = static_cast<std::size_t>(width) * height * channels;
    Level level{info.Width, info.Height, std::vector<std::byte>(bytes)};
    std::memcpy(level.bytes.data(), pixels.get(), bytes);
    if (channels == 4) {
        SwapRedBlue(level.bytes, image.format == D3DFMT_X8R8G8B8);   // RGBA -> BGRA
    }
    image.levels.push_back(std::move(level));
    return image;
}

std::optional<Kind> Probe(std::span<const std::byte> file, D3DXIMAGE_INFO& info)
{
    if (file.size() > static_cast<std::size_t>(INT_MAX)) {
        return std::nullopt;
    }
    if (file.size() >= 4 && U32(file, 0) == 0x20534444) {   // "DDS "
        return DdsInfo(file, info) ? std::optional(Kind::Dds) : std::nullopt;
    }
    if (StbInfo(file, info)) {
        return Kind::Stb;
    }
    // TGA has no signature: D3DX tried it last, as here.
    return TgaInfo(file, info) ? std::optional(Kind::Tga) : std::nullopt;
}

}  // namespace

bool ReadInfo(std::span<const std::byte> file, D3DXIMAGE_INFO& info)
{
    return Probe(file, info).has_value();
}

std::optional<Image> Decode(std::span<const std::byte> file)
{
    D3DXIMAGE_INFO info{};
    const std::optional<Kind> kind = Probe(file, info);
    if (!kind) {
        return std::nullopt;
    }
    switch (*kind) {
    case Kind::Dds: return DecodeDds(file, info);
    case Kind::Tga:
        if (info.Format == D3DFMT_A1R5G5B5 || info.Format == D3DFMT_X1R5G5B5) {
            return DecodeTga16(file, info);
        }
        return DecodeStb(file, info);
    case Kind::Stb: return DecodeStb(file, info);
    }
    return std::nullopt;
}

}  // namespace x2::d3d9::image
