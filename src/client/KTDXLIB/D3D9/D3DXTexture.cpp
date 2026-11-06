// D3DX's texture functions on KTDXLIB's D3D9 device (decision T38). The image loaders (.dds, .tga) are the next step.

#include "D3DXTypes.h"
#include "Device.h"

#include <algorithm>
#include <bit>

HRESULT D3DXGetImageInfoFromFileInMemory(const void* /*pSrcData*/, UINT /*srcDataSize*/, D3DXIMAGE_INFO* /*pSrcInfo*/)
{
    return D3DXERR_INVALIDDATA;
}

HRESULT D3DXCreateTextureFromFileInMemoryEx(x2::d3d9::Device* /*pDevice*/, const void* /*pSrcData*/,
                                            UINT /*srcDataSize*/, UINT /*width*/, UINT /*height*/, UINT /*mipLevels*/,
                                            DWORD /*usage*/, D3DFORMAT /*format*/, D3DPOOL /*pool*/, DWORD /*filter*/,
                                            DWORD /*mipFilter*/, D3DCOLOR /*colorKey*/, D3DXIMAGE_INFO* /*pSrcInfo*/,
                                            void* /*pPalette*/, x2::d3d9::Texture** ppTexture)
{
    if (ppTexture != nullptr) {
        *ppTexture = nullptr;
    }
    return D3DXERR_INVALIDDATA;
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
