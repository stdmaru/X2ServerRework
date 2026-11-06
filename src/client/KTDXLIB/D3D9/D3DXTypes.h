#pragma once

// The D3DX types the engine's headers still name: meshes, skinning, animation controllers, effects, lines, image
// information. They are declared here with D3DX's names and layouts, and nothing implements them yet: they come with
// the steps that load `.x` files and the effects on KTDXLIB's own classes (decision T37,
// docs/porting/client-d3d9.md). A file that calls them does not compile into KTDXLIB until then.
//
// The interfaces are abstract classes without COM (T38): STDMETHOD and the other declaration macros only spell
// virtual methods, so the engine's declarations keep compiling unchanged.

#include "D3D9Types.h"
#include "Fvf.h"

#include "x2/platform/d3dx_math.h"
#include "x2/platform/types.h"

#ifndef STDMETHOD
#define STDMETHOD(method)        virtual HRESULT method
#define STDMETHOD_(type, method) virtual type method
#define STDMETHODIMP             HRESULT
#define STDMETHODCALLTYPE
#define STDMETHODIMP_(type)      type
#define THIS_
#define THIS                     void
#define PURE                     = 0
#endif

namespace x2::d3d9 { class Device; class Texture; }

typedef const char* D3DXHANDLE;

struct ID3DXBuffer;
struct ID3DXBaseMesh;
struct ID3DXMesh;
struct ID3DXPMesh;
struct ID3DXSkinInfo;
struct ID3DXAnimationSet;
struct ID3DXKeyframedAnimationSet;
struct ID3DXCompressedAnimationSet;
struct ID3DXAnimationCallbackHandler;
struct ID3DXEffect;
struct ID3DXLine;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;

typedef ID3DXBuffer* LPD3DXBUFFER;
typedef ID3DXBaseMesh* LPD3DXBASEMESH;
typedef ID3DXMesh* LPD3DXMESH;
typedef ID3DXPMesh* LPD3DXPMESH;
typedef ID3DXSkinInfo* LPD3DXSKININFO;
typedef ID3DXAnimationSet* LPD3DXANIMATIONSET;
typedef ID3DXKeyframedAnimationSet* LPD3DXKEYFRAMEDANIMATIONSET;
typedef ID3DXCompressedAnimationSet* LPD3DXCOMPRESSEDANIMATIONSET;
typedef ID3DXEffect* LPD3DXEFFECT;
typedef ID3DXLine* LPD3DXLINE;
typedef IDirect3DVertexShader9* LPDIRECT3DVERTEXSHADER9;
class IDirect3DVertexDeclaration9;
typedef IDirect3DVertexDeclaration9* LPDIRECT3DVERTEXDECLARATION9;
typedef IDirect3DPixelShader9* LPDIRECT3DPIXELSHADER9;

// --- Meshes (d3dx9mesh.h) --------------------------------------------------------------------------------------------

enum D3DXMESHDATATYPE { D3DXMESHTYPE_MESH = 1, D3DXMESHTYPE_PMESH = 2, D3DXMESHTYPE_PATCHMESH = 3,
                        D3DXMESHTYPE_FORCE_DWORD = 0x7fffffff };

struct D3DXMESHDATA {
    D3DXMESHDATATYPE Type;
    union {
        LPD3DXMESH pMesh;
        LPD3DXPMESH pPMesh;
        void* pPatchMesh;
    };
};

struct D3DXMATERIAL {
    D3DMATERIAL9 MatD3D;
    LPSTR pTextureFilename;
};

enum D3DXEFFECTDEFAULTTYPE { D3DXEDT_STRING = 1, D3DXEDT_FLOATS = 2, D3DXEDT_DWORD = 3, D3DXEDT_FORCEDWORD = 0x7fffffff };

struct D3DXEFFECTDEFAULT {
    LPSTR pParamName;
    D3DXEFFECTDEFAULTTYPE Type;
    DWORD NumBytes;
    void* pValue;
};

struct D3DXEFFECTINSTANCE {
    LPSTR pEffectFilename;
    DWORD NumDefaults;
    D3DXEFFECTDEFAULT* pDefaults;
};

struct D3DXATTRIBUTERANGE {
    DWORD AttribId;
    DWORD FaceStart;
    DWORD FaceCount;
    DWORD VertexStart;
    DWORD VertexCount;
};
typedef D3DXATTRIBUTERANGE* LPD3DXATTRIBUTERANGE;

// --- Frame hierarchies and animation (d3dx9anim.h) -------------------------------------------------------------------

struct D3DXMESHCONTAINER {
    LPSTR Name;
    D3DXMESHDATA MeshData;
    D3DXMATERIAL* pMaterials;
    D3DXEFFECTINSTANCE* pEffects;
    DWORD NumMaterials;
    DWORD* pAdjacency;
    LPD3DXSKININFO pSkinInfo;
    D3DXMESHCONTAINER* pNextMeshContainer;
};
typedef D3DXMESHCONTAINER* LPD3DXMESHCONTAINER;

struct D3DXFRAME {
    LPSTR Name;
    D3DXMATRIX TransformationMatrix;
    LPD3DXMESHCONTAINER pMeshContainer;
    D3DXFRAME* pFrameSibling;
    D3DXFRAME* pFrameFirstChild;
};
typedef D3DXFRAME* LPD3DXFRAME;

// The loader's callbacks to allocate and free the frames and mesh containers of a hierarchy.
class ID3DXAllocateHierarchy {
public:
    virtual ~ID3DXAllocateHierarchy() = default;
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME* ppNewFrame) PURE;
    STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials,
                                   const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials,
                                   const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo,
                                   LPD3DXMESHCONTAINER* ppNewMeshContainer) PURE;
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) PURE;
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree) PURE;
};
typedef ID3DXAllocateHierarchy* LPD3DXALLOCATEHIERARCHY;

enum D3DXPRIORITY_TYPE { D3DXPRIORITY_LOW = 0, D3DXPRIORITY_HIGH = 1, D3DXPRIORITY_FORCE_DWORD = 0x7fffffff };
enum D3DXTRANSITION_TYPE { D3DXTRANSITION_LINEAR = 0, D3DXTRANSITION_EASEINEASEOUT = 1,
                           D3DXTRANSITION_FORCE_DWORD = 0x7fffffff };

struct D3DXTRACK_DESC {
    D3DXPRIORITY_TYPE Priority;
    FLOAT Weight;
    FLOAT Speed;
    double Position;
    BOOL Enable;
};

// The members of ID3DXAnimationController the engine calls (its reference counting included, until the `.x` step
// gives it an owner).
class ID3DXAnimationController {
public:
    virtual ~ID3DXAnimationController() = default;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD_(UINT, GetMaxNumAnimationOutputs)(THIS) PURE;
    STDMETHOD_(UINT, GetMaxNumAnimationSets)(THIS) PURE;
    STDMETHOD_(UINT, GetMaxNumTracks)(THIS) PURE;
    STDMETHOD_(UINT, GetMaxNumEvents)(THIS) PURE;
    STDMETHOD(RegisterAnimationSet)(THIS_ LPD3DXANIMATIONSET pAnimSet) PURE;
    STDMETHOD(UnregisterAnimationSet)(THIS_ LPD3DXANIMATIONSET pAnimSet) PURE;
    STDMETHOD_(UINT, GetNumAnimationSets)(THIS) PURE;
    STDMETHOD(GetAnimationSet)(THIS_ UINT Index, LPD3DXANIMATIONSET* ppAnimationSet) PURE;
    STDMETHOD(GetAnimationSetByName)(THIS_ LPCSTR szName, LPD3DXANIMATIONSET* ppAnimationSet) PURE;
    STDMETHOD(AdvanceTime)(THIS_ double TimeDelta, ID3DXAnimationCallbackHandler* pCallbackHandler) PURE;
    STDMETHOD(ResetTime)(THIS) PURE;
    STDMETHOD_(double, GetTime)(THIS) PURE;
    STDMETHOD(SetTrackAnimationSet)(THIS_ UINT Track, LPD3DXANIMATIONSET pAnimSet) PURE;
    STDMETHOD(GetTrackAnimationSet)(THIS_ UINT Track, LPD3DXANIMATIONSET* ppAnimSet) PURE;
    STDMETHOD(SetTrackPriority)(THIS_ UINT Track, D3DXPRIORITY_TYPE Priority) PURE;
    STDMETHOD(SetTrackSpeed)(THIS_ UINT Track, FLOAT Speed) PURE;
    STDMETHOD(SetTrackWeight)(THIS_ UINT Track, FLOAT Weight) PURE;
    STDMETHOD(SetTrackPosition)(THIS_ UINT Track, double Position) PURE;
    STDMETHOD(SetTrackEnable)(THIS_ UINT Track, BOOL Enable) PURE;
    STDMETHOD(SetTrackDesc)(THIS_ UINT Track, D3DXTRACK_DESC* pDesc) PURE;
    STDMETHOD(GetTrackDesc)(THIS_ UINT Track, D3DXTRACK_DESC* pDesc) PURE;
    STDMETHOD_(DWORD, KeyTrackSpeed)(THIS_ UINT Track, FLOAT NewSpeed, double StartTime, double Duration,
                                     D3DXTRANSITION_TYPE Transition) PURE;
    STDMETHOD_(DWORD, KeyTrackWeight)(THIS_ UINT Track, FLOAT NewWeight, double StartTime, double Duration,
                                      D3DXTRANSITION_TYPE Transition) PURE;
    STDMETHOD(UnkeyAllTrackEvents)(THIS_ UINT Track) PURE;
    STDMETHOD(CloneAnimationController)(THIS_ UINT MaxNumAnimationOutputs, UINT MaxNumAnimationSets, UINT MaxNumTracks,
                                        UINT MaxNumEvents, ID3DXAnimationController** ppAnimController) PURE;
};
typedef ID3DXAnimationController* LPD3DXANIMATIONCONTROLLER;

// --- Effects (d3dx9effect.h) -----------------------------------------------------------------------------------------

// The callbacks an effect makes instead of calling the device. The effects are rewritten as shaders (T37); the
// engine's CKTDGEffectStateManager still implements this until then.
class ID3DXEffectStateManager {
public:
    virtual ~ID3DXEffectStateManager() = default;
    STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) PURE;
    STDMETHOD(SetMaterial)(THIS_ const D3DMATERIAL9* pMaterial) PURE;
    STDMETHOD(SetLight)(THIS_ DWORD Index, const D3DLIGHT9* pLight) PURE;
    STDMETHOD(LightEnable)(THIS_ DWORD Index, BOOL Enable) PURE;
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State, DWORD Value) PURE;
    STDMETHOD(SetTexture)(THIS_ DWORD Stage, x2::d3d9::Texture* pTexture) PURE;
    STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) PURE;
    STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) PURE;
    STDMETHOD(SetNPatchMode)(THIS_ FLOAT NumSegments) PURE;
    STDMETHOD(SetFVF)(THIS_ DWORD FVF) PURE;
    STDMETHOD(SetVertexShader)(THIS_ LPDIRECT3DVERTEXSHADER9 pShader) PURE;
    STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT RegisterIndex, const FLOAT* pConstantData, UINT RegisterCount) PURE;
    STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT RegisterIndex, const INT* pConstantData, UINT RegisterCount) PURE;
    STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT RegisterIndex, const BOOL* pConstantData, UINT RegisterCount) PURE;
    STDMETHOD(SetPixelShader)(THIS_ LPDIRECT3DPIXELSHADER9 pShader) PURE;
    STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT RegisterIndex, const FLOAT* pConstantData, UINT RegisterCount) PURE;
    STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT RegisterIndex, const INT* pConstantData, UINT RegisterCount) PURE;
    STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT RegisterIndex, const BOOL* pConstantData, UINT RegisterCount) PURE;
};
typedef ID3DXEffectStateManager* LPD3DXEFFECTSTATEMANAGER;

// --- Textures (d3dx9tex.h) -------------------------------------------------------------------------------------------

enum D3DXIMAGE_FILEFORMAT { D3DXIFF_BMP = 0, D3DXIFF_JPG = 1, D3DXIFF_TGA = 2, D3DXIFF_PNG = 3, D3DXIFF_DDS = 4,
                            D3DXIFF_PPM = 5, D3DXIFF_DIB = 6, D3DXIFF_HDR = 7, D3DXIFF_PFM = 8,
                            D3DXIFF_FORCE_DWORD = 0x7fffffff };

struct D3DXIMAGE_INFO {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    D3DFORMAT Format;
    D3DRESOURCETYPE ResourceType;
    D3DXIMAGE_FILEFORMAT ImageFileFormat;
};

// --- Textures from files -------------------------------------------------------------------------------------------

#define D3DERR_NOTAVAILABLE     static_cast<HRESULT>(0x8876086AL)  // MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY static_cast<HRESULT>(0x8876017CL)  // MAKE_D3DHRESULT(380)
#define D3DXERR_INVALIDDATA     static_cast<HRESULT>(0x88760B59L)  // MAKE_DDHRESULT(2905)

#define D3DX_DEFAULT            (static_cast<UINT>(-1))
#define D3DX_FILTER_NONE        (1 << 0)
#define D3DX_FILTER_POINT       (2 << 0)
#define D3DX_FILTER_LINEAR      (3 << 0)
#define D3DX_FILTER_TRIANGLE    (4 << 0)
#define D3DX_FILTER_BOX         (5 << 0)

// The image loaders (.dds, .tga) are the next step: until then these two fail with D3DXERR_INVALIDDATA
// (docs/porting/client-d3d9.md).
HRESULT D3DXGetImageInfoFromFileInMemory(const void* pSrcData, UINT srcDataSize, D3DXIMAGE_INFO* pSrcInfo);
HRESULT D3DXCreateTextureFromFileInMemoryEx(x2::d3d9::Device* pDevice, const void* pSrcData, UINT srcDataSize,
                                            UINT width, UINT height, UINT mipLevels, DWORD usage, D3DFORMAT format,
                                            D3DPOOL pool, DWORD filter, DWORD mipFilter, D3DCOLOR colorKey,
                                            D3DXIMAGE_INFO* pSrcInfo, void* pPalette, x2::d3d9::Texture** ppTexture);
// What the device can create: any size; D3DX_DEFAULT and 0 become 256 and a full mip chain, an unknown format
// D3DFMT_A8R8G8B8. Fails for a format the device does not have.
HRESULT D3DXCheckTextureRequirements(x2::d3d9::Device* pDevice, UINT* pWidth, UINT* pHeight, UINT* pNumMipLevels,
                                     DWORD usage, D3DFORMAT* pFormat, D3DPOOL pool);

// --- Mesh functions (declared for the inline callers in KTDXCollision.h; the `.x` step implements them) --------------

HRESULT D3DXIntersect(LPD3DXBASEMESH pMesh, const D3DXVECTOR3* pRayPos, const D3DXVECTOR3* pRayDir, BOOL* pHit,
                      DWORD* pFaceIndex, FLOAT* pU, FLOAT* pV, FLOAT* pDist, LPD3DXBUFFER* ppAllHits,
                      DWORD* pCountOfHits);

// Bytes per vertex of an FVF (0 for one without a position).
inline UINT D3DXGetFVFVertexSize(DWORD fvf)
{
    return x2::d3d9::DescribeFvf(fvf).size;
}
