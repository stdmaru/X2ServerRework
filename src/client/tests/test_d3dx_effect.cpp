// KTDXLIB's effects (decision T37, docs/porting/client-d3d9.md#effects-2026-11-07-clienteffects): XRenderer.fx's
// techniques and parameters, passes applied to the device, parameters in the vertex shader registers, and draws on
// the null backend with the uniform bytes they push.

#include "D3DXTypes.h"
#include "Device.h"
#include "Effect.h"

#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <cstring>
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

// The effect with one reference, released at the end of the scope.
struct EffectRef {
    LPD3DXEFFECT effect = nullptr;
    explicit EffectRef(x2::d3d9::Device& device) { REQUIRE(x2::d3d9::CreateXRendererEffect(&device, &effect) == D3D_OK); }
    ~EffectRef() { effect->Release(); }
    ID3DXEffect* operator->() const { return effect; }
    x2::d3d9::Effect& Get() const { return *static_cast<x2::d3d9::Effect*>(effect); }
};

const x2::d3d9::EffectProgram* Program(x2::d3d9::Device& device)
{
    x2::d3d9::VertexShader* shader = nullptr;
    device.GetVertexShader(&shader);
    return shader ? &shader->Program() : nullptr;
}

D3DXMATRIX Translation(float x, float y, float z)
{
    D3DXMATRIX m;
    D3DXMatrixTranslation(&m, x, y, z);
    return m;
}

const char* const kTechniques[] = {
    "REAL_COLOR", "CARTOON_BLACK_EDGE", "CARTOON_COLOR_EDGE", "CARTOON", "ADD_CARTOON_BLACK_EDGE",
    "ADD_CARTOON_COLOR_EDGE", "ADD_CARTOON", "BLACK_EDGE", "COLOR_EDGE", "LIGHT_FLOW_WORLD_X", "LIGHT_FLOW_WORLD_Y",
    "LIGHT_FLOW_WORLD_Z", "LIGHT_FLOW_WORLD_POINT", "LIGHT_FLOW_LOCAL_X", "LIGHT_FLOW_LOCAL_Y", "LIGHT_FLOW_LOCAL_Z",
    "LIGHT_FLOW_LOCAL_POINT", "CARTOON_FADE", "CARTOON_FADE_NONTEX",
};

}  // namespace

TEST_CASE("d3dx: the XRenderer effect's techniques and parameters")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    EffectRef effect(device);

    CHECK(x2::d3d9::XRendererEffect().techniques.size() == 19);
    for (const char* name : kTechniques) {
        const D3DXHANDLE technique = effect->GetTechniqueByName(name);
        REQUIRE_MESSAGE(technique != nullptr, name);
        CHECK(std::string(technique) == name);
        CHECK(effect->SetTechnique(technique) == D3D_OK);
        UINT passes = 0;
        REQUIRE(effect->Begin(&passes, D3DXFX_DONOTSAVESTATE) == D3D_OK);
        const std::string n = name;
        const bool twoPasses = n == "CARTOON_BLACK_EDGE" || n == "CARTOON_COLOR_EDGE" || n == "ADD_CARTOON_BLACK_EDGE" ||
                               n == "ADD_CARTOON_COLOR_EDGE";
        CHECK_MESSAGE(passes == (twoPasses ? 2u : 1u), name);
        CHECK(effect->End() == D3D_OK);
    }
    CHECK(effect->GetTechniqueByName("NO_SUCH_TECHNIQUE") == nullptr);
    CHECK(effect->SetTechnique("CARTOON") == D3D_OK);      // D3DX takes names as handles

    for (const char* name : {"g_mViewProj", "g_mWorldViewProj", "g_Color", "g_TexOffSet01", "g_LightPos",
                             "g_OutLineColor", "g_LightFlowPoint", "g_fTexOffSet2X", "g_fTexOffSet2Y", "g_fOutLineWide",
                             "g_fLightFlowImpact", "g_fLightFlowWide", "g_bFog", "g_fFog", "g_fEye", "g_fColor",
                             "g_fDensity", "g_amWorldPalette", "g_iShaderIndex"}) {
        const D3DXHANDLE parameter = effect->GetParameterByName(nullptr, name);
        REQUIRE_MESSAGE(parameter != nullptr, name);
        CHECK(std::string(parameter) == name);
        CHECK(effect->GetParameterByName(nullptr, name) == parameter);  // stable handles
    }
    CHECK(effect->GetParameterByName(nullptr, "g_NoSuchParameter") == nullptr);
    CHECK(effect->GetParameterByName("g_Color", "x") == nullptr);    // no structures
    CHECK(effect->SetTexture("g_Color", nullptr) == D3DERR_INVALIDCALL);
    CHECK(effect->AddRef() == 2);
    CHECK(effect->Release() == 1);
}

TEST_CASE("d3dx: an effect pass applies its states and selects its shader")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    EffectRef effect(device);

    CHECK(effect->BeginPass(0) == D3DERR_INVALIDCALL);     // outside Begin
    REQUIRE(effect->SetTechnique("CARTOON_BLACK_EDGE") == D3D_OK);
    UINT passes = 0;
    REQUIRE(effect->Begin(&passes, 0) == D3D_OK);
    CHECK(effect->SetTechnique("CARTOON") == D3DERR_INVALIDCALL);  // inside Begin
    CHECK(effect->Begin(&passes, 0) == D3DERR_INVALIDCALL);
    REQUIRE(effect->BeginPass(0) == D3D_OK);
    CHECK(effect->BeginPass(1) == D3DERR_INVALIDCALL);     // a pass is active
    CHECK(device.GetTextureStageState(1, D3DTSS_COLOROP) == D3DTOP_MODULATE);
    CHECK(device.GetTextureStageState(1, D3DTSS_COLORARG2) == D3DTA_TEXTURE);
    CHECK(device.GetSamplerState(1, D3DSAMP_ADDRESSU) == D3DTADDRESS_CLAMP);
    CHECK(device.GetTextureStageState(2, D3DTSS_ALPHAOP) == D3DTOP_SELECTARG1);
    CHECK(device.GetRenderState(D3DRS_FOGENABLE) == FALSE);
    CHECK(device.GetRenderState(D3DRS_FOGVERTEXMODE) == D3DFOG_LINEAR);
    REQUIRE(Program(device) != nullptr);
    CHECK(Program(device)->kind == x2::d3d9::EffectProgram::Cartoon);
    CHECK(Program(device)->bones == 0);
    CHECK_FALSE(Program(device)->fog);

    // Parameters the states depend on reach the device at CommitChanges, not before.
    CHECK(effect->SetBool("g_bFog", TRUE) == D3D_OK);
    const D3DXVECTOR4 red(1, 0, 0, 1);
    CHECK(effect->SetVector("g_fColor", &red) == D3D_OK);
    CHECK(effect->SetInt("g_iShaderIndex", 7) == D3D_OK);      // two bones, with fog
    CHECK(Program(device)->bones == 0);
    REQUIRE(effect->CommitChanges() == D3D_OK);
    CHECK(Program(device)->bones == 2);
    CHECK(Program(device)->fog);
    CHECK(device.GetRenderState(D3DRS_FOGENABLE) == TRUE);
    CHECK(device.GetRenderState(D3DRS_FOGCOLOR) == 0xFFFF0000);
    REQUIRE(effect->EndPass() == D3D_OK);
    CHECK(effect->CommitChanges() == D3DERR_INVALIDCALL);  // no active pass

    // The outline pass: the texture factor's black, front faces culled, stages 1 and 2 off.
    REQUIRE(effect->BeginPass(1) == D3D_OK);
    CHECK(device.GetRenderState(D3DRS_CULLMODE) == D3DCULL_CW);
    CHECK(device.GetRenderState(D3DRS_TEXTUREFACTOR) == 0xFF000000);
    CHECK(device.GetTextureStageState(0, D3DTSS_COLORARG1) == D3DTA_TFACTOR);
    CHECK(device.GetTextureStageState(1, D3DTSS_COLOROP) == D3DTOP_DISABLE);
    CHECK(device.GetTextureStageState(2, D3DTSS_ALPHAOP) == D3DTOP_DISABLE);
    CHECK(Program(device)->kind == x2::d3d9::EffectProgram::BlackEdge);
    CHECK(Program(device)->bones == 2);
    CHECK(effect->SetInt("g_iShaderIndex", 10) == D3D_OK);     // past the array
    CHECK(effect->CommitChanges() == D3DERR_INVALIDCALL);
    CHECK(Program(device)->bones == 2);
    REQUIRE(effect->EndPass() == D3D_OK);
    REQUIRE(effect->End() == D3D_OK);
    CHECK(effect->End() == D3DERR_INVALIDCALL);

    // Light flow arrays have five shaders (bones 0-4, no fog).
    REQUIRE(effect->SetTechnique("LIGHT_FLOW_LOCAL_POINT") == D3D_OK);
    REQUIRE(effect->Begin(&passes, 0) == D3D_OK);
    CHECK(effect->SetInt("g_iShaderIndex", 5) == D3D_OK);
    CHECK(effect->BeginPass(0) == D3DERR_INVALIDCALL);
    CHECK(effect->SetInt("g_iShaderIndex", 4) == D3D_OK);
    REQUIRE(effect->BeginPass(0) == D3D_OK);
    CHECK(Program(device)->kind == x2::d3d9::EffectProgram::FlowLocal);
    CHECK(Program(device)->axis == 3);
    CHECK(Program(device)->bones == 4);
    CHECK(device.GetRenderState(D3DRS_ALPHABLENDENABLE) == TRUE);
    CHECK(device.GetRenderState(D3DRS_DESTBLEND) == D3DBLEND_DESTALPHA);
    CHECK(device.GetTextureStageState(0, D3DTSS_ALPHAOP) == D3DTOP_ADD);
    effect->EndPass();
    effect->End();

    // The effect's shaders go with it: the device forgets the one it had.
    {
        LPD3DXEFFECT other = nullptr;
        REQUIRE(x2::d3d9::CreateXRendererEffect(&device, &other) == D3D_OK);
        other->SetTechnique("REAL_COLOR");
        other->Begin(&passes, 0);
        other->BeginPass(0);
        CHECK(Program(device) != nullptr);
        other->EndPass();
        other->End();
        other->Release();
        CHECK(Program(device) == nullptr);
    }
    LPD3DXEFFECT none = reinterpret_cast<LPD3DXEFFECT>(&device);
    CHECK(x2::d3d9::CreateXRendererEffect(nullptr, &none) == D3DERR_INVALIDCALL);
    CHECK(none == nullptr);
}

TEST_CASE("d3dx: effect parameters are the vertex shader registers")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    EffectRef effect(device);
    const D3DXMATRIX palette[3] = {Translation(1, 2, 3), Translation(4, 5, 6), Translation(7, 8, 9)};
    REQUIRE(effect->SetMatrixArray("g_amWorldPalette", palette, 3) == D3D_OK);
    const D3DXMATRIX viewProj = Translation(10, 20, 30);
    REQUIRE(effect->SetMatrix("g_mViewProj", &viewProj) == D3D_OK);
    const D3DXVECTOR4 color(0.25f, 0.5f, 0.75f, 1.0f);
    REQUIRE(effect->SetVector("g_Color", &color) == D3D_OK);
    REQUIRE(effect->SetFloat("g_fOutLineWide", 1.5f) == D3D_OK);
    REQUIRE(effect->SetFloat("g_fLightFlowWide", 3.0f) == D3D_OK);
    CHECK(effect->SetMatrixArray("g_amWorldPalette", palette, 21) == D3DERR_INVALIDCALL);
    CHECK(effect->SetFloat("g_mViewProj", 1.0f) == D3DERR_INVALIDCALL);
    CHECK(effect->SetVector("g_mViewProj", &color) == D3DERR_INVALIDCALL);
    CHECK(effect->SetMatrix("g_Color", &viewProj) == D3DERR_INVALIDCALL);
    CHECK(effect->SetFloat("g_NoSuchParameter", 1.0f) == D3DERR_INVALIDCALL);
    CHECK(effect->SetVector("g_fTexOffSet2Y", &color) == D3D_OK);     // a float takes the vector's x

    UINT passes = 0;
    REQUIRE(effect->SetTechnique("BLACK_EDGE") == D3D_OK);
    REQUIRE(effect->Begin(&passes, 0) == D3D_OK);
    REQUIRE(effect->BeginPass(0) == D3D_OK);
    float r[x2::d3d9::kEffectRegisters * 4] = {};
    REQUIRE(device.GetVertexShaderConstantF(0, r, x2::d3d9::kEffectRegisters));
    CHECK(std::memcmp(r, palette, sizeof(palette)) == 0);           // c0-c11, rows as they are
    CHECK(r[4 * 3 + 0] == 1.0f);                                     // row 3 of slot 0: the translation
    CHECK(r[4 * 3 + 2] == 3.0f);
    CHECK(std::memcmp(r + 80 * 4, &viewProj, sizeof(viewProj)) == 0);
    CHECK(r[88 * 4 + 1] == 0.5f);
    CHECK(r[95 * 4 + 1] == 0.25f);
    CHECK(r[95 * 4 + 2] == 1.5f);
    CHECK(r[96 * 4 + 0] == 3.0f);
    CHECK(std::memcmp(effect.Get().Registers(), r, sizeof(r)) == 0);

    // CommitChanges writes what was set since: a register changed behind the effect's back stays changed.
    const float junk[4] = {9, 9, 9, 9};
    REQUIRE(device.SetVertexShaderConstantF(88, junk, 1));
    REQUIRE(effect->SetFloat("g_fDensity", 0.125f) == D3D_OK);
    REQUIRE(effect->CommitChanges() == D3D_OK);
    REQUIRE(device.GetVertexShaderConstantF(0, r, x2::d3d9::kEffectRegisters));
    CHECK(r[96 * 4 + 1] == 0.125f);
    CHECK(r[88 * 4] == 9.0f);
    // ...and the next pass writes them all.
    effect->EndPass();
    REQUIRE(effect->BeginPass(0) == D3D_OK);
    REQUIRE(device.GetVertexShaderConstantF(0, r, x2::d3d9::kEffectRegisters));
    CHECK(r[88 * 4] == 0.25f);
    effect->EndPass();
    effect->End();
}

namespace {

struct SkinnedVertex {
    float x, y, z;
    float weights[3];
    std::uint8_t indices[4];
    float nx, ny, nz;
    float u, v;
};
static_assert(sizeof(SkinnedVertex) == 48);
constexpr DWORD kSkinnedFvf = D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1;

const x2::render::null::PushedUniform* Pushed(x2::render::Device& render, x2::render::ShaderStage stage,
                                              std::uint32_t slot, std::size_t nth = 0)
{
    for (const auto& uniform : x2::render::null::PushedUniforms(render)) {
        if (uniform.stage == stage && uniform.slot == slot && nth-- == 0) {
            return &uniform;
        }
    }
    return nullptr;
}

}  // namespace

TEST_CASE("d3dx: a skinned draw through an effect on the null backend")
{
    using x2::render::ShaderStage;
    auto render = NullRender();
    {
        x2::d3d9::Device device(*render, nullptr);
        REQUIRE(device.Ok());
        auto target = device.CreateTexture(32, 32, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
        REQUIRE(device.SetRenderTarget(0, target.get()));

        const SkinnedVertex vertices[3] = {
            {0, 0, 0, {0.5f, 0.25f, 0.25f}, {0, 1, 2, 0}, 0, 0, -1, 0, 0},
            {1, 0, 0, {1.0f, 0.0f, 0.0f}, {1, 0, 0, 0}, 0, 0, -1, 1, 0},
            {0, 1, 0, {0.0f, 1.0f, 0.0f}, {0, 2, 0, 0}, 0, 0, -1, 0, 1},
        };
        auto vb = device.CreateVertexBuffer(sizeof(vertices), 0, kSkinnedFvf, D3DPOOL_MANAGED);
        void* data = nullptr;
        REQUIRE(vb->Lock(0, 0, &data, 0));
        std::memcpy(data, vertices, sizeof(vertices));
        vb->Unlock();
        auto ib = device.CreateIndexBuffer(6, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED);
        REQUIRE(ib->Lock(0, 0, &data, 0));
        const std::uint16_t indices[3] = {0, 1, 2};
        std::memcpy(data, indices, sizeof(indices));
        ib->Unlock();

        EffectRef effect(device);
        const D3DXMATRIX palette[3] = {Translation(1, 0, 0), Translation(0, 1, 0), Translation(0, 0, 1)};
        effect->SetMatrixArray("g_amWorldPalette", palette, 3);
        const D3DXVECTOR4 color(1, 1, 1, 0.5f);
        effect->SetVector("g_Color", &color);
        effect->SetBool("g_bFog", TRUE);
        const D3DXVECTOR4 fogColor(0, 1, 0, 1);
        effect->SetVector("g_fColor", &fogColor);
        effect->SetInt("g_iShaderIndex", 4 + 5);    // four bones, fog

        // The key tells the effect program from fixed function.
        const x2::d3d9::PipelineKey fixedKey = device.MakePipelineKey(D3DPT_TRIANGLELIST, kSkinnedFvf, 48);

        x2::render::null::ClearCommands(*render);
        REQUIRE(device.BeginScene());
        device.SetFVF(kSkinnedFvf);
        device.SetStreamSource(0, vb.get(), 0, sizeof(SkinnedVertex));
        device.SetIndices(ib.get());
        UINT passes = 0;
        REQUIRE(effect->SetTechnique("REAL_COLOR") == D3D_OK);
        REQUIRE(effect->Begin(&passes, 0) == D3D_OK);
        REQUIRE(effect->BeginPass(0) == D3D_OK);
        CHECK_FALSE(device.MakePipelineKey(D3DPT_TRIANGLELIST, kSkinnedFvf, 48) == fixedKey);
        REQUIRE_MESSAGE(device.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1), device.LastError());
        effect->EndPass();
        effect->End();
        // Back to fixed function, as the state manager restores it.
        device.SetVertexShader(nullptr);
        REQUIRE(device.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1));
        device.EndScene();
        REQUIRE(device.Present());
        CHECK(device.PipelineCount() == 2);

        const auto* registers = Pushed(*render, ShaderStage::Vertex, 0);
        REQUIRE(registers != nullptr);
        REQUIRE(registers->bytes.size() == x2::d3d9::kEffectRegisters * 16);
        CHECK(std::memcmp(registers->bytes.data(), palette, sizeof(palette)) == 0);
        float g[4];
        std::memcpy(g, registers->bytes.data() + 88 * 16, sizeof(g));
        CHECK(g[3] == 0.5f);
        const auto* program = Pushed(*render, ShaderStage::Vertex, 1);
        REQUIRE(program != nullptr);
        REQUIRE(program->bytes.size() == 32);
        std::int32_t p[4];
        std::memcpy(p, program->bytes.data(), sizeof(p));
        CHECK(p[0] == x2::d3d9::EffectProgram::RealColor);
        CHECK(p[1] == 4);
        CHECK(p[2] == 1);
        float pixelOffset[2];
        std::memcpy(pixelOffset, program->bytes.data() + 16, sizeof(pixelOffset));
        CHECK(pixelOffset[0] == doctest::Approx(1.0f / 32));
        CHECK(pixelOffset[1] == doctest::Approx(-1.0f / 32));
        // The fragment block: fog on, in the effect's color.
        const auto* fragment = Pushed(*render, ShaderStage::Fragment, 0);
        REQUIRE(fragment != nullptr);
        REQUIRE(fragment->bytes.size() == 160);
        float fog[4];
        std::memcpy(fog, fragment->bytes.data() + 144, sizeof(fog));
        CHECK(fog[0] == 0.0f);
        CHECK(fog[1] == 1.0f);
        CHECK(fog[3] == 1.0f);
        // The fixed-function draw after it pushes its own 112-byte block.
        const auto* fixed = Pushed(*render, ShaderStage::Vertex, 0, 1);
        REQUIRE(fixed != nullptr);
        CHECK(fixed->bytes.size() == 112);
        CHECK(Pushed(*render, ShaderStage::Vertex, 1, 1) == nullptr);
    }
    CHECK(x2::render::null::LiveResourceCount(*render) == 0);
}
