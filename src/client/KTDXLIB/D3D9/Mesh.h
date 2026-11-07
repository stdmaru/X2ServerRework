#pragma once
// D3DX's mesh (ID3DXMesh) on KTDXLIB's device: a vertex buffer, an index buffer and a per-face attribute buffer, with the
// operations the engine calls (clone, optimize, weld, adjacency, intersection).
// docs/porting/client-d3d9.md#meshes-2026-11-07-clientx-loader

#include "D3DXTypes.h"
#include "Device.h"

#include <memory>
#include <vector>

namespace x2::d3d9 {

// Where each element of an FVF vertex sits: -1 for an element the format does not have.
struct VertexElements {
    UINT size = 0;
    int position = -1;              // 3 floats (XYZRHW and XYZW: 4)
    UINT positionFloats = 3;
    int weights = -1;               // blend weights after the position (floats)
    UINT weightCount = 0;
    int indices = -1;               // LASTBETA_UBYTE4/D3DCOLOR: the blend indices, the position's last DWORD
    bool indicesAreColor = false;   // LASTBETA_D3DCOLOR
    int normal = -1;
    int pointSize = -1;
    int diffuse = -1;
    int specular = -1;
    UINT texCount = 0;
    int tex[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    UINT texFloats[8] = {};
};

VertexElements DescribeVertex(DWORD fvf);
// One vertex from one layout into another: shared elements copied, the others zero (diffuse white).
void ConvertVertex(const std::byte* in, const VertexElements& from, std::byte* out, const VertexElements& to);

class Mesh final : public ID3DXMesh {
public:
    // Null when the arguments are invalid or the device cannot create the buffers.
    static Mesh* Create(Device* device, DWORD faces, DWORD vertices, DWORD options, DWORD fvf);

    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override;
    HRESULT DrawSubset(DWORD attribId) override;
    DWORD GetNumFaces() override { return m_faces; }
    DWORD GetNumVertices() override { return m_vertices; }
    DWORD GetFVF() override { return m_fvf; }
    HRESULT GetDeclaration(D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE]) override;
    DWORD GetNumBytesPerVertex() override { return m_stride; }
    DWORD GetOptions() override { return m_options; }
    HRESULT CloneMeshFVF(DWORD options, DWORD fvf, Device* device, LPD3DXMESH* cloneMesh) override;
    // No reference is added: buffers are not reference counted (T38).
    HRESULT GetVertexBuffer(VertexBuffer** vertexBuffer) override;
    HRESULT GetIndexBuffer(IndexBuffer** indexBuffer) override;
    HRESULT LockVertexBuffer(DWORD flags, LPVOID* data) override;
    HRESULT UnlockVertexBuffer() override;
    HRESULT LockIndexBuffer(DWORD flags, LPVOID* data) override;
    HRESULT UnlockIndexBuffer() override;
    HRESULT GetAttributeTable(D3DXATTRIBUTERANGE* table, DWORD* tableSize) override;
    HRESULT GenerateAdjacency(FLOAT epsilon, DWORD* adjacency) override;
    HRESULT UpdateSemantics(D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE]) override;
    HRESULT LockAttributeBuffer(DWORD flags, DWORD** data) override;
    HRESULT UnlockAttributeBuffer() override;
    HRESULT Optimize(DWORD flags, const DWORD* adjacencyIn, DWORD* adjacencyOut, DWORD* faceRemap,
                     LPD3DXBUFFER* vertexRemap, LPD3DXMESH* optMesh) override;

    // --- For the D3DX functions and tests ---
    Device* GetDevice() const { return m_device; }
    bool Uses32BitIndices() const { return (m_options & D3DXMESH_32BIT) != 0; }
    // Copies of the buffers (indices widened to 32 bits).
    std::vector<std::byte> Vertices() const;
    std::vector<std::uint32_t> Indices() const;
    const std::vector<DWORD>& Attributes() const { return m_attributes; }
    // Replaces the contents: vertex bytes, three indices per face, one attribute per face. The counts must match the
    // mesh's; the attribute table is kept.
    bool SetContents(const std::vector<std::byte>& vertices, const std::vector<std::uint32_t>& indices,
                     const std::vector<DWORD>& attributes);
    void SetAttributeTable(std::vector<D3DXATTRIBUTERANGE> table) { m_table = std::move(table); }
    // Shrinks the mesh to `vertices` vertices and `faces` faces (D3DXWeldVertices).
    bool Resize(DWORD faces, DWORD vertices);

private:
    Mesh() = default;
    bool DrawRange(DWORD faceStart, DWORD faceCount, DWORD vertexStart, DWORD vertexCount);

    Device* m_device = nullptr;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    std::vector<DWORD> m_attributes;
    std::vector<D3DXATTRIBUTERANGE> m_table;
    DWORD m_faces = 0, m_vertices = 0, m_options = 0, m_fvf = 0, m_stride = 0;
    ULONG m_references = 1;
};

// Point representatives: for each vertex, the first vertex at the same position (within epsilon on each axis).
std::vector<DWORD> PointReps(const std::vector<std::byte>& vertices, UINT stride, DWORD count, float epsilon);
// Three neighbors per face (0xFFFFFFFF for none), faces sharing an edge through their point representatives.
std::vector<DWORD> Adjacency(const std::vector<std::uint32_t>& indices, const std::vector<DWORD>& pointReps);

}  // namespace x2::d3d9
