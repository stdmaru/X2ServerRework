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

// Each one only where Windows' headers have not defined it (winnt.h has STDMETHODIMP without STDMETHOD).
#ifndef STDMETHOD
#define STDMETHOD(method)        virtual HRESULT method
#define STDMETHOD_(type, method) virtual type method
#endif
#ifndef STDMETHODIMP
#define STDMETHODIMP             HRESULT
#endif
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE
#endif
#ifndef STDMETHODIMP_
#define STDMETHODIMP_(type)      type
#endif
#ifndef THIS_
#define THIS_
#endif
#ifndef THIS
#define THIS                     void
#endif
#ifndef PURE
#define PURE                     = 0
#endif

namespace x2::d3d9 { class Device; class Texture; class VertexBuffer; class IndexBuffer; }

typedef const char* D3DXHANDLE;
typedef double DOUBLE;
typedef D3DXMATRIX* LPD3DXMATRIX;

class ID3DXBuffer;
class ID3DXBaseMesh;
class ID3DXMesh;
class ID3DXPMesh;
class ID3DXSkinInfo;
class ID3DXAnimationSet;
class ID3DXKeyframedAnimationSet;
class ID3DXCompressedAnimationSet;
struct ID3DXAnimationCallbackHandler;
class ID3DXEffect;

typedef ID3DXBuffer* LPD3DXBUFFER;
typedef ID3DXBaseMesh* LPD3DXBASEMESH;
typedef ID3DXMesh* LPD3DXMESH;
typedef ID3DXPMesh* LPD3DXPMESH;
typedef ID3DXSkinInfo* LPD3DXSKININFO;
typedef ID3DXAnimationSet* LPD3DXANIMATIONSET;
typedef ID3DXKeyframedAnimationSet* LPD3DXKEYFRAMEDANIMATIONSET;
typedef ID3DXCompressedAnimationSet* LPD3DXCOMPRESSEDANIMATIONSET;
typedef ID3DXEffect* LPD3DXEFFECT;

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
    STDMETHOD_(DWORD, KeyTrackEnable)(THIS_ UINT Track, BOOL NewEnable, double StartTime) PURE;
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

// --- Buffers, meshes and skinning (d3dx9mesh.h) ---------------------------------------------------------------------

class ID3DXBuffer {
public:
    virtual ~ID3DXBuffer() = default;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};

// Vertex declarations (d3d9types.h): only to read and patch a mesh's declaration.
struct D3DVERTEXELEMENT9 {
    WORD Stream;  // 0xFF ends the declaration
    WORD Offset;
    BYTE Type;
    BYTE Method;
    BYTE Usage;
    BYTE UsageIndex;
};
#define MAXD3DDECLLENGTH  64
#define MAX_FVF_DECL_SIZE (MAXD3DDECLLENGTH + 1)
enum D3DDECLTYPE { D3DDECLTYPE_FLOAT1 = 0, D3DDECLTYPE_FLOAT2 = 1, D3DDECLTYPE_FLOAT3 = 2, D3DDECLTYPE_FLOAT4 = 3,
                   D3DDECLTYPE_D3DCOLOR = 4, D3DDECLTYPE_UBYTE4 = 5, D3DDECLTYPE_UNUSED = 17 };
enum D3DDECLUSAGE { D3DDECLUSAGE_POSITION = 0, D3DDECLUSAGE_BLENDWEIGHT = 1, D3DDECLUSAGE_BLENDINDICES = 2,
                    D3DDECLUSAGE_NORMAL = 3, D3DDECLUSAGE_TEXCOORD = 5, D3DDECLUSAGE_COLOR = 10 };

using IDirect3DIndexBuffer9 = x2::d3d9::IndexBuffer;
using LPDIRECT3DINDEXBUFFER9 = x2::d3d9::IndexBuffer*;

enum _D3DXMESH {
    D3DXMESH_32BIT = 0x001,
    D3DXMESH_SYSTEMMEM = 0x110,
    D3DXMESH_MANAGED = 0x220,
    D3DXMESH_VB_SHARE = 0x1000,
};
enum _D3DXMESHOPT {
    D3DXMESHOPT_COMPACT = 0x01000000,
    D3DXMESHOPT_ATTRSORT = 0x02000000,
    D3DXMESHOPT_VERTEXCACHE = 0x04000000,
    D3DXMESHOPT_STRIPREORDER = 0x08000000,
    D3DXMESHOPT_IGNOREVERTS = 0x10000000,
};
#define D3DXERR_CANNOTATTRSORT static_cast<HRESULT>(0x88760B54L)  // MAKE_DDHRESULT(2900)

struct D3DXWELDEPSILONS {
    FLOAT Position;
    FLOAT BlendWeights;
    FLOAT Normal;
    FLOAT PSize;
    FLOAT Specular;
    FLOAT Diffuse;
    FLOAT Texcoord[8];
    FLOAT Tangent;
    FLOAT Binormal;
    FLOAT TessFactor;
};

struct D3DXBONECOMBINATION {
    DWORD AttribId;
    DWORD FaceStart;
    DWORD FaceCount;
    DWORD VertexStart;
    DWORD VertexCount;
    DWORD* BoneId;
};
typedef D3DXBONECOMBINATION* LPD3DXBONECOMBINATION;

// The members of the meshes the engine calls.
class ID3DXBaseMesh {
public:
    virtual ~ID3DXBaseMesh() = default;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD(DrawSubset)(THIS_ DWORD AttribId) PURE;
    STDMETHOD_(DWORD, GetNumFaces)(THIS) PURE;
    STDMETHOD_(DWORD, GetNumVertices)(THIS) PURE;
    STDMETHOD_(DWORD, GetFVF)(THIS) PURE;
    STDMETHOD(GetDeclaration)(THIS_ D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE]) PURE;
    STDMETHOD_(DWORD, GetNumBytesPerVertex)(THIS) PURE;
    STDMETHOD_(DWORD, GetOptions)(THIS) PURE;
    STDMETHOD(CloneMeshFVF)(THIS_ DWORD Options, DWORD FVF, x2::d3d9::Device* pD3DDevice, LPD3DXMESH* ppCloneMesh) PURE;
    STDMETHOD(GetVertexBuffer)(THIS_ x2::d3d9::VertexBuffer** ppVB) PURE;
    STDMETHOD(GetIndexBuffer)(THIS_ LPDIRECT3DINDEXBUFFER9* ppIB) PURE;
    STDMETHOD(LockVertexBuffer)(THIS_ DWORD Flags, LPVOID* ppData) PURE;
    STDMETHOD(UnlockVertexBuffer)(THIS) PURE;
    STDMETHOD(LockIndexBuffer)(THIS_ DWORD Flags, LPVOID* ppData) PURE;
    STDMETHOD(UnlockIndexBuffer)(THIS) PURE;
    STDMETHOD(GetAttributeTable)(THIS_ D3DXATTRIBUTERANGE* pAttribTable, DWORD* pAttribTableSize) PURE;
    STDMETHOD(GenerateAdjacency)(THIS_ FLOAT Epsilon, DWORD* pAdjacency) PURE;
    STDMETHOD(UpdateSemantics)(THIS_ D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE]) PURE;
};

class ID3DXMesh : public ID3DXBaseMesh {
public:
    STDMETHOD(LockAttributeBuffer)(THIS_ DWORD Flags, DWORD** ppData) PURE;
    STDMETHOD(UnlockAttributeBuffer)(THIS) PURE;
    STDMETHOD(Optimize)(THIS_ DWORD Flags, const DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, DWORD* pFaceRemap,
                        LPD3DXBUFFER* ppVertexRemap, LPD3DXMESH* ppOptMesh) PURE;
};

class ID3DXPMesh : public ID3DXBaseMesh {
public:
    STDMETHOD(ClonePMeshFVF)(THIS_ DWORD Options, DWORD FVF, x2::d3d9::Device* pD3DDevice,
                             LPD3DXPMESH* ppCloneMesh) PURE;
    STDMETHOD(SetNumFaces)(THIS_ DWORD Faces) PURE;
    STDMETHOD(SetNumVertices)(THIS_ DWORD Vertices) PURE;
    STDMETHOD_(DWORD, GetMaxFaces)(THIS) PURE;
    STDMETHOD_(DWORD, GetMinFaces)(THIS) PURE;
    STDMETHOD_(DWORD, GetMaxVertices)(THIS) PURE;
    STDMETHOD_(DWORD, GetMinVertices)(THIS) PURE;
};

class ID3DXSkinInfo {
public:
    virtual ~ID3DXSkinInfo() = default;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD(SetBoneInfluence)(THIS_ DWORD Bone, DWORD NumInfluences, const DWORD* Vertices,
                                const FLOAT* Weights) PURE;
    STDMETHOD_(DWORD, GetNumBoneInfluences)(THIS_ DWORD Bone) PURE;
    STDMETHOD(GetBoneInfluence)(THIS_ DWORD Bone, DWORD* Vertices, FLOAT* Weights) PURE;
    STDMETHOD_(DWORD, GetNumBones)(THIS) PURE;
    STDMETHOD(SetBoneName)(THIS_ DWORD Bone, LPCSTR pName) PURE;
    STDMETHOD_(LPCSTR, GetBoneName)(THIS_ DWORD Bone) PURE;
    STDMETHOD(SetBoneOffsetMatrix)(THIS_ DWORD Bone, const D3DXMATRIX* pBoneTransform) PURE;
    STDMETHOD_(LPD3DXMATRIX, GetBoneOffsetMatrix)(THIS_ DWORD Bone) PURE;
    STDMETHOD_(DWORD, GetFVF)(THIS) PURE;
    STDMETHOD(Remap)(THIS_ DWORD NumVertices, DWORD* pVertexRemap) PURE;
    STDMETHOD(ConvertToIndexedBlendedMesh)(THIS_ LPD3DXMESH pMesh, DWORD Options, DWORD paletteSize,
                                           const DWORD* pAdjacencyIn, LPDWORD pAdjacencyOut, DWORD* pFaceRemap,
                                           LPD3DXBUFFER* ppVertexRemap, DWORD* pMaxVertexInfl,
                                           DWORD* pNumBoneCombinations, LPD3DXBUFFER* ppBoneCombinationTable,
                                           LPD3DXMESH* ppMesh) PURE;
};

// --- Animation sets (d3dx9anim.h) ------------------------------------------------------------------------------------

enum D3DXPLAYBACK_TYPE { D3DXPLAY_LOOP = 0, D3DXPLAY_ONCE = 1, D3DXPLAY_PINGPONG = 2,
                         D3DXPLAY_FORCE_DWORD = 0x7fffffff };

// D3DXKEY_VECTOR3 and D3DXKEY_QUATERNION are plain data in x2/platform/d3dx_math.h.
typedef D3DXKEY_VECTOR3* LPD3DXKEY_VECTOR3;
typedef D3DXKEY_QUATERNION* LPD3DXKEY_QUATERNION;

struct D3DXKEY_CALLBACK {
    FLOAT Time;
    LPVOID pCallbackData;
};
typedef D3DXKEY_CALLBACK* LPD3DXKEY_CALLBACK;

// Animation sets are told apart with dynamic_cast, where COM code called QueryInterface.
class ID3DXAnimationSet {
public:
    virtual ~ID3DXAnimationSet() = default;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD_(LPCSTR, GetName)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetPeriod)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetPeriodicPosition)(THIS_ DOUBLE Position) PURE;
    STDMETHOD_(UINT, GetNumAnimations)(THIS) PURE;
    STDMETHOD(GetAnimationNameByIndex)(THIS_ UINT Index, LPCSTR* ppName) PURE;
    STDMETHOD(GetAnimationIndexByName)(THIS_ LPCSTR pName, UINT* pIndex) PURE;
    STDMETHOD(GetSRT)(THIS_ DOUBLE PeriodicPosition, UINT Animation, D3DXVECTOR3* pScale, D3DXQUATERNION* pRotation,
                      D3DXVECTOR3* pTranslation) PURE;
};

class ID3DXKeyframedAnimationSet : public ID3DXAnimationSet {
public:
    STDMETHOD_(D3DXPLAYBACK_TYPE, GetPlaybackType)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetSourceTicksPerSecond)(THIS) PURE;
    STDMETHOD_(UINT, GetNumScaleKeys)(THIS_ UINT Animation) PURE;
    STDMETHOD(GetScaleKeys)(THIS_ UINT Animation, LPD3DXKEY_VECTOR3 pScaleKeys) PURE;
    STDMETHOD_(UINT, GetNumRotationKeys)(THIS_ UINT Animation) PURE;
    STDMETHOD(GetRotationKeys)(THIS_ UINT Animation, LPD3DXKEY_QUATERNION pRotationKeys) PURE;
    STDMETHOD_(UINT, GetNumTranslationKeys)(THIS_ UINT Animation) PURE;
    STDMETHOD(GetTranslationKeys)(THIS_ UINT Animation, LPD3DXKEY_VECTOR3 pTranslationKeys) PURE;
    STDMETHOD(RegisterAnimationSRTKeys)(THIS_ LPCSTR pName, UINT NumScaleKeys, UINT NumRotationKeys,
                                        UINT NumTranslationKeys, const D3DXKEY_VECTOR3* pScaleKeys,
                                        const D3DXKEY_QUATERNION* pRotationKeys,
                                        const D3DXKEY_VECTOR3* pTranslationKeys, DWORD* pAnimationIndex) PURE;
    STDMETHOD(Compress)(THIS_ DWORD Flags, FLOAT Lossiness, LPD3DXFRAME pHierarchy,
                        LPD3DXBUFFER* ppCompressedData) PURE;
};

class ID3DXCompressedAnimationSet : public ID3DXAnimationSet {
public:
    STDMETHOD_(D3DXPLAYBACK_TYPE, GetPlaybackType)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetSourceTicksPerSecond)(THIS) PURE;
    STDMETHOD(GetCompressedData)(THIS_ LPD3DXBUFFER* ppCompressedData) PURE;
};

// --- Effects (d3dx9effect.h) -----------------------------------------------------------------------------------------

#define D3DXFX_DONOTSAVESTATE        (1 << 0)
#define D3DXFX_DONOTSAVESHADERSTATE  (1 << 1)
#define D3DXFX_DONOTSAVESAMPLERSTATE (1 << 2)
#define D3DXFX_NOT_CLONEABLE         (1 << 11)
#define D3DXSHADER_DEBUG             (1 << 0)
#define D3DXSHADER_SKIPOPTIMIZATION  (1 << 2)

// The members of ID3DXEffect the engine calls. The `.fx` effects become the engine's own shaders (T37).
class ID3DXEffect {
public:
    virtual ~ID3DXEffect() = default;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE hParameter, LPCSTR pName) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ LPCSTR pName) PURE;
    STDMETHOD(SetBool)(THIS_ D3DXHANDLE hParameter, BOOL b) PURE;
    STDMETHOD(SetInt)(THIS_ D3DXHANDLE hParameter, INT n) PURE;
    STDMETHOD(SetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT f) PURE;
    STDMETHOD(SetVector)(THIS_ D3DXHANDLE hParameter, const D3DXVECTOR4* pVector) PURE;
    STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix) PURE;
    STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count) PURE;
    STDMETHOD(SetTexture)(THIS_ D3DXHANDLE hParameter, x2::d3d9::Texture* pTexture) PURE;
    STDMETHOD(SetTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
    STDMETHOD(Begin)(THIS_ UINT* pPasses, DWORD Flags) PURE;
    STDMETHOD(BeginPass)(THIS_ UINT Pass) PURE;
    STDMETHOD(CommitChanges)(THIS) PURE;
    STDMETHOD(EndPass)(THIS) PURE;
    STDMETHOD(End)(THIS) PURE;
    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;
    STDMETHOD(SetStateManager)(THIS_ class ID3DXEffectStateManager* pManager) PURE;
};

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
#define D3DX_DEFAULT_NONPOW2    (static_cast<UINT>(-2))
#define D3DX_FROM_FILE          (static_cast<UINT>(-3))
#define D3DFMT_FROM_FILE        (static_cast<D3DFORMAT>(-3))
#define D3DX_FILTER_NONE        (1 << 0)
#define D3DX_FILTER_POINT       (2 << 0)
#define D3DX_FILTER_LINEAR      (3 << 0)
#define D3DX_FILTER_TRIANGLE    (4 << 0)
#define D3DX_FILTER_BOX         (5 << 0)

// .dds, .tga, .png, .jpg and .bmp, recognized by their contents (Image.h; docs/porting/client-d3d9.md#image-files).
// Sizes 0, D3DX_DEFAULT and D3DX_DEFAULT_NONPOW2 keep the file's (the device takes any size); a format the device does
// not take becomes A8R8G8B8; pPalette is ignored. Fail with D3DXERR_INVALIDDATA for a file they do not read.
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

// --- ATL's CComPtr, for the D3DX objects (they keep AddRef/Release) ------------------------------------------------

// Holds a reference: taking a pointer adds one, letting it go releases it. &p is for out-parameters (p must be empty).
template <class T>
class CComPtr {
public:
    CComPtr() = default;
    CComPtr(T* pointer) : p(pointer)
    {
        if (p != nullptr) {
            p->AddRef();
        }
    }
    CComPtr(const CComPtr& other) : CComPtr(other.p) {}
    ~CComPtr() { Release(); }
    CComPtr& operator=(T* pointer)
    {
        if (pointer != nullptr) {
            pointer->AddRef();
        }
        Release();
        p = pointer;
        return *this;
    }
    CComPtr& operator=(const CComPtr& other) { return *this = other.p; }

    void Release()
    {
        T* pointer = p;
        p = nullptr;
        if (pointer != nullptr) {
            pointer->Release();
        }
    }
    operator T*() const { return p; }
    T* operator->() const { return p; }
    T** operator&() { return &p; }
    bool operator!() const { return p == nullptr; }

    T* p = nullptr;
};

// --- Creation functions --------------------------------------------------------------------------------------------
// A texture on the device (the image loaders above are what fail until their step).
HRESULT D3DXCreateTexture(x2::d3d9::Device* pDevice, UINT width, UINT height, UINT mipLevels, DWORD usage,
                          D3DFORMAT format, D3DPOOL pool, x2::d3d9::Texture** ppTexture);
// A buffer of the given size.
HRESULT D3DXCreateBuffer(DWORD numBytes, LPD3DXBUFFER* ppBuffer);

// Frame hierarchies: walking them needs no mesh.
HRESULT D3DXFrameDestroy(LPD3DXFRAME pFrameRoot, LPD3DXALLOCATEHIERARCHY pAlloc);
LPD3DXFRAME D3DXFrameFind(const D3DXFRAME* pFrameRoot, LPCSTR Name);
UINT D3DXFrameNumNamedMatrices(const D3DXFRAME* pFrameRoot);

// Meshes, skinning, animation sets and controllers, and effects: the `.x` step and T37 implement them; until then they
// fail with E_NOTIMPL and return null objects (docs/porting/client-d3d9.md).
class ID3DXLoadUserData;
HRESULT D3DXLoadMeshFromXInMemory(LPCVOID Memory, DWORD SizeOfMemory, DWORD Options, x2::d3d9::Device* pD3DDevice,
                                  LPD3DXBUFFER* ppAdjacency, LPD3DXBUFFER* ppMaterials,
                                  LPD3DXBUFFER* ppEffectInstances, DWORD* pNumMaterials, LPD3DXMESH* ppMesh);
HRESULT D3DXLoadMeshHierarchyFromXInMemory(LPCVOID Memory, DWORD SizeOfMemory, DWORD MeshOptions,
                                           x2::d3d9::Device* pDevice, LPD3DXALLOCATEHIERARCHY pAlloc,
                                           ID3DXLoadUserData* pUserDataLoader, LPD3DXFRAME* ppFrameHierarchy,
                                           LPD3DXANIMATIONCONTROLLER* ppAnimController);
HRESULT D3DXCreateMeshFVF(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, x2::d3d9::Device* pD3DDevice,
                          LPD3DXMESH* ppMesh);
HRESULT D3DXWeldVertices(LPD3DXMESH pMesh, DWORD Flags, const D3DXWELDEPSILONS* pEpsilons,
                         const DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, DWORD* pFaceRemap,
                         LPD3DXBUFFER* ppVertexRemap);
HRESULT D3DXFrameCalculateBoundingSphere(const D3DXFRAME* pFrameRoot, D3DXVECTOR3* pObjectCenter,
                                         FLOAT* pObjectRadius);
HRESULT D3DXCreateSkinInfoFVF(DWORD NumVertices, DWORD FVF, DWORD NumBones, LPD3DXSKININFO* ppSkinInfo);
HRESULT D3DXCreateAnimationController(UINT MaxNumMatrices, UINT MaxNumAnimationSets, UINT MaxNumTracks,
                                      UINT MaxNumEvents, LPD3DXANIMATIONCONTROLLER* ppAnimController);
HRESULT D3DXFrameRegisterNamedMatrices(LPD3DXFRAME pFrameRoot, LPD3DXANIMATIONCONTROLLER pAnimController);
HRESULT D3DXCreateKeyframedAnimationSet(LPCSTR pName, DOUBLE TicksPerSecond, D3DXPLAYBACK_TYPE Playback,
                                        UINT NumAnimations, UINT NumCallbackKeys,
                                        const D3DXKEY_CALLBACK* pCallbackKeys,
                                        LPD3DXKEYFRAMEDANIMATIONSET* ppAnimationSet);
HRESULT D3DXCreateCompressedAnimationSet(LPCSTR pName, DOUBLE TicksPerSecond, D3DXPLAYBACK_TYPE Playback,
                                         LPD3DXBUFFER pCompressedData, UINT NumCallbackKeys,
                                         const D3DXKEY_CALLBACK* pCallbackKeys,
                                         LPD3DXCOMPRESSEDANIMATIONSET* ppAnimationSet);
struct D3DXMACRO;
class ID3DXInclude;
class ID3DXEffectPool;
HRESULT D3DXCreateEffect(x2::d3d9::Device* pDevice, LPCVOID pSrcData, UINT SrcDataLen, const D3DXMACRO* pDefines,
                         ID3DXInclude* pInclude, DWORD Flags, ID3DXEffectPool* pPool, LPD3DXEFFECT* ppEffect,
                         LPD3DXBUFFER* ppCompilationErrors);
