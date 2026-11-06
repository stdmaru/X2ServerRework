// D3DX's buffers, frame hierarchies, meshes, animation and effects on KTDXLIB's D3D9 device (decision T38). Buffers
// and walking a frame hierarchy work; what needs `.x` meshes, animation data or effects fails until the `.x` step and
// the effects on KTDXLIB's own shaders (T37). See docs/porting/client-d3d9.md.

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

template <class T>
HRESULT NotImplemented(T** ppObject)
{
    if (ppObject != nullptr) {
        *ppObject = nullptr;
    }
    return E_NOTIMPL;
}

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

HRESULT D3DXLoadMeshFromXInMemory(LPCVOID /*Memory*/, DWORD /*SizeOfMemory*/, DWORD /*Options*/,
                                  x2::d3d9::Device* /*pD3DDevice*/, LPD3DXBUFFER* ppAdjacency,
                                  LPD3DXBUFFER* ppMaterials, LPD3DXBUFFER* ppEffectInstances,
                                  DWORD* pNumMaterials, LPD3DXMESH* ppMesh)
{
    NotImplemented(ppAdjacency);
    NotImplemented(ppMaterials);
    NotImplemented(ppEffectInstances);
    if (pNumMaterials != nullptr) {
        *pNumMaterials = 0;
    }
    return NotImplemented(ppMesh);
}

HRESULT D3DXLoadMeshHierarchyFromXInMemory(LPCVOID /*Memory*/, DWORD /*SizeOfMemory*/, DWORD /*MeshOptions*/,
                                           x2::d3d9::Device* /*pDevice*/, LPD3DXALLOCATEHIERARCHY /*pAlloc*/,
                                           ID3DXLoadUserData* /*pUserDataLoader*/, LPD3DXFRAME* ppFrameHierarchy,
                                           LPD3DXANIMATIONCONTROLLER* ppAnimController)
{
    NotImplemented(ppAnimController);
    return NotImplemented(ppFrameHierarchy);
}

HRESULT D3DXCreateMeshFVF(DWORD /*NumFaces*/, DWORD /*NumVertices*/, DWORD /*Options*/, DWORD /*FVF*/,
                          x2::d3d9::Device* /*pD3DDevice*/, LPD3DXMESH* ppMesh)
{
    return NotImplemented(ppMesh);
}

HRESULT D3DXWeldVertices(LPD3DXMESH /*pMesh*/, DWORD /*Flags*/, const D3DXWELDEPSILONS* /*pEpsilons*/,
                         const DWORD* /*pAdjacencyIn*/, DWORD* /*pAdjacencyOut*/, DWORD* /*pFaceRemap*/,
                         LPD3DXBUFFER* ppVertexRemap)
{
    return NotImplemented(ppVertexRemap);
}

HRESULT D3DXFrameCalculateBoundingSphere(const D3DXFRAME* /*pFrameRoot*/, D3DXVECTOR3* pObjectCenter,
                                         FLOAT* pObjectRadius)
{
    if (pObjectCenter != nullptr) {
        *pObjectCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }
    if (pObjectRadius != nullptr) {
        *pObjectRadius = 0.0f;
    }
    return E_NOTIMPL;
}

HRESULT D3DXCreateSkinInfoFVF(DWORD /*NumVertices*/, DWORD /*FVF*/, DWORD /*NumBones*/, LPD3DXSKININFO* ppSkinInfo)
{
    return NotImplemented(ppSkinInfo);
}

HRESULT D3DXCreateAnimationController(UINT /*MaxNumMatrices*/, UINT /*MaxNumAnimationSets*/, UINT /*MaxNumTracks*/,
                                      UINT /*MaxNumEvents*/, LPD3DXANIMATIONCONTROLLER* ppAnimController)
{
    return NotImplemented(ppAnimController);
}

HRESULT D3DXFrameRegisterNamedMatrices(LPD3DXFRAME /*pFrameRoot*/, LPD3DXANIMATIONCONTROLLER /*pAnimController*/)
{
    return E_NOTIMPL;
}

HRESULT D3DXCreateKeyframedAnimationSet(LPCSTR /*pName*/, DOUBLE /*TicksPerSecond*/, D3DXPLAYBACK_TYPE /*Playback*/,
                                        UINT /*NumAnimations*/, UINT /*NumCallbackKeys*/,
                                        const D3DXKEY_CALLBACK* /*pCallbackKeys*/,
                                        LPD3DXKEYFRAMEDANIMATIONSET* ppAnimationSet)
{
    return NotImplemented(ppAnimationSet);
}

HRESULT D3DXCreateCompressedAnimationSet(LPCSTR /*pName*/, DOUBLE /*TicksPerSecond*/, D3DXPLAYBACK_TYPE /*Playback*/,
                                         LPD3DXBUFFER /*pCompressedData*/, UINT /*NumCallbackKeys*/,
                                         const D3DXKEY_CALLBACK* /*pCallbackKeys*/,
                                         LPD3DXCOMPRESSEDANIMATIONSET* ppAnimationSet)
{
    return NotImplemented(ppAnimationSet);
}

HRESULT D3DXCreateEffect(x2::d3d9::Device* /*pDevice*/, LPCVOID /*pSrcData*/, UINT /*SrcDataLen*/,
                         const D3DXMACRO* /*pDefines*/, ID3DXInclude* /*pInclude*/, DWORD /*Flags*/,
                         ID3DXEffectPool* /*pPool*/, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
    NotImplemented(ppCompilationErrors);
    return NotImplemented(ppEffect);
}

HRESULT D3DXIntersect(LPD3DXBASEMESH /*pMesh*/, const D3DXVECTOR3* /*pRayPos*/, const D3DXVECTOR3* /*pRayDir*/,
                      BOOL* pHit, DWORD* /*pFaceIndex*/, FLOAT* /*pU*/, FLOAT* /*pV*/, FLOAT* /*pDist*/,
                      LPD3DXBUFFER* ppAllHits, DWORD* pCountOfHits)
{
    if (pHit != nullptr) {
        *pHit = FALSE;
    }
    if (pCountOfHits != nullptr) {
        *pCountOfHits = 0;
    }
    return NotImplemented(ppAllHits);
}
