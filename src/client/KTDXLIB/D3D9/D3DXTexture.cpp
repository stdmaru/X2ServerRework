// D3DX's texture functions on KTDXLIB's D3D9 device (decision T38), with the image files of Image.h.
// docs/porting/client-d3d9.md#image-files

#include "D3DXTypes.h"
#include "Device.h"
#include "Formats.h"
#include "Image.h"

#include <algorithm>
#include <bit>
#include <cstring>
#include <memory>
#include <optional>

namespace {

std::span<const std::byte> Bytes(const void* data, UINT size)
{
    return {static_cast<const std::byte*>(data), size};
}

bool IsDefault(UINT value)
{
    return value == 0 || value == D3DX_DEFAULT || value == D3DX_DEFAULT_NONPOW2;
}

// D3DX's color key: texels of exactly that color become transparent black.
void ApplyColorKey(std::vector<std::uint8_t>& bgra, D3DCOLOR colorKey)
{
    if (colorKey == 0) {
        return;
    }
    for (std::size_t i = 0; i + 3 < bgra.size(); i += 4) {
        const D3DCOLOR texel = static_cast<D3DCOLOR>(bgra[i] | (bgra[i + 1] << 8) | (bgra[i + 2] << 16) |
                                                     (static_cast<D3DCOLOR>(bgra[i + 3]) << 24));
        if (texel == colorKey) {
            std::fill_n(bgra.begin() + static_cast<std::ptrdiff_t>(i), 4, std::uint8_t{0});
        }
    }
}

std::vector<std::uint8_t> LevelToBgra8(D3DFORMAT format, const x2::d3d9::image::Level& level)
{
    std::vector<std::uint8_t> bgra(static_cast<std::size_t>(level.width) * level.height * 4);
    x2::d3d9::image::ToBgra8(format, level.bytes, level.width, level.height, bgra.data());
    return bgra;
}

bool Fill(x2::d3d9::Texture& texture, UINT level, std::span<const std::byte> bytes)
{
    D3DSURFACE_DESC desc{};
    D3DLOCKED_RECT locked{};
    if (!texture.GetLevelDesc(level, &desc) || !texture.LockRect(level, &locked, nullptr, 0)) {
        return false;
    }
    const UINT rowBytes = x2::d3d9::Pitch(desc.Format, desc.Width), rows = x2::d3d9::Rows(desc.Format, desc.Height);
    for (UINT row = 0; row < rows && static_cast<std::size_t>(row + 1) * rowBytes <= bytes.size(); ++row) {
        std::memcpy(static_cast<std::byte*>(locked.pBits) + static_cast<std::size_t>(row) * locked.Pitch,
                    bytes.data() + static_cast<std::size_t>(row) * rowBytes, rowBytes);
    }
    return texture.UnlockRect(level);
}

}  // namespace

HRESULT D3DXGetImageInfoFromFileInMemory(const void* pSrcData, UINT srcDataSize, D3DXIMAGE_INFO* pSrcInfo)
{
    if (pSrcData == nullptr || srcDataSize == 0) {
        return D3DERR_INVALIDCALL;
    }
    D3DXIMAGE_INFO info{};
    if (!x2::d3d9::image::ReadInfo(Bytes(pSrcData, srcDataSize), info)) {
        return D3DXERR_INVALIDDATA;
    }
    if (pSrcInfo != nullptr) {
        *pSrcInfo = info;
    }
    return D3D_OK;
}

HRESULT D3DXCreateTextureFromFileInMemoryEx(x2::d3d9::Device* pDevice, const void* pSrcData, UINT srcDataSize,
                                            UINT width, UINT height, UINT mipLevels, DWORD usage, D3DFORMAT format,
                                            D3DPOOL pool, DWORD filter, DWORD mipFilter, D3DCOLOR colorKey,
                                            D3DXIMAGE_INFO* pSrcInfo, void* /*pPalette*/, x2::d3d9::Texture** ppTexture)
{
    namespace image = x2::d3d9::image;
    if (ppTexture == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    *ppTexture = nullptr;
    if (pDevice == nullptr || pSrcData == nullptr || srcDataSize == 0) {
        return D3DERR_INVALIDCALL;
    }
    const std::optional<image::Image> file = image::Decode(Bytes(pSrcData, srcDataSize));
    if (!file) {
        return D3DXERR_INVALIDDATA;
    }
    const D3DXIMAGE_INFO& info = file->info;
    width = IsDefault(width) ? info.Width : width;
    height = IsDefault(height) ? info.Height : height;
    if (format == D3DFMT_UNKNOWN || format == D3DFMT_FROM_FILE || static_cast<UINT>(format) == D3DX_DEFAULT) {
        format = file->format;
    }
    if (!pDevice->SupportsTextureFormat(format, usage)) {
        format = D3DFMT_A8R8G8B8;
    }
    const UINT fullChain = static_cast<UINT>(std::bit_width(std::max(width, height)));
    UINT levels = mipLevels == 0 || mipLevels == D3DX_DEFAULT ? fullChain
                  : mipLevels == D3DX_FROM_FILE              ? static_cast<UINT>(file->levels.size())
                                                             : mipLevels;
    levels = std::clamp(levels, 1u, fullChain);
    std::unique_ptr<x2::d3d9::Texture> texture = pDevice->CreateTexture(width, height, levels, usage, format, pool);
    if (!texture) {
        return D3DERR_NOTAVAILABLE;
    }

    // Levels the file has at the texture's size and format are copied; the others are converted, resized from the
    // file's top level, or generated from the level above.
    const bool sameSize = width == info.Width && height == info.Height;
    const bool direct = sameSize && format == file->format && colorKey == 0;
    const image::Filter mipFilterKind =
        image::FilterFromD3DX(mipFilter) == image::Filter::None ? image::Filter::Box : image::FilterFromD3DX(mipFilter);
    std::vector<std::uint8_t> above;        // the previous level as BGRA8, when converted
    for (UINT level = 0; level < texture->GetLevelCount(); ++level) {
        const UINT w = std::max(1u, width >> level), h = std::max(1u, height >> level);
        const bool fromFile = sameSize && level < file->levels.size();
        if (direct && fromFile) {
            if (!Fill(*texture, level, file->levels[level].bytes)) {
                return D3DERR_INVALIDCALL;
            }
            continue;
        }
        std::vector<std::uint8_t> bgra;
        if (fromFile) {
            bgra = LevelToBgra8(file->format, file->levels[level]);
            ApplyColorKey(bgra, colorKey);
        } else if (level == 0) {
            const image::Level& top = file->levels[0];
            bgra = LevelToBgra8(file->format, top);
            ApplyColorKey(bgra, colorKey);
            bgra = image::Resize(bgra.data(), top.width, top.height, w, h, image::FilterFromD3DX(filter));
        } else {
            if (above.empty()) {     // the level above was copied as it was
                above = LevelToBgra8(file->format, file->levels[level - 1]);
            }
            const UINT aw = std::max(1u, width >> (level - 1)), ah = std::max(1u, height >> (level - 1));
            bgra = image::Resize(above.data(), aw, ah, w, h, mipFilterKind);
        }
        if (!Fill(*texture, level, image::FromBgra8(format, bgra.data(), w, h))) {
            return D3DERR_INVALIDCALL;
        }
        above = std::move(bgra);
    }
    if (pSrcInfo != nullptr) {
        *pSrcInfo = info;
    }
    *ppTexture = texture.release();
    return D3D_OK;
}

HRESULT D3DXCheckTextureRequirements(x2::d3d9::Device* pDevice, UINT* pWidth, UINT* pHeight, UINT* pNumMipLevels,
                                     DWORD usage, D3DFORMAT* pFormat, D3DPOOL /*pool*/)
{
    if (pDevice == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    UINT width = pWidth != nullptr && *pWidth != 0 && *pWidth != D3DX_DEFAULT ? *pWidth : 256;
    UINT height = pHeight != nullptr && *pHeight != 0 && *pHeight != D3DX_DEFAULT ? *pHeight : 256;
    D3DFORMAT format = pFormat != nullptr && *pFormat != D3DFMT_UNKNOWN ? *pFormat : D3DFMT_A8R8G8B8;
    if (!pDevice->SupportsTextureFormat(format, usage)) {
        return D3DERR_NOTAVAILABLE;
    }
    if (pWidth != nullptr) {
        *pWidth = width;
    }
    if (pHeight != nullptr) {
        *pHeight = height;
    }
    if (pFormat != nullptr) {
        *pFormat = format;
    }
    if (pNumMipLevels != nullptr) {
        const UINT fullChain = static_cast<UINT>(std::bit_width(std::max(width, height)));
        *pNumMipLevels = *pNumMipLevels == 0 || *pNumMipLevels == D3DX_DEFAULT ? fullChain
                                                                                : std::min(*pNumMipLevels, fullChain);
    }
    return D3D_OK;
}

HRESULT D3DXCreateTexture(x2::d3d9::Device* pDevice, UINT width, UINT height, UINT mipLevels, DWORD usage,
                          D3DFORMAT format, D3DPOOL pool, x2::d3d9::Texture** ppTexture)
{
    if (ppTexture == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    *ppTexture = nullptr;
    if (pDevice == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    UINT levels = mipLevels;
    if (FAILED(D3DXCheckTextureRequirements(pDevice, &width, &height, &levels, usage, &format, pool))) {
        return D3DERR_NOTAVAILABLE;
    }
    *ppTexture = pDevice->CreateTexture(width, height, levels, usage, format, pool).release();
    return *ppTexture != nullptr ? D3D_OK : D3DERR_OUTOFVIDEOMEMORY;
}
