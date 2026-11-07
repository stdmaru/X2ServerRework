#pragma once
// Meshes read from .x files: one Mesh data object as D3DX lays it out (vertices split by normal, polygons as triangle
// fans), and D3DXLoadMeshFromXInMemory on top. docs/porting/client-d3d9.md#meshes-2026-11-07-clientx-loader

#include "D3DXTypes.h"
#include "XFile.h"

#include <string>
#include <vector>

namespace x2::d3d9 {

struct XMaterial {
    D3DMATERIAL9 material{};
    std::string texture;            // empty for none
};

struct XMesh {
    DWORD fvf = 0;                  // XYZ, plus NORMAL / DIFFUSE / TEX1 when the file has them
    std::vector<std::byte> vertices;
    std::vector<DWORD> source;      // per vertex: the file's vertex it came from (for skin weights)
    std::vector<std::uint32_t> indices;
    std::vector<DWORD> attributes;
    std::vector<XMaterial> materials;   // at least one (D3DX's default when the file has none)

    DWORD VertexCount() const { return static_cast<DWORD>(source.size()); }
    DWORD FaceCount() const { return static_cast<DWORD>(attributes.size()); }
};

// One Mesh object. `file` resolves references to Material objects. False (and `error`) when the object is malformed.
bool ReadXMesh(const xfile::File& file, const xfile::Object& mesh, XMesh& out, std::string& error);

// A buffer of D3DXMATERIAL entries followed by their texture names.
HRESULT CreateMaterialBuffer(const std::vector<XMaterial>& materials, LPD3DXBUFFER* buffer);

}  // namespace x2::d3d9
