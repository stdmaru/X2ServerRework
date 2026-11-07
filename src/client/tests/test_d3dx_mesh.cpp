// D3DX's meshes on KTDXLIB's device (docs/porting/client-d3d9.md#meshes-2026-11-07-clientx-loader): indexed draws, .x
// files in text, binary and MSZIP form, the mesh operations the engine calls, D3DXLoadMeshFromXInMemory, and, with
// X2_MASSFILE_CLIENT_DATA set to the client's data folder, every .x file of the client's archives.

#include "D3DXTypes.h"
#include "Device.h"
#include "Mesh.h"
#include "XFile.h"

#include "x2/kom/mount.h"
#include "x2/render/null.h"

#include <doctest/doctest.h>
#include <zlib.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace {

namespace xfile = x2::d3d9::xfile;
using Bytes = std::vector<std::byte>;

std::unique_ptr<x2::render::Device> NullRender()
{
    std::string error;
    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::Null;
    auto device = x2::render::Device::Create(options, error);
    REQUIRE_MESSAGE(device, error);
    return device;
}

std::vector<std::string> Lines(x2::render::Device& device, const std::string& prefix)
{
    std::vector<std::string> lines;
    for (const std::string& line : x2::render::null::Commands(device)) {
        if (line.rfind(prefix, 0) == 0) {
            lines.push_back(line);
        }
    }
    return lines;
}

Bytes FromText(const std::string& text)
{
    Bytes out(text.size());
    std::memcpy(out.data(), text.data(), text.size());
    return out;
}

// Binary .x tokens.
struct BinaryWriter {
    Bytes out = FromText("xof 0303bin 0032");
    void Word(unsigned v)
    {
        out.push_back(static_cast<std::byte>(v & 0xFF));
        out.push_back(static_cast<std::byte>((v >> 8) & 0xFF));
    }
    void Dword(std::uint32_t v)
    {
        Word(v & 0xFFFF);
        Word(v >> 16);
    }
    void Name(const std::string& name)
    {
        Word(1);
        Dword(static_cast<std::uint32_t>(name.size()));
        out.insert(out.end(), reinterpret_cast<const std::byte*>(name.data()),
                   reinterpret_cast<const std::byte*>(name.data()) + name.size());
    }
    void String(const std::string& text)
    {
        Word(2);
        Dword(static_cast<std::uint32_t>(text.size()));
        out.insert(out.end(), reinterpret_cast<const std::byte*>(text.data()),
                   reinterpret_cast<const std::byte*>(text.data()) + text.size());
        Word(20);   // ';'
    }
    void Open() { Word(10); }
    void Close() { Word(11); }
    void Integers(std::initializer_list<std::uint32_t> values)
    {
        Word(6);
        Dword(static_cast<std::uint32_t>(values.size()));
        for (std::uint32_t v : values) {
            Dword(v);
        }
    }
    void Floats(std::initializer_list<float> values)
    {
        Word(7);
        Dword(static_cast<std::uint32_t>(values.size()));
        for (float v : values) {
            std::uint32_t bits;
            std::memcpy(&bits, &v, 4);
            Dword(bits);
        }
    }
};

// MSZIP: the plain file's body in blocks of `blockSize`, each with the previous block as dictionary. `noNlen` stores
// the blocks uncompressed and drops the stored blocks' NLEN, as some of the client's files have it.
Bytes Compress(const Bytes& plain, bool binary, std::size_t blockSize, bool noNlen = false)
{
    Bytes out = FromText(binary ? "xof 0303bzip0032" : "xof 0303tzip0032");
    auto put16 = [&](unsigned v) {
        out.push_back(static_cast<std::byte>(v & 0xFF));
        out.push_back(static_cast<std::byte>(v >> 8));
    };
    const auto total = static_cast<std::uint32_t>(plain.size());
    put16(total & 0xFFFF);
    put16(total >> 16);
    for (std::size_t at = 16; at < plain.size(); at += blockSize) {
        const std::size_t size = std::min(blockSize, plain.size() - at);
        z_stream z{};
        REQUIRE(deflateInit2(&z, noNlen ? 0 : 9, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) == Z_OK);
        if (at > 16) {
            const std::size_t dictionary = std::min<std::size_t>(at - 16, 32768);
            deflateSetDictionary(&z, reinterpret_cast<const Bytef*>(plain.data() + at - dictionary),
                                 static_cast<uInt>(dictionary));
        }
        std::vector<Bytef> packed(deflateBound(&z, static_cast<uLong>(size)));
        z.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(plain.data() + at));
        z.avail_in = static_cast<uInt>(size);
        z.next_out = packed.data();
        z.avail_out = static_cast<uInt>(packed.size());
        REQUIRE(deflate(&z, Z_FINISH) == Z_STREAM_END);
        std::size_t packedSize = z.total_out;
        deflateEnd(&z);
        if (noNlen) {
            // One final stored block: the header byte, LEN, NLEN, the data.
            REQUIRE(packedSize == size + 5);
            packed.erase(packed.begin() + 3, packed.begin() + 5);
            packedSize -= 2;
        }
        put16(static_cast<unsigned>(size));
        put16(static_cast<unsigned>(packedSize + 2));
        out.push_back(std::byte{'C'});
        out.push_back(std::byte{'K'});
        out.insert(out.end(), reinterpret_cast<const std::byte*>(packed.data()),
                   reinterpret_cast<const std::byte*>(packed.data()) + packedSize);
    }
    return out;
}

// A unit quad in a frame moved by +10 on x, its two halves in two materials, the second one by reference; normals
// split the corner shared by both halves.
const char* const kQuad = R"(xof 0303txt 0032
// a comment
template Unused { <3D82AB43-62DA-11cf-AB39-0020AF71E433> DWORD x; }
Material Shared {
  0.0; 1.0; 0.0; 1.0;;
  5.0;
  0.1; 0.2; 0.3;;
  0.0; 0.0; 0.0;;
  TextureFilename { "green.dds"; }
}
Frame Root {
  FrameTransformMatrix { 1,0,0,0, 0,1,0,0, 0,0,1,0, 10,0,0,1;; }
  Mesh Quad {
    4;
    0;0;0;, 1;0;0;, 1;1;0;, 0;1;0;;
    2;
    3;0,1,2;, 3;0,2,3;;
    MeshNormals {
      2;
      0;0;-1;, 0;0;1;;
      2;
      3;0,0,0;, 3;1,0,0;;
    }
    MeshTextureCoords { 4; 0;0;, 1;0;, 1;1;, 0;1;; }
    MeshMaterialList {
      2; 2; 0, 1;;
      Material Red { 1.0;0.0;0.0;1.0;; 1.0; 0.0;0.0;0.0;; 0.0;0.0;0.0;; }
      { Shared }
    }
  }
}
)";

struct MeshPtr {
    LPD3DXMESH p = nullptr;
    ~MeshPtr()
    {
        if (p) {
            p->Release();
        }
    }
    x2::d3d9::Mesh* operator->() const { return static_cast<x2::d3d9::Mesh*>(p); }
};

struct BufferPtr {
    LPD3DXBUFFER p = nullptr;
    ~BufferPtr()
    {
        if (p) {
            p->Release();
        }
    }
    template <class T>
    T* As() const
    {
        return static_cast<T*>(p->GetBufferPointer());
    }
};

D3DXVECTOR3 PositionOf(x2::d3d9::Mesh& mesh, DWORD vertex)
{
    const auto vertices = mesh.Vertices();
    D3DXVECTOR3 p;
    std::memcpy(&p, vertices.data() + std::size_t{vertex} * mesh.GetNumBytesPerVertex(), 12);
    return p;
}

struct PositionVertex {
    float x, y, z;
};

// A mesh of `positions` and `indices` in D3DFVF_XYZ.
MeshPtr MakeMesh(x2::d3d9::Device& device, const std::vector<PositionVertex>& positions,
                 const std::vector<std::uint32_t>& indices, const std::vector<DWORD>& attributes, DWORD options = 0)
{
    MeshPtr mesh;
    REQUIRE(D3DXCreateMeshFVF(static_cast<DWORD>(attributes.size()), static_cast<DWORD>(positions.size()),
                              options | D3DXMESH_MANAGED, D3DFVF_XYZ, &device, &mesh.p) == D3D_OK);
    Bytes bytes(positions.size() * sizeof(PositionVertex));
    std::memcpy(bytes.data(), positions.data(), bytes.size());
    REQUIRE(mesh->SetContents(bytes, indices, attributes));
    return mesh;
}

}  // namespace

TEST_CASE("d3d9: indexed draws")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    auto vertices = device.CreateVertexBuffer(4 * 12, 0, D3DFVF_XYZ, D3DPOOL_MANAGED);
    auto indices = device.CreateIndexBuffer(6 * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED);
    auto wide = device.CreateIndexBuffer(6 * 4, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED);
    REQUIRE(vertices);
    REQUIRE(indices);
    REQUIRE(wide);
    CHECK(indices->Size() == 12);
    CHECK_FALSE(device.CreateIndexBuffer(12, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED));
    void* data = nullptr;
    REQUIRE(indices->Lock(0, 0, &data, 0));
    const std::uint16_t list[6] = {0, 1, 2, 0, 2, 3};
    std::memcpy(data, list, sizeof(list));
    REQUIRE(indices->Unlock());

    auto target = device.CreateTexture(16, 16, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    device.SetRenderTarget(0, target.get());
    device.SetFVF(D3DFVF_XYZ);
    device.SetStreamSource(0, vertices.get(), 0, 12);
    REQUIRE(device.BeginScene());
    CHECK_FALSE(device.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2));     // no indices yet
    device.SetIndices(indices.get());
    CHECK(device.GetIndices() == indices.get());
    CHECK(device.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2));
    CHECK(device.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 1, 0, 3, 3, 1));
    CHECK_FALSE(device.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 3, 2));     // past the buffer
    CHECK_FALSE(device.DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, 4, 0, 2));
    device.SetIndices(wide.get());
    CHECK(device.DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 4, 0, 2));
    device.EndScene();
    x2::render::null::ClearCommands(*render);
    device.Present();
    CHECK(Lines(*render, "DrawIndexedPrimitives indices=6 instances=1 first=0 vertexOffset=0").size() == 1);
    CHECK(Lines(*render, "DrawIndexedPrimitives indices=3 instances=1 first=3 vertexOffset=1").size() == 1);
    CHECK(Lines(*render, "DrawIndexedPrimitives indices=4 instances=1 first=0 vertexOffset=0").size() == 1);
    const auto binds = Lines(*render, "BindIndexBuffer");
    REQUIRE(binds.size() >= 2);
    CHECK(binds.front().ends_with(" u16"));
    CHECK(binds.back().ends_with(" u32"));

    // A released index buffer is unset.
    indices.reset();
    wide.reset();
    CHECK(device.GetIndices() == nullptr);
}

TEST_CASE("xfile: text files")
{
    std::string error;
    CHECK_FALSE(xfile::ReadHeader(FromText("DDS ")));
    const auto header = xfile::ReadHeader(FromText("xof 0303bzip0064"));
    REQUIRE(header);
    CHECK(header->encoding == xfile::Encoding::Binary);
    CHECK(header->compressed);
    CHECK(header->floatBits == 64);

    const auto file = xfile::Read(FromText(kQuad), error);
    REQUIRE_MESSAGE(file, error);
    REQUIRE(file->objects.size() == 2);     // the template is skipped
    const xfile::Object& material = file->objects[0];
    CHECK(material.type == "Material");
    CHECK(material.name == "Shared");
    xfile::Cursor in(material);
    CHECK(in.Float() == 0.0f);
    CHECK(in.Float() == 1.0f);
    CHECK(in.Remaining() == 9);
    const xfile::Object* texture = xfile::FirstChildOfType(material, "TextureFilename");
    REQUIRE(texture);
    CHECK(xfile::Cursor(*texture).String() == "green.dds");

    const xfile::Object& root = file->objects[1];
    CHECK(root.type == "Frame");
    const xfile::Object* mesh = xfile::FirstChildOfType(root, "Mesh");
    REQUIRE(mesh);
    CHECK(mesh->name == "Quad");
    CHECK(mesh->data.size() == 1 + 12 + 1 + 8);
    const xfile::Object* list = xfile::FirstChildOfType(*mesh, "MeshMaterialList");
    REQUIRE(list);
    REQUIRE(list->children.size() == 2);
    CHECK(list->children[1].IsReference());
    CHECK(list->children[1].name == "Shared");

    xfile::Cursor past(*texture);
    past.String();
    CHECK(past.Number() == 0);
    CHECK(past.Failed());

    CHECK_FALSE(xfile::Read(FromText("xof 0303txt 0032 Mesh { 1; "), error));
    CHECK_FALSE(error.empty());
}

TEST_CASE("xfile: binary and compressed files")
{
    BinaryWriter w;
    w.Name("Header");
    w.Open();
    w.Integers({1, 0, 1});
    w.Close();
    w.Name("Frame");
    w.Name("Root");
    w.Open();
    w.Name("TextureFilename");
    w.Open();
    w.String("a.dds");
    w.Close();
    w.Name("Mesh");
    w.Open();
    w.Integers({3});
    w.Floats({0, 0, 0, 1, 0, 0, 0, 1, 0});
    w.Integers({1, 3, 0, 1, 2});
    w.Close();
    w.Open();
    w.Name("Shared");
    w.Close();
    w.Close();

    std::string error;
    auto check = [&](const Bytes& bytes) {
        const auto file = xfile::Read(bytes, error);
        REQUIRE_MESSAGE(file, error);
        REQUIRE(file->objects.size() == 2);
        CHECK(file->objects[0].type == "Header");
        CHECK(file->objects[0].data.size() == 3);
        const xfile::Object& root = file->objects[1];
        CHECK(root.name == "Root");
        REQUIRE(root.children.size() == 3);
        CHECK(xfile::Cursor(root.children[0]).String() == "a.dds");
        const xfile::Object& mesh = root.children[1];
        CHECK(mesh.type == "Mesh");
        CHECK(mesh.name.empty());
        xfile::Cursor in(mesh);
        CHECK(in.Dword() == 3);
        CHECK(in.Remaining() == 9 + 5);
        CHECK(root.children[2].IsReference());
        CHECK(root.children[2].name == "Shared");
    };
    check(w.out);
    // Compressed in one block, and in blocks that lean on the previous one's data.
    check(Compress(w.out, true, 0x8000));
    check(Compress(w.out, true, 7));
    check(Compress(w.out, true, 40, true));
    const auto text = xfile::Read(Compress(FromText(kQuad), false, 50), error);
    REQUIRE_MESSAGE(text, error);
    CHECK(text->objects.size() == 2);

    Bytes broken = Compress(w.out, true, 0x8000);
    broken[16 + 4 + 4] = std::byte{'X'};    // the "CK" signature
    CHECK_FALSE(xfile::Read(broken, error));
    Bytes truncated(w.out.begin(), w.out.end() - 6);
    CHECK_FALSE(xfile::Read(truncated, error));
}

TEST_CASE("d3dx: D3DXLoadMeshFromXInMemory")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    const Bytes bytes = FromText(kQuad);
    MeshPtr mesh;
    BufferPtr adjacency, materials, effects;
    DWORD materialCount = 0;
    REQUIRE(D3DXLoadMeshFromXInMemory(bytes.data(), static_cast<DWORD>(bytes.size()), D3DXMESH_MANAGED, &device,
                                      &adjacency.p, &materials.p, &effects.p, &materialCount, &mesh.p) == D3D_OK);
    CHECK(mesh->GetFVF() == (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));
    CHECK(mesh->GetNumFaces() == 2);
    CHECK(mesh->GetNumVertices() == 5);     // vertex 0 split by the second half's normal
    CHECK_FALSE(mesh->Uses32BitIndices());
    CHECK(mesh->Indices() == std::vector<std::uint32_t>{0, 1, 2, 4, 2, 3});
    CHECK(mesh->Attributes() == std::vector<DWORD>{0, 1});
    const D3DXVECTOR3 moved = PositionOf(*mesh.operator->(), 1);
    CHECK(moved.x == doctest::Approx(11.0f));
    CHECK(moved.y == doctest::Approx(0.0f));

    REQUIRE(materialCount == 2);
    const auto* entries = materials.As<D3DXMATERIAL>();
    CHECK(entries[0].MatD3D.Diffuse.r == 1.0f);
    CHECK(entries[0].pTextureFilename == nullptr);
    CHECK(entries[1].MatD3D.Diffuse.g == 1.0f);
    CHECK(entries[1].MatD3D.Power == 5.0f);
    CHECK(entries[1].MatD3D.Specular.b == doctest::Approx(0.3f));
    REQUIRE(entries[1].pTextureFilename != nullptr);
    CHECK(std::string(entries[1].pTextureFilename) == "green.dds");
    CHECK(effects.p->GetBufferSize() == 2 * sizeof(D3DXEFFECTINSTANCE));
    // The halves share the diagonal through point representatives even though their vertices differ.
    const DWORD* adjacent = adjacency.As<DWORD>();
    CHECK(std::count(adjacent, adjacent + 6, 1u) == 1);
    CHECK(std::count(adjacent, adjacent + 6, 0u) == 1);
    CHECK(std::count(adjacent, adjacent + 6, 0xFFFFFFFFu) == 4);

    // No materials: D3DX's default one.
    const Bytes plain = FromText("xof 0303txt 0032 Mesh { 3; 0;0;0;, 1;0;0;, 0;1;0;; 1; 3;0,1,2;; }");
    MeshPtr bare;
    BufferPtr bareMaterials;
    REQUIRE(D3DXLoadMeshFromXInMemory(plain.data(), static_cast<DWORD>(plain.size()), 0, &device, nullptr,
                                      &bareMaterials.p, nullptr, &materialCount, &bare.p) == D3D_OK);
    CHECK(bare->GetFVF() == D3DFVF_XYZ);
    REQUIRE(materialCount == 1);
    CHECK(bareMaterials.As<D3DXMATERIAL>()[0].MatD3D.Diffuse.r == 0.5f);

    MeshPtr none;
    const Bytes empty = FromText("xof 0303txt 0032 Frame { }");
    CHECK(FAILED(D3DXLoadMeshFromXInMemory(empty.data(), static_cast<DWORD>(empty.size()), 0, &device, nullptr,
                                           nullptr, nullptr, &materialCount, &none.p)));
    CHECK(none.p == nullptr);
}

TEST_CASE("d3dx: mesh operations")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    MeshPtr fail;
    CHECK(D3DXCreateMeshFVF(1, 70000, 0, D3DFVF_XYZ, &device, &fail.p) == D3DERR_INVALIDCALL);   // needs 32BIT
    CHECK(D3DXCreateMeshFVF(1, 3, 0, 0, &device, &fail.p) == D3DERR_INVALIDCALL);

    // Two quads side by side, with the shared corners duplicated and the materials interleaved.
    const std::vector<PositionVertex> positions = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
                                                   {1, 0, 0}, {2, 0, 0}, {2, 1, 0}, {1, 1, 0}, {5, 5, 5}};
    const std::vector<std::uint32_t> indices = {0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 6, 7};
    MeshPtr mesh = MakeMesh(device, positions, indices, {0, 1, 0, 1});
    CHECK(mesh->GetNumBytesPerVertex() == 12);

    D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE];
    MeshPtr colored;
    REQUIRE(mesh.p->CloneMeshFVF(D3DXMESH_32BIT | D3DXMESH_MANAGED, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE |
                                                                        D3DFVF_TEX1, &device, &colored.p) == D3D_OK);
    CHECK(colored->Uses32BitIndices());
    CHECK(colored->GetNumBytesPerVertex() == 36);
    CHECK(colored->Indices() == indices);
    const auto coloredVertices = colored->Vertices();
    DWORD diffuse = 0;
    std::memcpy(&diffuse, coloredVertices.data() + 36 + 24, 4);
    CHECK(diffuse == 0xFFFFFFFF);
    CHECK(PositionOf(*colored.operator->(), 5).x == 2.0f);
    REQUIRE(colored.p->GetDeclaration(declaration) == D3D_OK);
    CHECK(declaration[1].Offset == 12);
    CHECK(declaration[1].Usage == 3);   // normal
    CHECK(declaration[2].Type == 4);    // D3DCOLOR
    CHECK(declaration[3].Usage == 5);   // texcoord
    CHECK(declaration[4].Stream == 0xFF);

    // Adjacency: the quads' halves through their shared diagonals, and the quads through the duplicated corners.
    std::vector<DWORD> adjacency(12);
    REQUIRE(mesh.p->GenerateAdjacency(0.0f, adjacency.data()) == D3D_OK);
    CHECK(adjacency == std::vector<DWORD>{0xFFFFFFFF, 3, 2, 0xFFFFFFFF, 0xFFFFFFFF, 3, 0, 0xFFFFFFFF, 0xFFFFFFFF, 1,
                                          0xFFFFFFFF, 0});

    // Welding with zero epsilons merges the duplicated corners and removes them; the unused vertex stays.
    const D3DXWELDEPSILONS zero{};
    BufferPtr remap;
    REQUIRE(D3DXWeldVertices(mesh.p, 0, &zero, nullptr, nullptr, nullptr, &remap.p) == D3D_OK);
    CHECK(mesh->GetNumVertices() == 7);
    CHECK(mesh->Indices() == std::vector<std::uint32_t>{0, 1, 2, 1, 4, 5, 0, 2, 3, 1, 5, 2});
    CHECK(remap.p->GetBufferSize() == 7 * 4);
    CHECK(remap.As<DWORD>()[6] == 8);
    CHECK(PositionOf(*mesh.operator->(), 6).z == 5.0f);

    // Optimize: faces sorted by attribute (stable), vertices in first use order, the stray one gone.
    MeshPtr optimized;
    std::vector<DWORD> faceRemap(4), optimizedAdjacency(12);
    REQUIRE(mesh.p->Optimize(DWORD{D3DXMESH_MANAGED} | D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_ATTRSORT, nullptr,
                             optimizedAdjacency.data(), faceRemap.data(), nullptr, &optimized.p) == D3D_OK);
    CHECK(faceRemap == std::vector<DWORD>{0, 2, 1, 3});
    CHECK(optimized->GetNumVertices() == 6);
    CHECK(optimized->Attributes() == std::vector<DWORD>{0, 0, 1, 1});
    CHECK(optimized->Indices() == std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3, 1, 4, 5, 1, 5, 2});
    DWORD tableSize = 0;
    REQUIRE(optimized.p->GetAttributeTable(nullptr, &tableSize) == D3D_OK);
    REQUIRE(tableSize == 2);
    D3DXATTRIBUTERANGE table[2];
    optimized.p->GetAttributeTable(table, nullptr);
    CHECK(table[0].AttribId == 0);
    CHECK(table[0].FaceStart == 0);
    CHECK(table[0].FaceCount == 2);
    CHECK(table[0].VertexStart == 0);
    CHECK(table[0].VertexCount == 4);
    CHECK(table[1].FaceStart == 2);
    CHECK(table[1].VertexStart == 1);
    CHECK(table[1].VertexCount == 5);
    CHECK(optimizedAdjacency[2] == 1);      // face 0's diagonal is face 1 after the sort
    CHECK(optimizedAdjacency[1] == 3);

    // A ray down the z axis hits the second quad's first half from both sides; the closest hit wins.
    BOOL hit = FALSE;
    DWORD face = 0, hits = 0;
    float u = 0, v = 0, distance = 0;
    const D3DXVECTOR3 origin(1.75f, 0.25f, -3.0f), down(0, 0, 1), up(0, 0, -1);
    REQUIRE(D3DXIntersect(optimized.p, &origin, &down, &hit, &face, &u, &v, &distance, nullptr, &hits) == D3D_OK);
    CHECK(hit);
    CHECK(face == 2);
    CHECK(distance == doctest::Approx(3.0f));
    CHECK(hits == 1);
    REQUIRE(D3DXIntersect(optimized.p, &origin, &up, &hit, &face, &u, &v, &distance, nullptr, &hits) == D3D_OK);
    CHECK_FALSE(hit);
    CHECK(hits == 0);

    // DrawSubset draws each attribute's range.
    auto target = device.CreateTexture(16, 16, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    device.SetRenderTarget(0, target.get());
    REQUIRE(device.BeginScene());
    CHECK(optimized.p->DrawSubset(1) == D3D_OK);
    CHECK(optimized.p->DrawSubset(7) == D3D_OK);   // nothing to draw
    CHECK(mesh.p->DrawSubset(0) == D3D_OK);         // no table: each run of the attribute
    device.EndScene();
    x2::render::null::ClearCommands(*render);
    device.Present();
    CHECK(Lines(*render, "DrawIndexedPrimitives indices=6 instances=1 first=6").size() == 1);
    CHECK(Lines(*render, "DrawIndexedPrimitives indices=3 instances=1 first=0").size() == 1);
    CHECK(Lines(*render, "DrawIndexedPrimitives indices=3 instances=1 first=6").size() == 1);
}

TEST_CASE("d3dx: the client's .x files (X2_MASSFILE_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_MASSFILE_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0') {
        MESSAGE("X2_MASSFILE_CLIENT_DATA not set: skipped");
        return;
    }
    std::string error;
    x2::kom::Mount mount([](const std::string&) {});
    REQUIRE_MESSAGE(mount.AddFolder(folder, error), error);
    std::vector<std::string> names;
    std::set<std::string> seen;
    for (const auto& archive : mount.Archives()) {
        for (const x2::kom::Entry& entry : archive->Entries()) {
            std::string lower = entry.name;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                           [](char c) { return static_cast<char>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c); });
            if ((lower.ends_with(".x") || lower.ends_with(".y")) && seen.insert(lower).second) {
                names.push_back(entry.name);
            }
        }
    }

    // Every xof file reads; every one with a mesh loads, and goes through the engine's static mesh steps
    // (CKTDXDeviceXMesh: clone to its vertex format, weld, optimize).
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    std::atomic<std::size_t> next{0};
    std::mutex mutex, deviceMutex;
    std::size_t xof = 0, withMesh = 0, loaded = 0;
    std::map<std::string, std::string> failed;
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&] {
            std::vector<std::uint8_t> bytes;
            std::string readError;
            for (std::size_t i = next++; i < names.size(); i = next++) {
                if (!mount.Read(names[i], bytes, readError) || bytes.size() < 16) {
                    continue;
                }
                const std::span<const std::byte> view(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size());
                if (!xfile::ReadHeader(view)) {
                    continue;   // KME/KSM, read by the engine itself
                }
                std::string parseError;
                const auto file = xfile::Read(view, parseError);
                bool hasMesh = false;
                if (file) {
                    std::vector<const xfile::Object*> stack;
                    for (const auto& object : file->objects) {
                        stack.push_back(&object);
                    }
                    while (!stack.empty() && !hasMesh) {
                        const xfile::Object* object = stack.back();
                        stack.pop_back();
                        hasMesh = object->type == "Mesh";
                        for (const auto& child : object->children) {
                            stack.push_back(&child);
                        }
                    }
                }
                std::string meshError;
                if (file && hasMesh) {
                    const std::lock_guard lock(deviceMutex);
                    MeshPtr mesh, clone, optimized;
                    BufferPtr adjacencyBuffer, materials;
                    DWORD count = 0;
                    HRESULT hr = D3DXLoadMeshFromXInMemory(bytes.data(), static_cast<DWORD>(bytes.size()),
                                                           D3DXMESH_MANAGED, &device, &adjacencyBuffer.p,
                                                           &materials.p, nullptr, &count, &mesh.p);
                    if (FAILED(hr)) {
                        meshError = "D3DXLoadMeshFromXInMemory";
                    } else if (FAILED(mesh.p->CloneMeshFVF(D3DXMESH_32BIT | D3DXMESH_MANAGED,
                                                           D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                                           &device, &clone.p))) {
                        meshError = "CloneMeshFVF";
                    } else {
                        std::vector<DWORD> adjacency(clone->GetNumFaces() * 3);
                        const D3DXWELDEPSILONS zero{};
                        std::memcpy(adjacency.data(), adjacencyBuffer.p->GetBufferPointer(), adjacency.size() * 4);
                        if (FAILED(D3DXWeldVertices(clone.p, 0, &zero, adjacency.data(), adjacency.data(), nullptr,
                                                    nullptr))) {
                            meshError = "D3DXWeldVertices";
                        } else if (FAILED(clone.p->Optimize(DWORD{D3DXMESH_MANAGED} | D3DXMESHOPT_VERTEXCACHE |
                                                                D3DXMESHOPT_ATTRSORT,
                                                            adjacency.data(), nullptr, nullptr, nullptr,
                                                            &optimized.p))) {
                            meshError = "Optimize";
                        } else {
                            for (DWORD attribute : optimized->Attributes()) {
                                if (attribute >= count) {
                                    meshError = "attribute past the materials";
                                    break;
                                }
                            }
                        }
                    }
                }
                const std::lock_guard lock(mutex);
                ++xof;
                if (!file) {
                    failed[names[i]] = parseError;
                } else if (hasMesh) {
                    ++withMesh;
                    if (meshError.empty()) {
                        ++loaded;
                    } else {
                        failed[names[i]] = meshError;
                    }
                }
            }
        });
    }
    for (std::thread& thread : threads) {
        thread.join();
    }
    MESSAGE(names.size() << " .x/.y names, " << xof << " xof files, " << withMesh << " with meshes, " << loaded
                         << " loaded");
    std::size_t shown = 0;
    for (const auto& [name, why] : failed) {
        if (shown++ < 40) {
            MESSAGE(name << ": " << why);
        }
    }
    CHECK(failed.empty());
}
