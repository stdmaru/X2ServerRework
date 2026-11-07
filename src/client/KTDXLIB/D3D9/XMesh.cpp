// Meshes read from .x files, and D3DXLoadMeshFromXInMemory.
// docs/porting/client-d3d9.md#meshes-2026-11-07-clientx-loader

#include "XMesh.h"

#include "Mesh.h"

#include <algorithm>
#include <cstring>
#include <map>

namespace x2::d3d9 {
namespace {

using xfile::Cursor;
using xfile::Object;

// D3DX's material for a mesh without a material list.
XMaterial DefaultMaterial()
{
    XMaterial m;
    m.material.Diffuse = {0.5f, 0.5f, 0.5f, 1.0f};
    m.material.Specular = {0.5f, 0.5f, 0.5f, 1.0f};
    return m;
}

const Object* FindNamed(const std::vector<Object>& objects, const char* type, const std::string& name)
{
    for (const Object& object : objects) {
        if (object.type == type && object.name == name) {
            return &object;
        }
        if (const Object* found = FindNamed(object.children, type, name)) {
            return found;
        }
    }
    return nullptr;
}

XMaterial ReadMaterial(const Object& object)
{
    Cursor in(object);
    XMaterial m;
    m.material.Diffuse = {in.Float(), in.Float(), in.Float(), in.Float()};
    m.material.Power = in.Float();
    m.material.Specular = {in.Float(), in.Float(), in.Float(), 1.0f};
    m.material.Emissive = {in.Float(), in.Float(), in.Float(), 1.0f};
    if (const Object* texture = xfile::FirstChildOfType(object, "TextureFilename")) {
        m.texture = Cursor(*texture).String();
    }
    return m;
}

// Polygons: corner indices, each below `limit`.
bool ReadFaces(Cursor& in, DWORD limit, std::vector<std::vector<DWORD>>& faces)
{
    const std::uint32_t count = in.Dword();
    if (in.Failed() || count > in.Remaining()) {
        return false;
    }
    faces.resize(count);
    for (std::vector<DWORD>& face : faces) {
        const std::uint32_t corners = in.Dword();
        if (in.Failed() || corners > in.Remaining()) {
            return false;
        }
        face.resize(corners);
        for (DWORD& index : face) {
            index = in.Dword();
            if (index >= limit) {
                return false;
            }
        }
    }
    return !in.Failed();
}

D3DXVECTOR3 ReadVector(Cursor& in)
{
    const float x = in.Float(), y = in.Float(), z = in.Float();
    return {x, y, z};
}

DWORD ToColor(float r, float g, float b, float a)
{
    auto channel = [](float value) {
        return static_cast<DWORD>(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
    };
    return (channel(a) << 24) | (channel(r) << 16) | (channel(g) << 8) | channel(b);
}

}  // namespace

bool ReadXMesh(const xfile::File& file, const Object& mesh, XMesh& out, std::string& error)
{
    out = {};
    Cursor in(mesh);
    const std::uint32_t positionCount = in.Dword();
    if (in.Failed() || positionCount > in.Remaining() / 3) {
        error = "Mesh " + mesh.name + ": bad vertex count";
        return false;
    }
    std::vector<D3DXVECTOR3> positions(positionCount);
    for (D3DXVECTOR3& p : positions) {
        p = ReadVector(in);
    }
    std::vector<std::vector<DWORD>> faces;
    if (!ReadFaces(in, positionCount, faces)) {
        error = "Mesh " + mesh.name + ": bad faces";
        return false;
    }

    // Normals: their own index per face corner.
    std::vector<D3DXVECTOR3> normals;
    std::vector<std::vector<DWORD>> normalFaces;
    if (const Object* object = xfile::FirstChildOfType(mesh, "MeshNormals")) {
        Cursor n(*object);
        const std::uint32_t count = n.Dword();
        if (n.Failed() || count > n.Remaining() / 3) {
            error = "Mesh " + mesh.name + ": bad MeshNormals";
            return false;
        }
        normals.resize(count);
        for (D3DXVECTOR3& normal : normals) {
            normal = ReadVector(n);
        }
        bool matches = ReadFaces(n, count, normalFaces) && normalFaces.size() == faces.size();
        for (std::size_t f = 0; matches && f < faces.size(); ++f) {
            matches = normalFaces[f].size() == faces[f].size();
        }
        if (!matches) {
            error = "Mesh " + mesh.name + ": MeshNormals faces do not match the mesh's";
            return false;
        }
    }
    std::vector<float> texcoords;
    if (const Object* object = xfile::FirstChildOfType(mesh, "MeshTextureCoords")) {
        Cursor t(*object);
        if (t.Dword() != positionCount || t.Remaining() < positionCount * 2u) {
            error = "Mesh " + mesh.name + ": MeshTextureCoords count differs from the vertex count";
            return false;
        }
        texcoords.resize(positionCount * 2u);
        for (float& value : texcoords) {
            value = t.Float();
        }
    }
    std::vector<DWORD> colors;
    if (const Object* object = xfile::FirstChildOfType(mesh, "MeshVertexColors")) {
        Cursor c(*object);
        const std::uint32_t count = c.Dword();
        colors.assign(positionCount, 0xFFFFFFFF);
        for (std::uint32_t i = 0; i < count && !c.Failed(); ++i) {
            const std::uint32_t index = c.Dword();
            const float r = c.Float(), g = c.Float(), b = c.Float(), a = c.Float();
            if (index < positionCount) {
                colors[index] = ToColor(r, g, b, a);
            }
        }
    }

    // Materials, inline or referenced, and one per polygon (the last one repeats when the list is short).
    std::vector<DWORD> polygonMaterial(faces.size(), 0);
    if (const Object* list = xfile::FirstChildOfType(mesh, "MeshMaterialList")) {
        Cursor m(*list);
        const std::uint32_t count = m.Dword();
        const std::uint32_t indexCount = m.Dword();
        if (m.Failed() || indexCount > m.Remaining()) {
            error = "Mesh " + mesh.name + ": bad MeshMaterialList";
            return false;
        }
        DWORD last = 0;
        for (std::size_t f = 0; f < faces.size(); ++f) {
            if (f < indexCount) {
                last = m.Dword();
            }
            polygonMaterial[f] = last;
        }
        for (const Object& child : list->children) {
            if (child.type == "Material") {
                out.materials.push_back(ReadMaterial(child));
            } else if (child.IsReference()) {
                const Object* material = FindNamed(file.objects, "Material", child.name);
                if (!material) {
                    error = "Mesh " + mesh.name + ": no Material named " + child.name;
                    return false;
                }
                out.materials.push_back(ReadMaterial(*material));
            }
        }
        while (out.materials.size() < count) {
            out.materials.push_back(DefaultMaterial());
        }
    }
    if (out.materials.empty()) {
        out.materials.push_back(DefaultMaterial());
        std::fill(polygonMaterial.begin(), polygonMaterial.end(), 0);
    }

    // A vertex per (position, normal) pair: a position's first pair keeps its index, the others are appended.
    out.source.resize(positionCount);
    for (DWORD v = 0; v < positionCount; ++v) {
        out.source[v] = v;
    }
    std::vector<DWORD> normalOf(positionCount, 0xFFFFFFFF);
    std::vector<DWORD> vertexNormal(positionCount, 0xFFFFFFFF);
    std::map<std::pair<DWORD, DWORD>, DWORD> splits;
    auto vertexFor = [&](DWORD p, DWORD n) -> DWORD {
        if (normals.empty()) {
            return p;
        }
        if (normalOf[p] == 0xFFFFFFFF) {
            normalOf[p] = n;
            vertexNormal[p] = n;
            return p;
        }
        if (normalOf[p] == n) {
            return p;
        }
        const auto [it, added] = splits.try_emplace({p, n}, static_cast<DWORD>(out.source.size()));
        if (added) {
            out.source.push_back(p);
            vertexNormal.push_back(n);
        }
        return it->second;
    };
    for (std::size_t f = 0; f < faces.size(); ++f) {
        std::vector<DWORD> corners(faces[f].size());
        for (std::size_t c = 0; c < corners.size(); ++c) {
            corners[c] = vertexFor(faces[f][c], normals.empty() ? 0 : normalFaces[f][c]);
        }
        for (std::size_t c = 2; c < corners.size(); ++c) {
            out.indices.insert(out.indices.end(), {corners[0], corners[c - 1], corners[c]});
            out.attributes.push_back(polygonMaterial[f]);
        }
    }

    out.fvf = D3DFVF_XYZ | (normals.empty() ? 0 : D3DFVF_NORMAL) | (colors.empty() ? 0 : D3DFVF_DIFFUSE) |
              (texcoords.empty() ? 0 : D3DFVF_TEX1);
    const VertexElements e = DescribeVertex(out.fvf);
    out.vertices.assign(out.source.size() * e.size, std::byte{0});
    for (std::size_t v = 0; v < out.source.size(); ++v) {
        std::byte* vertex = out.vertices.data() + v * e.size;
        const DWORD p = out.source[v];
        std::memcpy(vertex + e.position, &positions[p], 12);
        if (e.normal >= 0 && vertexNormal[v] != 0xFFFFFFFF) {
            std::memcpy(vertex + e.normal, &normals[vertexNormal[v]], 12);
        }
        if (e.diffuse >= 0) {
            std::memcpy(vertex + e.diffuse, &colors[p], 4);
        }
        if (e.tex[0] >= 0) {
            std::memcpy(vertex + e.tex[0], &texcoords[p * 2u], 8);
        }
    }
    return true;
}

HRESULT CreateMaterialBuffer(const std::vector<XMaterial>& materials, LPD3DXBUFFER* buffer)
{
    std::size_t size = materials.size() * sizeof(D3DXMATERIAL);
    for (const XMaterial& m : materials) {
        size += m.texture.empty() ? 0 : m.texture.size() + 1;
    }
    const HRESULT hr = D3DXCreateBuffer(static_cast<DWORD>(size), buffer);
    if (FAILED(hr)) {
        return hr;
    }
    auto* bytes = static_cast<char*>((*buffer)->GetBufferPointer());
    auto* entries = reinterpret_cast<D3DXMATERIAL*>(bytes);
    char* names = bytes + materials.size() * sizeof(D3DXMATERIAL);
    for (std::size_t i = 0; i < materials.size(); ++i) {
        entries[i].MatD3D = materials[i].material;
        entries[i].pTextureFilename = nullptr;
        if (!materials[i].texture.empty()) {
            std::memcpy(names, materials[i].texture.c_str(), materials[i].texture.size() + 1);
            entries[i].pTextureFilename = names;
            names += materials[i].texture.size() + 1;
        }
    }
    return D3D_OK;
}

}  // namespace x2::d3d9

namespace {

using x2::d3d9::XMesh;

struct Placed {
    XMesh mesh;
    D3DXMATRIX transform;
};

bool Collect(const x2::d3d9::xfile::File& file, const x2::d3d9::xfile::Object& object, const D3DXMATRIX& parent,
             std::vector<Placed>& out, std::string& error)
{
    if (object.type == "Mesh") {
        Placed placed{{}, parent};
        if (!x2::d3d9::ReadXMesh(file, object, placed.mesh, error)) {
            return false;
        }
        if (placed.mesh.FaceCount() > 0) {
            out.push_back(std::move(placed));
        }
        return true;
    }
    if (object.type != "Frame") {
        return true;
    }
    D3DXMATRIX transform = parent;
    if (const auto* matrix = x2::d3d9::xfile::FirstChildOfType(object, "FrameTransformMatrix")) {
        x2::d3d9::xfile::Cursor in(*matrix);
        D3DXMATRIX local;
        for (int i = 0; i < 16; ++i) {
            local.m[i / 4][i % 4] = in.Float();
        }
        D3DXMatrixMultiply(&transform, &local, &parent);
    }
    for (const auto& child : object.children) {
        if (!Collect(file, child, transform, out, error)) {
            return false;
        }
    }
    return true;
}

}  // namespace

// Every mesh in the file, in its frame's place, as one mesh; materials in file order.
HRESULT D3DXLoadMeshFromXInMemory(LPCVOID Memory, DWORD SizeOfMemory, DWORD Options, x2::d3d9::Device* pD3DDevice,
                                  LPD3DXBUFFER* ppAdjacency, LPD3DXBUFFER* ppMaterials,
                                  LPD3DXBUFFER* ppEffectInstances, DWORD* pNumMaterials, LPD3DXMESH* ppMesh)
{
    for (LPD3DXBUFFER* buffer : {ppAdjacency, ppMaterials, ppEffectInstances}) {
        if (buffer) {
            *buffer = nullptr;
        }
    }
    if (pNumMaterials) {
        *pNumMaterials = 0;
    }
    if (!Memory || !pD3DDevice || !ppMesh) {
        return D3DERR_INVALIDCALL;
    }
    *ppMesh = nullptr;
    std::string error;
    const auto file = x2::d3d9::xfile::Read(
        std::span(static_cast<const std::byte*>(Memory), SizeOfMemory), error);
    if (!file) {
        return E_FAIL;
    }
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    std::vector<Placed> meshes;
    for (const auto& object : file->objects) {
        if (!Collect(*file, object, identity, meshes, error)) {
            return E_FAIL;
        }
    }
    if (meshes.empty()) {
        return E_FAIL;
    }

    DWORD fvf = 0, vertexCount = 0, faceCount = 0;
    for (const Placed& placed : meshes) {
        fvf |= placed.mesh.fvf;
        vertexCount += placed.mesh.VertexCount();
        faceCount += placed.mesh.FaceCount();
    }
    const x2::d3d9::VertexElements to = x2::d3d9::DescribeVertex(fvf);
    std::vector<std::byte> vertices(std::size_t{vertexCount} * to.size);
    std::vector<std::uint32_t> indices;
    std::vector<DWORD> attributes;
    std::vector<x2::d3d9::XMaterial> materials;
    indices.reserve(std::size_t{faceCount} * 3);
    attributes.reserve(faceCount);
    DWORD firstVertex = 0;
    for (const Placed& placed : meshes) {
        const x2::d3d9::VertexElements from = x2::d3d9::DescribeVertex(placed.mesh.fvf);
        D3DXMATRIX normalTransform;
        D3DXMatrixInverse(&normalTransform, nullptr, &placed.transform);
        D3DXMatrixTranspose(&normalTransform, &normalTransform);
        for (DWORD v = 0; v < placed.mesh.VertexCount(); ++v) {
            std::byte* vertex = vertices.data() + std::size_t{firstVertex + v} * to.size;
            x2::d3d9::ConvertVertex(placed.mesh.vertices.data() + std::size_t{v} * from.size, from, vertex, to);
            D3DXVECTOR3 p;
            std::memcpy(&p, vertex + to.position, 12);
            D3DXVec3TransformCoord(&p, &p, &placed.transform);
            std::memcpy(vertex + to.position, &p, 12);
            if (from.normal >= 0) {
                D3DXVECTOR3 n;
                std::memcpy(&n, vertex + to.normal, 12);
                D3DXVec3TransformNormal(&n, &n, &normalTransform);
                D3DXVec3Normalize(&n, &n);
                std::memcpy(vertex + to.normal, &n, 12);
            }
        }
        for (std::uint32_t index : placed.mesh.indices) {
            indices.push_back(firstVertex + index);
        }
        const DWORD firstMaterial = static_cast<DWORD>(materials.size());
        for (DWORD attribute : placed.mesh.attributes) {
            attributes.push_back(firstMaterial + attribute);
        }
        materials.insert(materials.end(), placed.mesh.materials.begin(), placed.mesh.materials.end());
        firstVertex += placed.mesh.VertexCount();
    }

    if (vertexCount > 0xFFFF) {
        Options |= D3DXMESH_32BIT;
    }
    x2::d3d9::Mesh* mesh = x2::d3d9::Mesh::Create(pD3DDevice, faceCount, vertexCount, Options, fvf);
    if (!mesh) {
        return D3DERR_INVALIDCALL;
    }
    if (!mesh->SetContents(vertices, indices, attributes)) {
        mesh->Release();
        return D3DERR_INVALIDCALL;
    }
    if (ppAdjacency && SUCCEEDED(D3DXCreateBuffer(faceCount * 3 * sizeof(DWORD), ppAdjacency))) {
        mesh->GenerateAdjacency(0.0f, static_cast<DWORD*>((*ppAdjacency)->GetBufferPointer()));
    }
    if (ppMaterials) {
        x2::d3d9::CreateMaterialBuffer(materials, ppMaterials);
    }
    if (ppEffectInstances &&
        SUCCEEDED(D3DXCreateBuffer(static_cast<DWORD>(materials.size() * sizeof(D3DXEFFECTINSTANCE)),
                                   ppEffectInstances))) {
        // No effect files: .x files do not name any here.
        std::memset((*ppEffectInstances)->GetBufferPointer(), 0, materials.size() * sizeof(D3DXEFFECTINSTANCE));
    }
    if (pNumMaterials) {
        *pNumMaterials = static_cast<DWORD>(materials.size());
    }
    *ppMesh = mesh;
    return D3D_OK;
}
