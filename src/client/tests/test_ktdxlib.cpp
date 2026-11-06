// KTDXLIB's first engine files on the D3D9 device (docs/porting/client-ktdxlib.md): the state manager, the dynamic
// vertex data, render target textures and the silent sound backend, headless on the null backend. g_pKTDXApp is the
// real CKTDXApp; the globals X2.cpp defines are in x2_globals.cpp.

#include "stdafx.h"

#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <memory>
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

// The device, the application on it (set up as X2.cpp's device callbacks do), and a 64 x 32 target bound so draws have somewhere to go.
struct Engine {
    std::unique_ptr<x2::render::Device> render = NullRender();
    x2::d3d9::Device device{*render, nullptr};
    std::unique_ptr<CKTDXApp> app;
    std::unique_ptr<x2::d3d9::Texture> target;

    Engine()
    {
        REQUIRE_MESSAGE(device.Ok(), device.LastError());
        g_pd3dDevice = &device;
        app = std::make_unique<CKTDXApp>(nullptr, nullptr, &device);
        CKTDGStateManager::RestoreDeviceObjects();
        target = device.CreateTexture(64, 32, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
        REQUIRE(target);
        CKTDGStateManager::SetRenderTarget(target.get());
    }
    ~Engine()
    {
        CKTDGStateManager::SetRenderTarget(NULL);
        target.reset();
        CKTDGStateManager::InvalidateDeviceObjects();
        app.reset();
        g_pd3dDevice = NULL;
    }
};

struct XyzDiffuse {
    float x, y, z;
    D3DCOLOR color;
};

// Render target textures and sounds have protected destructors (the device manager releases them).
struct RenderTarget : CKTDXDeviceRenderTargetTexture {
    using CKTDXDeviceRenderTargetTexture::CKTDXDeviceRenderTargetTexture;
    ~RenderTarget() override = default;
};

struct Sound : CKTDXDeviceSound {
    using CKTDXDeviceSound::CKTDXDeviceSound;
    ~Sound() override = default;
};

}  // namespace

TEST_CASE("ktdxlib: the state manager caches and stacks the device's states")
{
    Engine engine;
    CHECK(CKTDGStateManager::GetRenderState(D3DRS_ZENABLE) == engine.device.GetRenderState(D3DRS_ZENABLE));

    CKTDGStateManager::SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    CHECK(engine.device.GetRenderState(D3DRS_ALPHABLENDENABLE) == TRUE);
    CKTDGStateManager::PushRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    CHECK(engine.device.GetRenderState(D3DRS_ALPHABLENDENABLE) == FALSE);
    CKTDGStateManager::PopRenderState(D3DRS_ALPHABLENDENABLE);
    CHECK(engine.device.GetRenderState(D3DRS_ALPHABLENDENABLE) == TRUE);
    CHECK(CKTDGStateManager::GetStackTop() == 0);

    // A render target's viewport is its size.
    const D3DVIEWPORT* viewport = CKTDGStateManager::GetViewport();
    CHECK(viewport->Width == 64);
    CHECK(viewport->Height == 32);
    D3DVIEWPORT9 deviceViewport;
    engine.device.GetViewport(&deviceViewport);
    CHECK(deviceViewport.Width == 64);
}

TEST_CASE("ktdxlib: dynamic vertex data longer than one buffer is drawn in parts")
{
    Engine engine;
    CKTDGDynamicVBManager* vb = g_pKTDXApp->GetDVBManager();
    REQUIRE(vb != NULL);
    CHECK(vb->GetDynamicVBMode() == CKTDGDynamicVBManager::DYNAMIC_VB_MODE_PRIMITIVEUP);

    std::vector<XyzDiffuse> vertices(1200, XyzDiffuse{0, 0, 0.5f, 0xFFFFFFFF});
    x2::render::null::ClearCommands(*engine.render);
    REQUIRE(engine.device.BeginScene());

    // DVB_TYPE_XYZ_DIFFUSE takes 1024 vertices per draw: a list of 400 triangles is 341 + 59 of them.
    vb->DrawPrimitive(CKTDGDynamicVBManager::DVB_TYPE_XYZ_DIFFUSE, D3DPT_TRIANGLELIST, 400, vertices.data());
    // A strip of 1100 triangles is 1022 (even, keeping the winding) + 78, the second part sharing two vertices.
    vb->DrawPrimitive(CKTDGDynamicVBManager::DVB_TYPE_XYZ_DIFFUSE, D3DPT_TRIANGLESTRIP, 1100, vertices.data());
    // Multipass draws the same vertices again.
    vb->DrawPrimitive(CKTDGDynamicVBManager::DVB_TYPE_XYZ_DIFFUSE, D3DPT_TRIANGLELIST, 2, vertices.data(), 3);
    // The first vertices of a split draw come from one array and the rest from another.
    vb->DrawPrimitive_SplitData(CKTDGDynamicVBManager::DVB_TYPE_XYZ_DIFFUSE, D3DPT_TRIANGLESTRIP, 2, 1,
                                vertices.data(), vertices.data() + 1);
    CHECK(engine.device.EndScene());
    CHECK(engine.device.Present());

    const auto draws = Lines(*engine.render, "DrawPrimitives vertices=");
    REQUIRE(draws.size() == 8);
    CHECK(draws[0].rfind("DrawPrimitives vertices=1023 ", 0) == 0);
    CHECK(draws[1].rfind("DrawPrimitives vertices=177 ", 0) == 0);
    CHECK(draws[2].rfind("DrawPrimitives vertices=1024 ", 0) == 0);
    CHECK(draws[3].rfind("DrawPrimitives vertices=80 ", 0) == 0);
    for (int i = 4; i != 7; ++i) {
        CHECK(draws[i].rfind("DrawPrimitives vertices=6 ", 0) == 0);
    }
    CHECK(draws[7].rfind("DrawPrimitives vertices=4 ", 0) == 0);
}

TEST_CASE("ktdxlib: render target textures render into themselves and back")
{
    Engine engine;
    CKTDGStateManager::SetRenderTarget(NULL);

    RenderTarget* rt = new RenderTarget(&engine.device, L"RT_TEST", 128, 64);
    REQUIRE(rt->IsOk());
    CHECK(rt->GetWidth() == 128);
    D3DSURFACE_DESC desc;
    REQUIRE(rt->GetTexture()->GetLevelDesc(0, &desc));
    CHECK(desc.Format == D3DFMT_A8R8G8B8);
    CHECK(desc.Width == 128);

    REQUIRE(engine.device.BeginScene());
    rt->BeginRender();
    CHECK(engine.device.GetRenderTarget() == rt->GetTexture());
    CHECK(CKTDGStateManager::GetViewport()->Width == 128);
    rt->EndRender();
    CHECK(engine.device.GetRenderTarget() == NULL);
    CHECK(engine.device.EndScene());

    // Releasing a target that is bound puts the previous one back.
    rt->BeginRender(false);
    delete rt;
    CHECK(engine.device.GetRenderTarget() == NULL);
}

TEST_CASE("ktdxlib: the silent sound backend keeps its state and plays nothing")
{
    Engine engine;
    CKTDSManager* sound = g_pKTDXApp->GetDSManager();
    REQUIRE(sound != NULL);
    CHECK(sound->GetInit());
    CHECK(sound->GetSystem() == NULL);
    CHECK_FALSE(sound->GetCapable3DSound());
    CHECK(sound->OnFrameMove(0.0, 0.016f) == S_OK);
    sound->SetListenerData(D3DXVECTOR3(1, 2, 3));

    CKTDSMP3Play* music = sound->GetMP3Play();
    REQUIRE(music != NULL);
    music->Play(L"MusicWin.ogg", false);
    CHECK(music->GetPlayingFileName() == L"MusicWin.ogg");
    music->SetPaused(true);
    music->SetVolume(-1500);
    music->Stop();

    Sound* effect = new Sound(sound->GetSystem(), L"BUTTON.OGG");
    effect->Play(true);
    CHECK_FALSE(effect->IsPlaying());
    effect->SetMax3DDistance(1000.f);
    effect->Set3DPosition(D3DXVECTOR3(0, 0, 0));
    effect->Stop();
    delete effect;
}
