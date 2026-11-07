#pragma once
// D3D9 texture formats as the device and the image loaders lay them out in memory: texel or block size, pitch, rows.
// docs/porting/client-d3d9.md

#include "D3D9Types.h"

#include <algorithm>
#include <cstddef>

namespace x2::d3d9 {

struct FormatInfo {
    UINT blockBytes = 0;        // bytes per texel, or per 4x4 block; 0 for a format the device does not take
    bool compressed = false;
};

inline FormatInfo Info(D3DFORMAT format)
{
    switch (format) {
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8: return {4, false};
    case D3DFMT_R8G8B8: return {3, false};
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_A8L8: return {2, false};
    case D3DFMT_A8:
    case D3DFMT_L8: return {1, false};
    case D3DFMT_DXT1: return {8, true};
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5: return {16, true};
    default: return {};
    }
}

inline UINT Pitch(D3DFORMAT format, UINT width)
{
    const FormatInfo info = Info(format);
    return info.compressed ? std::max(1u, (width + 3) / 4) * info.blockBytes : width * info.blockBytes;
}

inline UINT Rows(D3DFORMAT format, UINT height)
{
    return Info(format).compressed ? std::max(1u, (height + 3) / 4) : height;
}

// Bytes of a tightly packed level.
inline std::size_t LevelBytes(D3DFORMAT format, UINT width, UINT height)
{
    return static_cast<std::size_t>(Pitch(format, width)) * Rows(format, height);
}

}  // namespace x2::d3d9
