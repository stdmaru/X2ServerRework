// D3DX's buffers and frame hierarchies on KTDXLIB's D3D9 device (decision T38). Meshes are in Mesh.cpp and XMesh.cpp,
// skin info in SkinInfo.cpp, animation in Animation.cpp, effects in Effect.cpp. See docs/porting/client-d3d9.md.

#include "D3DXTypes.h"
#include "Device.h"

#include <cstring>
#include <vector>

namespace {

class Buffer final : public ID3DXBuffer {
public:
    explicit Buffer(DWORD size) : m_bytes(size) {}
    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override
    {
        const ULONG references = --m_references;
        if (references == 0) {
            delete this;
        }
        return references;
    }
    LPVOID GetBufferPointer() override { return m_bytes.data(); }
    DWORD GetBufferSize() override { return static_cast<DWORD>(m_bytes.size()); }

private:
    std::vector<std::byte> m_bytes;
    ULONG m_references = 1;
};

}  // namespace

HRESULT D3DXCreateBuffer(DWORD numBytes, LPD3DXBUFFER* ppBuffer)
{
    if (ppBuffer == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    *ppBuffer = new Buffer(numBytes);
    return D3D_OK;
}

HRESULT D3DXFrameDestroy(LPD3DXFRAME pFrameRoot, LPD3DXALLOCATEHIERARCHY pAlloc)
{
    if (pFrameRoot == nullptr || pAlloc == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    if (pFrameRoot->pFrameSibling != nullptr) {
        D3DXFrameDestroy(pFrameRoot->pFrameSibling, pAlloc);
    }
    if (pFrameRoot->pFrameFirstChild != nullptr) {
        D3DXFrameDestroy(pFrameRoot->pFrameFirstChild, pAlloc);
    }
    for (LPD3DXMESHCONTAINER container = pFrameRoot->pMeshContainer; container != nullptr;) {
        LPD3DXMESHCONTAINER next = container->pNextMeshContainer;
        pAlloc->DestroyMeshContainer(container);
        container = next;
    }
    return pAlloc->DestroyFrame(pFrameRoot);
}

LPD3DXFRAME D3DXFrameFind(const D3DXFRAME* pFrameRoot, LPCSTR Name)
{
    for (const D3DXFRAME* frame = pFrameRoot; frame != nullptr; frame = frame->pFrameSibling) {
        if ((Name == nullptr && frame->Name == nullptr) ||
            (Name != nullptr && frame->Name != nullptr && std::strcmp(frame->Name, Name) == 0)) {
            return const_cast<LPD3DXFRAME>(frame);
        }
        if (LPD3DXFRAME found = D3DXFrameFind(frame->pFrameFirstChild, Name); found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

UINT D3DXFrameNumNamedMatrices(const D3DXFRAME* pFrameRoot)
{
    UINT count = 0;
    for (const D3DXFRAME* frame = pFrameRoot; frame != nullptr; frame = frame->pFrameSibling) {
        if (frame->Name != nullptr) {
            ++count;
        }
        count += D3DXFrameNumNamedMatrices(frame->pFrameFirstChild);
    }
    return count;
}

namespace {

// The positions of every mesh under `frame` (its siblings included), moved by the frames' matrices.
void FramePositions(const D3DXFRAME* frame, const D3DXMATRIX& parent, std::vector<D3DXVECTOR3>& out)
{
    for (; frame != nullptr; frame = frame->pFrameSibling) {
        D3DXMATRIX transform;
        D3DXMatrixMultiply(&transform, &frame->TransformationMatrix, &parent);
        for (const D3DXMESHCONTAINER* container = frame->pMeshContainer; container != nullptr;
             container = container->pNextMeshContainer) {
            LPD3DXMESH mesh = container->MeshData.Type == D3DXMESHTYPE_MESH ? container->MeshData.pMesh : nullptr;
            void* data = nullptr;
            if (mesh == nullptr || FAILED(mesh->LockVertexBuffer(D3DLOCK_READONLY, &data))) {
                continue;
            }
            const DWORD stride = mesh->GetNumBytesPerVertex();
            for (DWORD v = 0; v < mesh->GetNumVertices(); ++v) {
                D3DXVECTOR3 p;
                std::memcpy(&p, static_cast<const std::byte*>(data) + std::size_t{v} * stride, sizeof(p));
                D3DXVec3TransformCoord(&p, &p, &transform);
                out.push_back(p);
            }
            mesh->UnlockVertexBuffer();
        }
        FramePositions(frame->pFrameFirstChild, transform, out);
    }
}

}  // namespace

// The sphere around the centroid of every vertex of the hierarchy, in the root's space.
HRESULT D3DXFrameCalculateBoundingSphere(const D3DXFRAME* pFrameRoot, D3DXVECTOR3* pObjectCenter,
                                         FLOAT* pObjectRadius)
{
    if (pFrameRoot == nullptr || pObjectCenter == nullptr || pObjectRadius == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    std::vector<D3DXVECTOR3> positions;
    FramePositions(pFrameRoot, identity, positions);
    *pObjectCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    *pObjectRadius = 0.0f;
    if (positions.empty()) {
        return D3D_OK;
    }
    return D3DXComputeBoundingSphere(positions.data(), static_cast<DWORD>(positions.size()), sizeof(D3DXVECTOR3),
                                     pObjectCenter, pObjectRadius);
}


