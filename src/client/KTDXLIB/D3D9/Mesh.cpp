// D3DX's mesh on KTDXLIB's device and the mesh functions that work on it: D3DXCreateMeshFVF, D3DXWeldVertices,
// D3DXIntersect. docs/porting/client-d3d9.md#meshes-2026-11-07-clientx-loader

#include "Mesh.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
#include <numeric>
#include <unordered_map>

namespace x2::d3d9 {
namespace {

constexpr DWORD kNone = 0xFFFFFFFF;
constexpr DWORD kOptimizeFlags = D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE |
                                 D3DXMESHOPT_STRIPREORDER | D3DXMESHOPT_IGNOREVERTS | 0x20000000 | 0x40000000;
constexpr BYTE kDeclFloat1 = 0, kDeclColor = 4, kDeclUbyte4 = 5, kDeclUnused = 17;
constexpr BYTE kUsagePosition = 0, kUsageBlendWeight = 1, kUsageBlendIndices = 2, kUsageNormal = 3, kUsagePSize = 4,
               kUsageTexcoord = 5, kUsagePositionT = 9, kUsageColor = 10;

float ReadFloat(const std::byte* vertex, int offset)
{
    float value;
    std::memcpy(&value, vertex + offset, 4);
    return value;
}

void WriteDword(std::byte* vertex, int offset, DWORD value) { std::memcpy(vertex + offset, &value, 4); }

std::vector<D3DXATTRIBUTERANGE> TableOf(const std::vector<std::uint32_t>& indices, const std::vector<DWORD>& attributes)
{
    std::vector<D3DXATTRIBUTERANGE> table;
    for (DWORD face = 0; face < attributes.size();) {
        D3DXATTRIBUTERANGE range{attributes[face], face, 0, kNone, 0};
        DWORD last = 0;
        for (; face < attributes.size() && attributes[face] == range.AttribId; ++face, ++range.FaceCount) {
            for (int corner = 0; corner < 3; ++corner) {
                const DWORD v = indices[face * 3 + corner];
                range.VertexStart = std::min(range.VertexStart, v);
                last = std::max(last, v);
            }
        }
        range.VertexCount = last - range.VertexStart + 1;
        table.push_back(range);
    }
    return table;
}

}  // namespace

VertexElements DescribeVertex(DWORD fvf)
{
    VertexElements e;
    UINT offset = 0;
    switch (fvf & D3DFVF_POSITION_MASK) {
    case D3DFVF_XYZ: e.position = 0; offset = 12; break;
    case D3DFVF_XYZRHW:
    case D3DFVF_XYZW: e.position = 0; e.positionFloats = 4; offset = 16; break;
    case D3DFVF_XYZB1:
    case D3DFVF_XYZB2:
    case D3DFVF_XYZB3:
    case D3DFVF_XYZB4:
    case D3DFVF_XYZB5:
        e.position = 0;
        e.weights = 12;
        e.weightCount = ((fvf & D3DFVF_POSITION_MASK) - D3DFVF_XYZB1) / 2 + 1;
        offset = 12 + 4 * e.weightCount;
        break;
    default: return {};
    }
    if (fvf & D3DFVF_NORMAL) {
        e.normal = static_cast<int>(offset);
        offset += 12;
    }
    if (fvf & D3DFVF_PSIZE) {
        e.pointSize = static_cast<int>(offset);
        offset += 4;
    }
    if (fvf & D3DFVF_DIFFUSE) {
        e.diffuse = static_cast<int>(offset);
        offset += 4;
    }
    if (fvf & D3DFVF_SPECULAR) {
        e.specular = static_cast<int>(offset);
        offset += 4;
    }
    e.texCount = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    if (e.texCount > 8) {
        return {};
    }
    for (UINT i = 0; i < e.texCount; ++i) {
        switch ((fvf >> (16 + i * 2)) & 3) {
        case D3DFVF_TEXTUREFORMAT1: e.texFloats[i] = 1; break;
        case D3DFVF_TEXTUREFORMAT3: e.texFloats[i] = 3; break;
        case D3DFVF_TEXTUREFORMAT4: e.texFloats[i] = 4; break;
        default: e.texFloats[i] = 2; break;
        }
        e.tex[i] = static_cast<int>(offset);
        offset += 4 * e.texFloats[i];
    }
    e.size = offset;
    return e;
}

void ConvertVertex(const std::byte* in, const VertexElements& from, std::byte* out, const VertexElements& to)
{
    std::memset(out, 0, to.size);
    auto copy = [&](int fromOffset, int toOffset, UINT bytes) {
        if (fromOffset >= 0 && toOffset >= 0) {
            std::memcpy(out + toOffset, in + fromOffset, bytes);
        }
    };
    copy(from.position, to.position, 4 * std::min(from.positionFloats, to.positionFloats));
    copy(from.weights, to.weights, 4 * std::min(from.weightCount, to.weightCount));
    copy(from.normal, to.normal, 12);
    copy(from.pointSize, to.pointSize, 4);
    if (to.diffuse >= 0) {
        if (from.diffuse >= 0) {
            copy(from.diffuse, to.diffuse, 4);
        } else {
            WriteDword(out, to.diffuse, 0xFFFFFFFF);
        }
    }
    copy(from.specular, to.specular, 4);
    for (UINT i = 0; i < to.texCount && i < from.texCount; ++i) {
        copy(from.tex[i], to.tex[i], 4 * std::min(from.texFloats[i], to.texFloats[i]));
    }
}

// --- Mesh -----------------------------------------------------------------------------------------------------------

Mesh* Mesh::Create(Device* device, DWORD faces, DWORD vertices, DWORD options, DWORD fvf)
{
    const VertexElements elements = DescribeVertex(fvf);
    const bool wide = (options & D3DXMESH_32BIT) != 0;
    if (!device || faces == 0 || vertices == 0 || elements.size == 0 || (!wide && vertices > 0xFFFF)) {
        return nullptr;
    }
    std::unique_ptr<Mesh> mesh(new Mesh());
    mesh->m_device = device;
    mesh->m_fvf = fvf;
    mesh->m_stride = elements.size;
    mesh->m_options = options & ~kOptimizeFlags;
    if (!mesh->Resize(faces, vertices)) {
        return nullptr;
    }
    return mesh.release();
}

bool Mesh::Resize(DWORD faces, DWORD vertices)
{
    const bool wide = Uses32BitIndices();
    auto vertexBuffer = m_device->CreateVertexBuffer(vertices * m_stride, 0, m_fvf, D3DPOOL_MANAGED);
    auto indexBuffer = m_device->CreateIndexBuffer(faces * 3 * (wide ? 4 : 2), 0, wide ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
                                                   D3DPOOL_MANAGED);
    if (!vertexBuffer || !indexBuffer) {
        return false;
    }
    m_vertexBuffer = std::move(vertexBuffer);
    m_indexBuffer = std::move(indexBuffer);
    m_faces = faces;
    m_vertices = vertices;
    m_attributes.assign(faces, 0);
    return true;
}

ULONG Mesh::Release()
{
    const ULONG references = --m_references;
    if (references == 0) {
        delete this;
    }
    return references;
}

bool Mesh::DrawRange(DWORD faceStart, DWORD faceCount, DWORD vertexStart, DWORD vertexCount)
{
    m_device->SetFVF(m_fvf);
    m_device->SetStreamSource(0, m_vertexBuffer.get(), 0, m_stride);
    m_device->SetIndices(m_indexBuffer.get());
    return m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vertexStart, vertexCount, faceStart * 3, faceCount);
}

HRESULT Mesh::DrawSubset(DWORD attribId)
{
    bool ok = true;
    if (!m_table.empty()) {
        for (const D3DXATTRIBUTERANGE& range : m_table) {
            if (range.AttribId == attribId && range.FaceCount > 0) {
                ok = DrawRange(range.FaceStart, range.FaceCount, range.VertexStart, range.VertexCount) && ok;
            }
        }
        return ok ? D3D_OK : D3DERR_INVALIDCALL;
    }
    // No attribute table: every run of faces with that attribute.
    for (DWORD face = 0; face < m_faces;) {
        if (m_attributes[face] != attribId) {
            ++face;
            continue;
        }
        const DWORD start = face;
        while (face < m_faces && m_attributes[face] == attribId) {
            ++face;
        }
        ok = DrawRange(start, face - start, 0, m_vertices) && ok;
    }
    return ok ? D3D_OK : D3DERR_INVALIDCALL;
}

HRESULT Mesh::GetDeclaration(D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE])
{
    if (!declaration) {
        return D3DERR_INVALIDCALL;
    }
    const VertexElements e = DescribeVertex(m_fvf);
    int n = 0;
    auto add = [&](int offset, BYTE type, BYTE usage, BYTE index) {
        declaration[n++] = {0, static_cast<WORD>(offset), type, 0, usage, index};
    };
    const bool rhw = (m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW;
    add(e.position, static_cast<BYTE>(kDeclFloat1 + e.positionFloats - 1), rhw ? kUsagePositionT : kUsagePosition, 0);
    if (e.weightCount > 0) {
        const bool lastIsIndices = (m_fvf & (D3DFVF_LASTBETA_UBYTE4 | D3DFVF_LASTBETA_D3DCOLOR)) != 0;
        const UINT weights = e.weightCount - (lastIsIndices ? 1 : 0);
        if (weights > 0) {
            add(e.weights, static_cast<BYTE>(kDeclFloat1 + weights - 1), kUsageBlendWeight, 0);
        }
        if (lastIsIndices) {
            add(e.weights + static_cast<int>(4 * weights),
                (m_fvf & D3DFVF_LASTBETA_D3DCOLOR) ? kDeclColor : kDeclUbyte4, kUsageBlendIndices, 0);
        }
    }
    if (e.normal >= 0) {
        add(e.normal, static_cast<BYTE>(kDeclFloat1 + 2), kUsageNormal, 0);
    }
    if (e.pointSize >= 0) {
        add(e.pointSize, kDeclFloat1, kUsagePSize, 0);
    }
    if (e.diffuse >= 0) {
        add(e.diffuse, kDeclColor, kUsageColor, 0);
    }
    if (e.specular >= 0) {
        add(e.specular, kDeclColor, kUsageColor, 1);
    }
    for (UINT i = 0; i < e.texCount; ++i) {
        add(e.tex[i], static_cast<BYTE>(kDeclFloat1 + e.texFloats[i] - 1), kUsageTexcoord, static_cast<BYTE>(i));
    }
    declaration[n] = {0xFF, 0, kDeclUnused, 0, 0, 0};
    return D3D_OK;
}

HRESULT Mesh::UpdateSemantics(D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE])
{
    // The layout is the FVF's; only the meaning of elements can change, which the device does not use (T37).
    return declaration ? D3D_OK : D3DERR_INVALIDCALL;
}

std::vector<std::byte> Mesh::Vertices() const
{
    void* data = nullptr;
    if (!m_vertexBuffer->Lock(0, 0, &data, D3DLOCK_READONLY)) {
        return {};
    }
    const auto* bytes = static_cast<const std::byte*>(data);
    std::vector<std::byte> out(bytes, bytes + static_cast<std::size_t>(m_vertices) * m_stride);
    m_vertexBuffer->Unlock();
    return out;
}

std::vector<std::uint32_t> Mesh::Indices() const
{
    void* data = nullptr;
    if (!m_indexBuffer->Lock(0, 0, &data, D3DLOCK_READONLY)) {
        return {};
    }
    std::vector<std::uint32_t> out(static_cast<std::size_t>(m_faces) * 3);
    if (Uses32BitIndices()) {
        std::memcpy(out.data(), data, out.size() * 4);
    } else {
        const auto* narrow = static_cast<const std::uint16_t*>(data);
        std::copy(narrow, narrow + out.size(), out.begin());
    }
    m_indexBuffer->Unlock();
    return out;
}

bool Mesh::SetContents(const std::vector<std::byte>& vertices, const std::vector<std::uint32_t>& indices,
                       const std::vector<DWORD>& attributes)
{
    if (vertices.size() != static_cast<std::size_t>(m_vertices) * m_stride || indices.size() != m_faces * 3u ||
        attributes.size() != m_faces) {
        return false;
    }
    void* data = nullptr;
    if (!m_vertexBuffer->Lock(0, 0, &data, 0)) {
        return false;
    }
    std::memcpy(data, vertices.data(), vertices.size());
    m_vertexBuffer->Unlock();
    if (!m_indexBuffer->Lock(0, 0, &data, 0)) {
        return false;
    }
    if (Uses32BitIndices()) {
        std::memcpy(data, indices.data(), indices.size() * 4);
    } else {
        auto* narrow = static_cast<std::uint16_t*>(data);
        for (std::size_t i = 0; i < indices.size(); ++i) {
            narrow[i] = static_cast<std::uint16_t>(indices[i]);
        }
    }
    m_indexBuffer->Unlock();
    m_attributes = attributes;
    return true;
}

HRESULT Mesh::CloneMeshFVF(DWORD options, DWORD fvf, Device* device, LPD3DXMESH* cloneMesh)
{
    if (!cloneMesh) {
        return D3DERR_INVALIDCALL;
    }
    *cloneMesh = nullptr;
    Mesh* clone = Create(device ? device : m_device, m_faces, m_vertices, options, fvf);
    if (!clone) {
        return D3DERR_INVALIDCALL;
    }
    const VertexElements from = DescribeVertex(m_fvf), to = DescribeVertex(fvf);
    const std::vector<std::byte> source = Vertices();
    std::vector<std::byte> converted(static_cast<std::size_t>(m_vertices) * to.size);
    for (DWORD v = 0; v < m_vertices; ++v) {
        ConvertVertex(source.data() + static_cast<std::size_t>(v) * from.size, from,
                      converted.data() + static_cast<std::size_t>(v) * to.size, to);
    }
    if (!clone->SetContents(converted, Indices(), m_attributes)) {
        clone->Release();
        return D3DERR_INVALIDCALL;
    }
    clone->m_table = m_table;
    *cloneMesh = clone;
    return D3D_OK;
}

HRESULT Mesh::GetVertexBuffer(VertexBuffer** vertexBuffer)
{
    if (!vertexBuffer) {
        return D3DERR_INVALIDCALL;
    }
    *vertexBuffer = m_vertexBuffer.get();
    return D3D_OK;
}

HRESULT Mesh::GetIndexBuffer(IndexBuffer** indexBuffer)
{
    if (!indexBuffer) {
        return D3DERR_INVALIDCALL;
    }
    *indexBuffer = m_indexBuffer.get();
    return D3D_OK;
}

HRESULT Mesh::LockVertexBuffer(DWORD flags, LPVOID* data)
{
    return data && m_vertexBuffer->Lock(0, 0, data, flags) ? D3D_OK : D3DERR_INVALIDCALL;
}

HRESULT Mesh::UnlockVertexBuffer() { return m_vertexBuffer->Unlock() ? D3D_OK : D3DERR_INVALIDCALL; }

HRESULT Mesh::LockIndexBuffer(DWORD flags, LPVOID* data)
{
    return data && m_indexBuffer->Lock(0, 0, data, flags) ? D3D_OK : D3DERR_INVALIDCALL;
}

HRESULT Mesh::UnlockIndexBuffer() { return m_indexBuffer->Unlock() ? D3D_OK : D3DERR_INVALIDCALL; }

HRESULT Mesh::LockAttributeBuffer(DWORD /*flags*/, DWORD** data)
{
    if (!data) {
        return D3DERR_INVALIDCALL;
    }
    *data = m_attributes.data();
    return D3D_OK;
}

HRESULT Mesh::UnlockAttributeBuffer() { return D3D_OK; }

HRESULT Mesh::GetAttributeTable(D3DXATTRIBUTERANGE* table, DWORD* tableSize)
{
    if (tableSize) {
        *tableSize = static_cast<DWORD>(m_table.size());
    }
    if (table) {
        std::copy(m_table.begin(), m_table.end(), table);
    }
    return D3D_OK;
}

HRESULT Mesh::GenerateAdjacency(FLOAT epsilon, DWORD* adjacency)
{
    if (!adjacency) {
        return D3DERR_INVALIDCALL;
    }
    const std::vector<DWORD> result = Adjacency(Indices(), PointReps(Vertices(), m_stride, m_vertices, epsilon));
    std::copy(result.begin(), result.end(), adjacency);
    return D3D_OK;
}

HRESULT Mesh::Optimize(DWORD flags, const DWORD* adjacencyIn, DWORD* adjacencyOut, DWORD* faceRemap,
                       LPD3DXBUFFER* vertexRemap, LPD3DXMESH* optMesh)
{
    if (!optMesh) {
        return D3DERR_INVALIDCALL;
    }
    *optMesh = nullptr;
    if (vertexRemap) {
        *vertexRemap = nullptr;
    }
    const std::vector<std::byte> vertices = Vertices();
    const std::vector<std::uint32_t> indices = Indices();

    // Faces grouped by attribute (stable): VERTEXCACHE and STRIPREORDER sort by attribute too.
    std::vector<DWORD> faces(m_faces);
    std::iota(faces.begin(), faces.end(), 0);
    const bool sort = (flags & (D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_STRIPREORDER)) != 0;
    if (sort) {
        std::stable_sort(faces.begin(), faces.end(),
                         [&](DWORD a, DWORD b) { return m_attributes[a] < m_attributes[b]; });
    }
    // Vertices in the order the faces first use them, unused ones dropped.
    std::vector<DWORD> newOfOld(m_vertices, kNone), oldOfNew;
    const bool reorder = !(flags & D3DXMESHOPT_IGNOREVERTS) &&
                         (flags & (D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_STRIPREORDER)) != 0;
    if (reorder) {
        for (DWORD face : faces) {
            for (int corner = 0; corner < 3; ++corner) {
                const std::uint32_t v = indices[face * 3 + corner];
                if (newOfOld[v] == kNone) {
                    newOfOld[v] = static_cast<DWORD>(oldOfNew.size());
                    oldOfNew.push_back(v);
                }
            }
        }
    } else {
        oldOfNew.resize(m_vertices);
        std::iota(oldOfNew.begin(), oldOfNew.end(), 0);
        newOfOld = oldOfNew;
    }
    if (oldOfNew.empty()) {
        return D3DERR_INVALIDCALL;
    }
    DWORD options = flags & ~kOptimizeFlags;
    if (oldOfNew.size() > 0xFFFF) {
        options |= D3DXMESH_32BIT;
    }
    Mesh* mesh = Create(m_device, m_faces, static_cast<DWORD>(oldOfNew.size()), options, m_fvf);
    if (!mesh) {
        return D3DERR_INVALIDCALL;
    }
    std::vector<std::byte> newVertices(oldOfNew.size() * m_stride);
    for (std::size_t v = 0; v < oldOfNew.size(); ++v) {
        std::memcpy(newVertices.data() + v * m_stride, vertices.data() + std::size_t{oldOfNew[v]} * m_stride, m_stride);
    }
    std::vector<std::uint32_t> newIndices(indices.size());
    std::vector<DWORD> newAttributes(m_faces), newFaceOfOld(m_faces);
    for (DWORD f = 0; f < m_faces; ++f) {
        newFaceOfOld[faces[f]] = f;
        newAttributes[f] = m_attributes[faces[f]];
        for (int corner = 0; corner < 3; ++corner) {
            newIndices[f * 3 + corner] = newOfOld[indices[faces[f] * 3 + corner]];
        }
    }
    if (!mesh->SetContents(newVertices, newIndices, newAttributes)) {
        mesh->Release();
        return D3DERR_INVALIDCALL;
    }
    if (sort) {
        mesh->m_table = TableOf(newIndices, newAttributes);
    }
    if (adjacencyOut) {
        std::vector<DWORD> adjacency;
        if (adjacencyIn) {
            adjacency.resize(indices.size());
            for (DWORD f = 0; f < m_faces; ++f) {
                for (int e = 0; e < 3; ++e) {
                    const DWORD old = adjacencyIn[faces[f] * 3 + e];
                    adjacency[f * 3 + e] = old < m_faces ? newFaceOfOld[old] : kNone;
                }
            }
        } else {
            adjacency = Adjacency(newIndices, PointReps(newVertices, m_stride, mesh->m_vertices, 0));
        }
        std::copy(adjacency.begin(), adjacency.end(), adjacencyOut);
    }
    if (faceRemap) {
        std::copy(faces.begin(), faces.end(), faceRemap);
    }
    if (vertexRemap && SUCCEEDED(D3DXCreateBuffer(static_cast<DWORD>(oldOfNew.size() * 4), vertexRemap))) {
        std::memcpy((*vertexRemap)->GetBufferPointer(), oldOfNew.data(), oldOfNew.size() * 4);
    }
    *optMesh = mesh;
    return D3D_OK;
}

// --- Adjacency ------------------------------------------------------------------------------------------------------

std::vector<DWORD> PointReps(const std::vector<std::byte>& vertices, UINT stride, DWORD count, float epsilon)
{
    std::vector<DWORD> reps(count);
    auto position = [&](DWORD v, int axis) {
        const float value = ReadFloat(vertices.data() + std::size_t{v} * stride, axis * 4);
        return value == 0.0f ? 0.0f : value;    // -0 and +0 are one position
    };
    if (epsilon <= 0) {
        std::unordered_map<std::uint64_t, std::vector<DWORD>> buckets;
        for (DWORD v = 0; v < count; ++v) {
            const float x = position(v, 0), y = position(v, 1), z = position(v, 2);
            const std::uint64_t key = (std::uint64_t{std::bit_cast<std::uint32_t>(x)} * 73856093u) ^
                                      (std::uint64_t{std::bit_cast<std::uint32_t>(y)} * 19349663u) ^
                                      (std::uint64_t{std::bit_cast<std::uint32_t>(z)} << 32);
            std::vector<DWORD>& bucket = buckets[key];
            reps[v] = v;
            for (DWORD other : bucket) {
                if (position(other, 0) == x && position(other, 1) == y && position(other, 2) == z) {
                    reps[v] = other;
                    break;
                }
            }
            if (reps[v] == v) {
                bucket.push_back(v);
            }
        }
        return reps;
    }
    std::vector<DWORD> order(count);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](DWORD a, DWORD b) {
        return position(a, 0) < position(b, 0) || (position(a, 0) == position(b, 0) && a < b);
    });
    std::fill(reps.begin(), reps.end(), kNone);
    for (std::size_t i = 0; i < order.size(); ++i) {
        const DWORD a = order[i];
        if (reps[a] != kNone) {
            continue;
        }
        // The group's representative is its lowest vertex.
        std::vector<DWORD> group{a};
        for (std::size_t j = i + 1; j < order.size() && position(order[j], 0) - position(a, 0) <= epsilon; ++j) {
            const DWORD b = order[j];
            if (reps[b] == kNone && std::fabs(position(b, 1) - position(a, 1)) <= epsilon &&
                std::fabs(position(b, 2) - position(a, 2)) <= epsilon) {
                group.push_back(b);
            }
        }
        const DWORD lowest = *std::min_element(group.begin(), group.end());
        for (DWORD v : group) {
            reps[v] = lowest;
        }
    }
    return reps;
}

std::vector<DWORD> Adjacency(const std::vector<std::uint32_t>& indices, const std::vector<DWORD>& pointReps)
{
    const std::size_t faces = indices.size() / 3;
    std::vector<DWORD> adjacency(indices.size(), kNone);
    std::unordered_map<std::uint64_t, std::vector<std::size_t>> edges;     // undirected edge -> face * 3 + edge
    edges.reserve(indices.size());
    for (std::size_t f = 0; f < faces; ++f) {
        for (std::size_t e = 0; e < 3; ++e) {
            const DWORD a = pointReps[indices[f * 3 + e]], b = pointReps[indices[f * 3 + (e + 1) % 3]];
            if (a != b) {
                edges[(std::uint64_t{std::min(a, b)} << 32) | std::max(a, b)].push_back(f * 3 + e);
            }
        }
    }
    for (const auto& [key, uses] : edges) {
        // Pairs, in order, of faces sharing the edge.
        for (std::size_t i = 0; i + 1 < uses.size(); i += 2) {
            const std::size_t a = uses[i], b = uses[i + 1];
            if (a / 3 != b / 3) {
                adjacency[a] = static_cast<DWORD>(b / 3);
                adjacency[b] = static_cast<DWORD>(a / 3);
            }
        }
    }
    return adjacency;
}

}  // namespace x2::d3d9

// --- D3DX functions -------------------------------------------------------------------------------------------------

using x2::d3d9::Mesh;

HRESULT D3DXCreateMeshFVF(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, x2::d3d9::Device* pD3DDevice,
                          LPD3DXMESH* ppMesh)
{
    if (!ppMesh) {
        return D3DERR_INVALIDCALL;
    }
    *ppMesh = Mesh::Create(pD3DDevice, NumFaces, NumVertices, Options, FVF);
    return *ppMesh ? D3D_OK : D3DERR_INVALIDCALL;
}

namespace {

// Whether two vertices weld: every element within its epsilon (position only with WELDALL).
bool Weldable(const std::byte* a, const std::byte* b, const x2::d3d9::VertexElements& e,
              const D3DXWELDEPSILONS& eps, bool positionOnly)
{
    auto within = [](const std::byte* p, const std::byte* q, int offset, UINT floats, float epsilon) {
        for (UINT i = 0; i < floats; ++i) {
            float x, y;
            std::memcpy(&x, p + offset + 4 * i, 4);
            std::memcpy(&y, q + offset + 4 * i, 4);
            if (!(std::fabs(x - y) <= epsilon)) {
                return false;
            }
        }
        return true;
    };
    auto color = [](const std::byte* p, const std::byte* q, int offset, float epsilon) {
        for (int i = 0; i < 4; ++i) {
            const float x = std::to_integer<int>(p[offset + i]) / 255.0f, y = std::to_integer<int>(q[offset + i]) / 255.0f;
            if (!(std::fabs(x - y) <= epsilon)) {
                return false;
            }
        }
        return true;
    };
    if (!within(a, b, e.position, e.positionFloats, eps.Position)) {
        return false;
    }
    if (positionOnly) {
        return true;
    }
    if ((e.weights >= 0 && !within(a, b, e.weights, e.weightCount, eps.BlendWeights)) ||
        (e.normal >= 0 && !within(a, b, e.normal, 3, eps.Normal)) ||
        (e.pointSize >= 0 && !within(a, b, e.pointSize, 1, eps.PSize)) ||
        (e.diffuse >= 0 && !color(a, b, e.diffuse, eps.Diffuse)) ||
        (e.specular >= 0 && !color(a, b, e.specular, eps.Specular))) {
        return false;
    }
    for (UINT i = 0; i < e.texCount; ++i) {
        if (!within(a, b, e.tex[i], e.texFloats[i], eps.Texcoord[i])) {
            return false;
        }
    }
    return true;
}

}  // namespace

HRESULT D3DXWeldVertices(LPD3DXMESH pMesh, DWORD Flags, const D3DXWELDEPSILONS* pEpsilons,
                         const DWORD* /*pAdjacencyIn*/, DWORD* pAdjacencyOut, DWORD* pFaceRemap,
                         LPD3DXBUFFER* ppVertexRemap)
{
    auto* mesh = dynamic_cast<Mesh*>(pMesh);
    if (!mesh) {
        return D3DERR_INVALIDCALL;
    }
    const D3DXWELDEPSILONS eps = pEpsilons ? *pEpsilons : D3DXWELDEPSILONS{1e-6f, 1e-6f, 1e-6f, 1e-6f, 1e-6f, 1e-6f,
                                                                          {1e-6f, 1e-6f, 1e-6f, 1e-6f, 1e-6f, 1e-6f,
                                                                           1e-6f, 1e-6f},
                                                                          1e-6f, 1e-6f, 1e-6f};
    const DWORD stride = mesh->GetNumBytesPerVertex(), count = mesh->GetNumVertices(), faces = mesh->GetNumFaces();
    const x2::d3d9::VertexElements elements = x2::d3d9::DescribeVertex(mesh->GetFVF());
    const std::vector<std::byte> vertices = mesh->Vertices();
    std::vector<std::uint32_t> indices = mesh->Indices();

    // Candidates share a position within the position epsilon; each welds to the first one it matches.
    const std::vector<DWORD> reps = x2::d3d9::PointReps(vertices, stride, count, eps.Position);
    std::vector<std::vector<DWORD>> groups(count);
    for (DWORD v = 0; v < count; ++v) {
        groups[reps[v]].push_back(v);
    }
    std::vector<DWORD> weldTo(count);
    std::iota(weldTo.begin(), weldTo.end(), 0);
    for (const std::vector<DWORD>& group : groups) {
        std::vector<DWORD> kept;
        for (DWORD v : group) {
            for (DWORD k : kept) {
                if (Weldable(vertices.data() + std::size_t{v} * stride, vertices.data() + std::size_t{k} * stride,
                             elements, eps, (Flags & D3DXWELDEPSILONS_WELDALL) != 0)) {
                    weldTo[v] = k;
                    break;
                }
            }
            if (weldTo[v] == v) {
                kept.push_back(v);
            }
        }
    }
    for (std::uint32_t& index : indices) {
        index = weldTo[index];
    }
    // Welded vertices removed: the others keep their order.
    std::vector<DWORD> newOfOld(count, 0xFFFFFFFF), oldOfNew;
    for (DWORD v = 0; v < count; ++v) {
        if ((Flags & D3DXWELDEPSILONS_DONOTREMOVEVERTICES) || weldTo[v] == v) {
            newOfOld[v] = static_cast<DWORD>(oldOfNew.size());
            oldOfNew.push_back(v);
        }
    }
    std::vector<std::byte> newVertices(oldOfNew.size() * stride);
    for (std::size_t v = 0; v < oldOfNew.size(); ++v) {
        std::memcpy(newVertices.data() + v * stride, vertices.data() + std::size_t{oldOfNew[v]} * stride, stride);
    }
    for (std::uint32_t& index : indices) {
        index = newOfOld[index];
    }
    const std::vector<DWORD> attributes = mesh->Attributes();
    std::vector<D3DXATTRIBUTERANGE> table(16);
    DWORD tableSize = 0;
    mesh->GetAttributeTable(nullptr, &tableSize);
    table.resize(tableSize);
    mesh->GetAttributeTable(table.data(), nullptr);
    if (oldOfNew.size() != count && !mesh->Resize(faces, static_cast<DWORD>(oldOfNew.size()))) {
        return E_OUTOFMEMORY;
    }
    if (!mesh->SetContents(newVertices, indices, attributes)) {
        return D3DERR_INVALIDCALL;
    }
    // The table's vertex ranges follow the new vertices.
    for (D3DXATTRIBUTERANGE& range : table) {
        DWORD first = 0xFFFFFFFF, last = 0;
        for (DWORD f = range.FaceStart; f < range.FaceStart + range.FaceCount && f < faces; ++f) {
            for (int corner = 0; corner < 3; ++corner) {
                first = std::min<DWORD>(first, indices[f * 3 + corner]);
                last = std::max<DWORD>(last, indices[f * 3 + corner]);
            }
        }
        if (first != 0xFFFFFFFF) {
            range.VertexStart = first;
            range.VertexCount = last - first + 1;
        }
    }
    mesh->SetAttributeTable(std::move(table));
    if (pAdjacencyOut) {
        const std::vector<DWORD> adjacency =
            x2::d3d9::Adjacency(indices, x2::d3d9::PointReps(newVertices, stride, mesh->GetNumVertices(), 0));
        std::copy(adjacency.begin(), adjacency.end(), pAdjacencyOut);
    }
    if (pFaceRemap) {
        std::iota(pFaceRemap, pFaceRemap + faces, 0);
    }
    if (ppVertexRemap) {
        *ppVertexRemap = nullptr;
        if (SUCCEEDED(D3DXCreateBuffer(static_cast<DWORD>(oldOfNew.size() * 4), ppVertexRemap))) {
            std::memcpy((*ppVertexRemap)->GetBufferPointer(), oldOfNew.data(), oldOfNew.size() * 4);
        }
    }
    return D3D_OK;
}

HRESULT D3DXIntersect(LPD3DXBASEMESH pMesh, const D3DXVECTOR3* pRayPos, const D3DXVECTOR3* pRayDir, BOOL* pHit,
                      DWORD* pFaceIndex, FLOAT* pU, FLOAT* pV, FLOAT* pDist, LPD3DXBUFFER* ppAllHits,
                      DWORD* pCountOfHits)
{
    auto* mesh = dynamic_cast<Mesh*>(pMesh);
    if (!mesh || !pRayPos || !pRayDir || !pHit) {
        return D3DERR_INVALIDCALL;
    }
    const DWORD stride = mesh->GetNumBytesPerVertex();
    const std::vector<std::byte> vertices = mesh->Vertices();
    const std::vector<std::uint32_t> indices = mesh->Indices();
    auto at = [&](std::uint32_t v) {
        D3DXVECTOR3 p;
        std::memcpy(&p, vertices.data() + std::size_t{v} * stride, sizeof(p));
        return p;
    };
    std::vector<D3DXINTERSECTINFO> hits;
    for (DWORD f = 0; f < mesh->GetNumFaces(); ++f) {
        // Möller-Trumbore, both sides.
        const D3DXVECTOR3 v0 = at(indices[f * 3]), v1 = at(indices[f * 3 + 1]), v2 = at(indices[f * 3 + 2]);
        const D3DXVECTOR3 e1 = v1 - v0, e2 = v2 - v0;
        D3DXVECTOR3 p, q;
        D3DXVec3Cross(&p, pRayDir, &e2);
        const float det = D3DXVec3Dot(&e1, &p);
        if (std::fabs(det) < 1e-12f) {
            continue;
        }
        const float inverse = 1.0f / det;
        const D3DXVECTOR3 t = *pRayPos - v0;
        const float u = D3DXVec3Dot(&t, &p) * inverse;
        if (u < 0.0f || u > 1.0f) {
            continue;
        }
        D3DXVec3Cross(&q, &t, &e1);
        const float v = D3DXVec3Dot(pRayDir, &q) * inverse;
        if (v < 0.0f || u + v > 1.0f) {
            continue;
        }
        const float distance = D3DXVec3Dot(&e2, &q) * inverse;
        if (distance >= 0.0f) {
            hits.push_back({f, u, v, distance});
        }
    }
    *pHit = hits.empty() ? FALSE : TRUE;
    if (!hits.empty()) {
        const auto nearest = std::min_element(hits.begin(), hits.end(),
                                              [](const auto& a, const auto& b) { return a.Dist < b.Dist; });
        if (pFaceIndex) {
            *pFaceIndex = nearest->FaceIndex;
        }
        if (pU) {
            *pU = nearest->U;
        }
        if (pV) {
            *pV = nearest->V;
        }
        if (pDist) {
            *pDist = nearest->Dist;
        }
    }
    if (pCountOfHits) {
        *pCountOfHits = static_cast<DWORD>(hits.size());
    }
    if (ppAllHits) {
        *ppAllHits = nullptr;
        if (!hits.empty() &&
            SUCCEEDED(D3DXCreateBuffer(static_cast<DWORD>(hits.size() * sizeof(D3DXINTERSECTINFO)), ppAllHits))) {
            std::memcpy((*ppAllHits)->GetBufferPointer(), hits.data(), hits.size() * sizeof(D3DXINTERSECTINFO));
        }
    }
    return D3D_OK;
}
