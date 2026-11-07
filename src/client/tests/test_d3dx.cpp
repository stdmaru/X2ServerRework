// KTDXLIB's D3D9 device state beyond drawing (lighting, UpdateTexture) and the D3DX pieces the engine builds on
// (docs/porting/client-d3d9.md): buffers, frame hierarchies, CComPtr, and the loaders that fail until their step.

#include "D3DXTypes.h"
#include "Device.h"

#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <cstring>
#include <memory>
#include <string>

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

// Frames named by the test, freed through the allocator as the loader's hierarchies are.
struct TestAllocator : ID3DXAllocateHierarchy {
    int frames = 0;
    int containers = 0;
    STDMETHOD(CreateFrame)(THIS_ LPCSTR, LPD3DXFRAME*) override { return E_NOTIMPL; }
    STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR, const D3DXMESHDATA*, const D3DXMATERIAL*, const D3DXEFFECTINSTANCE*,
                                   DWORD, const DWORD*, LPD3DXSKININFO, LPD3DXMESHCONTAINER*) override
    {
        return E_NOTIMPL;
    }
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME frame) override
    {
        ++frames;
        delete frame;
        return D3D_OK;
    }
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER container) override
    {
        ++containers;
        delete container;
        return D3D_OK;
    }
};

LPD3DXFRAME Frame(LPSTR name)
{
    auto* frame = new D3DXFRAME{};
    frame->Name = name;
    return frame;
}

}  // namespace

TEST_CASE("d3d9: fixed-function lighting state is kept")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());

    D3DMATERIAL9 material;
    std::memset(&material, 0x7F, sizeof(material));
    device.GetMaterial(&material);
    CHECK(material.Diffuse.r == 0.f);   // starts zeroed
    material.Diffuse.g = 0.5f;
    material.Power = 8.f;
    device.SetMaterial(&material);
    D3DMATERIAL9 read{};
    device.GetMaterial(&read);
    CHECK(read.Diffuse.g == 0.5f);
    CHECK(read.Power == 8.f);

    BOOL enabled = TRUE;
    D3DLIGHT9 light{};
    CHECK_FALSE(device.GetLight(3, &light));          // never set
    CHECK_FALSE(device.GetLightEnable(3, &enabled));
    REQUIRE(device.LightEnable(3, TRUE));             // enabling an unset light gives D3D9's default
    REQUIRE(device.GetLight(3, &light));
    CHECK(light.Type == D3DLIGHT_DIRECTIONAL);
    CHECK(light.Diffuse.r == 1.f);
    CHECK(light.Direction.z == 1.f);
    REQUIRE(device.GetLightEnable(3, &enabled));
    CHECK(enabled);

    light.Type = D3DLIGHT_POINT;
    light.Range = 100.f;
    REQUIRE(device.SetLight(0, &light));
    REQUIRE(device.GetLight(0, &light));
    CHECK(light.Type == D3DLIGHT_POINT);
    CHECK(light.Range == 100.f);
    REQUIRE(device.GetLightEnable(0, &enabled));
    CHECK_FALSE(enabled);   // set, not enabled
    CHECK_FALSE(device.SetLight(1 << 20, &light));   // beyond the device's lights

    IDirect3DVertexShader9* shader = reinterpret_cast<IDirect3DVertexShader9*>(&device);
    device.GetVertexShader(&shader);
    CHECK(shader == nullptr);   // no shader objects (T37)
}

TEST_CASE("d3d9: UpdateTexture copies the levels both textures have")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());

    auto source = device.CreateTexture(4, 4, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM);
    auto destination = device.CreateTexture(4, 4, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    REQUIRE(source);
    REQUIRE(destination);
    D3DLOCKED_RECT locked;
    REQUIRE(source->LockRect(0, &locked, nullptr, 0));
    for (DWORD i = 0; i < 16; ++i) {
        static_cast<DWORD*>(locked.pBits)[i] = D3DCOLOR_ARGB(0xFF, i, 0, 0);
    }
    REQUIRE(source->UnlockRect(0));

    REQUIRE(device.UpdateTexture(source.get(), destination.get()));
    device.Present();
    const auto bytes = x2::render::null::TextureBytes(*render, destination->RenderTexture());
    REQUIRE(bytes.size() == 64);
    CHECK(static_cast<std::uint8_t>(bytes[7 * 4 + 2]) == 7);   // R of texel 7

    auto other = device.CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    REQUIRE(other);
    CHECK_FALSE(device.UpdateTexture(source.get(), other.get()));   // sizes differ
    CHECK_FALSE(device.UpdateTexture(nullptr, destination.get()));
}

TEST_CASE("d3dx: buffers are reference counted, and CComPtr holds one reference")
{
    LPD3DXBUFFER buffer = nullptr;
    REQUIRE(D3DXCreateBuffer(16, &buffer) == D3D_OK);
    REQUIRE(buffer != nullptr);
    CHECK(buffer->GetBufferSize() == 16);
    REQUIRE(buffer->GetBufferPointer() != nullptr);
    std::memset(buffer->GetBufferPointer(), 0xAB, 16);

    {
        CComPtr<ID3DXBuffer> held(buffer);         // 2 references
        CComPtr<ID3DXBuffer> copy = held;          // 3
        CHECK(copy.p == buffer);
        CHECK(buffer->AddRef() == 4);
        CHECK(buffer->Release() == 3);
    }                                              // back to 1
    CHECK(buffer->Release() == 0);

    CComPtr<ID3DXBuffer> out;
    REQUIRE(D3DXCreateBuffer(4, &out) == D3D_OK);  // &out for out-parameters
    CHECK(!out == false);
    out.Release();
    CHECK(!out);
}

TEST_CASE("d3dx: frame hierarchies are searched, counted and destroyed")
{
    char root[] = "root", arm[] = "arm", hand[] = "hand", leg[] = "leg";
    LPD3DXFRAME pRoot = Frame(root);
    pRoot->pFrameFirstChild = Frame(arm);
    pRoot->pFrameFirstChild->pFrameFirstChild = Frame(hand);
    pRoot->pFrameFirstChild->pFrameSibling = Frame(leg);
    pRoot->pFrameFirstChild->pFrameSibling->pFrameSibling = Frame(nullptr);   // unnamed
    pRoot->pFrameFirstChild->pMeshContainer = new D3DXMESHCONTAINER{};

    CHECK(D3DXFrameFind(pRoot, "hand") == pRoot->pFrameFirstChild->pFrameFirstChild);
    CHECK(D3DXFrameFind(pRoot, "leg") == pRoot->pFrameFirstChild->pFrameSibling);
    CHECK(D3DXFrameFind(pRoot, "tail") == nullptr);
    CHECK(D3DXFrameNumNamedMatrices(pRoot) == 4);

    TestAllocator allocator;
    REQUIRE(D3DXFrameDestroy(pRoot, &allocator) == D3D_OK);
    CHECK(allocator.frames == 5);
    CHECK(allocator.containers == 1);
    CHECK(D3DXFrameDestroy(nullptr, &allocator) == D3DERR_INVALIDCALL);
}

TEST_CASE("d3dx: textures are created on the device; effects fail until their step")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());

    x2::d3d9::Texture* texture = nullptr;
    REQUIRE(D3DXCreateTexture(&device, 8, 4, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture) == D3D_OK);
    REQUIRE(texture != nullptr);
    CHECK(texture->GetLevelCount() == 4);
    delete texture;

    LPD3DXEFFECT effect = reinterpret_cast<LPD3DXEFFECT>(&device);
    CHECK(FAILED(D3DXCreateEffect(&device, "x", 1, nullptr, nullptr, 0, nullptr, &effect, nullptr)));
    CHECK(effect == nullptr);
}
