// D3DX's skin info and its indexed blended mesh. docs/porting/client-d3d9.md#skinned-meshes-2026-11-07-clientx-skin

#include "SkinInfo.h"

#include "Mesh.h"

#include <algorithm>
#include <cstring>
#include <map>
#include <numeric>
#include <set>

namespace x2::d3d9 {
namespace {

constexpr DWORD kNone = 0xFFFFFFFF;
constexpr std::size_t kMaxInfluences = 4;   // the blend indices are four bytes

struct Influence {
    DWORD bone;
    float weight;
};

}  // namespace

ULONG SkinInfo::Release()
{
    const ULONG references = --m_references;
    if (references == 0) {
        delete this;
    }
    return references;
}

HRESULT SkinInfo::SetBoneInfluence(DWORD bone, DWORD count, const DWORD* vertices, const FLOAT* weights)
{
    if (bone >= m_bones.size() || (count > 0 && (!vertices || !weights))) {
        return D3DERR_INVALIDCALL;
    }
    for (DWORD i = 0; i < count; ++i) {
        if (vertices[i] >= m_vertices) {
            return D3DERR_INVALIDCALL;
        }
    }
    m_bones[bone].vertices.assign(vertices, vertices + count);
    m_bones[bone].weights.assign(weights, weights + count);
    return D3D_OK;
}

DWORD SkinInfo::GetNumBoneInfluences(DWORD bone)
{
    return bone < m_bones.size() ? static_cast<DWORD>(m_bones[bone].vertices.size()) : 0;
}

HRESULT SkinInfo::GetBoneInfluence(DWORD bone, DWORD* vertices, FLOAT* weights)
{
    if (bone >= m_bones.size() || !vertices || !weights) {
        return D3DERR_INVALIDCALL;
    }
    std::copy(m_bones[bone].vertices.begin(), m_bones[bone].vertices.end(), vertices);
    std::copy(m_bones[bone].weights.begin(), m_bones[bone].weights.end(), weights);
    return D3D_OK;
}

HRESULT SkinInfo::SetBoneName(DWORD bone, LPCSTR name)
{
    if (bone >= m_bones.size() || !name) {
        return D3DERR_INVALIDCALL;
    }
    m_bones[bone].name = name;
    m_bones[bone].named = true;
    return D3D_OK;
}

LPCSTR SkinInfo::GetBoneName(DWORD bone)
{
    return bone < m_bones.size() && m_bones[bone].named ? m_bones[bone].name.c_str() : nullptr;
}

HRESULT SkinInfo::SetBoneOffsetMatrix(DWORD bone, const D3DXMATRIX* transform)
{
    if (bone >= m_bones.size() || !transform) {
        return D3DERR_INVALIDCALL;
    }
    m_bones[bone].offset = *transform;
    return D3D_OK;
}

LPD3DXMATRIX SkinInfo::GetBoneOffsetMatrix(DWORD bone)
{
    return bone < m_bones.size() ? &m_bones[bone].offset : nullptr;
}

// The new vertices' influences are their old vertices' (`vertexRemap`: new to old, as Optimize gives it).
HRESULT SkinInfo::Remap(DWORD vertices, DWORD* vertexRemap)
{
    if (!vertexRemap) {
        return D3DERR_INVALIDCALL;
    }
    std::vector<std::vector<DWORD>> newOfOld(m_vertices);
    for (DWORD v = 0; v < vertices; ++v) {
        if (vertexRemap[v] != kNone) {
            if (vertexRemap[v] >= m_vertices) {
                return D3DERR_INVALIDCALL;
            }
            newOfOld[vertexRemap[v]].push_back(v);
        }
    }
    for (Bone& bone : m_bones) {
        std::vector<DWORD> boneVertices;
        std::vector<float> boneWeights;
        for (std::size_t i = 0; i < bone.vertices.size(); ++i) {
            for (DWORD v : newOfOld[bone.vertices[i]]) {
                boneVertices.push_back(v);
                boneWeights.push_back(bone.weights[i]);
            }
        }
        bone.vertices = std::move(boneVertices);
        bone.weights = std::move(boneWeights);
    }
    m_vertices = vertices;
    return D3D_OK;
}

// Faces grouped by attribute, then into bone combinations of at most `paletteSize` bones; each combination's vertices
// copied after the previous one's, their blend indices pointing into its palette.
HRESULT SkinInfo::ConvertToIndexedBlendedMesh(LPD3DXMESH meshIn, DWORD options, DWORD paletteSize,
                                              const DWORD* adjacencyIn, LPDWORD adjacencyOut, DWORD* faceRemap,
                                              LPD3DXBUFFER* vertexRemap, DWORD* maxVertexInfluences,
                                              DWORD* boneCombinationCount, LPD3DXBUFFER* boneCombinations,
                                              LPD3DXMESH* blendedMesh)
{
    if (vertexRemap) {
        *vertexRemap = nullptr;
    }
    if (boneCombinations) {
        *boneCombinations = nullptr;
    }
    if (!blendedMesh) {
        return D3DERR_INVALIDCALL;
    }
    *blendedMesh = nullptr;
    auto* mesh = dynamic_cast<Mesh*>(meshIn);
    if (!mesh || paletteSize == 0 || m_bones.empty()) {
        return D3DERR_INVALIDCALL;
    }
    const DWORD vertexCount = mesh->GetNumVertices(), faceCount = mesh->GetNumFaces();

    // Each vertex's strongest influences (at most four); a vertex no bone moves follows bone 0.
    std::vector<std::vector<Influence>> influences(vertexCount);
    for (DWORD b = 0; b < m_bones.size(); ++b) {
        for (std::size_t i = 0; i < m_bones[b].vertices.size(); ++i) {
            const DWORD v = m_bones[b].vertices[i];
            if (v < vertexCount && m_bones[b].weights[i] > 0.0f) {
                influences[v].push_back({b, m_bones[b].weights[i]});
            }
        }
    }
    DWORD maxInfluences = 1;
    for (std::vector<Influence>& list : influences) {
        std::stable_sort(list.begin(), list.end(), [](const Influence& a, const Influence& b) {
            return a.weight > b.weight;
        });
        if (list.size() > kMaxInfluences) {
            list.resize(kMaxInfluences);
        }
        if (list.empty()) {
            list.push_back({0, 1.0f});
        }
        maxInfluences = std::max<DWORD>(maxInfluences, static_cast<DWORD>(list.size()));
    }

    const std::vector<std::uint32_t> indices = mesh->Indices();
    const std::vector<DWORD>& attributes = mesh->Attributes();
    std::vector<DWORD> faces(faceCount);
    std::iota(faces.begin(), faces.end(), 0);
    std::stable_sort(faces.begin(), faces.end(), [&](DWORD a, DWORD b) { return attributes[a] < attributes[b]; });

    struct Combination {
        DWORD attribute;
        DWORD faceStart, faceCount = 0;
        std::vector<DWORD> bones;   // palette slot -> bone
    };
    std::vector<Combination> combinations;
    for (DWORD f = 0; f < faceCount; ++f) {
        const DWORD face = faces[f];
        std::set<DWORD> needed;
        for (int corner = 0; corner < 3; ++corner) {
            for (const Influence& influence : influences[indices[face * 3 + corner]]) {
                needed.insert(influence.bone);
            }
        }
        if (needed.size() > paletteSize) {
            return D3DERR_INVALIDCALL;
        }
        bool fits = !combinations.empty() && combinations.back().attribute == attributes[face];
        if (fits) {
            const std::vector<DWORD>& bones = combinations.back().bones;
            std::size_t added = 0;
            for (DWORD bone : needed) {
                added += std::find(bones.begin(), bones.end(), bone) == bones.end() ? 1 : 0;
            }
            fits = bones.size() + added <= paletteSize;
        }
        if (!fits) {
            combinations.push_back({attributes[face], f, 0, {}});
        }
        Combination& combination = combinations.back();
        for (DWORD bone : needed) {
            if (std::find(combination.bones.begin(), combination.bones.end(), bone) == combination.bones.end()) {
                combination.bones.push_back(bone);
            }
        }
        ++combination.faceCount;
    }

    // Vertices: per combination, in first-use order.
    std::vector<DWORD> oldOfNew, newIndices(indices.size()), newAttributes(faceCount);
    std::vector<D3DXATTRIBUTERANGE> table;
    for (DWORD c = 0; c < combinations.size(); ++c) {
        const Combination& combination = combinations[c];
        std::map<DWORD, DWORD> copies;
        const DWORD vertexStart = static_cast<DWORD>(oldOfNew.size());
        for (DWORD f = combination.faceStart; f < combination.faceStart + combination.faceCount; ++f) {
            newAttributes[f] = c;
            for (int corner = 0; corner < 3; ++corner) {
                const DWORD old = indices[faces[f] * 3 + corner];
                const auto [it, added] = copies.try_emplace(old, static_cast<DWORD>(oldOfNew.size()));
                if (added) {
                    oldOfNew.push_back(old);
                }
                newIndices[f * 3 + corner] = it->second;
            }
        }
        table.push_back({c, combination.faceStart, combination.faceCount, vertexStart,
                         static_cast<DWORD>(oldOfNew.size()) - vertexStart});
    }

    const DWORD fvf = (mesh->GetFVF() & ~(D3DFVF_POSITION_MASK | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_LASTBETA_D3DCOLOR)) |
                      (D3DFVF_XYZB1 + 2 * (maxInfluences - 1)) | D3DFVF_LASTBETA_UBYTE4;
    if (oldOfNew.size() > 0xFFFF) {
        options |= D3DXMESH_32BIT;
    }
    Mesh* out = Mesh::Create(mesh->GetDevice(), faceCount, static_cast<DWORD>(oldOfNew.size()), options, fvf);
    if (!out) {
        return D3DERR_INVALIDCALL;
    }
    const VertexElements from = DescribeVertex(mesh->GetFVF()), to = DescribeVertex(fvf);
    const std::vector<std::byte> source = mesh->Vertices();
    std::vector<std::byte> vertices(oldOfNew.size() * to.size);
    for (DWORD c = 0; c < combinations.size(); ++c) {
        const std::vector<DWORD>& palette = combinations[c].bones;
        for (DWORD v = table[c].VertexStart; v < table[c].VertexStart + table[c].VertexCount; ++v) {
            std::byte* vertex = vertices.data() + std::size_t{v} * to.size;
            ConvertVertex(source.data() + std::size_t{oldOfNew[v]} * from.size, from, vertex, to);
            const std::vector<Influence>& list = influences[oldOfNew[v]];
            // The first maxInfluences - 1 weights are stored, the last one is what they leave of 1.
            std::memset(vertex + to.weights, 0, 4 * to.weightCount);
            for (std::size_t i = 0; i < list.size() && i < to.weightCount; ++i) {
                std::memcpy(vertex + to.weights + 4 * i, &list[i].weight, 4);
            }
            std::uint8_t slots[4] = {};
            for (std::size_t i = 0; i < list.size(); ++i) {
                slots[i] = static_cast<std::uint8_t>(
                    std::find(palette.begin(), palette.end(), list[i].bone) - palette.begin());
            }
            std::memcpy(vertex + to.indices, slots, 4);
        }
    }
    std::vector<std::uint32_t> outIndices(newIndices.begin(), newIndices.end());
    if (!out->SetContents(vertices, outIndices, newAttributes)) {
        out->Release();
        return D3DERR_INVALIDCALL;
    }
    out->SetAttributeTable(table);

    if (adjacencyOut) {
        std::vector<DWORD> adjacency;
        if (adjacencyIn) {
            std::vector<DWORD> newFaceOfOld(faceCount);
            for (DWORD f = 0; f < faceCount; ++f) {
                newFaceOfOld[faces[f]] = f;
            }
            adjacency.resize(indices.size());
            for (DWORD f = 0; f < faceCount; ++f) {
                for (int e = 0; e < 3; ++e) {
                    const DWORD old = adjacencyIn[faces[f] * 3 + e];
                    adjacency[f * 3 + e] = old < faceCount ? newFaceOfOld[old] : kNone;
                }
            }
        } else {
            adjacency = Adjacency(outIndices, PointReps(vertices, to.size, out->GetNumVertices(), 0));
        }
        std::copy(adjacency.begin(), adjacency.end(), adjacencyOut);
    }
    if (faceRemap) {
        std::copy(faces.begin(), faces.end(), faceRemap);
    }
    if (vertexRemap && SUCCEEDED(D3DXCreateBuffer(static_cast<DWORD>(oldOfNew.size() * 4), vertexRemap))) {
        std::memcpy((*vertexRemap)->GetBufferPointer(), oldOfNew.data(), oldOfNew.size() * 4);
    }
    if (maxVertexInfluences) {
        *maxVertexInfluences = maxInfluences;
    }
    if (boneCombinationCount) {
        *boneCombinationCount = static_cast<DWORD>(combinations.size());
    }
    if (boneCombinations) {
        // The entries, then each one's palette (unused slots 0xFFFFFFFF).
        const std::size_t entries = combinations.size() * sizeof(D3DXBONECOMBINATION);
        if (SUCCEEDED(D3DXCreateBuffer(static_cast<DWORD>(entries + combinations.size() * paletteSize * 4),
                                       boneCombinations))) {
            auto* bytes = static_cast<std::byte*>((*boneCombinations)->GetBufferPointer());
            auto* entry = reinterpret_cast<D3DXBONECOMBINATION*>(bytes);
            auto* palettes = reinterpret_cast<DWORD*>(bytes + entries);
            for (std::size_t c = 0; c < combinations.size(); ++c) {
                DWORD* palette = palettes + c * paletteSize;
                std::fill(palette, palette + paletteSize, kNone);
                std::copy(combinations[c].bones.begin(), combinations[c].bones.end(), palette);
                entry[c] = {combinations[c].attribute, table[c].FaceStart, table[c].FaceCount, table[c].VertexStart,
                            table[c].VertexCount, palette};
            }
        }
    }
    *blendedMesh = out;
    return D3D_OK;
}

}  // namespace x2::d3d9

HRESULT D3DXCreateSkinInfoFVF(DWORD NumVertices, DWORD FVF, DWORD NumBones, LPD3DXSKININFO* ppSkinInfo)
{
    if (!ppSkinInfo) {
        return D3DERR_INVALIDCALL;
    }
    *ppSkinInfo = new x2::d3d9::SkinInfo(NumVertices, FVF, NumBones);
    return D3D_OK;
}
