// KTDXLIB's D3D9 device (decision T38, docs/porting/client-d3d9.md): headless on the null backend (recorded commands,
// uploaded bytes), plus pixels read back on a real GPU where there is one (skipped otherwise: container, CI).

#include "Device.h"
#include "Fvf.h"
#include "GeneratedShader.h"

#include "x2/render/null.h"

#include <SDL3/SDL_init.h>
#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace generated {
#include "shaders/generated/FixedFunctionShaders.inc"
}

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

bool Has(x2::render::Device& device, const std::string& text)
{
    for (const std::string& line : x2::render::null::Commands(device)) {
        if (line.find(text) != std::string::npos) {
            return true;
        }
    }
    return false;
}

struct ScreenVertex {
    float x, y, z, rhw;
    D3DCOLOR color;
    float u, v;
};
constexpr DWORD kScreenFvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

void Quad(ScreenVertex (&quad)[4], float x1, float y1, float x2, float y2, D3DCOLOR color)
{
    quad[0] = {x1, y1, 0, 1, color, 0, 0};
    quad[1] = {x2, y1, 0, 1, color, 1, 0};
    quad[2] = {x1, y2, 0, 1, color, 0, 1};
    quad[3] = {x2, y2, 0, 1, color, 1, 1};
}

std::uint64_t Fnv(const std::string& bytes, std::uint64_t h)
{
    for (unsigned char b : bytes) {
        h = (h ^ b) * 0x100000001B3ULL;
    }
    return h;
}

}  // namespace

TEST_CASE("d3d9: the generated shaders match their sources (tools/shaders/compile.py)")
{
    const std::string dir = std::string(X2_CLIENT_SOURCE_DIR) + "/KTDXLIB/D3D9/shaders/";
    std::uint64_t h = 0xCBF29CE484222325ULL;
    for (const char* name : generated::kShaderSourceFiles) {
        std::ifstream file(dir + name, std::ios::binary);
        REQUIRE_MESSAGE(file, std::string(dir + name));
        const std::string bytes{std::istreambuf_iterator<char>(file), {}};
        h = Fnv(std::string(name) + '\0', h);
        h = Fnv(bytes + '\0', h);
    }
    CHECK_MESSAGE(h == generated::kShaderSourceHash,
                  "a shader source changed: run docker/build.sh python3 tools/shaders/compile.py "
                  "src/client/KTDXLIB/D3D9/shaders");
    CHECK(std::size(generated::kFixedFunctionVertex) == 12);
    for (const x2::GeneratedShader& shader : generated::kFixedFunctionVertex) {
        CHECK(!shader.spirv.empty());
        CHECK(!shader.dxil.empty());
        CHECK(!shader.msl.empty());
        CHECK(shader.samplerCount == 0);
        CHECK(shader.uniformBufferCount == 1);
    }
    CHECK(generated::kFixedFunctionFragment[0].samplerCount == 2);
}

TEST_CASE("d3d9: FVF layouts and shader variants")
{
    using x2::d3d9::DescribeFvf;
    const auto screen = DescribeFvf(kScreenFvf);
    CHECK(screen.valid);
    CHECK(screen.rhw);
    CHECK(screen.size == 28);
    CHECK(screen.diffuseOffset == 16);
    CHECK(screen.texOffset[0] == 20);
    CHECK(x2::d3d9::VertexShaderIndex(screen) == (1 * 2 + 1) * 3 + 1);
    // POSITION_RHW=1 DIFFUSE=1 TEXCOORDS=1 in the generated table
    CHECK(std::string(generated::kFixedFunctionVertex[x2::d3d9::VertexShaderIndex(screen)].variant) ==
          "POSITION_RHW=1 DIFFUSE=1 TEXCOORDS=1");

    const auto mesh = DescribeFvf(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
    CHECK(mesh.size == 32);
    CHECK(mesh.diffuseOffset == -1);
    CHECK(mesh.texOffset[0] == 24);

    const auto skinned = DescribeFvf(D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1);
    CHECK(skinned.size == 12 + 16 + 12 + 8);   // position, 4 betas, normal, one set

    const auto tex3 = DescribeFvf(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1));
    CHECK(tex3.size == 12 + 4 + 8 + 12);
    CHECK(tex3.texFormat[1] == x2::render::VertexElementFormat::Float3);
    CHECK(x2::d3d9::VertexShaderIndex(tex3) == (0 * 2 + 1) * 3 + 2);

    CHECK_FALSE(DescribeFvf(D3DFVF_DIFFUSE).valid);
}

TEST_CASE("d3d9: a frame on the null backend")
{
    auto render = NullRender();
    {
        x2::d3d9::Device device(*render, nullptr);
        REQUIRE_MESSAGE(device.Ok(), device.LastError());
        auto target = device.CreateTexture(64, 32, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
        REQUIRE(target);
        CHECK(device.SetRenderTarget(0, target.get()));
        D3DVIEWPORT9 viewport;
        device.GetViewport(&viewport);
        CHECK(viewport.Width == 64);
        CHECK(viewport.Height == 32);

        ScreenVertex quad[4];
        Quad(quad, -0.5f, -0.5f, 31.5f, 31.5f, 0xFFFFFFFF);
        device.SetFVF(kScreenFvf);
        CHECK_FALSE(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));   // no scene

        x2::render::null::ClearCommands(*render);
        REQUIRE(device.BeginScene());
        CHECK(device.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 0));
        CHECK(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));
        CHECK(device.DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, quad, sizeof(ScreenVertex)));
        CHECK(device.EndScene());
        CHECK(device.Present());

        const auto& commands = x2::render::null::Commands(*render);
        // Uploads are submitted before the frame that draws them.
        const auto upload = std::find_if(commands.begin(), commands.end(),
                                         [](const std::string& l) { return l.rfind("UploadToBuffer", 0) == 0; });
        const auto pass = std::find_if(commands.begin(), commands.end(),
                                       [](const std::string& l) { return l.rfind("BeginRenderPass", 0) == 0; });
        REQUIRE(upload != commands.end());
        REQUIRE(pass != commands.end());
        CHECK(upload < pass);
        CHECK(pass->find("load=clear(1,0,0,1)") != std::string::npos);   // the whole-target clear became the load op
        CHECK(Lines(*render, "UploadToBuffer").size() == 2);
        CHECK(Lines(*render, "DrawPrimitives vertices=4").size() == 1);
        CHECK(Lines(*render, "DrawIndexedPrimitives indices=6").size() == 1);   // the fan as a list
        CHECK(Lines(*render, "BindIndexBuffer").size() == 1);
        CHECK(Lines(*render, "Submit").size() == 2);
        CHECK(device.PipelineCount() == 2);     // strip and list
    }
    CHECK(x2::render::null::LiveResourceCount(*render) == 0);
}

TEST_CASE("d3d9: pipelines are cached by the states that live in them")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    auto target = device.CreateTexture(16, 16, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    device.SetRenderTarget(0, target.get());
    ScreenVertex quad[4];
    Quad(quad, 0, 0, 8, 8, 0xFFFFFFFF);
    device.SetFVF(kScreenFvf);
    REQUIRE(device.BeginScene());
    auto draw = [&] { REQUIRE(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex))); };

    draw();
    draw();
    CHECK(device.PipelineCount() == 1);
    // Uniforms and dynamic state: no new pipeline.
    device.SetRenderState(D3DRS_TEXTUREFACTOR, 0x80808080);
    device.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    device.SetRenderState(D3DRS_ALPHAREF, 0x40);
    device.SetRenderState(D3DRS_STENCILREF, 3);
    device.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
    draw();
    CHECK(device.PipelineCount() == 1);
    // Pipeline state: blending.
    device.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    draw();
    CHECK(device.PipelineCount() == 2);
    // Blend factors do not matter while blending is off.
    device.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    draw();
    device.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    draw();
    CHECK(device.PipelineCount() == 2);
    device.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    draw();
    CHECK(device.PipelineCount() == 3);
    device.EndScene();
    device.Present();
}

TEST_CASE("d3d9: texture locks upload in the device's format")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());

    SUBCASE("A8R8G8B8 as is, with its mip levels")
    {
        auto texture = device.CreateTexture(4, 4, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        REQUIRE(texture);
        CHECK(texture->GetLevelCount() == 3);
        CHECK(texture->RenderFormat() == x2::render::TextureFormat::B8G8R8A8Unorm);
        D3DLOCKED_RECT locked;
        REQUIRE(texture->LockRect(0, &locked, nullptr, 0));
        CHECK(locked.Pitch == 16);
        for (DWORD i = 0; i < 16; ++i) {
            static_cast<DWORD*>(locked.pBits)[i] = D3DCOLOR_ARGB(0x80, i, 2 * i, 3 * i);
        }
        CHECK_FALSE(texture->LockRect(0, &locked, nullptr, 0));   // already locked
        REQUIRE(texture->UnlockRect(0));
        device.Present();
        const auto bytes = x2::render::null::TextureBytes(*render, texture->RenderTexture());
        REQUIRE(bytes.size() == 64);
        CHECK(static_cast<std::uint8_t>(bytes[5 * 4 + 0]) == 15);    // B
        CHECK(static_cast<std::uint8_t>(bytes[5 * 4 + 1]) == 10);    // G
        CHECK(static_cast<std::uint8_t>(bytes[5 * 4 + 2]) == 5);     // R
        CHECK(static_cast<std::uint8_t>(bytes[5 * 4 + 3]) == 0x80);  // A
        CHECK(!Lines(*render, "UploadToTexture").empty());
        CHECK(Lines(*render, "UploadToTexture").back().find("cycle") == std::string::npos);   // 3 levels: no cycling
    }

    SUBCASE("X8R8G8B8 gets an opaque alpha; a single level cycles")
    {
        auto texture = device.CreateTexture(2, 2, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED);
        REQUIRE(texture);
        D3DLOCKED_RECT locked;
        REQUIRE(texture->LockRect(0, &locked, nullptr, 0));
        std::memset(locked.pBits, 0x11, 16);
        texture->UnlockRect(0);
        device.Present();
        const auto bytes = x2::render::null::TextureBytes(*render, texture->RenderTexture());
        REQUIRE(bytes.size() == 16);
        CHECK(static_cast<std::uint8_t>(bytes[3]) == 0xFF);
        CHECK(static_cast<std::uint8_t>(bytes[0]) == 0x11);
        CHECK(Lines(*render, "UploadToTexture").back().find("cycle") != std::string::npos);
    }

    SUBCASE("16-bit and luminance formats")
    {
        auto texture = device.CreateTexture(1, 1, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED);
        REQUIRE(texture);
        D3DLOCKED_RECT locked;
        REQUIRE(texture->LockRect(0, &locked, nullptr, 0));
        *static_cast<WORD*>(locked.pBits) = 0x8F00;     // A=8, R=F, G=0, B=0
        texture->UnlockRect(0);
        auto luminance = device.CreateTexture(1, 1, 1, 0, D3DFMT_A8L8, D3DPOOL_MANAGED);
        REQUIRE(luminance);
        REQUIRE(luminance->LockRect(0, &locked, nullptr, 0));
        *static_cast<WORD*>(locked.pBits) = 0x4020;     // L=0x20, A=0x40
        luminance->UnlockRect(0);
        device.Present();
        const auto bytes = x2::render::null::TextureBytes(*render, texture->RenderTexture());
        if (texture->RenderFormat() == x2::render::TextureFormat::B4G4R4A4Unorm) {
            REQUIRE(bytes.size() == 2);
            CHECK(static_cast<std::uint8_t>(bytes[1]) == 0x8F);
        } else {
            REQUIRE(bytes.size() == 4);
            CHECK(static_cast<std::uint8_t>(bytes[2]) == 0xFF);
            CHECK(static_cast<std::uint8_t>(bytes[3]) == 0x88);
        }
        const auto l = x2::render::null::TextureBytes(*render, luminance->RenderTexture());
        REQUIRE(l.size() == 4);
        CHECK(static_cast<std::uint8_t>(l[0]) == 0x20);
        CHECK(static_cast<std::uint8_t>(l[1]) == 0x20);
        CHECK(static_cast<std::uint8_t>(l[2]) == 0x20);
        CHECK(static_cast<std::uint8_t>(l[3]) == 0x40);
    }

    SUBCASE("DXT1 blocks and a locked rectangle")
    {
        auto texture = device.CreateTexture(8, 8, 1, 0, D3DFMT_DXT1, D3DPOOL_MANAGED);
        REQUIRE(texture);
        CHECK(texture->RenderFormat() == x2::render::TextureFormat::Bc1Unorm);
        D3DLOCKED_RECT locked;
        const RECT second{4, 4, 8, 8};      // the block at (1, 1)
        REQUIRE(texture->LockRect(0, &locked, &second, 0));
        CHECK(locked.Pitch == 16);
        std::memset(locked.pBits, 0xAB, 8);
        texture->UnlockRect(0);
        device.Present();
        const auto bytes = x2::render::null::TextureBytes(*render, texture->RenderTexture());
        REQUIRE(bytes.size() == 32);
        CHECK(static_cast<std::uint8_t>(bytes[24]) == 0xAB);
        CHECK(static_cast<std::uint8_t>(bytes[0]) == 0);
    }

    SUBCASE("render targets cannot be locked; unsupported formats fail")
    {
        auto target = device.CreateTexture(4, 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
        REQUIRE(target);
        D3DLOCKED_RECT locked;
        CHECK_FALSE(target->LockRect(0, &locked, nullptr, 0));
        CHECK_FALSE(device.CreateTexture(4, 4, 1, 0, D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED));
        CHECK_FALSE(device.CreateTexture(4, 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT));
    }
}

TEST_CASE("d3d9: vertex buffer locks follow D3D9's flags")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    auto buffer = device.CreateVertexBuffer(1024, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, kScreenFvf, D3DPOOL_DEFAULT);
    REQUIRE(buffer);
    void* data = nullptr;
    x2::render::null::ClearCommands(*render);

    REQUIRE(buffer->Lock(0, 256, &data, D3DLOCK_DISCARD));
    std::memset(data, 1, 256);
    REQUIRE(buffer->Unlock());
    REQUIRE(buffer->Lock(256, 128, &data, D3DLOCK_NOOVERWRITE));
    std::memset(data, 2, 128);
    REQUIRE(buffer->Unlock());
    REQUIRE(buffer->Lock(512, 16, &data, 0));
    std::memset(data, 3, 16);
    REQUIRE(buffer->Unlock());
    CHECK_FALSE(buffer->Lock(1000, 100, &data, 0));
    device.Present();

    const auto uploads = Lines(*render, "UploadToBuffer");
    REQUIRE(uploads.size() == 3);
    CHECK(uploads[0].find("+0 size=256 cycle") != std::string::npos);
    CHECK(uploads[1].find("+256 size=128") != std::string::npos);
    CHECK(uploads[1].find("cycle") == std::string::npos);
    CHECK(uploads[2].find("+0 size=1024 cycle") != std::string::npos);    // no flags: the whole buffer kept
    const auto bytes = x2::render::null::BufferBytes(*render, buffer->RenderBuffer());
    REQUIRE(bytes.size() == 1024);
    CHECK(static_cast<std::uint8_t>(bytes[0]) == 1);
    CHECK(static_cast<std::uint8_t>(bytes[300]) == 2);
    CHECK(static_cast<std::uint8_t>(bytes[520]) == 3);

    // Drawn from stream 0, then unset by DrawPrimitiveUP as in D3D9.
    auto target = device.CreateTexture(16, 16, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    device.SetRenderTarget(0, target.get());
    device.SetFVF(kScreenFvf);
    device.SetStreamSource(0, buffer.get(), 0, sizeof(ScreenVertex));
    REQUIRE(device.BeginScene());
    CHECK(device.DrawPrimitive(D3DPT_TRIANGLELIST, 2, 1));
    ScreenVertex quad[4];
    Quad(quad, 0, 0, 4, 4, 0xFFFFFFFF);
    CHECK(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));
    CHECK_FALSE(device.DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1));
    device.EndScene();
    x2::render::null::ClearCommands(*render);
    device.Present();
    CHECK(Lines(*render, "DrawPrimitives vertices=3 instances=1 first=2").size() == 1);
}

TEST_CASE("d3d9: a clear of part of the target is drawn")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    auto target = device.CreateTexture(32, 32, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    device.SetRenderTarget(0, target.get());
    const D3DVIEWPORT9 half{0, 0, 16, 32, 0.0f, 1.0f};
    device.SetViewport(&half);
    const DWORD cullBefore = device.GetRenderState(D3DRS_CULLMODE);
    x2::render::null::ClearCommands(*render);
    CHECK(device.Clear(0, nullptr, D3DCLEAR_TARGET, 0xFF00FF00, 1.0f, 0));
    const D3DRECT rects[2] = {{0, 0, 4, 4}, {8, 8, 40, 40}};    // the second is clipped to the viewport
    CHECK(device.Clear(2, rects, D3DCLEAR_TARGET, 0xFF0000FF, 1.0f, 0));
    device.Present();
    CHECK(Lines(*render, "DrawPrimitives vertices=4").size() == 3);
    CHECK_FALSE(Has(*render, "load=clear"));
    CHECK(device.GetRenderState(D3DRS_CULLMODE) == cullBefore);    // states restored
    D3DVIEWPORT9 after;
    device.GetViewport(&after);
    CHECK(after.Width == 16);
}

TEST_CASE("d3d9: vertex shader constants are kept per register")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    const float set[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    CHECK(device.SetVertexShaderConstantF(10, set, 2));
    float got[12] = {};
    CHECK(device.GetVertexShaderConstantF(9, got, 3));
    CHECK(got[0] == 0.0f);
    CHECK(got[4] == 1.0f);
    CHECK(got[11] == 8.0f);
    CHECK(device.SetVertexShaderConstantF(x2::d3d9::kVertexShaderConstants - 1, set, 1));
    CHECK_FALSE(device.SetVertexShaderConstantF(x2::d3d9::kVertexShaderConstants - 1, set, 2));
    CHECK_FALSE(device.GetVertexShaderConstantF(x2::d3d9::kVertexShaderConstants + 1, got, 0));
}

TEST_CASE("d3d9: resources released before or after the device")
{
    auto render = NullRender();
    std::unique_ptr<x2::d3d9::Texture> survivor;
    {
        x2::d3d9::Device device(*render, nullptr);
        REQUIRE(device.Ok());
        auto texture = device.CreateTexture(4, 4, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        device.SetTexture(0, texture.get());
        texture.reset();
        CHECK(device.GetTexture(0) == nullptr);     // unbound when destroyed
        auto target = device.CreateTexture(4, 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
        device.SetRenderTarget(0, target.get());
        target.reset();
        CHECK(device.GetRenderTarget() == nullptr);
        survivor = device.CreateTexture(4, 4, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
    }
    CHECK(x2::render::null::LiveResourceCount(*render) == 0);
    D3DLOCKED_RECT locked;
    CHECK_FALSE(survivor->LockRect(0, &locked, nullptr, 0));      // detached from the destroyed device
    survivor.reset();
}

// --- On a GPU -------------------------------------------------------------------------------------------------------

namespace {

// The target's pixels as B, G, R, A bytes.
std::vector<std::uint8_t> ReadBack(x2::d3d9::Device& device, x2::d3d9::Texture& target)
{
    x2::render::Device& render = device.RenderDevice();
    const std::uint32_t size = target.Width() * target.Height() * 4;
    const auto transfer = render.CreateTransferBuffer({.usage = x2::render::TransferBufferUsage::Download, .size = size});
    x2::render::CommandBuffer* commands = render.AcquireCommandBuffer();
    REQUIRE(commands);
    x2::render::CopyPass* copy = commands->BeginCopyPass();
    copy->DownloadFromTexture({.texture = target.RenderTexture(), .w = target.Width(), .h = target.Height()},
                              {transfer, 0, 0, 0});
    copy->End();
    REQUIRE(commands->Submit());
    REQUIRE(render.WaitIdle());
    std::vector<std::uint8_t> pixels(size);
    std::memcpy(pixels.data(), render.MapTransferBuffer(transfer, false), size);
    render.UnmapTransferBuffer(transfer);
    render.ReleaseTransferBuffer(transfer);
    return pixels;
}

}  // namespace

TEST_CASE("d3d9: pixels on a GPU (skipped without one)")
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        MESSAGE("no video (" << SDL_GetError() << "): skipped");
        return;
    }
    struct Quit {
        ~Quit() { SDL_QuitSubSystem(SDL_INIT_VIDEO); }
    } quit;
    std::string error;
    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::SdlGpu;
    auto render = x2::render::Device::Create(options, error);
    if (!render) {
        MESSAGE("no GPU device (" << error << "): skipped");
        return;
    }
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE_MESSAGE(device.Ok(), device.LastError());
    auto target = device.CreateTexture(16, 16, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
    REQUIRE(target);
    // A 4 x 4 texture with one color per texel.
    auto texture = device.CreateTexture(4, 4, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
    D3DLOCKED_RECT locked;
    REQUIRE(texture->LockRect(0, &locked, nullptr, 0));
    for (DWORD i = 0; i < 16; ++i) {
        static_cast<DWORD*>(locked.pBits)[i] = D3DCOLOR_ARGB(255, i * 16, 255 - i * 16, i % 2 ? 255 : 0);
    }
    texture->UnlockRect(0);

    device.SetRenderTarget(0, target.get());
    device.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    device.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device.SetFVF(kScreenFvf);
    REQUIRE(device.BeginScene());
    device.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);
    // Texels to pixels 1:1 at (8, 0)-(12, 4), D3D9 style: -0.5 aligns them. Point sampling.
    ScreenVertex quad[4];
    Quad(quad, 7.5f, -0.5f, 11.5f, 3.5f, 0xFFFFFFFF);
    device.SetTexture(0, texture.get());
    REQUIRE(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));
    // Untextured (MODULATE with the white texel): the vertex color, pixels (0, 8)-(4, 12).
    device.SetTexture(0, nullptr);
    Quad(quad, -0.5f, 7.5f, 3.5f, 11.5f, D3DCOLOR_ARGB(255, 10, 200, 30));
    REQUIRE(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));
    // Blended over it: 50% white on (2, 8)-(4, 12); alpha-tested away on (0, 12)-(4, 16). The vertex alpha: the
    // default ALPHAARG1 is the texture, and with no texture that is the white texel's 1.
    device.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    device.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    Quad(quad, 1.5f, 7.5f, 3.5f, 11.5f, D3DCOLOR_ARGB(128, 255, 255, 255));
    REQUIRE(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));
    device.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    device.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    device.SetRenderState(D3DRS_ALPHAREF, 0x80);
    Quad(quad, -0.5f, 11.5f, 3.5f, 15.5f, D3DCOLOR_ARGB(128, 255, 0, 0));
    REQUIRE(device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex)));
    device.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    device.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    // A fan covering (12, 12)-(16, 16).
    ScreenVertex fan[4];
    Quad(fan, 11.5f, 11.5f, 15.5f, 15.5f, D3DCOLOR_ARGB(255, 0, 0, 255));
    std::swap(fan[2], fan[3]);      // strip order -> fan order around the square
    REQUIRE(device.DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, fan, sizeof(ScreenVertex)));
    device.EndScene();
    REQUIRE(device.Present());

    const std::vector<std::uint8_t> pixels = ReadBack(device, *target);
    auto at = [&](int x, int y) {
        const std::size_t i = (static_cast<std::size_t>(y) * 16 + x) * 4;
        return D3DCOLOR_ARGB(pixels[i + 3], pixels[i + 2], pixels[i + 1], pixels[i]);
    };
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            const DWORD i = static_cast<DWORD>(y * 4 + x);
            CHECK_MESSAGE(at(8 + x, y) == D3DCOLOR_ARGB(255, i * 16, 255 - i * 16, i % 2 ? 255 : 0),
                          "texel " << x << "," << y);
        }
    }
    CHECK(at(7, 0) == D3DCOLOR_ARGB(255, 0, 0, 0));     // nothing outside the quad
    CHECK(at(12, 0) == D3DCOLOR_ARGB(255, 0, 0, 0));
    CHECK(at(8, 4) == D3DCOLOR_ARGB(255, 0, 0, 0));
    CHECK(at(0, 8) == D3DCOLOR_ARGB(255, 10, 200, 30));
    const DWORD blended = at(3, 9);
    CHECK(std::abs(static_cast<int>((blended >> 16) & 0xFF) - (10 + 255) / 2) <= 2);
    CHECK(std::abs(static_cast<int>((blended >> 8) & 0xFF) - (200 + 255) / 2) <= 2);
    CHECK(at(1, 13) == D3DCOLOR_ARGB(255, 0, 0, 0));    // alpha 128 is not > 128
    CHECK(at(13, 13) == D3DCOLOR_ARGB(255, 0, 0, 255));
    CHECK(at(15, 15) == D3DCOLOR_ARGB(255, 0, 0, 255));
    CHECK(at(11, 13) == D3DCOLOR_ARGB(255, 0, 0, 0));
}
