// KTDXLIB's first engine files on the D3D9 device (docs/porting/client-ktdxlib.md): the state manager, the dynamic
// vertex data, render target textures and the silent sound backend, headless on the null backend. g_pKTDXApp is the
// real CKTDXApp; the globals X2.cpp defines are in x2_globals.cpp.

#include "KTDX.h"   // not stdafx.h: Windows would find X2ServerProtocol/StdAfx.h first

#include "XFile.h"
#include "KTDXTickInterpolation.h"

#include "x2/kom/mount.h"
#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <set>
#include <thread>
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

namespace {

// A stage that draws one triangle: what a loading screen's OnFrameRender does between loading steps.
struct DrawingStage : CKTDXStage {
    int renders = 0;
    HRESULT OnFrameRender() override
    {
        ++renders;
        const XyzDiffuse triangle[3] = {{0, 0, 0, 0xFFFFFFFF}, {1, 0, 0, 0xFFFFFFFF}, {0, 1, 0, 0xFFFFFFFF}};
        g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, triangle, sizeof(XyzDiffuse));
        return S_OK;
    }
};

}  // namespace

TEST_CASE("ktdxlib: RenderStageNow draws and presents one frame of a stage (T38)")
{
    Engine engine;
    x2::render::null::ClearCommands(*engine.render);
    DrawingStage stage;
    g_pKTDXApp->RenderStageNow(&stage);
    CHECK(stage.renders == 1);
    CHECK(!Lines(*engine.render, "Draw").empty());
    CHECK(!Lines(*engine.render, "Submit").empty());

    x2::render::null::ClearCommands(*engine.render);
    g_pKTDXApp->RenderStageNow(NULL);   // no stage: still presents a frame
    CHECK(!Lines(*engine.render, "Submit").empty());
}

extern CKTDGEffectStateManager* g_pEffectStateManager;     // x2_globals.cpp, as X2.cpp defines it

namespace {

// g_pEffectStateManager as X2.cpp sets it up, for the scope.
struct EffectStateManager {
    EffectStateManager() { g_pEffectStateManager = new CKTDGEffectStateManager; }
    ~EffectStateManager()
    {
        g_pEffectStateManager->Release();
        g_pEffectStateManager = NULL;
    }
};

const x2::d3d9::EffectProgram* BoundProgram(x2::d3d9::Device& device)
{
    x2::d3d9::VertexShader* shader = nullptr;
    device.GetVertexShader(&shader);
    return shader ? &shader->Program() : nullptr;
}

}  // namespace

// CKTDGXRenderer on its effect (T37) through CKTDGEffectStateManager: passes set their states and shader, the
// constants reach the device when the manager flushes them, and the end of the render restores the device.
TEST_CASE("ktdxlib: CKTDGXRenderer runs XRenderer's techniques through the state manager")
{
    Engine engine;
    EffectStateManager manager;
    CKTDGXRenderer renderer(&engine.device);
    REQUIRE(SUCCEEDED(renderer.OnResetDevice()));   // no archives: the cartoon textures are missing

    const DWORD cull = engine.device.GetRenderState(D3DRS_CULLMODE);
    const DWORD factor = engine.device.GetRenderState(D3DRS_TEXTUREFACTOR);
    CKTDGXRenderer::RenderParam param;
    param.renderType = CKTDGXRenderer::RT_BLACK_EDGE;
    param.fOutLineWide = 2.0f;
    D3DXMATRIX world;
    D3DXMatrixTranslation(&world, 5, 6, 7);
    int passes = 0;
    for (HRESULT hr = renderer.BeginRender(param, &world); SUCCEEDED(hr); hr = renderer.NextRender()) {
        ++passes;
        CHECK(engine.device.GetRenderState(D3DRS_CULLMODE) == D3DCULL_CW);
        CHECK(engine.device.GetRenderState(D3DRS_TEXTUREFACTOR) == 0xFF000000);
        REQUIRE(BoundProgram(engine.device) != nullptr);
        CHECK(BoundProgram(engine.device)->kind == x2::d3d9::EffectProgram::BlackEdge);
        CHECK(BoundProgram(engine.device)->bones == 0);
        // The manager caches the constants until CKTDGXRenderer's commit flushes them.
        g_pEffectStateManager->PostCommitChanges();
        float r[4 * 4];
        REQUIRE(engine.device.GetVertexShaderConstantF(0, r, 4));
        CHECK(std::memcmp(r, &world, sizeof(world)) == 0);     // the world matrix is palette slot 0
        REQUIRE(engine.device.GetVertexShaderConstantF(95, r, 1));
        CHECK(r[2] == 2.0f);
    }
    CHECK(passes == 1);
    CHECK(BoundProgram(engine.device) == nullptr);
    CHECK(engine.device.GetRenderState(D3DRS_CULLMODE) == cull);
    CHECK(engine.device.GetRenderState(D3DRS_TEXTUREFACTOR) == factor);

    // Light flow: blending on for the pass, off after it.
    param.renderType = CKTDGXRenderer::RT_LIGHT_FLOW_WORLD_X;
    passes = 0;
    for (HRESULT hr = renderer.BeginRender(param, &world); SUCCEEDED(hr); hr = renderer.NextRender()) {
        ++passes;
        CHECK(engine.device.GetRenderState(D3DRS_ALPHABLENDENABLE) == TRUE);
        CHECK(engine.device.GetRenderState(D3DRS_DESTBLEND) == D3DBLEND_DESTALPHA);
        CHECK(BoundProgram(engine.device)->kind == x2::d3d9::EffectProgram::FlowWorld);
    }
    CHECK(passes == 1);
    CHECK(engine.device.GetRenderState(D3DRS_ALPHABLENDENABLE) == FALSE);

    // A render group of a two-pass technique.
    int groupPasses = 0;
    for (HRESULT hr = renderer.BeginRenderGroup(CKTDGXRenderer::RT_CARTOON_COLOR_EDGE); SUCCEEDED(hr);
         hr = renderer.NextRenderGroup()) {
        ++groupPasses;
    }
    CHECK(groupPasses == 2);
    CHECK(BoundProgram(engine.device) == nullptr);
    renderer.OnLostDevice();
}

TEST_CASE("ktdxlib: KFileSerializer and CRC_32 read and write whole files")
{
    const std::filesystem::path dir = std::filesystem::temp_directory_path() / "x2_ktdxlib_tests";
    std::filesystem::create_directories(dir);
    const std::string file = (dir / "serializer.bin").string();

    KFileSerializer out;
    CHECK(FAILED(out.SaveFile(file.c_str())));   // nothing to write
    const char data[] = "elsword";
    out.AppendData(sizeof(data), data);
    REQUIRE(SUCCEEDED(out.SaveFile(file.c_str())));
    CHECK(std::filesystem::file_size(file) == sizeof(data));

    KFileSerializer in;
    REQUIRE(SUCCEEDED(in.LoadFile(file.c_str())));
    std::vector<BYTE> bytes;
    in.Swap(bytes);
    REQUIRE(bytes.size() == sizeof(data));
    CHECK(std::memcmp(bytes.data(), data, sizeof(data)) == 0);

    KFileSerializer capped;
    REQUIRE(SUCCEEDED(capped.LoadFile(std::filesystem::path(file).wstring().c_str(), 3)));   // at most 3 bytes
    CHECK(capped.GetSize() == 3);
    CHECK(FAILED(capped.LoadFile((dir / "missing.bin").string().c_str())));

    CHECK(CRC_32::GetCRCFromFile(file.c_str()) == CRC_32::GetCRCFromBuffer(data, sizeof(data)));
    CHECK(CRC_32::GetCRCFromFile((dir / "missing.bin").string().c_str()).empty());
    std::filesystem::remove_all(dir);
}

namespace {

struct SkinMesh : CKTDXDeviceXSkinMesh {
    using CKTDXDeviceXSkinMesh::CKTDXDeviceXSkinMesh;
    using CKTDXDeviceXSkinMesh::_Load;
    ~SkinMesh() override = default;
};

bool HasObject(const std::vector<x2::d3d9::xfile::Object>& objects, const char* type)
{
    for (const auto& object : objects) {
        if (object.type == type || HasObject(object.children, type)) {
            return true;
        }
    }
    return false;
}

bool FiniteFrames(const D3DXFRAME* frame)
{
    for (; frame != nullptr; frame = frame->pFrameSibling) {
        for (int i = 0; i < 16; ++i) {
            if (!std::isfinite(frame->TransformationMatrix.m[i / 4][i % 4])) {
                return false;
            }
        }
        if (!FiniteFrames(frame->pFrameFirstChild)) {
            return false;
        }
    }
    return true;
}

}  // namespace

// Every skinned mesh of the client (.x with SkinWeights, KOG's KSM) loaded by CKTDXDeviceXSkinMesh as the game does:
// hierarchy (KSM: frames, then its KAC animation controller), optimize and remap, indexed blended mesh, then its
// textures. Every one must load, and every animation set of a clone of its controller plays (finite frame matrices).
TEST_CASE("ktdxlib: preprocessed .tet keys are UTF-16, as the Windows tools wrote them")
{
    // DLG_Login0.tet's layout: header, one key offset, one UV rectangle, then the key "BG01" (WORD length, UTF-16, null)
    std::vector<BYTE> file;
    auto put = [&](const void* data, std::size_t size) {
        const auto* bytes = static_cast<const BYTE*>(data);
        file.insert(file.end(), bytes, bytes + size);
    };
    KTETFormatHeader header;
    header.m_dwMagic = KTET_FORMAT_MAGIC;
    header.m_dwVersion = KTET_FORMAT_VERSION;
    header.m_dwNumTextureUV = 1;
    put(&header, sizeof header);
    const DWORD keyOffset = sizeof(KTETFormatHeader) + sizeof(DWORD) + sizeof(KTETFormatTextureUV);
    put(&keyOffset, sizeof keyOffset);
    KTETFormatTextureUV uv;
    uv.rightBottom = D3DXVECTOR2(512, 512);
    uv.rectSize = D3DXVECTOR2(512, 512);
    put(&uv, sizeof uv);
    const WORD length = 4;
    put(&length, sizeof length);
    const char16_t key[] = u"BG01";
    put(key, sizeof key);

    CKTDXDeviceTET_Preprocessing tet;
    REQUIRE(tet.OpenFile(file.data(), static_cast<int>(file.size()), L"DLG_Login0.TGA"));
    CHECK(tet.GetNumTexUVs() == 1);
    CHECK(tet.GetKeyFromIndex(0) == L"BG01");
    const KTETFormatTextureUV* found = tet.GetTexUV(L"BG01");
    REQUIRE(found != nullptr);
    CHECK(found->rectSize.x == 512.0f);
    CHECK(tet.GetTexUV(L"BG0") == nullptr);
    CHECK(tet.GetTexUV(L"BG010") == nullptr);
    CHECK(tet.GetTexUV(L"BG02") == nullptr);

    CHECK(KComparePreprocessedString(u"Abc", L"aBC", true) == 0);
    CHECK(KComparePreprocessedString(u"Abc", L"aBC", false) != 0);
    CHECK(KComparePreprocessedString(u"ab", L"abc", true) < 0);
    CHECK(KReadPreprocessedString(file, 0).empty());
    CHECK(KReadPreprocessedString(file, static_cast<DWORD>(file.size())).empty());
}

TEST_CASE("ktdxlib: the client's skinned meshes load (X2_MASSFILE_CLIENT_DATA)")
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
    // Which ones are skinned, on 8 threads.
    std::vector<std::string> skinned;
    std::set<std::string> ksm;
    {
        std::atomic<std::size_t> next{0};
        std::mutex mutex;
        std::vector<std::thread> threads;
        for (int t = 0; t < 8; ++t) {
            threads.emplace_back([&] {
                std::vector<std::uint8_t> bytes;
                std::string readError;
                for (std::size_t i = next++; i < names.size(); i = next++) {
                    if (!mount.Read(names[i], bytes, readError) || bytes.size() < 16) {
                        continue;
                    }
                    const bool isKsm = std::memcmp(bytes.data(), "KSM ", 4) == 0;
                    bool isSkinned = isKsm;
                    if (!isKsm) {
                        const auto file = x2::d3d9::xfile::Read(
                            std::span(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size()), readError);
                        isSkinned = file && HasObject(file->objects, "SkinWeights");
                    }
                    if (isSkinned) {
                        const std::lock_guard lock(mutex);
                        skinned.push_back(names[i]);
                        if (isKsm) {
                            ksm.insert(names[i]);
                        }
                    }
                }
            });
        }
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
    std::sort(skinned.begin(), skinned.end());

    Engine engine;
    KGCMassFileManager* files = g_pKTDXApp->GetDeviceManager()->GetMassFileManager();
    REQUIRE_MESSAGE(files->AddMassFiles(folder, KGCMassFileManager::GetClientArchiveNames(), error), error);
    files->LockMassFileMap();
    EffectStateManager manager;
    CKTDGXRenderer renderer(&engine.device);
    REQUIRE(SUCCEEDED(renderer.OnResetDevice()));
    std::vector<std::string> failed, notDrawn;
    std::size_t withBones = 0, ksmLoaded = 0, animated = 0, sets = 0, draws = 0;
    for (const std::string& name : skinned) {
        std::wstring wide(name.begin(), name.end());
        auto* mesh = new SkinMesh(&engine.device, wide);
        bool loaded = SUCCEEDED(mesh->_Load(true)) && mesh->GetFrameRoot() != nullptr && mesh->GetFrameNum() > 0;
        if (loaded && !mesh->GetIsOnlyModelData()) {
            LPD3DXANIMATIONCONTROLLER clone = mesh->GetCloneAC();
            loaded = clone != nullptr && clone->GetNumAnimationSets() > 0;
            for (UINT s = 0; loaded && s < clone->GetNumAnimationSets(); ++s) {
                LPD3DXANIMATIONSET set = nullptr;
                loaded = SUCCEEDED(clone->GetAnimationSet(s, &set)) && SUCCEEDED(clone->SetTrackAnimationSet(0, set)) &&
                         SUCCEEDED(clone->SetTrackPosition(0, set->GetPeriod() / 2)) &&
                         SUCCEEDED(clone->AdvanceTime(0.0, nullptr)) && FiniteFrames(mesh->GetFrameRoot());
                if (set != nullptr) {
                    set->Release();
                }
                ++sets;
            }
            if (clone != nullptr) {
                clone->Release();
            }
            ++animated;
        }
        if (!loaded) {
            failed.push_back(name);
        } else if (ksm.contains(name)) {
            ++ksmLoaded;
        }
        if (loaded && mesh->GetMaxSkinningVertices() > 0) {
            ++withBones;
        }
        if (loaded && mesh->GetDrawFrameNum() > 0) {
            // Drawn as a unit is: the cartoon technique with its black outline, every frame through the palette.
            x2::render::null::ClearCommands(*engine.render);
            REQUIRE(engine.device.BeginScene());
            CKTDGXRenderer::RenderParam param;
            param.renderType = CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
            param.fOutLineWide = 0.5f;
            for (HRESULT hr = renderer.BeginRender(param); SUCCEEDED(hr); hr = renderer.NextRender()) {
                for (DWORD j = 0; j < mesh->GetDrawFrameNum(); ++j) {
                    renderer.DrawFrame(*mesh, *mesh->GetDrawFrame(j));
                }
            }
            engine.device.EndScene();
            REQUIRE(engine.device.Present());
            const std::size_t drawn = Lines(*engine.render, "DrawIndexedPrimitives").size();
            if (drawn == 0) {
                notDrawn.push_back(name);
            }
            draws += drawn;
        }
        delete mesh;
        x2::render::null::ClearCommands(*engine.render);
    }
    MESSAGE(skinned.size() << " skinned meshes (" << ksm.size() << " KSM, " << ksmLoaded << " of them loaded), "
                           << withBones << " loaded with bones, " << animated << " animated (" << sets
                           << " animation sets), " << failed.size() << " failed; " << draws
                           << " draws through CKTDGXRenderer, " << notDrawn.size() << " meshes drew nothing, "
                           << engine.device.PipelineCount() << " pipelines");
    for (std::size_t i = 0; i < failed.size() && i < 40; ++i) {
        MESSAGE(failed[i]);
    }
    for (std::size_t i = 0; i < notDrawn.size() && i < 40; ++i) {
        MESSAGE("not drawn: " << notDrawn[i]);
    }
    CHECK(failed.empty());
    CHECK(notDrawn.empty());
    CHECK_MESSAGE(engine.device.LastError().empty(), engine.device.LastError());
    renderer.OnLostDevice();
}

TEST_CASE("ktdxlib: drawing between ticks interpolates views and matrices, and snaps on jumps")
{
    D3DXMATRIX a, b, out;
    const D3DXVECTOR3 up(0, 1, 0), eyeA(0, 100, -500), eyeB(20, 100, -500);
    const D3DXVECTOR3 atA = eyeA + D3DXVECTOR3(0, 0, 1), atB = eyeB + D3DXVECTOR3(0, 0, 1);
    D3DXMatrixLookAtLH(&a, &eyeA, &atA, &up);
    D3DXMatrixLookAtLH(&b, &eyeB, &atB, &up);
    REQUIRE(x2::ktdx::InterpolateView(a, b, 0.25f, out));
    D3DXMATRIX camera;
    D3DXMatrixInverse(&camera, nullptr, &out);
    CHECK(camera._41 == doctest::Approx(5.f));
    CHECK(camera._33 == doctest::Approx(1.f));

    // a cut: the camera jumps, so the last view is drawn as it is
    const D3DXVECTOR3 farEye(5000, 100, -500), farAt = farEye + D3DXVECTOR3(0, 0, 1);
    D3DXMatrixLookAtLH(&b, &farEye, &farAt, &up);
    CHECK_FALSE(x2::ktdx::InterpolateView(a, b, 0.5f, out));
    CHECK(out == b);

    // world matrices: a small step is interpolated, a turn around or a teleport is not
    D3DXMATRIX w0, w1, w2, w3;
    D3DXMatrixTranslation(&w0, 0, 0, 0);
    D3DXMatrixTranslation(&w1, 10, 0, 0);
    CHECK_FALSE(x2::ktdx::IsWorldJump(w0, w1));
    x2::ktdx::LerpMatrix(w0, w1, 0.5f, out);
    CHECK(out._41 == doctest::Approx(5.f));
    D3DXMatrixRotationY(&w2, D3DX_PI);
    CHECK(x2::ktdx::IsWorldJump(w0, w2));
    D3DXMatrixTranslation(&w3, 1000, 0, 0);
    CHECK(x2::ktdx::IsWorldJump(w0, w3));
}
