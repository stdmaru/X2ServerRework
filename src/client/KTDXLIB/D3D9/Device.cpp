// KTDXLIB's D3D9 device on x2::render (decision T38). See Device.h and docs/porting/client-d3d9.md.

#include "Device.h"

#include "Fvf.h"
#include "GeneratedShader.h"

#include <SDL3/SDL_video.h>

#include <algorithm>
#include <bit>
#include <cstring>

namespace x2::d3d9 {
namespace {

#include "shaders/generated/Shaders.inc"

constexpr std::uint32_t kUploadChunkSize = 4u << 20;
constexpr std::uint32_t kUploadFlushThreshold = 64u << 20;     // flush staged uploads past this, even mid-frame
constexpr std::uint32_t kDynamicVertexBytes = 4u << 20;        // DrawPrimitiveUP ring
constexpr std::uint32_t kMaxFanTriangles = 16384;

std::uint32_t AlignUp(std::uint32_t value, std::uint32_t alignment)
{
    return (value + alignment - 1) / alignment * alignment;
}

DWORD FloatBits(float value)
{
    return std::bit_cast<DWORD>(value);
}

float BitsFloat(DWORD value)
{
    return std::bit_cast<float>(value);
}

D3DMATRIX Identity()
{
    D3DMATRIX m{};
    m._11 = m._22 = m._33 = m._44 = 1.0f;
    return m;
}

render::Color ColorFromD3D(D3DCOLOR color)
{
    return {static_cast<float>((color >> 16) & 0xFF) / 255.0f, static_cast<float>((color >> 8) & 0xFF) / 255.0f,
            static_cast<float>(color & 0xFF) / 255.0f, static_cast<float>((color >> 24) & 0xFF) / 255.0f};
}

render::CompareOp Compare(DWORD func)
{
    switch (func) {
    case D3DCMP_NEVER: return render::CompareOp::Never;
    case D3DCMP_LESS: return render::CompareOp::Less;
    case D3DCMP_EQUAL: return render::CompareOp::Equal;
    case D3DCMP_LESSEQUAL: return render::CompareOp::LessOrEqual;
    case D3DCMP_GREATER: return render::CompareOp::Greater;
    case D3DCMP_NOTEQUAL: return render::CompareOp::NotEqual;
    case D3DCMP_GREATEREQUAL: return render::CompareOp::GreaterOrEqual;
    default: return render::CompareOp::Always;
    }
}

render::StencilOp Stencil(DWORD op)
{
    switch (op) {
    case D3DSTENCILOP_ZERO: return render::StencilOp::Zero;
    case D3DSTENCILOP_REPLACE: return render::StencilOp::Replace;
    case D3DSTENCILOP_INCRSAT: return render::StencilOp::IncrementAndClamp;
    case D3DSTENCILOP_DECRSAT: return render::StencilOp::DecrementAndClamp;
    case D3DSTENCILOP_INVERT: return render::StencilOp::Invert;
    case D3DSTENCILOP_INCR: return render::StencilOp::IncrementAndWrap;
    case D3DSTENCILOP_DECR: return render::StencilOp::DecrementAndWrap;
    default: return render::StencilOp::Keep;
    }
}

render::BlendFactor Blend(DWORD blend)
{
    switch (blend) {
    case D3DBLEND_ZERO: return render::BlendFactor::Zero;
    case D3DBLEND_SRCCOLOR: return render::BlendFactor::SrcColor;
    case D3DBLEND_INVSRCCOLOR: return render::BlendFactor::OneMinusSrcColor;
    case D3DBLEND_SRCALPHA: return render::BlendFactor::SrcAlpha;
    case D3DBLEND_INVSRCALPHA: return render::BlendFactor::OneMinusSrcAlpha;
    case D3DBLEND_DESTALPHA: return render::BlendFactor::DstAlpha;
    case D3DBLEND_INVDESTALPHA: return render::BlendFactor::OneMinusDstAlpha;
    case D3DBLEND_DESTCOLOR: return render::BlendFactor::DstColor;
    case D3DBLEND_INVDESTCOLOR: return render::BlendFactor::OneMinusDstColor;
    case D3DBLEND_SRCALPHASAT: return render::BlendFactor::SrcAlphaSaturate;
    case D3DBLEND_BLENDFACTOR: return render::BlendFactor::ConstantColor;
    case D3DBLEND_INVBLENDFACTOR: return render::BlendFactor::OneMinusConstantColor;
    default: return render::BlendFactor::One;
    }
}

render::BlendOp BlendOperation(DWORD op)
{
    switch (op) {
    case D3DBLENDOP_SUBTRACT: return render::BlendOp::Subtract;
    case D3DBLENDOP_REVSUBTRACT: return render::BlendOp::ReverseSubtract;
    case D3DBLENDOP_MIN: return render::BlendOp::Min;
    case D3DBLENDOP_MAX: return render::BlendOp::Max;
    default: return render::BlendOp::Add;
    }
}

// D3DBLEND_BOTHSRCALPHA / BOTHINVSRCALPHA set the destination factor too (D3D9 ignores DESTBLEND then).
void BlendPair(DWORD src, DWORD dst, render::BlendFactor& srcFactor, render::BlendFactor& dstFactor)
{
    if (src == D3DBLEND_BOTHSRCALPHA) {
        srcFactor = render::BlendFactor::SrcAlpha;
        dstFactor = render::BlendFactor::OneMinusSrcAlpha;
    } else if (src == D3DBLEND_BOTHINVSRCALPHA) {
        srcFactor = render::BlendFactor::OneMinusSrcAlpha;
        dstFactor = render::BlendFactor::SrcAlpha;
    } else {
        srcFactor = Blend(src);
        dstFactor = Blend(dst);
    }
}

render::AddressMode Address(DWORD address)
{
    switch (address) {
    case D3DTADDRESS_MIRROR:
    case D3DTADDRESS_MIRRORONCE: return render::AddressMode::MirroredRepeat;     // mirror-once: no SDL_GPU mode
    case D3DTADDRESS_CLAMP:
    case D3DTADDRESS_BORDER: return render::AddressMode::ClampToEdge;            // border: no SDL_GPU mode
    default: return render::AddressMode::Repeat;
    }
}

render::Filter TextureFilter(DWORD filter)
{
    return filter == D3DTEXF_NONE || filter == D3DTEXF_POINT ? render::Filter::Nearest : render::Filter::Linear;
}

struct PrimitiveShape {
    render::PrimitiveType type;
    std::uint32_t vertexCount;
    bool fan;
};

bool Shape(D3DPRIMITIVETYPE type, UINT count, PrimitiveShape& shape)
{
    switch (type) {
    case D3DPT_POINTLIST: shape = {render::PrimitiveType::PointList, count, false}; return true;
    case D3DPT_LINELIST: shape = {render::PrimitiveType::LineList, count * 2, false}; return true;
    case D3DPT_LINESTRIP: shape = {render::PrimitiveType::LineStrip, count + 1, false}; return true;
    case D3DPT_TRIANGLELIST: shape = {render::PrimitiveType::TriangleList, count * 3, false}; return true;
    case D3DPT_TRIANGLESTRIP: shape = {render::PrimitiveType::TriangleStrip, count + 2, false}; return true;
    case D3DPT_TRIANGLEFAN: shape = {render::PrimitiveType::TriangleList, count + 2, true}; return true;
    default: return false;
    }
}

// Uniform blocks of the fixed-function shaders (FixedFunction.*.hlsl); 16-byte rows as in HLSL's cbuffer packing.
struct VertexUniforms {
    float worldViewProjection[16];
    float viewport[4];
    float depthRange[4];
    float pixelOffset[4];
};
static_assert(sizeof(VertexUniforms) == 112);

struct FragmentUniforms {
    std::int32_t colorStage[kTextureStages][4];
    std::int32_t alphaStage[kTextureStages][4];
    std::int32_t texCoordIndex[4];
    float textureFactor[4];
    float alphaTest[4];
    float fogColor[4];
};
static_assert(sizeof(FragmentUniforms) == 160);

// The effect vertex program's own block (XRenderer.vert.hlsl, slot 1; slot 0 is the registers).
struct EffectProgramUniforms {
    std::int32_t program[4];
    float pixelOffset[4];
};
static_assert(sizeof(EffectProgramUniforms) == 32);

// Words of PipelineKey.
enum KeyWord : std::size_t {
    kPrimitive, kFvf, kStride, kColorFormat, kDepthFormat, kCull, kFill,
    kZEnable, kZWrite, kZFunc, kStencilEnable, kStencilFail, kStencilZFail, kStencilPass, kStencilFunc,
    kStencilMask, kStencilWriteMask,
    kBlendEnable, kSrcBlend, kDestBlend, kBlendOp, kSeparateAlpha, kSrcBlendAlpha, kDestBlendAlpha, kBlendOpAlpha,
    kColorWrite, kDepthBias, kSlopeScaleDepthBias, kVertexProgram, kKeyWordCount,
};
static_assert(kKeyWordCount == std::tuple_size_v<decltype(PipelineKey::words)>);

}  // namespace

std::size_t PipelineKeyHash::operator()(const PipelineKey& key) const
{
    std::uint64_t h = 0xCBF29CE484222325ULL;
    for (std::uint32_t word : key.words) {
        h = (h ^ word) * 0x100000001B3ULL;
    }
    return static_cast<std::size_t>(h);
}

// --- Construction ---------------------------------------------------------------------------------------------------

Device::Device(render::Device& render, SDL_Window* window) : m_render(render), m_window(window)
{
    // D3D9's default states.
    auto& rs = m_renderStates;
    rs[D3DRS_ZENABLE] = D3DZB_TRUE;    // the device has a depth-stencil buffer (EnableAutoDepthStencil)
    rs[D3DRS_FILLMODE] = D3DFILL_SOLID;
    rs[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
    rs[D3DRS_ZWRITEENABLE] = TRUE;
    rs[D3DRS_LASTPIXEL] = TRUE;
    rs[D3DRS_SRCBLEND] = D3DBLEND_ONE;
    rs[D3DRS_DESTBLEND] = D3DBLEND_ZERO;
    rs[D3DRS_CULLMODE] = D3DCULL_CCW;
    rs[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL;
    rs[D3DRS_ALPHAFUNC] = D3DCMP_ALWAYS;
    rs[D3DRS_FOGEND] = FloatBits(1.0f);
    rs[D3DRS_FOGDENSITY] = FloatBits(1.0f);
    rs[D3DRS_STENCILFAIL] = rs[D3DRS_STENCILZFAIL] = rs[D3DRS_STENCILPASS] = D3DSTENCILOP_KEEP;
    rs[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
    rs[D3DRS_STENCILMASK] = rs[D3DRS_STENCILWRITEMASK] = 0xFFFFFFFF;
    rs[D3DRS_TEXTUREFACTOR] = 0xFFFFFFFF;
    rs[D3DRS_CLIPPING] = TRUE;
    rs[D3DRS_LIGHTING] = TRUE;
    rs[D3DRS_COLORVERTEX] = TRUE;
    rs[D3DRS_LOCALVIEWER] = TRUE;
    rs[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
    rs[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
    rs[D3DRS_POINTSIZE] = rs[D3DRS_POINTSIZE_MIN] = rs[D3DRS_POINTSCALE_A] = FloatBits(1.0f);
    rs[D3DRS_POINTSIZE_MAX] = FloatBits(64.0f);
    rs[D3DRS_MULTISAMPLEANTIALIAS] = TRUE;
    rs[D3DRS_MULTISAMPLEMASK] = 0xFFFFFFFF;
    rs[D3DRS_COLORWRITEENABLE] = rs[D3DRS_COLORWRITEENABLE1] = rs[D3DRS_COLORWRITEENABLE2] =
        rs[D3DRS_COLORWRITEENABLE3] = 0xF;
    rs[D3DRS_BLENDOP] = rs[D3DRS_BLENDOPALPHA] = D3DBLENDOP_ADD;
    rs[D3DRS_BLENDFACTOR] = 0xFFFFFFFF;
    rs[D3DRS_SRCBLENDALPHA] = D3DBLEND_ONE;
    rs[D3DRS_DESTBLENDALPHA] = D3DBLEND_ZERO;
    rs[D3DRS_CCW_STENCILFAIL] = rs[D3DRS_CCW_STENCILZFAIL] = rs[D3DRS_CCW_STENCILPASS] = D3DSTENCILOP_KEEP;
    rs[D3DRS_CCW_STENCILFUNC] = D3DCMP_ALWAYS;
    for (DWORD stage = 0; stage < kTextureStages; ++stage) {
        auto& ts = m_stageStates[stage];
        ts[D3DTSS_COLOROP] = stage == 0 ? D3DTOP_MODULATE : D3DTOP_DISABLE;
        ts[D3DTSS_ALPHAOP] = stage == 0 ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE;
        ts[D3DTSS_COLORARG1] = ts[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
        ts[D3DTSS_COLORARG2] = ts[D3DTSS_ALPHAARG2] = D3DTA_CURRENT;
        ts[D3DTSS_COLORARG0] = ts[D3DTSS_ALPHAARG0] = ts[D3DTSS_RESULTARG] = D3DTA_CURRENT;
        ts[D3DTSS_TEXCOORDINDEX] = stage;
        auto& ss = m_samplerStates[stage];
        ss[D3DSAMP_ADDRESSU] = ss[D3DSAMP_ADDRESSV] = ss[D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
        ss[D3DSAMP_MAGFILTER] = ss[D3DSAMP_MINFILTER] = D3DTEXF_POINT;
        ss[D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
        ss[D3DSAMP_MAXANISOTROPY] = 1;
    }
    m_world = m_view = m_projection = Identity();
    int width = 0, height = 0;
    if (m_window) {
        SDL_GetWindowSizeInPixels(m_window, &width, &height);
        m_backBufferFormat = m_render.SwapchainTextureFormat(m_window);
    }
    m_viewport = {0, 0, static_cast<DWORD>(width), static_cast<DWORD>(height), 0.0f, 1.0f};

    // Shaders: the first format the device takes, in the order SPIR-V, DXIL, MSL.
    const render::ShaderFormat formats = m_render.ShaderFormats();
    for (render::ShaderFormat f : {render::ShaderFormat::SpirV, render::ShaderFormat::Dxil, render::ShaderFormat::Msl}) {
        if (render::HasFlag(formats, f)) {
            m_shaderFormat = f;
            break;
        }
    }
    if (m_shaderFormat == render::ShaderFormat::None) {
        Fail("the render device takes none of SPIR-V, DXIL or MSL");
        return;
    }
    auto create = [&](const GeneratedShader& shader) {
        const std::span<const unsigned char> code = m_shaderFormat == render::ShaderFormat::SpirV ? shader.spirv
                                                    : m_shaderFormat == render::ShaderFormat::Dxil ? shader.dxil
                                                                                                   : shader.msl;
        return m_render.CreateShader({.code = std::as_bytes(code),
                                      .entryPoint = m_shaderFormat == render::ShaderFormat::Msl ? shader.mslEntryPoint
                                                                                                 : "main",
                                      .format = m_shaderFormat,
                                      .stage = shader.stage,
                                      .samplerCount = shader.samplerCount,
                                      .uniformBufferCount = shader.uniformBufferCount});
    };
    for (const GeneratedShader& shader : kFixedFunctionVertex) {
        m_vertexShaders.push_back(create(shader));
        if (!m_vertexShaders.back()) {
            Fail(std::string("vertex shader ") + shader.variant + ": " + m_render.LastError());
            return;
        }
    }
    for (const GeneratedShader& shader : kEffectVertex) {
        m_effectShaders.push_back(create(shader));
        if (!m_effectShaders.back()) {
            Fail(std::string("effect vertex shader ") + shader.variant + ": " + m_render.LastError());
            return;
        }
    }
    m_fragmentShader = create(kFixedFunctionFragment[0]);
    if (!m_fragmentShader) {
        Fail("fragment shader: " + m_render.LastError());
        return;
    }

    // A white texel for stages without a texture, the fan index buffer and the DrawPrimitiveUP ring.
    m_white = m_render.CreateTexture({.format = render::TextureFormat::B8G8R8A8Unorm, .width = 1, .height = 1,
                                      .name = "d3d9 white"});
    std::vector<std::uint16_t> fan;
    fan.reserve(kMaxFanTriangles * 3);
    for (std::uint32_t i = 1; i <= kMaxFanTriangles; ++i) {
        fan.insert(fan.end(), {0, static_cast<std::uint16_t>(i), static_cast<std::uint16_t>(i + 1)});
    }
    m_fanIndices = m_render.CreateBuffer({.usage = render::BufferUsage::Index,
                                          .size = static_cast<std::uint32_t>(fan.size() * sizeof(std::uint16_t)),
                                          .name = "d3d9 fan indices"});
    m_dynamicVertices = m_render.CreateBuffer({.usage = render::BufferUsage::Vertex, .size = kDynamicVertexBytes,
                                               .name = "d3d9 DrawPrimitiveUP"});
    if (!m_white || !m_fanIndices || !m_dynamicVertices) {
        Fail("device resources: " + m_render.LastError());
        return;
    }
    const std::uint8_t white[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    if (!UploadToTexture(m_white, 0, 1, 1, std::as_bytes(std::span(white)), false) ||
        !UploadToBuffer(m_fanIndices, 0, std::as_bytes(std::span(fan)), false) || !FlushUploads()) {
        return;
    }
    m_ok = true;
}

Device::~Device()
{
    if (m_frame) {
        EndPass();
        m_frame->Cancel();
    }
    FlushUploads();
    for (Texture* texture : m_liveTextures) {
        m_render.ReleaseTexture(texture->m_texture);
        texture->m_texture = {};
        texture->m_device = nullptr;
    }
    for (Buffer* buffer : m_liveBuffers) {
        m_render.ReleaseBuffer(buffer->m_buffer);
        buffer->m_buffer = {};
        buffer->m_device = nullptr;
    }
    for (VertexShader* shader : m_liveShaders) {
        shader->m_device = nullptr;
    }
    for (auto& [key, pipeline] : m_pipelines) {
        m_render.ReleaseGraphicsPipeline(pipeline);
    }
    for (auto& [key, sampler] : m_samplers) {
        m_render.ReleaseSampler(sampler);
    }
    for (render::Shader shader : m_vertexShaders) {
        m_render.ReleaseShader(shader);
    }
    for (render::Shader shader : m_effectShaders) {
        m_render.ReleaseShader(shader);
    }
    m_render.ReleaseShader(m_fragmentShader);
    for (UploadChunk& chunk : m_chunks) {
        m_render.ReleaseTransferBuffer(chunk.buffer);
    }
    m_render.ReleaseTexture(m_white);
    m_render.ReleaseTexture(m_depth);
    m_render.ReleaseBuffer(m_fanIndices);
    m_render.ReleaseBuffer(m_dynamicVertices);
}

bool Device::Fail(std::string message)
{
    m_error = "d3d9: " + std::move(message);
    return false;
}

// --- States ---------------------------------------------------------------------------------------------------------

void Device::SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
    if (static_cast<DWORD>(state) < X2_D3DRS_COUNT) {
        m_renderStates[state] = value;
    }
}

DWORD Device::GetRenderState(D3DRENDERSTATETYPE state) const
{
    return static_cast<DWORD>(state) < X2_D3DRS_COUNT ? m_renderStates[state] : 0;
}

void Device::SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
    if (stage < kTextureStages && static_cast<DWORD>(type) < X2_D3DTSS_COUNT) {
        m_stageStates[stage][type] = value;
    }
}

DWORD Device::GetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type) const
{
    if (stage < kTextureStages && static_cast<DWORD>(type) < X2_D3DTSS_COUNT) {
        return m_stageStates[stage][type];
    }
    // Stages the shader does not run read as D3D9's defaults for them.
    return type == D3DTSS_COLOROP || type == D3DTSS_ALPHAOP ? D3DTOP_DISABLE : 0;
}

void Device::SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value)
{
    if (sampler < kTextureStages && static_cast<DWORD>(type) < X2_D3DSAMP_COUNT) {
        m_samplerStates[sampler][type] = value;
    }
}

DWORD Device::GetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type) const
{
    if (sampler < kTextureStages && static_cast<DWORD>(type) < X2_D3DSAMP_COUNT) {
        return m_samplerStates[sampler][type];
    }
    return 0;
}

void Device::SetTexture(DWORD stage, Texture* texture)
{
    if (stage < kTextureStages) {
        m_textures[stage] = texture;
    }
}

Texture* Device::GetTexture(DWORD stage) const
{
    return stage < kTextureStages ? m_textures[stage] : nullptr;
}

void Device::SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix)
{
    if (!matrix) {
        return;
    }
    if (state == D3DTS_WORLD) {
        m_world = *matrix;
    } else if (state == D3DTS_VIEW) {
        m_view = *matrix;
    } else if (state == D3DTS_PROJECTION) {
        m_projection = *matrix;
    }
}

void Device::GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX* matrix) const
{
    if (!matrix) {
        return;
    }
    *matrix = state == D3DTS_WORLD ? m_world : state == D3DTS_VIEW ? m_view : state == D3DTS_PROJECTION ? m_projection
                                                                                                       : Identity();
}

void Device::SetViewport(const D3DVIEWPORT9* viewport)
{
    if (viewport) {
        m_viewport = *viewport;
    }
}

void Device::GetViewport(D3DVIEWPORT9* viewport) const
{
    if (viewport) {
        *viewport = m_viewport;
    }
}

void Device::SetFVF(DWORD fvf)
{
    m_fvf = fvf;
}

bool Device::SetVertexShaderConstantF(UINT startRegister, const float* data, UINT registerCount)
{
    if (data == nullptr || startRegister > kVertexShaderConstants || registerCount > kVertexShaderConstants - startRegister)
        return false;
    std::copy_n(data, registerCount * 4, m_vsConstants.begin() + startRegister * 4);
    return true;
}

bool Device::GetVertexShaderConstantF(UINT startRegister, float* data, UINT registerCount) const
{
    if (data == nullptr || startRegister > kVertexShaderConstants || registerCount > kVertexShaderConstants - startRegister)
        return false;
    std::copy_n(m_vsConstants.begin() + startRegister * 4, registerCount * 4, data);
    return true;
}

void Device::SetMaterial(const D3DMATERIAL9* material)
{
    if (material != nullptr) {
        m_material = *material;
    }
}

void Device::GetMaterial(D3DMATERIAL9* material) const
{
    if (material != nullptr) {
        *material = m_material;
    }
}

namespace {

// What D3D9 gives a light enabled before it was set: white, directional, along +z.
D3DLIGHT9 DefaultLight()
{
    D3DLIGHT9 light{};
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse = D3DCOLORVALUE{1.0f, 1.0f, 1.0f, 0.0f};
    light.Direction = D3DVECTOR{0.0f, 0.0f, 1.0f};
    return light;
}

constexpr DWORD kMaxLights = 1024;  // indices are sparse in D3D9; a bound keeps a bad index from allocating

}  // namespace

bool Device::SetLight(DWORD index, const D3DLIGHT9* light)
{
    if (light == nullptr || index >= kMaxLights) {
        return false;
    }
    if (index >= m_lights.size()) {
        m_lights.resize(index + 1, Light{DefaultLight(), false});
    }
    m_lights[index].light = *light;
    return true;
}

bool Device::GetLight(DWORD index, D3DLIGHT9* light) const
{
    if (light == nullptr || index >= m_lights.size()) {
        return false;
    }
    *light = m_lights[index].light;
    return true;
}

bool Device::LightEnable(DWORD index, BOOL enable)
{
    if (index >= kMaxLights) {
        return false;
    }
    if (index >= m_lights.size()) {
        m_lights.resize(index + 1, Light{DefaultLight(), false});
    }
    m_lights[index].enabled = enable != FALSE;
    return true;
}

bool Device::GetLightEnable(DWORD index, BOOL* enable) const
{
    if (enable == nullptr || index >= m_lights.size()) {
        return false;
    }
    *enable = m_lights[index].enabled ? TRUE : FALSE;
    return true;
}

void Device::SetVertexDeclaration(IDirect3DVertexDeclaration9* /*declaration*/) {}

void Device::GetVertexDeclaration(IDirect3DVertexDeclaration9** declaration) const
{
    if (declaration != nullptr) {
        *declaration = nullptr;
    }
}

std::unique_ptr<VertexShader> Device::CreateVertexShader(const EffectProgram& program)
{
    std::unique_ptr<VertexShader> shader(new VertexShader());
    shader->m_device = this;
    shader->m_program = program;
    m_liveShaders.insert(shader.get());
    return shader;
}

void Device::GetVertexShader(VertexShader** shader) const
{
    if (shader != nullptr) {
        *shader = m_vertexShader;
    }
}

void Device::ForgetVertexShader(VertexShader* shader)
{
    if (m_vertexShader == shader) {
        m_vertexShader = nullptr;
    }
    m_liveShaders.erase(shader);
}

VertexShader::~VertexShader()
{
    if (m_device) {
        m_device->ForgetVertexShader(this);
    }
}

void Device::SetPixelShader(IDirect3DPixelShader9* /*shader*/) {}

void Device::GetPixelShader(IDirect3DPixelShader9** shader) const
{
    if (shader != nullptr) {
        *shader = nullptr;
    }
}

UINT Device::GetAvailableTextureMem() const
{
    return 512u * 1024u * 1024u;
}

void Device::SetStreamSource(UINT stream, VertexBuffer* buffer, UINT offset, UINT stride)
{
    if (stream == 0) {
        m_stream = buffer;
        m_streamOffset = offset;
        m_streamStride = stride;
    }
}

bool Device::SetRenderTarget(DWORD index, Texture* target)
{
    if (index != 0) {
        return Fail("SetRenderTarget: only render target 0 exists");
    }
    if (target && !(target->m_usage & D3DUSAGE_RENDERTARGET)) {
        return Fail("SetRenderTarget: the texture was not created with D3DUSAGE_RENDERTARGET");
    }
    if (target != m_target) {
        EndPass();
        m_pendingClear = {};
        m_target = target;
    }
    const Target t = CurrentTarget();
    m_viewport = {0, 0, t.width, t.height, 0.0f, 1.0f};
    return true;
}

// --- Frame ----------------------------------------------------------------------------------------------------------

Device::Target Device::CurrentTarget() const
{
    if (m_target) {
        return {m_target->m_texture, m_target->m_renderFormat, m_target->m_width, m_target->m_height};
    }
    return {m_backBuffer.texture, m_backBufferFormat, m_backBuffer.width, m_backBuffer.height};
}

bool Device::EnsureFrame()
{
    if (m_frame) {
        return true;
    }
    m_frame = m_render.AcquireCommandBuffer();
    if (!m_frame) {
        return Fail("AcquireCommandBuffer: " + m_render.LastError());
    }
    const UINT oldWidth = m_backBuffer.width, oldHeight = m_backBuffer.height;
    m_backBuffer = {};
    if (m_window) {
        if (!m_frame->WaitAndAcquireSwapchainTexture(m_window, m_backBuffer)) {
            m_frame->Cancel();
            m_frame = nullptr;
            return Fail("swapchain: " + m_render.LastError());
        }
        m_backBufferFormat = m_render.SwapchainTextureFormat(m_window);
        // A viewport covering the old back buffer follows a resize (D3D9 resets it with the device).
        if (!m_target && m_backBuffer.texture && m_viewport.X == 0 && m_viewport.Y == 0 &&
            ((m_viewport.Width == oldWidth && m_viewport.Height == oldHeight) || m_viewport.Width == 0)) {
            m_viewport.Width = m_backBuffer.width;
            m_viewport.Height = m_backBuffer.height;
        }
        if (m_backBuffer.texture && (m_depthWidth != m_backBuffer.width || m_depthHeight != m_backBuffer.height)) {
            m_render.ReleaseTexture(m_depth);
            const render::TextureFormat depthFormat =
                m_render.TextureSupportsFormat(render::TextureFormat::D24UnormS8Uint, render::TextureType::Texture2D,
                                               render::TextureUsage::DepthStencilTarget)
                    ? render::TextureFormat::D24UnormS8Uint
                    : render::TextureFormat::D32FloatS8Uint;
            m_depth = m_render.CreateTexture({.format = depthFormat,
                                              .usage = render::TextureUsage::DepthStencilTarget,
                                              .width = m_backBuffer.width,
                                              .height = m_backBuffer.height,
                                              .name = "d3d9 depth-stencil"});
            m_depthFormat = m_depth ? depthFormat : render::TextureFormat::Invalid;
            m_depthWidth = m_depth ? m_backBuffer.width : 0;
            m_depthHeight = m_depth ? m_backBuffer.height : 0;
        }
    }
    return true;
}

render::RenderPass* Device::EnsurePass()
{
    if (m_pass) {
        return m_pass;
    }
    const Target target = CurrentTarget();
    if (!m_frame || !target.color) {
        return nullptr;     // nothing to draw into (minimized, no swapchain)
    }
    render::ColorTargetInfo color{.texture = target.color};
    if (m_pendingClear.color) {
        color.loadOp = render::LoadOp::Clear;
        color.clearColor = m_pendingClear.colorValue;
    }
    render::DepthStencilTargetInfo depth{.texture = m_depth};
    const bool withDepth = m_depth && target.width == m_depthWidth && target.height == m_depthHeight;
    if (withDepth) {
        depth.loadOp = m_pendingClear.depth ? render::LoadOp::Clear : render::LoadOp::Load;
        depth.clearDepth = m_pendingClear.depthValue;
        depth.stencilLoadOp = m_pendingClear.stencil ? render::LoadOp::Clear : render::LoadOp::Load;
        depth.stencilStoreOp = render::StoreOp::Store;
        depth.clearStencil = m_pendingClear.stencilValue;
    }
    m_pendingClear = {};
    m_pass = m_frame->BeginRenderPass({&color, 1}, withDepth ? &depth : nullptr);
    if (!m_pass) {
        Fail("BeginRenderPass: " + m_render.LastError());
    }
    return m_pass;
}

void Device::EndPass()
{
    if (m_pass) {
        m_pass->End();
        m_pass = nullptr;
    }
}

bool Device::BeginScene()
{
    if (m_inScene) {
        return Fail("BeginScene: already in a scene");
    }
    if (!EnsureFrame()) {
        return false;
    }
    m_inScene = true;
    return true;
}

bool Device::EndScene()
{
    if (!m_inScene) {
        return Fail("EndScene: not in a scene");
    }
    m_inScene = false;
    return true;
}

bool Device::Present()
{
    if (!m_frame) {
        if (!EnsureFrame()) {
            return false;
        }
        m_pendingClear.color = true;            // a frame with nothing drawn shows black
        m_pendingClear.colorValue = {0.0f, 0.0f, 0.0f, 1.0f};
    }
    if (m_pendingClear.color || m_pendingClear.depth || m_pendingClear.stencil) {
        EnsurePass();                           // apply the clear
    }
    return SubmitFrame();
}

bool Device::SubmitFrame()
{
    EndPass();
    const bool uploaded = FlushUploads();       // before the frame, whose draws read them
    const bool submitted = m_frame->Submit();
    m_frame = nullptr;
    m_backBuffer.texture = {};
    if (!submitted) {
        return Fail("Submit: " + m_render.LastError());
    }
    return uploaded;
}

bool Device::Clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil)
{
    if (!EnsureFrame()) {
        return false;
    }
    const Target target = CurrentTarget();
    const bool whole = (count == 0 || !rects) && m_viewport.X == 0 && m_viewport.Y == 0 &&
                       m_viewport.Width >= target.width && m_viewport.Height >= target.height;
    if (!whole) {
        return ClearRects(count, rects, flags, color, z, stencil);
    }
    EndPass();
    if (flags & D3DCLEAR_TARGET) {
        m_pendingClear.color = true;
        m_pendingClear.colorValue = ColorFromD3D(color);
    }
    if (flags & D3DCLEAR_ZBUFFER) {
        m_pendingClear.depth = true;
        m_pendingClear.depthValue = z;
    }
    if (flags & D3DCLEAR_STENCIL) {
        m_pendingClear.stencil = true;
        m_pendingClear.stencilValue = static_cast<std::uint8_t>(stencil);
    }
    return true;
}

// A clear of part of the target: quads drawn with states that write exactly the cleared values.
bool Device::ClearRects(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil)
{
    const auto savedRender = m_renderStates;
    const auto savedStages = m_stageStates;
    const bool savedInScene = m_inScene;
    VertexBuffer* const savedStream = m_stream;
    const UINT savedOffset = m_streamOffset, savedStride = m_streamStride;
    const DWORD savedFvf = m_fvf;

    auto& rs = m_renderStates;
    const bool depth = (flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) != 0;
    rs[D3DRS_ZENABLE] = depth ? D3DZB_TRUE : D3DZB_FALSE;
    rs[D3DRS_ZWRITEENABLE] = (flags & D3DCLEAR_ZBUFFER) ? TRUE : FALSE;
    rs[D3DRS_ZFUNC] = D3DCMP_ALWAYS;
    rs[D3DRS_STENCILENABLE] = (flags & D3DCLEAR_STENCIL) ? TRUE : FALSE;
    rs[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
    rs[D3DRS_STENCILPASS] = rs[D3DRS_STENCILFAIL] = rs[D3DRS_STENCILZFAIL] = D3DSTENCILOP_REPLACE;
    rs[D3DRS_STENCILREF] = stencil;
    rs[D3DRS_STENCILWRITEMASK] = 0xFF;
    rs[D3DRS_COLORWRITEENABLE] = (flags & D3DCLEAR_TARGET) ? 0xF : 0;
    rs[D3DRS_ALPHABLENDENABLE] = rs[D3DRS_ALPHATESTENABLE] = FALSE;
    rs[D3DRS_CULLMODE] = D3DCULL_NONE;
    rs[D3DRS_FILLMODE] = D3DFILL_SOLID;
    rs[D3DRS_DEPTHBIAS] = rs[D3DRS_SLOPESCALEDEPTHBIAS] = 0;
    rs[D3DRS_FOGENABLE] = FALSE;
    VertexShader* const savedShader = m_vertexShader;
    m_vertexShader = nullptr;   // a clear is fixed function
    m_stageStates[0][D3DTSS_COLOROP] = m_stageStates[0][D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;
    m_stageStates[0][D3DTSS_COLORARG1] = m_stageStates[0][D3DTSS_ALPHAARG1] = D3DTA_DIFFUSE;
    m_stageStates[1][D3DTSS_COLOROP] = m_stageStates[1][D3DTSS_ALPHAOP] = D3DTOP_DISABLE;
    m_inScene = true;   // D3D9 clears outside scenes too

    struct ClearVertex {
        float x, y, z, rhw;
        D3DCOLOR color;
    };
    const D3DRECT whole{static_cast<LONG>(m_viewport.X), static_cast<LONG>(m_viewport.Y),
                        static_cast<LONG>(m_viewport.X + m_viewport.Width),
                        static_cast<LONG>(m_viewport.Y + m_viewport.Height)};
    // In D3D9 the clear is clipped to the viewport, and z is the depth value itself (not mapped by MinZ/MaxZ).
    const D3DVIEWPORT9 savedViewport = m_viewport;
    m_viewport.MinZ = 0.0f;
    m_viewport.MaxZ = 1.0f;
    bool ok = true;
    const DWORD n = (count == 0 || !rects) ? 1 : count;
    for (DWORD i = 0; i < n && ok; ++i) {
        const D3DRECT r = (count == 0 || !rects) ? whole : rects[i];
        const float x1 = static_cast<float>(std::max(r.x1, whole.x1)) - 0.5f;
        const float y1 = static_cast<float>(std::max(r.y1, whole.y1)) - 0.5f;
        const float x2 = static_cast<float>(std::min(r.x2, whole.x2)) - 0.5f;
        const float y2 = static_cast<float>(std::min(r.y2, whole.y2)) - 0.5f;
        if (x2 <= x1 || y2 <= y1) {
            continue;
        }
        const ClearVertex quad[4] = {{x1, y1, z, 1.0f, color}, {x2, y1, z, 1.0f, color},
                                     {x1, y2, z, 1.0f, color}, {x2, y2, z, 1.0f, color}};
        m_fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
        ok = DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ClearVertex));
    }
    m_viewport = savedViewport;
    m_renderStates = savedRender;
    m_stageStates = savedStages;
    m_inScene = savedInScene;
    m_stream = savedStream;
    m_streamOffset = savedOffset;
    m_streamStride = savedStride;
    m_fvf = savedFvf;
    m_vertexShader = savedShader;
    return ok;
}

// --- Draws ----------------------------------------------------------------------------------------------------------

PipelineKey Device::MakePipelineKey(D3DPRIMITIVETYPE type, DWORD fvf, UINT stride) const
{
    const auto& rs = m_renderStates;
    PipelineKey key;
    auto& w = key.words;
    PrimitiveShape shape{};
    Shape(type, 1, shape);
    const Target target = CurrentTarget();
    const bool withDepth = m_depth && target.width == m_depthWidth && target.height == m_depthHeight;
    w[kPrimitive] = static_cast<std::uint32_t>(shape.type);
    w[kFvf] = fvf;
    w[kStride] = stride;
    w[kColorFormat] = static_cast<std::uint32_t>(target.colorFormat);
    w[kDepthFormat] = withDepth ? static_cast<std::uint32_t>(m_depthFormat) : 0;
    w[kCull] = rs[D3DRS_CULLMODE];
    w[kFill] = rs[D3DRS_FILLMODE] == D3DFILL_WIREFRAME ? D3DFILL_WIREFRAME : D3DFILL_SOLID;
    if (withDepth) {
        w[kZEnable] = rs[D3DRS_ZENABLE] != D3DZB_FALSE;
        w[kZWrite] = w[kZEnable] && rs[D3DRS_ZWRITEENABLE];
        w[kZFunc] = w[kZEnable] ? rs[D3DRS_ZFUNC] : 0;
        w[kStencilEnable] = rs[D3DRS_STENCILENABLE] != FALSE;
        if (w[kStencilEnable]) {
            w[kStencilFail] = rs[D3DRS_STENCILFAIL];
            w[kStencilZFail] = rs[D3DRS_STENCILZFAIL];
            w[kStencilPass] = rs[D3DRS_STENCILPASS];
            w[kStencilFunc] = rs[D3DRS_STENCILFUNC];
            w[kStencilMask] = rs[D3DRS_STENCILMASK] & 0xFF;
            w[kStencilWriteMask] = rs[D3DRS_STENCILWRITEMASK] & 0xFF;
        }
        w[kDepthBias] = rs[D3DRS_DEPTHBIAS];
        w[kSlopeScaleDepthBias] = rs[D3DRS_SLOPESCALEDEPTHBIAS];
    }
    w[kBlendEnable] = rs[D3DRS_ALPHABLENDENABLE] != FALSE;
    if (w[kBlendEnable]) {
        w[kSrcBlend] = rs[D3DRS_SRCBLEND];
        w[kDestBlend] = rs[D3DRS_DESTBLEND];
        w[kBlendOp] = rs[D3DRS_BLENDOP];
        w[kSeparateAlpha] = rs[D3DRS_SEPARATEALPHABLENDENABLE] != FALSE;
        if (w[kSeparateAlpha]) {
            w[kSrcBlendAlpha] = rs[D3DRS_SRCBLENDALPHA];
            w[kDestBlendAlpha] = rs[D3DRS_DESTBLENDALPHA];
            w[kBlendOpAlpha] = rs[D3DRS_BLENDOPALPHA];
        }
    }
    w[kColorWrite] = rs[D3DRS_COLORWRITEENABLE] & 0xF;
    w[kVertexProgram] = m_vertexShader ? 1 : 0;
    return key;
}

render::GraphicsPipeline Device::Pipeline(const PipelineKey& key, DWORD fvf, UINT stride)
{
    if (auto it = m_pipelines.find(key); it != m_pipelines.end()) {
        return it->second;
    }
    const auto& w = key.words;
    const FvfLayout layout = DescribeFvf(fvf);
    const bool effect = w[kVertexProgram] != 0;
    std::vector<render::VertexAttribute> attributes;
    if (effect) {
        // The elements XRenderer.vert.hlsl's variant for this layout reads (EffectShaderIndex).
        attributes.push_back({0, 0, render::VertexElementFormat::Float3, 0});
        if (layout.texCount > 0) {
            attributes.push_back({2, 0, layout.texFormat[0], static_cast<std::uint32_t>(layout.texOffset[0])});
        }
        if (HasBlending(layout)) {
            constexpr render::VertexElementFormat weights[] = {render::VertexElementFormat::Float,
                                                               render::VertexElementFormat::Float2,
                                                               render::VertexElementFormat::Float3};
            attributes.push_back({4, 0, weights[std::min(layout.weightCount, 3) - 1], 12});
            attributes.push_back({5, 0, render::VertexElementFormat::UByte4,
                                  static_cast<std::uint32_t>(layout.indicesOffset)});
        }
        if (layout.normalOffset >= 0) {
            attributes.push_back({6, 0, render::VertexElementFormat::Float3,
                                  static_cast<std::uint32_t>(layout.normalOffset)});
        }
    } else {
        attributes.push_back({0, 0, layout.rhw ? render::VertexElementFormat::Float4 : render::VertexElementFormat::Float3,
                              0});
        if (layout.diffuseOffset >= 0) {
            attributes.push_back({1, 0, render::VertexElementFormat::UByte4Norm,
                                  static_cast<std::uint32_t>(layout.diffuseOffset)});
        }
        for (int i = 0; i < std::min(layout.texCount, 2); ++i) {
            attributes.push_back({static_cast<std::uint32_t>(2 + i), 0, layout.texFormat[i],
                                  static_cast<std::uint32_t>(layout.texOffset[i])});
        }
    }
    const render::VertexBufferDesc buffer{.slot = 0, .pitch = stride};

    render::GraphicsPipelineDesc desc;
    desc.vertexShader = effect ? m_effectShaders[EffectShaderIndex(layout)] : m_vertexShaders[VertexShaderIndex(layout)];
    desc.fragmentShader = m_fragmentShader;
    desc.vertexBuffers = {&buffer, 1};
    desc.vertexAttributes = attributes;
    desc.primitiveType = static_cast<render::PrimitiveType>(w[kPrimitive]);
    // D3D9's front faces are clockwise; D3DCULL_CCW (the default) culls counterclockwise triangles.
    desc.rasterizer.frontFace = render::FrontFace::Clockwise;
    desc.rasterizer.cullMode = w[kCull] == D3DCULL_CCW ? render::CullMode::Back
                             : w[kCull] == D3DCULL_CW  ? render::CullMode::Front
                                                       : render::CullMode::None;
    desc.rasterizer.fillMode = w[kFill] == D3DFILL_WIREFRAME ? render::FillMode::Line : render::FillMode::Fill;
    const float depthBias = BitsFloat(w[kDepthBias]);
    const float slopeBias = BitsFloat(w[kSlopeScaleDepthBias]);
    if (depthBias != 0.0f || slopeBias != 0.0f) {
        // D3D9's bias is in depth units; SDL_GPU's constant factor counts steps of a 24-bit buffer.
        desc.rasterizer.enableDepthBias = true;
        desc.rasterizer.depthBiasConstantFactor = depthBias * 16777216.0f;
        desc.rasterizer.depthBiasSlopeFactor = slopeBias;
    }
    desc.depthStencilFormat = static_cast<render::TextureFormat>(w[kDepthFormat]);
    if (w[kDepthFormat] != 0) {
        auto& ds = desc.depthStencil;
        ds.enableDepthTest = w[kZEnable] != 0;
        ds.enableDepthWrite = w[kZWrite] != 0;
        ds.compareOp = Compare(w[kZFunc]);
        ds.enableStencilTest = w[kStencilEnable] != 0;
        if (ds.enableStencilTest) {
            const render::StencilOpState op{Stencil(w[kStencilFail]), Stencil(w[kStencilPass]),
                                            Stencil(w[kStencilZFail]), Compare(w[kStencilFunc])};
            ds.frontStencil = ds.backStencil = op;
            ds.compareMask = static_cast<std::uint8_t>(w[kStencilMask]);
            ds.writeMask = static_cast<std::uint8_t>(w[kStencilWriteMask]);
        }
    }
    render::ColorTargetDesc color{.format = static_cast<render::TextureFormat>(w[kColorFormat])};
    color.blend.writeMask = static_cast<render::ColorComponent>(w[kColorWrite]);
    if (w[kBlendEnable]) {
        auto& b = color.blend;
        b.enableBlend = true;
        BlendPair(w[kSrcBlend], w[kDestBlend], b.srcColor, b.dstColor);
        b.colorOp = BlendOperation(w[kBlendOp]);
        if (w[kSeparateAlpha]) {
            BlendPair(w[kSrcBlendAlpha], w[kDestBlendAlpha], b.srcAlpha, b.dstAlpha);
            b.alphaOp = BlendOperation(w[kBlendOpAlpha]);
        } else {
            b.srcAlpha = b.srcColor;
            b.dstAlpha = b.dstColor;
            b.alphaOp = b.colorOp;
        }
    }
    desc.colorTargets = {&color, 1};
    desc.name = effect ? "d3d9 effect" : "d3d9 fixed function";

    const render::GraphicsPipeline pipeline = m_render.CreateGraphicsPipeline(desc);
    if (!pipeline) {
        Fail("CreateGraphicsPipeline: " + m_render.LastError());
        return {};
    }
    m_pipelines.emplace(key, pipeline);
    return pipeline;
}

render::Sampler Device::SamplerFor(DWORD stage)
{
    const auto& ss = m_samplerStates[stage];
    std::uint64_t key = 0xCBF29CE484222325ULL;
    for (D3DSAMPLERSTATETYPE type : {D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV, D3DSAMP_ADDRESSW, D3DSAMP_MAGFILTER,
                                     D3DSAMP_MINFILTER, D3DSAMP_MIPFILTER, D3DSAMP_MIPMAPLODBIAS, D3DSAMP_MAXMIPLEVEL,
                                     D3DSAMP_MAXANISOTROPY}) {
        key = (key ^ ss[type]) * 0x100000001B3ULL;
    }
    if (auto it = m_samplers.find(key); it != m_samplers.end()) {
        return it->second;
    }
    render::SamplerDesc desc;
    desc.minFilter = TextureFilter(ss[D3DSAMP_MINFILTER]);
    desc.magFilter = TextureFilter(ss[D3DSAMP_MAGFILTER]);
    desc.mipmapMode = ss[D3DSAMP_MIPFILTER] == D3DTEXF_LINEAR ? render::MipmapMode::Linear : render::MipmapMode::Nearest;
    desc.addressU = Address(ss[D3DSAMP_ADDRESSU]);
    desc.addressV = Address(ss[D3DSAMP_ADDRESSV]);
    desc.addressW = Address(ss[D3DSAMP_ADDRESSW]);
    desc.mipLodBias = BitsFloat(ss[D3DSAMP_MIPMAPLODBIAS]);
    if (ss[D3DSAMP_MINFILTER] == D3DTEXF_ANISOTROPIC || ss[D3DSAMP_MAGFILTER] == D3DTEXF_ANISOTROPIC) {
        desc.maxAnisotropy = static_cast<float>(std::max<DWORD>(1, ss[D3DSAMP_MAXANISOTROPY]));
    }
    // D3DSAMP_MAXMIPLEVEL is the largest level used (0 is the biggest); D3DTEXF_NONE samples that level only.
    desc.minLod = static_cast<float>(ss[D3DSAMP_MAXMIPLEVEL]);
    desc.maxLod = ss[D3DSAMP_MIPFILTER] == D3DTEXF_NONE ? desc.minLod : 1000.0f;
    const render::Sampler sampler = m_render.CreateSampler(desc);
    if (sampler) {
        m_samplers.emplace(key, sampler);
    } else {
        Fail("CreateSampler: " + m_render.LastError());
    }
    return sampler;
}

void Device::PushUniforms(DWORD fvf)
{
    const float width = static_cast<float>(std::max<DWORD>(1, m_viewport.Width));
    const float height = static_cast<float>(std::max<DWORD>(1, m_viewport.Height));
    if (m_vertexShader) {
        const EffectProgram& p = m_vertexShader->m_program;
        const EffectProgramUniforms program{{p.kind, p.bones, p.fog ? 1 : 0, p.axis},
                                            {1.0f / width, -1.0f / height, 0.0f, 0.0f}};
        m_frame->PushVertexUniformData(
            0, std::as_bytes(std::span(m_vsConstants.data(), static_cast<std::size_t>(kEffectRegisters) * 4)));
        m_frame->PushVertexUniformData(1, std::as_bytes(std::span(&program, 1)));
    } else {
        PushFixedFunctionVertexUniforms(width, height);
    }
    (void)fvf;

    FragmentUniforms f{};

    for (DWORD stage = 0; stage < kTextureStages; ++stage) {
        const auto& ts = m_stageStates[stage];
        f.colorStage[stage][0] = static_cast<std::int32_t>(ts[D3DTSS_COLOROP]);
        f.colorStage[stage][1] = static_cast<std::int32_t>(ts[D3DTSS_COLORARG1]);
        f.colorStage[stage][2] = static_cast<std::int32_t>(ts[D3DTSS_COLORARG2]);
        f.colorStage[stage][3] = static_cast<std::int32_t>(ts[D3DTSS_COLORARG0]);
        f.alphaStage[stage][0] = static_cast<std::int32_t>(ts[D3DTSS_ALPHAOP]);
        f.alphaStage[stage][1] = static_cast<std::int32_t>(ts[D3DTSS_ALPHAARG1]);
        f.alphaStage[stage][2] = static_cast<std::int32_t>(ts[D3DTSS_ALPHAARG2]);
        f.alphaStage[stage][3] = static_cast<std::int32_t>(ts[D3DTSS_ALPHAARG0]);
        f.texCoordIndex[stage] = static_cast<std::int32_t>(std::min<DWORD>(ts[D3DTSS_TEXCOORDINDEX] & 0xFFFF, 2));
    }
    const render::Color factor = ColorFromD3D(m_renderStates[D3DRS_TEXTUREFACTOR]);
    f.textureFactor[0] = factor.r;
    f.textureFactor[1] = factor.g;
    f.textureFactor[2] = factor.b;
    f.textureFactor[3] = factor.a;
    f.alphaTest[0] = static_cast<float>(m_renderStates[D3DRS_ALPHATESTENABLE] ? m_renderStates[D3DRS_ALPHAFUNC]
                                                                               : DWORD{D3DCMP_ALWAYS});
    f.alphaTest[1] = static_cast<float>(m_renderStates[D3DRS_ALPHAREF] & 0xFF);
    const render::Color fog = ColorFromD3D(m_renderStates[D3DRS_FOGCOLOR]);
    f.fogColor[0] = fog.r;
    f.fogColor[1] = fog.g;
    f.fogColor[2] = fog.b;
    f.fogColor[3] = m_renderStates[D3DRS_FOGENABLE] ? 1.0f : 0.0f;
    m_frame->PushFragmentUniformData(0, std::as_bytes(std::span(&f, 1)));
}

void Device::PushFixedFunctionVertexUniforms(float width, float height)
{
    VertexUniforms v{};
    D3DXMATRIX worldView, worldViewProjection;
    const D3DXMATRIX world(m_world), view(m_view), projection(m_projection);
    D3DXMatrixMultiply(&worldView, &world, &view);
    D3DXMatrixMultiply(&worldViewProjection, &worldView, &projection);
    std::memcpy(v.worldViewProjection, &worldViewProjection, sizeof(v.worldViewProjection));
    v.viewport[0] = static_cast<float>(m_viewport.X);
    v.viewport[1] = static_cast<float>(m_viewport.Y);
    v.viewport[2] = width;
    v.viewport[3] = height;
    v.depthRange[0] = m_viewport.MinZ;
    v.depthRange[1] = m_viewport.MaxZ != m_viewport.MinZ ? 1.0f / (m_viewport.MaxZ - m_viewport.MinZ) : 1.0f;
    v.pixelOffset[0] = 1.0f / width;
    v.pixelOffset[1] = -1.0f / height;
    m_frame->PushVertexUniformData(0, std::as_bytes(std::span(&v, 1)));
}

bool Device::Draw(D3DPRIMITIVETYPE type, UINT primitiveCount, render::Buffer buffer, UINT offset, UINT stride,
                  INT firstVertex, DWORD fvf, const IndexBuffer* indices, UINT startIndex)
{
    PrimitiveShape shape{};
    if (!Shape(type, primitiveCount, shape)) {
        return Fail("draw: unknown primitive type " + std::to_string(static_cast<int>(type)));
    }
    if (shape.fan && primitiveCount > kMaxFanTriangles) {
        return Fail("draw: a triangle fan of more than " + std::to_string(kMaxFanTriangles) + " triangles");
    }
    if (!DescribeFvf(fvf).valid) {
        return Fail("draw: unsupported FVF " + std::to_string(fvf));
    }
    if (!CurrentTarget().color) {
        return true;        // nothing to draw into (minimized window): dropped
    }
    render::RenderPass* pass = EnsurePass();
    if (!pass) {
        return false;
    }
    const render::GraphicsPipeline pipeline = Pipeline(MakePipelineKey(type, fvf, stride), fvf, stride);
    if (!pipeline) {
        return false;
    }
    std::array<render::TextureSamplerBinding, kTextureStages> samplers;
    for (DWORD stage = 0; stage < kTextureStages; ++stage) {
        samplers[stage] = {m_textures[stage] && m_textures[stage]->m_texture ? m_textures[stage]->m_texture : m_white,
                           SamplerFor(stage)};
        if (!samplers[stage].sampler) {
            return false;
        }
    }
    PushUniforms(fvf);
    pass->BindGraphicsPipeline(pipeline);
    pass->SetViewport({static_cast<float>(m_viewport.X), static_cast<float>(m_viewport.Y),
                       static_cast<float>(m_viewport.Width), static_cast<float>(m_viewport.Height), m_viewport.MinZ,
                       m_viewport.MaxZ});
    pass->SetStencilReference(static_cast<std::uint8_t>(m_renderStates[D3DRS_STENCILREF]));
    pass->SetBlendConstants(ColorFromD3D(m_renderStates[D3DRS_BLENDFACTOR]));
    const render::BufferBinding vertices{buffer, offset};
    pass->BindVertexBuffers(0, {&vertices, 1});
    pass->BindFragmentSamplers(0, samplers);
    if (indices) {
        const bool wide = indices->m_format == D3DFMT_INDEX32;
        pass->BindIndexBuffer({indices->m_buffer, 0}, wide ? render::IndexElementSize::U32 : render::IndexElementSize::U16);
        pass->DrawIndexedPrimitives(shape.vertexCount, 1, startIndex, firstVertex, 0);
    } else if (shape.fan) {
        pass->BindIndexBuffer({m_fanIndices, 0}, render::IndexElementSize::U16);
        pass->DrawIndexedPrimitives(primitiveCount * 3, 1, 0, firstVertex, 0);
    } else {
        pass->DrawPrimitives(shape.vertexCount, 1, static_cast<std::uint32_t>(firstVertex), 0);
    }
    return true;
}

bool Device::DrawPrimitive(D3DPRIMITIVETYPE type, UINT startVertex, UINT primitiveCount)
{
    if (!m_inScene) {
        return Fail("DrawPrimitive outside BeginScene/EndScene");
    }
    if (!m_stream || !m_stream->m_buffer || m_streamStride == 0) {
        return Fail("DrawPrimitive: no vertex buffer on stream 0");
    }
    return Draw(type, primitiveCount, m_stream->m_buffer, m_streamOffset, m_streamStride, static_cast<INT>(startVertex),
                m_fvf);
}

bool Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, INT baseVertexIndex, UINT /*minIndex*/,
                                  UINT /*numVertices*/, UINT startIndex, UINT primitiveCount)
{
    if (!m_inScene) {
        return Fail("DrawIndexedPrimitive outside BeginScene/EndScene");
    }
    if (!m_stream || !m_stream->m_buffer || m_streamStride == 0) {
        return Fail("DrawIndexedPrimitive: no vertex buffer on stream 0");
    }
    if (!m_indices || !m_indices->m_buffer) {
        return Fail("DrawIndexedPrimitive: no index buffer");
    }
    if (type == D3DPT_TRIANGLEFAN) {
        return Fail("DrawIndexedPrimitive: indexed triangle fans are not supported");
    }
    PrimitiveShape shape{};
    if (Shape(type, primitiveCount, shape)) {
        const UINT indexBytes = m_indices->m_format == D3DFMT_INDEX32 ? 4 : 2;
        if ((static_cast<std::uint64_t>(startIndex) + shape.vertexCount) * indexBytes > m_indices->Size()) {
            return Fail("DrawIndexedPrimitive: indices out of the index buffer");
        }
    }
    return Draw(type, primitiveCount, m_stream->m_buffer, m_streamOffset, m_streamStride, baseVertexIndex, m_fvf,
                m_indices, startIndex);
}

bool Device::DrawPrimitiveUP(D3DPRIMITIVETYPE type, UINT primitiveCount, const void* vertices, UINT stride)
{
    m_stream = nullptr;
    m_streamOffset = m_streamStride = 0;
    if (!m_inScene) {
        return Fail("DrawPrimitiveUP outside BeginScene/EndScene");
    }
    PrimitiveShape shape{};
    if (!Shape(type, primitiveCount, shape) || !vertices || stride == 0) {
        return Fail("DrawPrimitiveUP: bad arguments");
    }
    if (primitiveCount == 0) {
        return true;
    }
    const std::uint32_t size = shape.vertexCount * stride;
    if (size > kDynamicVertexBytes) {
        return Fail("DrawPrimitiveUP: " + std::to_string(size) + " bytes of vertices");
    }
    std::uint32_t offset = AlignUp(m_dynamicOffset, 16);
    bool cycle = false;
    if (offset + size > kDynamicVertexBytes) {
        offset = 0;
        cycle = true;       // draws recorded earlier keep the old contents
    }
    if (!UploadToBuffer(m_dynamicVertices, offset, {static_cast<const std::byte*>(vertices), size}, cycle)) {
        return false;
    }
    m_dynamicOffset = offset + size;
    return Draw(type, primitiveCount, m_dynamicVertices, offset, stride, 0, m_fvf);
}

// --- Uploads --------------------------------------------------------------------------------------------------------

render::CopyPass* Device::UploadPass()
{
    if (!m_uploads) {
        m_uploads = m_render.AcquireCommandBuffer();
        if (!m_uploads) {
            Fail("AcquireCommandBuffer: " + m_render.LastError());
            return nullptr;
        }
    }
    if (!m_uploadPass) {
        m_uploadPass = m_uploads->BeginCopyPass();
        if (!m_uploadPass) {
            Fail("BeginCopyPass: " + m_render.LastError());
        }
    }
    return m_uploadPass;
}

std::optional<render::TransferBufferLocation> Device::Stage(std::span<const std::byte> bytes)
{
    const std::uint32_t size = static_cast<std::uint32_t>(bytes.size());
    auto findRoom = [&]() -> UploadChunk* {
        for (UploadChunk& c : m_chunks) {
            if (AlignUp(c.used, 16) + size <= c.size) {
                return &c;
            }
        }
        return nullptr;
    };
    UploadChunk* chunk = findRoom();
    if (!chunk) {
        std::uint32_t staged = 0;
        for (const UploadChunk& c : m_chunks) {
            staged += c.used;
        }
        // Past the threshold, submit what is staged and reuse the chunks (uploads still run before the frame).
        if (staged > kUploadFlushThreshold) {
            if (!FlushUploads()) {
                return std::nullopt;
            }
            chunk = findRoom();
        }
    }
    if (!chunk) {
        const std::uint32_t chunkSize = std::max(kUploadChunkSize, AlignUp(size, 16));
        const render::TransferBuffer buffer =
            m_render.CreateTransferBuffer({.usage = render::TransferBufferUsage::Upload, .size = chunkSize});
        if (!buffer) {
            Fail("CreateTransferBuffer: " + m_render.LastError());
            return std::nullopt;
        }
        m_chunks.push_back({buffer, chunkSize, 0, false});
        chunk = &m_chunks.back();
    }
    const std::uint32_t offset = AlignUp(chunk->used, 16);
    // The first map in a batch cycles: the previous batch may still be reading the buffer.
    void* memory = m_render.MapTransferBuffer(chunk->buffer, !chunk->mappedThisBatch);
    if (!memory) {
        Fail("MapTransferBuffer: " + m_render.LastError());
        return std::nullopt;
    }
    std::memcpy(static_cast<std::byte*>(memory) + offset, bytes.data(), bytes.size());
    m_render.UnmapTransferBuffer(chunk->buffer);
    chunk->mappedThisBatch = true;
    chunk->used = offset + size;
    return render::TransferBufferLocation{chunk->buffer, offset};
}

bool Device::UploadToBuffer(render::Buffer buffer, std::uint32_t offset, std::span<const std::byte> bytes, bool cycle)
{
    const auto source = Stage(bytes);
    render::CopyPass* pass = source ? UploadPass() : nullptr;
    if (!pass) {
        return false;
    }
    pass->UploadToBuffer(*source, {buffer, offset, static_cast<std::uint32_t>(bytes.size())}, cycle);
    return true;
}

bool Device::UploadToTexture(render::Texture texture, std::uint32_t level, std::uint32_t width, std::uint32_t height,
                             std::span<const std::byte> bytes, bool cycle)
{
    const auto source = Stage(bytes);
    render::CopyPass* pass = source ? UploadPass() : nullptr;
    if (!pass) {
        return false;
    }
    pass->UploadToTexture({source->transferBuffer, source->offset, 0, 0},
                          {.texture = texture, .mipLevel = level, .w = width, .h = height, .d = 1}, cycle);
    return true;
}

bool Device::FlushUploads()
{
    if (!m_uploads) {
        return true;
    }
    if (m_uploadPass) {
        m_uploadPass->End();
        m_uploadPass = nullptr;
    }
    const bool submitted = m_uploads->Submit();
    m_uploads = nullptr;
    for (UploadChunk& chunk : m_chunks) {
        chunk.used = 0;
        chunk.mappedThisBatch = false;
    }
    return submitted || Fail("Submit (uploads): " + m_render.LastError());
}

void Device::ForgetTexture(Texture* texture)
{
    for (Texture*& bound : m_textures) {
        if (bound == texture) {
            bound = nullptr;
        }
    }
    if (m_target == texture) {
        SetRenderTarget(0, nullptr);
    }
    m_liveTextures.erase(texture);
}

void Device::ForgetBuffer(Buffer* buffer)
{
    if (m_stream == buffer) {
        m_stream = nullptr;
    }
    if (m_indices == buffer) {
        m_indices = nullptr;
    }
    m_liveBuffers.erase(buffer);
}

}  // namespace x2::d3d9
