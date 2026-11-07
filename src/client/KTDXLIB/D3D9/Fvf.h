#pragma once

// D3D9 flexible vertex formats: where each element sits in a vertex, and which vertex shader variant reads it
// (decision T38): a fixed-function one, or an effect's (T37).

#include "D3D9Types.h"

#include "x2/render/render.h"

namespace x2::d3d9 {

struct FvfLayout {
    bool valid = false;
    bool rhw = false;               // D3DFVF_XYZRHW: already in screen space
    int weightCount = 0;            // blend weights after the position (XYZB<n>, less the last DWORD if it holds indices)
    int indicesOffset = -1;         // the blend indices: the position's last DWORD with LASTBETA_UBYTE4 or _D3DCOLOR
    int normalOffset = -1;
    int diffuseOffset = -1;
    int texCount = 0;               // sets in the vertex (the shader reads the first two)
    int texOffset[2] = {-1, -1};
    render::VertexElementFormat texFormat[2] = {render::VertexElementFormat::Float2, render::VertexElementFormat::Float2};
    UINT size = 0;                  // bytes per vertex
};

inline FvfLayout DescribeFvf(DWORD fvf)
{
    FvfLayout layout;
    UINT offset = 0;
    int betas = 0;
    switch (fvf & D3DFVF_POSITION_MASK) {
    case D3DFVF_XYZ: offset = 12; break;
    case D3DFVF_XYZRHW: offset = 16; layout.rhw = true; break;
    case D3DFVF_XYZW: offset = 16; break;
    case D3DFVF_XYZB1: betas = 1; break;
    case D3DFVF_XYZB2: betas = 2; break;
    case D3DFVF_XYZB3: betas = 3; break;
    case D3DFVF_XYZB4: betas = 4; break;
    case D3DFVF_XYZB5: betas = 5; break;
    default: return layout;     // no position
    }
    if (betas > 0) {
        offset = 12 + 4 * static_cast<UINT>(betas);
        layout.weightCount = betas;
        if (fvf & (D3DFVF_LASTBETA_UBYTE4 | D3DFVF_LASTBETA_D3DCOLOR)) {
            layout.weightCount = betas - 1;
            layout.indicesOffset = static_cast<int>(offset) - 4;
        }
    }
    if (fvf & D3DFVF_NORMAL) {
        layout.normalOffset = static_cast<int>(offset);
        offset += 12;
    }
    if (fvf & D3DFVF_PSIZE) {
        offset += 4;
    }
    if (fvf & D3DFVF_DIFFUSE) {
        layout.diffuseOffset = static_cast<int>(offset);
        offset += 4;
    }
    if (fvf & D3DFVF_SPECULAR) {
        offset += 4;
    }
    layout.texCount = static_cast<int>((fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
    if (layout.texCount > 8) {
        return layout;
    }
    for (int i = 0; i < layout.texCount; ++i) {
        render::VertexElementFormat format = render::VertexElementFormat::Float2;
        UINT size = 8;
        switch ((fvf >> (16 + i * 2)) & 3) {
        case D3DFVF_TEXTUREFORMAT1: format = render::VertexElementFormat::Float; size = 4; break;
        case D3DFVF_TEXTUREFORMAT3: format = render::VertexElementFormat::Float3; size = 12; break;
        case D3DFVF_TEXTUREFORMAT4: format = render::VertexElementFormat::Float4; size = 16; break;
        default: break;
        }
        if (i < 2) {
            layout.texOffset[i] = static_cast<int>(offset);
            layout.texFormat[i] = format;
        }
        offset += size;
    }
    layout.size = offset;
    layout.valid = true;
    return layout;
}

// Index into kFixedFunctionVertex: defines POSITION_RHW, DIFFUSE, TEXCOORDS (0..2), the last one fastest.
inline std::size_t VertexShaderIndex(const FvfLayout& layout)
{
    const std::size_t tex = static_cast<std::size_t>(layout.texCount < 2 ? layout.texCount : 2);
    return ((layout.rhw ? 1u : 0u) * 2 + (layout.diffuseOffset >= 0 ? 1u : 0u)) * 3 + tex;
}

// Whether an effect's vertex shader can blend this layout's vertices with the matrix palette: weights and indices.
inline bool HasBlending(const FvfLayout& layout)
{
    return layout.weightCount > 0 && layout.indicesOffset >= 0;
}

// Index into kEffectVertex: defines SKINNED, NORMAL, TEXCOORD (each 0 or 1), the last one fastest. An element the
// layout lacks is not read (the shader uses D3D9's defaults for it).
inline std::size_t EffectShaderIndex(const FvfLayout& layout)
{
    return ((HasBlending(layout) ? 1u : 0u) * 2 + (layout.normalOffset >= 0 ? 1u : 0u)) * 2 +
           (layout.texCount > 0 ? 1u : 0u);
}

}  // namespace x2::d3d9
