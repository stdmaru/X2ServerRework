#pragma once
// D3DX's skin info (ID3DXSkinInfo): bones with names, offset matrices and vertex influences, and the indexed blended
// mesh the engine draws skinned meshes from. docs/porting/client-d3d9.md#skinned-meshes-2026-11-07-clientx-skin

#include "D3DXTypes.h"

#include <string>
#include <vector>

namespace x2::d3d9 {

class SkinInfo final : public ID3DXSkinInfo {
public:
    SkinInfo(DWORD vertices, DWORD fvf, DWORD bones) : m_vertices(vertices), m_fvf(fvf), m_bones(bones) {}

    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override;
    HRESULT SetBoneInfluence(DWORD bone, DWORD count, const DWORD* vertices, const FLOAT* weights) override;
    DWORD GetNumBoneInfluences(DWORD bone) override;
    HRESULT GetBoneInfluence(DWORD bone, DWORD* vertices, FLOAT* weights) override;
    DWORD GetNumBones() override { return static_cast<DWORD>(m_bones.size()); }
    HRESULT SetBoneName(DWORD bone, LPCSTR name) override;
    LPCSTR GetBoneName(DWORD bone) override;
    HRESULT SetBoneOffsetMatrix(DWORD bone, const D3DXMATRIX* transform) override;
    LPD3DXMATRIX GetBoneOffsetMatrix(DWORD bone) override;
    DWORD GetFVF() override { return m_fvf; }
    HRESULT Remap(DWORD vertices, DWORD* vertexRemap) override;
    HRESULT ConvertToIndexedBlendedMesh(LPD3DXMESH mesh, DWORD options, DWORD paletteSize, const DWORD* adjacencyIn,
                                        LPDWORD adjacencyOut, DWORD* faceRemap, LPD3DXBUFFER* vertexRemap,
                                        DWORD* maxVertexInfluences, DWORD* boneCombinationCount,
                                        LPD3DXBUFFER* boneCombinations, LPD3DXMESH* blendedMesh) override;

    DWORD VertexCount() const { return m_vertices; }

private:
    struct Bone {
        std::string name;
        bool named = false;
        D3DXMATRIX offset{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        std::vector<DWORD> vertices;
        std::vector<float> weights;
    };

    DWORD m_vertices;
    DWORD m_fvf;
    std::vector<Bone> m_bones;
    ULONG m_references = 1;
};

}  // namespace x2::d3d9
