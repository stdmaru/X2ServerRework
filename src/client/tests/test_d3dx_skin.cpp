// D3DX's skinned meshes on KTDXLIB's device (docs/porting/client-d3d9.md#skinned-meshes-2026-11-07-clientx-skin): skin
// info, indexed blended meshes, mesh hierarchies from .x files and their bounding spheres.

#include "D3DXTypes.h"
#include "Device.h"
#include "Mesh.h"
#include "SkinInfo.h"

#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace {

std::unique_ptr<x2::render::Device> NullRender()
{
    std::string error;
    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::Null;
    auto device = x2::render::Device::Create(options, error);
    REQUIRE_MESSAGE(device, error);
    return device;
}

template <class T>
struct Ref {
    T* p = nullptr;
    ~Ref()
    {
        if (p) {
            p->Release();
        }
    }
};

char* Copy(const char* text)
{
    if (text == nullptr) {
        return nullptr;
    }
    char* copy = new char[std::strlen(text) + 1];
    std::memcpy(copy, text, std::strlen(text) + 1);
    return copy;
}

// Frames and containers as the engine's allocator makes them: names copied, the mesh and skin info referenced.
struct Allocator : ID3DXAllocateHierarchy {
    int frames = 0, containers = 0;
    STDMETHOD(CreateFrame)(THIS_ LPCSTR name, LPD3DXFRAME* frame) override
    {
        auto* f = new D3DXFRAME{};
        f->Name = Copy(name);
        *frame = f;
        ++frames;
        return D3D_OK;
    }
    STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR name, const D3DXMESHDATA* data, const D3DXMATERIAL* materials,
                                   const D3DXEFFECTINSTANCE*, DWORD materialCount, const DWORD* adjacency,
                                   LPD3DXSKININFO skin, LPD3DXMESHCONTAINER* container) override
    {
        auto* c = new D3DXMESHCONTAINER{};
        c->Name = Copy(name);
        c->MeshData = *data;
        c->MeshData.pMesh->AddRef();
        c->NumMaterials = materialCount;
        c->pMaterials = new D3DXMATERIAL[materialCount];
        for (DWORD i = 0; i < materialCount; ++i) {
            c->pMaterials[i].MatD3D = materials[i].MatD3D;
            c->pMaterials[i].pTextureFilename =
                Copy(materials[i].pTextureFilename);
        }
        const DWORD faces = data->pMesh->GetNumFaces();
        c->pAdjacency = new DWORD[faces * 3];
        std::memcpy(c->pAdjacency, adjacency, faces * 3 * sizeof(DWORD));
        c->pSkinInfo = skin;
        if (skin) {
            skin->AddRef();
        }
        *container = c;
        ++containers;
        return D3D_OK;
    }
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME frame) override
    {
        delete[] frame->Name;
        delete frame;
        --frames;
        return D3D_OK;
    }
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER c) override
    {
        delete[] c->Name;
        for (DWORD i = 0; i < c->NumMaterials; ++i) {
            delete[] c->pMaterials[i].pTextureFilename;
        }
        delete[] c->pMaterials;
        delete[] c->pAdjacency;
        c->MeshData.pMesh->Release();
        if (c->pSkinInfo) {
            c->pSkinInfo->Release();
        }
        delete c;
        --containers;
        return D3D_OK;
    }
};

struct Vertex {
    float x, y, z, nx, ny, nz, u, v;
};
constexpr DWORD kFvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

// A strip of four quads along x (vertices 0..9: bottom/top pairs), materials 0, 0, 1, 1.
x2::d3d9::Mesh* Strip(x2::d3d9::Device& device)
{
    LPD3DXMESH mesh = nullptr;
    REQUIRE(D3DXCreateMeshFVF(8, 10, D3DXMESH_MANAGED, kFvf, &device, &mesh) == D3D_OK);
    std::vector<Vertex> vertices;
    for (int i = 0; i < 5; ++i) {
        vertices.push_back({float(i), 0, 0, 0, 0, -1, float(i) / 4, 1});
        vertices.push_back({float(i), 1, 0, 0, 0, -1, float(i) / 4, 0});
    }
    std::vector<std::uint32_t> indices;
    for (std::uint32_t q = 0; q < 4; ++q) {
        const std::uint32_t a = q * 2;
        indices.insert(indices.end(), {a, a + 1, a + 3, a, a + 3, a + 2});
    }
    std::vector<std::byte> bytes(vertices.size() * sizeof(Vertex));
    std::memcpy(bytes.data(), vertices.data(), bytes.size());
    auto* strip = static_cast<x2::d3d9::Mesh*>(mesh);
    REQUIRE(strip->SetContents(bytes, indices, {0, 0, 0, 0, 1, 1, 1, 1}));
    return strip;
}

// Bones 0..4, one per column of vertices; the middle column split between bones 1 and 2.
Ref<ID3DXSkinInfo> StripSkin()
{
    Ref<ID3DXSkinInfo> skin;
    REQUIRE(D3DXCreateSkinInfoFVF(10, kFvf, 5, &skin.p) == D3D_OK);
    for (DWORD bone = 0; bone < 5; ++bone) {
        std::vector<DWORD> vertices{bone * 2, bone * 2 + 1};
        std::vector<float> weights{1, 1};
        if (bone == 1) {
            vertices.insert(vertices.end(), {4, 5});
            weights.insert(weights.end(), {0.25f, 0.25f});
        }
        if (bone == 2) {
            weights = {0.75f, 0.75f};
        }
        REQUIRE(skin.p->SetBoneInfluence(bone, static_cast<DWORD>(vertices.size()), vertices.data(), weights.data()) ==
                D3D_OK);
        const std::string name = "Bone" + std::to_string(bone);
        REQUIRE(skin.p->SetBoneName(bone, name.c_str()) == D3D_OK);
    }
    return skin;
}

float WeightOf(const std::vector<std::byte>& vertices, std::size_t stride, DWORD vertex, int i)
{
    float w;
    std::memcpy(&w, vertices.data() + vertex * stride + 12 + 4 * i, 4);
    return w;
}

}  // namespace

TEST_CASE("d3dx: skin info")
{
    Ref<ID3DXSkinInfo> skin;
    REQUIRE(D3DXCreateSkinInfoFVF(6, kFvf, 2, &skin.p) == D3D_OK);
    CHECK(skin.p->GetNumBones() == 2);
    CHECK(skin.p->GetFVF() == kFvf);
    CHECK(skin.p->GetBoneName(0) == nullptr);
    CHECK(skin.p->SetBoneName(0, "Root") == D3D_OK);
    CHECK(std::string(skin.p->GetBoneName(0)) == "Root");
    CHECK(skin.p->SetBoneName(2, "Past") == D3DERR_INVALIDCALL);
    CHECK(skin.p->GetBoneOffsetMatrix(1)->_11 == 1.0f);     // identity until set
    D3DXMATRIX offset;
    D3DXMatrixTranslation(&offset, 1, 2, 3);
    CHECK(skin.p->SetBoneOffsetMatrix(1, &offset) == D3D_OK);
    CHECK(skin.p->GetBoneOffsetMatrix(1)->_42 == 2.0f);
    CHECK(skin.p->GetBoneOffsetMatrix(2) == nullptr);

    const DWORD vertices[] = {0, 3, 5};
    const float weights[] = {1.0f, 0.5f, 0.25f};
    CHECK(skin.p->SetBoneInfluence(1, 3, vertices, weights) == D3D_OK);
    const DWORD outside[] = {6};
    CHECK(skin.p->SetBoneInfluence(1, 1, outside, weights) == D3DERR_INVALIDCALL);
    CHECK(skin.p->GetNumBoneInfluences(1) == 3);
    CHECK(skin.p->GetNumBoneInfluences(0) == 0);
    DWORD gotVertices[3];
    float gotWeights[3];
    REQUIRE(skin.p->GetBoneInfluence(1, gotVertices, gotWeights) == D3D_OK);
    CHECK(gotVertices[1] == 3);
    CHECK(gotWeights[2] == 0.25f);

    // Remap (new to old, as Optimize gives it): vertex 3 duplicated, vertex 0 dropped.
    DWORD remap[] = {5, 3, 3, 1};
    REQUIRE(skin.p->Remap(4, remap) == D3D_OK);
    REQUIRE(skin.p->GetNumBoneInfluences(1) == 3);
    skin.p->GetBoneInfluence(1, gotVertices, gotWeights);
    CHECK(std::set<DWORD>(gotVertices, gotVertices + 3) == std::set<DWORD>{0, 1, 2});
    DWORD bad[] = {9};
    CHECK(skin.p->Remap(1, bad) == D3DERR_INVALIDCALL);
}

TEST_CASE("d3dx: ConvertToIndexedBlendedMesh")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    Ref<ID3DXMesh> mesh;
    mesh.p = Strip(device);
    Ref<ID3DXSkinInfo> skin = StripSkin();

    // A palette of three bones: material 0 (bones 0-2) fits one combination; material 1 (bones 1-4) needs two.
    Ref<ID3DXMesh> blended;
    Ref<ID3DXBuffer> combinations, vertexRemap;
    DWORD maxInfluences = 0, combinationCount = 0;
    std::vector<DWORD> adjacency(24), faceRemap(8);
    REQUIRE(mesh.p->GenerateAdjacency(0, adjacency.data()) == D3D_OK);
    std::vector<DWORD> newAdjacency(24);
    REQUIRE(skin.p->ConvertToIndexedBlendedMesh(mesh.p, D3DXMESH_MANAGED, 3, adjacency.data(), newAdjacency.data(),
                                                faceRemap.data(), &vertexRemap.p, &maxInfluences, &combinationCount,
                                                &combinations.p, &blended.p) == D3D_OK);
    CHECK(maxInfluences == 2);
    CHECK(blended.p->GetFVF() == (D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1));
    CHECK(blended.p->GetNumBytesPerVertex() == 12 + 8 + 12 + 8);
    CHECK(faceRemap == std::vector<DWORD>{0, 1, 2, 3, 4, 5, 6, 7});
    REQUIRE(combinationCount == 3);
    const auto* table = static_cast<const D3DXBONECOMBINATION*>(combinations.p->GetBufferPointer());
    CHECK(table[0].AttribId == 0);
    CHECK(table[0].FaceStart == 0);
    CHECK(table[0].FaceCount == 4);
    CHECK(table[0].VertexStart == 0);
    CHECK(table[0].VertexCount == 6);
    CHECK(std::vector<DWORD>(table[0].BoneId, table[0].BoneId + 3) == std::vector<DWORD>{0, 1, 2});
    CHECK(table[1].AttribId == 1);
    CHECK(table[1].FaceStart == 4);
    CHECK(table[1].FaceCount == 2);
    CHECK(std::vector<DWORD>(table[1].BoneId, table[1].BoneId + 3) == std::vector<DWORD>{1, 2, 3});
    CHECK(table[2].AttribId == 1);
    CHECK(table[2].FaceStart == 6);
    CHECK(table[2].VertexStart == table[1].VertexStart + table[1].VertexCount);
    CHECK(std::vector<DWORD>(table[2].BoneId, table[2].BoneId + 3) == std::vector<DWORD>{3, 4, 0xFFFFFFFF});
    // The third column (vertices 6, 7) is in both combinations of material 1: copied.
    const DWORD totalVertices = table[2].VertexStart + table[2].VertexCount;
    CHECK(blended.p->GetNumVertices() == totalVertices);
    CHECK(vertexRemap.p->GetBufferSize() == totalVertices * 4);
    CHECK(newAdjacency == adjacency);   // the faces kept their order

    // Weights: the strongest first, the last one implied; indices are palette slots.
    auto* out = static_cast<x2::d3d9::Mesh*>(blended.p);
    const std::vector<std::byte> bytes = out->Vertices();
    const DWORD* remap = static_cast<const DWORD*>(vertexRemap.p->GetBufferPointer());
    const std::size_t stride = out->GetNumBytesPerVertex();
    for (DWORD v = 0; v < table[0].VertexCount; ++v) {
        std::uint8_t slots[4];
        std::memcpy(slots, bytes.data() + v * stride + 16, 4);
        if (remap[v] == 4) {    // bones 2 (0.75) and 1 (0.25)
            CHECK(WeightOf(bytes, stride, v, 0) == 0.75f);
            CHECK(slots[0] == 2);
            CHECK(slots[1] == 1);
        } else if (remap[v] == 0) {
            CHECK(WeightOf(bytes, stride, v, 0) == 1.0f);
            CHECK(slots[0] == 0);
        }
    }
    float u;
    std::memcpy(&u, bytes.data() + 0 * stride + 32, 4);
    CHECK(u == float(remap[0] / 2) / 4);

    // Cloned to the engine's format, the blend indices stay the last DWORD of the position.
    Ref<ID3DXMesh> wide;
    REQUIRE(blended.p->CloneMeshFVF(D3DXMESH_MANAGED,
                                    D3DFVF_XYZB4 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4, &device,
                                    &wide.p) == D3D_OK);
    const std::vector<std::byte> wideBytes = static_cast<x2::d3d9::Mesh*>(wide.p)->Vertices();
    for (DWORD v = 0; v < table[0].VertexCount; ++v) {
        CHECK(std::memcmp(wideBytes.data() + v * 48 + 24, bytes.data() + v * stride + 16, 4) == 0);
        CHECK(WeightOf(wideBytes, 48, v, 0) == WeightOf(bytes, stride, v, 0));
        CHECK(WeightOf(wideBytes, 48, v, 1) == 0.0f);
    }
    D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE];
    REQUIRE(wide.p->GetDeclaration(declaration) == D3D_OK);
    CHECK(declaration[1].Usage == 1);       // blend weights, FLOAT3
    CHECK(declaration[1].Type == 2);
    CHECK(declaration[2].Usage == 2);       // blend indices, UBYTE4 at 24
    CHECK(declaration[2].Offset == 24);
    CHECK(declaration[2].Type == 5);

    // A palette too small for one face fails.
    Ref<ID3DXMesh> none;
    CHECK(skin.p->ConvertToIndexedBlendedMesh(mesh.p, D3DXMESH_MANAGED, 1, nullptr, nullptr, nullptr, nullptr,
                                              nullptr, nullptr, nullptr, &none.p) == D3DERR_INVALIDCALL);
    CHECK(none.p == nullptr);
}

namespace {

// A root frame moved by +1 on y holding a skinned triangle and a child frame with an unskinned one.
const char* const kHierarchy = R"(xof 0303txt 0032
Frame Root {
  FrameTransformMatrix { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,1,0,1;; }
  Mesh Body {
    3; 0;0;0;, 2;0;0;, 0;2;0;;
    2; 3;0,1,2;, 3;0,1,2;;
    MeshNormals { 2; 0;0;-1;, 0;0;1;; 2; 3;0,0,0;, 3;0,0,1;; }
    MeshMaterialList { 1; 1; 0;; Material { 1;1;1;1;; 0; 0;0;0;; 0;0;0;; TextureFilename { "body.dds"; } } }
    XSkinMeshHeader { 2; 2; 2; }
    SkinWeights { "Bip01"; 2; 0, 2; 1.0, 0.5; 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1;; }
    SkinWeights { "Bip01_Head"; 1; 2; 0.5; 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,-2,0,1;; }
  }
  Frame Bip01 {
    FrameTransformMatrix { 1,0,0,0, 0,1,0,0, 0,0,1,0, 10,0,0,1;; }
    Frame Bip01_Head { }
    Mesh { 3; 0;0;0;, 1;0;0;, 0;1;0;; 1; 3;0,1,2;; }
  }
}
)";

}  // namespace

TEST_CASE("d3dx: D3DXLoadMeshHierarchyFromXInMemory and D3DXFrameCalculateBoundingSphere")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    Allocator alloc;
    LPD3DXFRAME root = nullptr;
    LPD3DXANIMATIONCONTROLLER controller = reinterpret_cast<LPD3DXANIMATIONCONTROLLER>(&alloc);
    const std::string text = kHierarchy;
    REQUIRE(D3DXLoadMeshHierarchyFromXInMemory(text.data(), static_cast<DWORD>(text.size()), D3DXMESH_MANAGED, &device,
                                               &alloc, nullptr, &root, &controller) == D3D_OK);
    CHECK(controller == nullptr);   // the animation step
    REQUIRE(root != nullptr);
    CHECK(std::string(root->Name) == "Root");
    CHECK(root->TransformationMatrix._42 == 1.0f);
    CHECK(root->pFrameSibling == nullptr);
    CHECK(alloc.frames == 3);
    CHECK(alloc.containers == 2);

    LPD3DXMESHCONTAINER body = root->pMeshContainer;
    REQUIRE(body != nullptr);
    CHECK(std::string(body->Name) == "Body");
    CHECK(body->pNextMeshContainer == nullptr);
    CHECK(body->NumMaterials == 1);
    CHECK(std::string(body->pMaterials[0].pTextureFilename) == "body.dds");
    CHECK(body->MeshData.pMesh->GetNumVertices() == 4);     // vertex 2 split by its normal
    REQUIRE(body->pSkinInfo != nullptr);
    LPD3DXSKININFO skin = body->pSkinInfo;
    CHECK(skin->GetNumBones() == 2);
    CHECK(skin->GetFVF() == body->MeshData.pMesh->GetFVF());
    CHECK(std::string(skin->GetBoneName(1)) == "Bip01_Head");
    CHECK(skin->GetBoneOffsetMatrix(1)->_42 == -2.0f);
    // File vertex 2 is now vertices 2 and 3: both take its weights.
    REQUIRE(skin->GetNumBoneInfluences(0) == 3);
    REQUIRE(skin->GetNumBoneInfluences(1) == 2);
    DWORD vertices[3];
    float weights[3];
    skin->GetBoneInfluence(1, vertices, weights);
    CHECK(std::set<DWORD>(vertices, vertices + 2) == std::set<DWORD>{2, 3});
    CHECK(weights[0] == 0.5f);

    LPD3DXFRAME bip = root->pFrameFirstChild;
    REQUIRE(bip != nullptr);
    CHECK(std::string(bip->Name) == "Bip01");
    REQUIRE(bip->pFrameFirstChild != nullptr);
    CHECK(std::string(bip->pFrameFirstChild->Name) == "Bip01_Head");
    REQUIRE(bip->pMeshContainer != nullptr);
    CHECK(bip->pMeshContainer->Name == nullptr);
    CHECK(bip->pMeshContainer->pSkinInfo == nullptr);
    CHECK(bip->pMeshContainer->NumMaterials == 1);      // D3DX's default material
    CHECK(D3DXFrameFind(root, "Bip01_Head") == bip->pFrameFirstChild);

    // The sphere around every vertex moved by its frames: the body at y + 1, the other triangle at x + 10.
    D3DXVECTOR3 center;
    float radius = 0;
    REQUIRE(D3DXFrameCalculateBoundingSphere(root, &center, &radius) == D3D_OK);
    // Positions: (0,1) (2,1) (0,3) (0,3) and (10,1) (11,1) (10,2), all z 0.
    CHECK(center.x == doctest::Approx(33.0f / 7));
    CHECK(center.y == doctest::Approx(12.0f / 7));
    CHECK(radius == doctest::Approx(std::sqrt((11 - 33.0f / 7) * (11 - 33.0f / 7) + (1 - 12.0f / 7) * (1 - 12.0f / 7))));
    REQUIRE(D3DXFrameCalculateBoundingSphere(bip->pFrameFirstChild, &center, &radius) == D3D_OK);
    CHECK(radius == 0.0f);      // no mesh

    CHECK(D3DXFrameDestroy(root, &alloc) == D3D_OK);
    CHECK(alloc.frames == 0);
    CHECK(alloc.containers == 0);

    // Two top-level frames: a nameless root around them.
    const std::string two = "xof 0303txt 0032 Frame A { } Frame B { Mesh { 3; 0;0;0;, 1;0;0;, 0;1;0;; 1; 3;0,1,2;; } }";
    REQUIRE(D3DXLoadMeshHierarchyFromXInMemory(two.data(), static_cast<DWORD>(two.size()), D3DXMESH_MANAGED, &device,
                                               &alloc, nullptr, &root, nullptr) == D3D_OK);
    CHECK(root->Name == nullptr);
    REQUIRE(root->pFrameFirstChild != nullptr);
    CHECK(std::string(root->pFrameFirstChild->Name) == "A");
    CHECK(std::string(root->pFrameFirstChild->pFrameSibling->Name) == "B");
    D3DXFrameDestroy(root, &alloc);
    CHECK(alloc.frames == 0);

    const std::string empty = "xof 0303txt 0032 Material { 1;1;1;1;; 0; 0;0;0;; 0;0;0;; }";
    CHECK(FAILED(D3DXLoadMeshHierarchyFromXInMemory(empty.data(), static_cast<DWORD>(empty.size()), 0, &device,
                                                    &alloc, nullptr, &root, nullptr)));
    CHECK(root == nullptr);
}
