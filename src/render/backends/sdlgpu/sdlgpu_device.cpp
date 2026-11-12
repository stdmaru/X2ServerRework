// The SDL_GPU backend (decisions T19, T36): a near 1:1 wrapper. Handles hold the SDL object pointers; enums are mapped by
// table. The only file besides the escape hatch (x2/render/sdlgpu.h) that uses SDL_GPU. See docs/porting/render.md.

#include "x2/render/sdlgpu.h"

#include "../../src/backends.h"

#include <SDL3/SDL.h>

#include <bit>
#include <mutex>
#include <vector>

namespace x2::render {

namespace {

template <typename T>
T* Ptr(std::uint64_t value)
{
    return reinterpret_cast<T*>(static_cast<std::uintptr_t>(value));
}

template <typename H, typename T>
H Wrap(T* pointer)
{
    return H{static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(pointer))};
}

SDL_GPUBuffer* Sdl(Buffer h) { return Ptr<SDL_GPUBuffer>(h.value); }
SDL_GPUTransferBuffer* Sdl(TransferBuffer h) { return Ptr<SDL_GPUTransferBuffer>(h.value); }
SDL_GPUTexture* Sdl(Texture h) { return Ptr<SDL_GPUTexture>(h.value); }
SDL_GPUSampler* Sdl(Sampler h) { return Ptr<SDL_GPUSampler>(h.value); }
SDL_GPUShader* Sdl(Shader h) { return Ptr<SDL_GPUShader>(h.value); }
SDL_GPUGraphicsPipeline* Sdl(GraphicsPipeline h) { return Ptr<SDL_GPUGraphicsPipeline>(h.value); }

// --- Enum mapping -----------------------------------------------------------------------------------------------------

SDL_GPUTextureFormat ToSdl(TextureFormat f)
{
    switch (f) {
    case TextureFormat::Invalid: return SDL_GPU_TEXTUREFORMAT_INVALID;
    case TextureFormat::A8Unorm: return SDL_GPU_TEXTUREFORMAT_A8_UNORM;
    case TextureFormat::R8Unorm: return SDL_GPU_TEXTUREFORMAT_R8_UNORM;
    case TextureFormat::R8G8Unorm: return SDL_GPU_TEXTUREFORMAT_R8G8_UNORM;
    case TextureFormat::R8G8B8A8Unorm: return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    case TextureFormat::R8G8B8A8UnormSrgb: return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB;
    case TextureFormat::B8G8R8A8Unorm: return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
    case TextureFormat::B8G8R8A8UnormSrgb: return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB;
    case TextureFormat::B5G6R5Unorm: return SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM;
    case TextureFormat::B5G5R5A1Unorm: return SDL_GPU_TEXTUREFORMAT_B5G5R5A1_UNORM;
    case TextureFormat::B4G4R4A4Unorm: return SDL_GPU_TEXTUREFORMAT_B4G4R4A4_UNORM;
    case TextureFormat::R16G16B16A16Float: return SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT;
    case TextureFormat::R32Float: return SDL_GPU_TEXTUREFORMAT_R32_FLOAT;
    case TextureFormat::Bc1Unorm: return SDL_GPU_TEXTUREFORMAT_BC1_RGBA_UNORM;
    case TextureFormat::Bc2Unorm: return SDL_GPU_TEXTUREFORMAT_BC2_RGBA_UNORM;
    case TextureFormat::Bc3Unorm: return SDL_GPU_TEXTUREFORMAT_BC3_RGBA_UNORM;
    case TextureFormat::D16Unorm: return SDL_GPU_TEXTUREFORMAT_D16_UNORM;
    case TextureFormat::D24Unorm: return SDL_GPU_TEXTUREFORMAT_D24_UNORM;
    case TextureFormat::D32Float: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    case TextureFormat::D24UnormS8Uint: return SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
    case TextureFormat::D32FloatS8Uint: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
    }
    return SDL_GPU_TEXTUREFORMAT_INVALID;
}

TextureFormat FromSdl(SDL_GPUTextureFormat f)
{
    for (int i = 0; i <= static_cast<int>(TextureFormat::D32FloatS8Uint); ++i) {
        const auto format = static_cast<TextureFormat>(i);
        if (ToSdl(format) == f) {
            return format;
        }
    }
    return TextureFormat::Invalid;
}

SDL_GPUShaderFormat ToSdl(ShaderFormat f)
{
    SDL_GPUShaderFormat out = SDL_GPU_SHADERFORMAT_INVALID;
    if (HasFlag(f, ShaderFormat::SpirV)) out |= SDL_GPU_SHADERFORMAT_SPIRV;
    if (HasFlag(f, ShaderFormat::Dxil)) out |= SDL_GPU_SHADERFORMAT_DXIL;
    if (HasFlag(f, ShaderFormat::Msl)) out |= SDL_GPU_SHADERFORMAT_MSL;
    return out;
}

ShaderFormat FromSdl(SDL_GPUShaderFormat f)
{
    ShaderFormat out = ShaderFormat::None;
    if (f & SDL_GPU_SHADERFORMAT_SPIRV) out = out | ShaderFormat::SpirV;
    if (f & SDL_GPU_SHADERFORMAT_DXIL) out = out | ShaderFormat::Dxil;
    if (f & SDL_GPU_SHADERFORMAT_MSL) out = out | ShaderFormat::Msl;
    return out;
}

SDL_GPUTextureType ToSdl(TextureType t)
{
    switch (t) {
    case TextureType::Texture2D: return SDL_GPU_TEXTURETYPE_2D;
    case TextureType::Texture2DArray: return SDL_GPU_TEXTURETYPE_2D_ARRAY;
    case TextureType::Texture3D: return SDL_GPU_TEXTURETYPE_3D;
    case TextureType::Cube: return SDL_GPU_TEXTURETYPE_CUBE;
    case TextureType::CubeArray: return SDL_GPU_TEXTURETYPE_CUBE_ARRAY;
    }
    return SDL_GPU_TEXTURETYPE_2D;
}

SDL_GPUTextureUsageFlags ToSdl(TextureUsage u)
{
    SDL_GPUTextureUsageFlags out = 0;
    if (HasFlag(u, TextureUsage::Sampler)) out |= SDL_GPU_TEXTUREUSAGE_SAMPLER;
    if (HasFlag(u, TextureUsage::ColorTarget)) out |= SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    if (HasFlag(u, TextureUsage::DepthStencilTarget)) out |= SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    return out;
}

SDL_GPUSampleCount ToSdl(SampleCount s)
{
    switch (s) {
    case SampleCount::X1: return SDL_GPU_SAMPLECOUNT_1;
    case SampleCount::X2: return SDL_GPU_SAMPLECOUNT_2;
    case SampleCount::X4: return SDL_GPU_SAMPLECOUNT_4;
    case SampleCount::X8: return SDL_GPU_SAMPLECOUNT_8;
    }
    return SDL_GPU_SAMPLECOUNT_1;
}

SDL_GPUBufferUsageFlags ToSdl(BufferUsage u)
{
    SDL_GPUBufferUsageFlags out = 0;
    if (HasFlag(u, BufferUsage::Vertex)) out |= SDL_GPU_BUFFERUSAGE_VERTEX;
    if (HasFlag(u, BufferUsage::Index)) out |= SDL_GPU_BUFFERUSAGE_INDEX;
    if (HasFlag(u, BufferUsage::Indirect)) out |= SDL_GPU_BUFFERUSAGE_INDIRECT;
    return out;
}

SDL_GPUVertexElementFormat ToSdl(VertexElementFormat f)
{
    switch (f) {
    case VertexElementFormat::Invalid: return SDL_GPU_VERTEXELEMENTFORMAT_INVALID;
    case VertexElementFormat::Int: return SDL_GPU_VERTEXELEMENTFORMAT_INT;
    case VertexElementFormat::Int2: return SDL_GPU_VERTEXELEMENTFORMAT_INT2;
    case VertexElementFormat::Int3: return SDL_GPU_VERTEXELEMENTFORMAT_INT3;
    case VertexElementFormat::Int4: return SDL_GPU_VERTEXELEMENTFORMAT_INT4;
    case VertexElementFormat::UInt: return SDL_GPU_VERTEXELEMENTFORMAT_UINT;
    case VertexElementFormat::UInt2: return SDL_GPU_VERTEXELEMENTFORMAT_UINT2;
    case VertexElementFormat::UInt3: return SDL_GPU_VERTEXELEMENTFORMAT_UINT3;
    case VertexElementFormat::UInt4: return SDL_GPU_VERTEXELEMENTFORMAT_UINT4;
    case VertexElementFormat::Float: return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
    case VertexElementFormat::Float2: return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    case VertexElementFormat::Float3: return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    case VertexElementFormat::Float4: return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    case VertexElementFormat::Byte2: return SDL_GPU_VERTEXELEMENTFORMAT_BYTE2;
    case VertexElementFormat::Byte4: return SDL_GPU_VERTEXELEMENTFORMAT_BYTE4;
    case VertexElementFormat::UByte2: return SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2;
    case VertexElementFormat::UByte4: return SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4;
    case VertexElementFormat::Byte2Norm: return SDL_GPU_VERTEXELEMENTFORMAT_BYTE2_NORM;
    case VertexElementFormat::Byte4Norm: return SDL_GPU_VERTEXELEMENTFORMAT_BYTE4_NORM;
    case VertexElementFormat::UByte2Norm: return SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2_NORM;
    case VertexElementFormat::UByte4Norm: return SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
    case VertexElementFormat::Short2: return SDL_GPU_VERTEXELEMENTFORMAT_SHORT2;
    case VertexElementFormat::Short4: return SDL_GPU_VERTEXELEMENTFORMAT_SHORT4;
    case VertexElementFormat::UShort2: return SDL_GPU_VERTEXELEMENTFORMAT_USHORT2;
    case VertexElementFormat::UShort4: return SDL_GPU_VERTEXELEMENTFORMAT_USHORT4;
    case VertexElementFormat::Short2Norm: return SDL_GPU_VERTEXELEMENTFORMAT_SHORT2_NORM;
    case VertexElementFormat::Short4Norm: return SDL_GPU_VERTEXELEMENTFORMAT_SHORT4_NORM;
    case VertexElementFormat::UShort2Norm: return SDL_GPU_VERTEXELEMENTFORMAT_USHORT2_NORM;
    case VertexElementFormat::UShort4Norm: return SDL_GPU_VERTEXELEMENTFORMAT_USHORT4_NORM;
    case VertexElementFormat::Half2: return SDL_GPU_VERTEXELEMENTFORMAT_HALF2;
    case VertexElementFormat::Half4: return SDL_GPU_VERTEXELEMENTFORMAT_HALF4;
    }
    return SDL_GPU_VERTEXELEMENTFORMAT_INVALID;
}

SDL_GPUPrimitiveType ToSdl(PrimitiveType p)
{
    switch (p) {
    case PrimitiveType::TriangleList: return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    case PrimitiveType::TriangleStrip: return SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
    case PrimitiveType::LineList: return SDL_GPU_PRIMITIVETYPE_LINELIST;
    case PrimitiveType::LineStrip: return SDL_GPU_PRIMITIVETYPE_LINESTRIP;
    case PrimitiveType::PointList: return SDL_GPU_PRIMITIVETYPE_POINTLIST;
    }
    return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
}

SDL_GPUCompareOp ToSdl(CompareOp op)
{
    switch (op) {
    case CompareOp::Never: return SDL_GPU_COMPAREOP_NEVER;
    case CompareOp::Less: return SDL_GPU_COMPAREOP_LESS;
    case CompareOp::Equal: return SDL_GPU_COMPAREOP_EQUAL;
    case CompareOp::LessOrEqual: return SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
    case CompareOp::Greater: return SDL_GPU_COMPAREOP_GREATER;
    case CompareOp::NotEqual: return SDL_GPU_COMPAREOP_NOT_EQUAL;
    case CompareOp::GreaterOrEqual: return SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;
    case CompareOp::Always: return SDL_GPU_COMPAREOP_ALWAYS;
    }
    return SDL_GPU_COMPAREOP_ALWAYS;
}

SDL_GPUStencilOp ToSdl(StencilOp op)
{
    switch (op) {
    case StencilOp::Keep: return SDL_GPU_STENCILOP_KEEP;
    case StencilOp::Zero: return SDL_GPU_STENCILOP_ZERO;
    case StencilOp::Replace: return SDL_GPU_STENCILOP_REPLACE;
    case StencilOp::IncrementAndClamp: return SDL_GPU_STENCILOP_INCREMENT_AND_CLAMP;
    case StencilOp::DecrementAndClamp: return SDL_GPU_STENCILOP_DECREMENT_AND_CLAMP;
    case StencilOp::Invert: return SDL_GPU_STENCILOP_INVERT;
    case StencilOp::IncrementAndWrap: return SDL_GPU_STENCILOP_INCREMENT_AND_WRAP;
    case StencilOp::DecrementAndWrap: return SDL_GPU_STENCILOP_DECREMENT_AND_WRAP;
    }
    return SDL_GPU_STENCILOP_KEEP;
}

SDL_GPUBlendFactor ToSdl(BlendFactor f)
{
    switch (f) {
    case BlendFactor::Zero: return SDL_GPU_BLENDFACTOR_ZERO;
    case BlendFactor::One: return SDL_GPU_BLENDFACTOR_ONE;
    case BlendFactor::SrcColor: return SDL_GPU_BLENDFACTOR_SRC_COLOR;
    case BlendFactor::OneMinusSrcColor: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DstColor: return SDL_GPU_BLENDFACTOR_DST_COLOR;
    case BlendFactor::OneMinusDstColor: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::SrcAlpha: return SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    case BlendFactor::OneMinusSrcAlpha: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DstAlpha: return SDL_GPU_BLENDFACTOR_DST_ALPHA;
    case BlendFactor::OneMinusDstAlpha: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
    case BlendFactor::ConstantColor: return SDL_GPU_BLENDFACTOR_CONSTANT_COLOR;
    case BlendFactor::OneMinusConstantColor: return SDL_GPU_BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::SrcAlphaSaturate: return SDL_GPU_BLENDFACTOR_SRC_ALPHA_SATURATE;
    }
    return SDL_GPU_BLENDFACTOR_ONE;
}

SDL_GPUBlendOp ToSdl(BlendOp op)
{
    switch (op) {
    case BlendOp::Add: return SDL_GPU_BLENDOP_ADD;
    case BlendOp::Subtract: return SDL_GPU_BLENDOP_SUBTRACT;
    case BlendOp::ReverseSubtract: return SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
    case BlendOp::Min: return SDL_GPU_BLENDOP_MIN;
    case BlendOp::Max: return SDL_GPU_BLENDOP_MAX;
    }
    return SDL_GPU_BLENDOP_ADD;
}

SDL_GPUColorComponentFlags ToSdl(ColorComponent c)
{
    SDL_GPUColorComponentFlags out = 0;
    if (HasFlag(c, ColorComponent::R)) out |= SDL_GPU_COLORCOMPONENT_R;
    if (HasFlag(c, ColorComponent::G)) out |= SDL_GPU_COLORCOMPONENT_G;
    if (HasFlag(c, ColorComponent::B)) out |= SDL_GPU_COLORCOMPONENT_B;
    if (HasFlag(c, ColorComponent::A)) out |= SDL_GPU_COLORCOMPONENT_A;
    return out;
}

SDL_GPUFilter ToSdl(Filter f) { return f == Filter::Linear ? SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST; }

SDL_GPUSamplerMipmapMode ToSdl(MipmapMode m)
{
    return m == MipmapMode::Linear ? SDL_GPU_SAMPLERMIPMAPMODE_LINEAR : SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
}

SDL_GPUSamplerAddressMode ToSdl(AddressMode m)
{
    switch (m) {
    case AddressMode::Repeat: return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    case AddressMode::MirroredRepeat: return SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
    case AddressMode::ClampToEdge: return SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    }
    return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
}

SDL_GPULoadOp ToSdl(LoadOp op)
{
    switch (op) {
    case LoadOp::Load: return SDL_GPU_LOADOP_LOAD;
    case LoadOp::Clear: return SDL_GPU_LOADOP_CLEAR;
    case LoadOp::DontCare: return SDL_GPU_LOADOP_DONT_CARE;
    }
    return SDL_GPU_LOADOP_LOAD;
}

SDL_GPUStoreOp ToSdl(StoreOp op) { return op == StoreOp::Store ? SDL_GPU_STOREOP_STORE : SDL_GPU_STOREOP_DONT_CARE; }

SDL_GPUPresentMode ToSdl(PresentMode m)
{
    switch (m) {
    case PresentMode::Vsync: return SDL_GPU_PRESENTMODE_VSYNC;
    case PresentMode::Immediate: return SDL_GPU_PRESENTMODE_IMMEDIATE;
    case PresentMode::Mailbox: return SDL_GPU_PRESENTMODE_MAILBOX;
    }
    return SDL_GPU_PRESENTMODE_VSYNC;
}

SDL_GPUTextureTransferInfo ToSdl(const TextureTransferInfo& t)
{
    return {Sdl(t.transferBuffer), t.offset, t.pixelsPerRow, t.rowsPerLayer};
}

SDL_GPUTextureRegion ToSdl(const TextureRegion& r)
{
    return {Sdl(r.texture), r.mipLevel, r.layer, r.x, r.y, r.z, r.w, r.h, r.d};
}

SDL_GPUTextureLocation ToSdl(const TextureLocation& l)
{
    return {Sdl(l.texture), l.mipLevel, l.layer, l.x, l.y, l.z};
}

class SdlCommandBuffer;

}  // namespace

namespace detail {

class SdlGpuDevice final : public Device {
public:
    explicit SdlGpuDevice(SDL_GPUDevice* device) : m_device(device) {}
    ~SdlGpuDevice() override;

    Backend GetBackend() const override { return Backend::SdlGpu; }
    std::string_view DriverName() const override
    {
        const char* name = SDL_GetGPUDeviceDriver(m_device);
        return name ? name : "";
    }
    ShaderFormat ShaderFormats() const override { return FromSdl(SDL_GetGPUShaderFormats(m_device)); }
    const std::string& LastError() const override { return m_lastError; }

    bool ClaimWindow(SDL_Window* window) override
    {
        return SDL_ClaimWindowForGPUDevice(m_device, window) || Fail("SDL_ClaimWindowForGPUDevice");
    }
    void ReleaseWindow(SDL_Window* window) override { SDL_ReleaseWindowFromGPUDevice(m_device, window); }
    bool WindowSupportsPresentMode(SDL_Window* window, PresentMode mode) override
    {
        return SDL_WindowSupportsGPUPresentMode(m_device, window, ToSdl(mode));
    }
    bool SetSwapchainParameters(SDL_Window* window, PresentMode mode) override
    {
        return SDL_SetGPUSwapchainParameters(m_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, ToSdl(mode)) ||
               Fail("SDL_SetGPUSwapchainParameters");
    }
    TextureFormat SwapchainTextureFormat(SDL_Window* window) override
    {
        return FromSdl(SDL_GetGPUSwapchainTextureFormat(m_device, window));
    }
    bool TextureSupportsFormat(TextureFormat format, TextureType type, TextureUsage usage) override
    {
        return format != TextureFormat::Invalid &&
               SDL_GPUTextureSupportsFormat(m_device, ToSdl(format), ToSdl(type), ToSdl(usage));
    }

    Buffer CreateBuffer(const BufferDesc& desc) override
    {
        const SDL_GPUBufferCreateInfo info{ToSdl(desc.usage), desc.size, 0};
        SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(m_device, &info);
        if (!buffer) {
            Fail("SDL_CreateGPUBuffer");
            return {};
        }
        if (!desc.name.empty()) {
            SDL_SetGPUBufferName(m_device, buffer, std::string(desc.name).c_str());
        }
        return Wrap<Buffer>(buffer);
    }

    TransferBuffer CreateTransferBuffer(const TransferBufferDesc& desc) override
    {
        const SDL_GPUTransferBufferCreateInfo info{desc.usage == TransferBufferUsage::Upload
                                                       ? SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD
                                                       : SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD,
                                                   desc.size, 0};
        SDL_GPUTransferBuffer* buffer = SDL_CreateGPUTransferBuffer(m_device, &info);
        if (!buffer) {
            Fail("SDL_CreateGPUTransferBuffer");
        }
        return Wrap<TransferBuffer>(buffer);
    }

    Texture CreateTexture(const TextureDesc& desc) override
    {
        SDL_GPUTextureCreateInfo info{};
        info.type = ToSdl(desc.type);
        info.format = ToSdl(desc.format);
        info.usage = ToSdl(desc.usage);
        info.width = desc.width;
        info.height = desc.height;
        info.layer_count_or_depth = desc.layerCountOrDepth;
        info.num_levels = desc.levelCount;
        info.sample_count = ToSdl(desc.sampleCount);
        SDL_GPUTexture* texture = SDL_CreateGPUTexture(m_device, &info);
        if (!texture) {
            Fail("SDL_CreateGPUTexture");
            return {};
        }
        if (!desc.name.empty()) {
            SDL_SetGPUTextureName(m_device, texture, std::string(desc.name).c_str());
        }
        return Wrap<Texture>(texture);
    }

    Sampler CreateSampler(const SamplerDesc& desc) override
    {
        SDL_GPUSamplerCreateInfo info{};
        info.min_filter = ToSdl(desc.minFilter);
        info.mag_filter = ToSdl(desc.magFilter);
        info.mipmap_mode = ToSdl(desc.mipmapMode);
        info.address_mode_u = ToSdl(desc.addressU);
        info.address_mode_v = ToSdl(desc.addressV);
        info.address_mode_w = ToSdl(desc.addressW);
        info.mip_lod_bias = desc.mipLodBias;
        info.max_anisotropy = desc.maxAnisotropy;
        info.enable_anisotropy = desc.maxAnisotropy > 0.0f;
        info.compare_op = ToSdl(desc.compareOp);
        info.enable_compare = desc.enableCompare;
        info.min_lod = desc.minLod;
        info.max_lod = desc.maxLod;
        SDL_GPUSampler* sampler = SDL_CreateGPUSampler(m_device, &info);
        if (!sampler) {
            Fail("SDL_CreateGPUSampler");
        }
        return Wrap<Sampler>(sampler);
    }

    Shader CreateShader(const ShaderDesc& desc) override
    {
        const std::string entryPoint(desc.entryPoint);
        SDL_GPUShaderCreateInfo info{};
        info.code_size = desc.code.size();
        info.code = reinterpret_cast<const Uint8*>(desc.code.data());
        info.entrypoint = entryPoint.c_str();
        info.format = ToSdl(desc.format);
        info.stage = desc.stage == ShaderStage::Vertex ? SDL_GPU_SHADERSTAGE_VERTEX : SDL_GPU_SHADERSTAGE_FRAGMENT;
        info.num_samplers = desc.samplerCount;
        info.num_uniform_buffers = desc.uniformBufferCount;
        SDL_GPUShader* shader = SDL_CreateGPUShader(m_device, &info);
        if (!shader) {
            Fail("SDL_CreateGPUShader");
        }
        return Wrap<Shader>(shader);
    }

    GraphicsPipeline CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override
    {
        std::vector<SDL_GPUVertexBufferDescription> buffers;
        for (const VertexBufferDesc& b : desc.vertexBuffers) {
            buffers.push_back({b.slot, b.pitch,
                               b.inputRate == VertexInputRate::Vertex ? SDL_GPU_VERTEXINPUTRATE_VERTEX
                                                                      : SDL_GPU_VERTEXINPUTRATE_INSTANCE,
                               0});
        }
        std::vector<SDL_GPUVertexAttribute> attributes;
        for (const VertexAttribute& a : desc.vertexAttributes) {
            attributes.push_back({a.location, a.bufferSlot, ToSdl(a.format), a.offset});
        }
        std::vector<SDL_GPUColorTargetDescription> targets;
        for (const ColorTargetDesc& t : desc.colorTargets) {
            SDL_GPUColorTargetDescription target{};
            target.format = ToSdl(t.format);
            target.blend_state.enable_blend = t.blend.enableBlend;
            target.blend_state.src_color_blendfactor = ToSdl(t.blend.srcColor);
            target.blend_state.dst_color_blendfactor = ToSdl(t.blend.dstColor);
            target.blend_state.color_blend_op = ToSdl(t.blend.colorOp);
            target.blend_state.src_alpha_blendfactor = ToSdl(t.blend.srcAlpha);
            target.blend_state.dst_alpha_blendfactor = ToSdl(t.blend.dstAlpha);
            target.blend_state.alpha_blend_op = ToSdl(t.blend.alphaOp);
            target.blend_state.color_write_mask = ToSdl(t.blend.writeMask);
            target.blend_state.enable_color_write_mask = t.blend.writeMask != ColorComponent::All;
            targets.push_back(target);
        }
        auto stencil = [](const StencilOpState& s) {
            return SDL_GPUStencilOpState{ToSdl(s.failOp), ToSdl(s.passOp), ToSdl(s.depthFailOp), ToSdl(s.compareOp)};
        };

        SDL_GPUGraphicsPipelineCreateInfo info{};
        info.vertex_shader = Sdl(desc.vertexShader);
        info.fragment_shader = Sdl(desc.fragmentShader);
        info.vertex_input_state.vertex_buffer_descriptions = buffers.data();
        info.vertex_input_state.num_vertex_buffers = static_cast<Uint32>(buffers.size());
        info.vertex_input_state.vertex_attributes = attributes.data();
        info.vertex_input_state.num_vertex_attributes = static_cast<Uint32>(attributes.size());
        info.primitive_type = ToSdl(desc.primitiveType);
        const RasterizerState& r = desc.rasterizer;
        info.rasterizer_state.fill_mode = r.fillMode == FillMode::Fill ? SDL_GPU_FILLMODE_FILL : SDL_GPU_FILLMODE_LINE;
        info.rasterizer_state.cull_mode = r.cullMode == CullMode::None    ? SDL_GPU_CULLMODE_NONE
                                          : r.cullMode == CullMode::Front ? SDL_GPU_CULLMODE_FRONT
                                                                          : SDL_GPU_CULLMODE_BACK;
        info.rasterizer_state.front_face = r.frontFace == FrontFace::CounterClockwise
                                               ? SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
                                               : SDL_GPU_FRONTFACE_CLOCKWISE;
        info.rasterizer_state.enable_depth_bias = r.enableDepthBias;
        info.rasterizer_state.depth_bias_constant_factor = r.depthBiasConstantFactor;
        info.rasterizer_state.depth_bias_clamp = r.depthBiasClamp;
        info.rasterizer_state.depth_bias_slope_factor = r.depthBiasSlopeFactor;
        info.rasterizer_state.enable_depth_clip = r.enableDepthClip;
        info.multisample_state.sample_count = ToSdl(desc.sampleCount);
        const DepthStencilState& d = desc.depthStencil;
        info.depth_stencil_state.enable_depth_test = d.enableDepthTest;
        info.depth_stencil_state.enable_depth_write = d.enableDepthWrite;
        info.depth_stencil_state.compare_op = ToSdl(d.compareOp);
        info.depth_stencil_state.enable_stencil_test = d.enableStencilTest;
        info.depth_stencil_state.back_stencil_state = stencil(d.backStencil);
        info.depth_stencil_state.front_stencil_state = stencil(d.frontStencil);
        info.depth_stencil_state.compare_mask = d.compareMask;
        info.depth_stencil_state.write_mask = d.writeMask;
        info.target_info.color_target_descriptions = targets.data();
        info.target_info.num_color_targets = static_cast<Uint32>(targets.size());
        info.target_info.has_depth_stencil_target = desc.depthStencilFormat != TextureFormat::Invalid;
        info.target_info.depth_stencil_format = ToSdl(desc.depthStencilFormat);

        SDL_PropertiesID props = 0;
        if (!desc.name.empty()) {
            props = SDL_CreateProperties();
            SDL_SetStringProperty(props, SDL_PROP_GPU_GRAPHICSPIPELINE_CREATE_NAME_STRING, std::string(desc.name).c_str());
            info.props = props;
        }
        SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &info);
        if (props) {
            SDL_DestroyProperties(props);
        }
        if (!pipeline) {
            Fail("SDL_CreateGPUGraphicsPipeline");
        }
        return Wrap<GraphicsPipeline>(pipeline);
    }

    void ReleaseBuffer(Buffer b) override { if (b) SDL_ReleaseGPUBuffer(m_device, Sdl(b)); }
    void ReleaseTransferBuffer(TransferBuffer b) override { if (b) SDL_ReleaseGPUTransferBuffer(m_device, Sdl(b)); }
    void ReleaseTexture(Texture t) override { if (t) SDL_ReleaseGPUTexture(m_device, Sdl(t)); }
    void ReleaseSampler(Sampler s) override { if (s) SDL_ReleaseGPUSampler(m_device, Sdl(s)); }
    void ReleaseShader(Shader s) override { if (s) SDL_ReleaseGPUShader(m_device, Sdl(s)); }
    void ReleaseGraphicsPipeline(GraphicsPipeline p) override { if (p) SDL_ReleaseGPUGraphicsPipeline(m_device, Sdl(p)); }

    void* MapTransferBuffer(TransferBuffer buffer, bool cycle) override
    {
        void* data = SDL_MapGPUTransferBuffer(m_device, Sdl(buffer), cycle);
        if (!data) {
            Fail("SDL_MapGPUTransferBuffer");
        }
        return data;
    }
    void UnmapTransferBuffer(TransferBuffer buffer) override { SDL_UnmapGPUTransferBuffer(m_device, Sdl(buffer)); }

    CommandBuffer* AcquireCommandBuffer() override;
    bool WaitIdle() override { return SDL_WaitForGPUIdle(m_device) || Fail("SDL_WaitForGPUIdle"); }

    SDL_GPUDevice* Native() const { return m_device; }
    bool Fail(const char* what)
    {
        std::lock_guard lock(m_errorMutex);
        m_lastError = std::string(what) + ": " + SDL_GetError();
        return false;
    }
    void Finish(SdlCommandBuffer* commands);

private:
    SDL_GPUDevice* m_device;
    std::mutex m_errorMutex;
    std::string m_lastError;
    std::mutex m_poolMutex;
    std::vector<std::unique_ptr<SdlCommandBuffer>> m_free;
};

}  // namespace detail

namespace {

using detail::SdlGpuDevice;

class SdlCommandBuffer final : public CommandBuffer {
public:
    explicit SdlCommandBuffer(SdlGpuDevice& device) : m_device(device) {}

    void Reset(SDL_GPUCommandBuffer* commands) { m_commands = commands; }
    SDL_GPUCommandBuffer* Native() const { return m_commands; }

    void PushVertexUniformData(std::uint32_t slot, std::span<const std::byte> data) override
    {
        SDL_PushGPUVertexUniformData(m_commands, slot, data.data(), static_cast<Uint32>(data.size()));
    }
    void PushFragmentUniformData(std::uint32_t slot, std::span<const std::byte> data) override
    {
        SDL_PushGPUFragmentUniformData(m_commands, slot, data.data(), static_cast<Uint32>(data.size()));
    }

    RenderPass* BeginRenderPass(std::span<const ColorTargetInfo> colorTargets,
                                const DepthStencilTargetInfo* depthStencilTarget) override
    {
        std::vector<SDL_GPUColorTargetInfo> colors;
        colors.reserve(colorTargets.size());
        for (const ColorTargetInfo& t : colorTargets) {
            SDL_GPUColorTargetInfo info{};
            info.texture = Sdl(t.texture);
            info.mip_level = t.mipLevel;
            info.layer_or_depth_plane = t.layerOrDepthPlane;
            info.clear_color = SDL_FColor{t.clearColor.r, t.clearColor.g, t.clearColor.b, t.clearColor.a};
            info.load_op = ToSdl(t.loadOp);
            info.store_op = ToSdl(t.storeOp);
            info.cycle = t.cycle;
            colors.push_back(info);
        }
        SDL_GPUDepthStencilTargetInfo depth{};
        if (depthStencilTarget) {
            depth.texture = Sdl(depthStencilTarget->texture);
            depth.clear_depth = depthStencilTarget->clearDepth;
            depth.load_op = ToSdl(depthStencilTarget->loadOp);
            depth.store_op = ToSdl(depthStencilTarget->storeOp);
            depth.stencil_load_op = ToSdl(depthStencilTarget->stencilLoadOp);
            depth.stencil_store_op = ToSdl(depthStencilTarget->stencilStoreOp);
            depth.cycle = depthStencilTarget->cycle;
            depth.clear_stencil = depthStencilTarget->clearStencil;
        }
        m_render.pass = SDL_BeginGPURenderPass(m_commands, colors.data(), static_cast<Uint32>(colors.size()),
                                               depthStencilTarget ? &depth : nullptr);
        if (!m_render.pass) {
            m_device.Fail("SDL_BeginGPURenderPass");
            return nullptr;
        }
        return &m_render;
    }

    CopyPass* BeginCopyPass() override
    {
        m_copy.pass = SDL_BeginGPUCopyPass(m_commands);
        if (!m_copy.pass) {
            m_device.Fail("SDL_BeginGPUCopyPass");
            return nullptr;
        }
        return &m_copy;
    }

    void GenerateMipmaps(Texture texture) override { SDL_GenerateMipmapsForGPUTexture(m_commands, Sdl(texture)); }
    void PushDebugGroup(std::string_view name) override { SDL_PushGPUDebugGroup(m_commands, std::string(name).c_str()); }
    void PopDebugGroup() override { SDL_PopGPUDebugGroup(m_commands); }
    void InsertDebugLabel(std::string_view text) override
    {
        SDL_InsertGPUDebugLabel(m_commands, std::string(text).c_str());
    }

    bool WaitAndAcquireSwapchainTexture(SDL_Window* window, SwapchainTexture& swapchain) override
    {
        swapchain = {};
        SDL_GPUTexture* texture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_commands, window, &texture, &swapchain.width, &swapchain.height)) {
            return m_device.Fail("SDL_WaitAndAcquireGPUSwapchainTexture");
        }
        swapchain.texture = Wrap<Texture>(texture);
        return true;
    }

    bool Submit() override
    {
        const bool ok = SDL_SubmitGPUCommandBuffer(m_commands) || m_device.Fail("SDL_SubmitGPUCommandBuffer");
        m_device.Finish(this);
        return ok;
    }
    void Cancel() override
    {
        SDL_CancelGPUCommandBuffer(m_commands);
        m_device.Finish(this);
    }

    struct Render final : RenderPass {
        SDL_GPURenderPass* pass = nullptr;

        void BindGraphicsPipeline(GraphicsPipeline pipeline) override { SDL_BindGPUGraphicsPipeline(pass, Sdl(pipeline)); }
        void SetViewport(const Viewport& v) override
        {
            const SDL_GPUViewport viewport{v.x, v.y, v.w, v.h, v.minDepth, v.maxDepth};
            SDL_SetGPUViewport(pass, &viewport);
        }
        void SetScissor(const Rect& r) override
        {
            const SDL_Rect rect{r.x, r.y, r.w, r.h};
            SDL_SetGPUScissor(pass, &rect);
        }
        void SetBlendConstants(const Color& c) override { SDL_SetGPUBlendConstants(pass, SDL_FColor{c.r, c.g, c.b, c.a}); }
        void SetStencilReference(std::uint8_t reference) override { SDL_SetGPUStencilReference(pass, reference); }
        void BindVertexBuffers(std::uint32_t firstSlot, std::span<const BufferBinding> bindings) override
        {
            std::vector<SDL_GPUBufferBinding> b;
            for (const BufferBinding& binding : bindings) {
                b.push_back({Sdl(binding.buffer), binding.offset});
            }
            SDL_BindGPUVertexBuffers(pass, firstSlot, b.data(), static_cast<Uint32>(b.size()));
        }
        void BindIndexBuffer(const BufferBinding& binding, IndexElementSize size) override
        {
            const SDL_GPUBufferBinding b{Sdl(binding.buffer), binding.offset};
            SDL_BindGPUIndexBuffer(pass, &b,
                                   size == IndexElementSize::U16 ? SDL_GPU_INDEXELEMENTSIZE_16BIT
                                                                 : SDL_GPU_INDEXELEMENTSIZE_32BIT);
        }
        static std::vector<SDL_GPUTextureSamplerBinding> Samplers(std::span<const TextureSamplerBinding> bindings)
        {
            std::vector<SDL_GPUTextureSamplerBinding> b;
            for (const TextureSamplerBinding& binding : bindings) {
                b.push_back({Sdl(binding.texture), Sdl(binding.sampler)});
            }
            return b;
        }
        void BindVertexSamplers(std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings) override
        {
            const auto b = Samplers(bindings);
            SDL_BindGPUVertexSamplers(pass, firstSlot, b.data(), static_cast<Uint32>(b.size()));
        }
        void BindFragmentSamplers(std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings) override
        {
            const auto b = Samplers(bindings);
            SDL_BindGPUFragmentSamplers(pass, firstSlot, b.data(), static_cast<Uint32>(b.size()));
        }
        void DrawPrimitives(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
                            std::uint32_t firstInstance) override
        {
            SDL_DrawGPUPrimitives(pass, vertexCount, instanceCount, firstVertex, firstInstance);
        }
        void DrawIndexedPrimitives(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex,
                                   std::int32_t vertexOffset, std::uint32_t firstInstance) override
        {
            SDL_DrawGPUIndexedPrimitives(pass, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }
        void End() override
        {
            SDL_EndGPURenderPass(pass);
            pass = nullptr;
        }
    };

    struct Copy final : CopyPass {
        SDL_GPUCopyPass* pass = nullptr;

        void UploadToTexture(const TextureTransferInfo& source, const TextureRegion& destination, bool cycle) override
        {
            const SDL_GPUTextureTransferInfo s = ToSdl(source);
            const SDL_GPUTextureRegion d = ToSdl(destination);
            SDL_UploadToGPUTexture(pass, &s, &d, cycle);
        }
        void UploadToBuffer(const TransferBufferLocation& source, const BufferRegion& destination, bool cycle) override
        {
            const SDL_GPUTransferBufferLocation s{Sdl(source.transferBuffer), source.offset};
            const SDL_GPUBufferRegion d{Sdl(destination.buffer), destination.offset, destination.size};
            SDL_UploadToGPUBuffer(pass, &s, &d, cycle);
        }
        void CopyTextureToTexture(const TextureLocation& source, const TextureLocation& destination, std::uint32_t w,
                                  std::uint32_t h, std::uint32_t d, bool cycle) override
        {
            const SDL_GPUTextureLocation s = ToSdl(source);
            const SDL_GPUTextureLocation t = ToSdl(destination);
            SDL_CopyGPUTextureToTexture(pass, &s, &t, w, h, d, cycle);
        }
        void CopyBufferToBuffer(const BufferLocation& source, const BufferLocation& destination, std::uint32_t size,
                                bool cycle) override
        {
            const SDL_GPUBufferLocation s{Sdl(source.buffer), source.offset};
            const SDL_GPUBufferLocation t{Sdl(destination.buffer), destination.offset};
            SDL_CopyGPUBufferToBuffer(pass, &s, &t, size, cycle);
        }
        void DownloadFromTexture(const TextureRegion& source, const TextureTransferInfo& destination) override
        {
            const SDL_GPUTextureRegion s = ToSdl(source);
            const SDL_GPUTextureTransferInfo d = ToSdl(destination);
            SDL_DownloadFromGPUTexture(pass, &s, &d);
        }
        void DownloadFromBuffer(const BufferRegion& source, const TransferBufferLocation& destination) override
        {
            const SDL_GPUBufferRegion s{Sdl(source.buffer), source.offset, source.size};
            const SDL_GPUTransferBufferLocation d{Sdl(destination.transferBuffer), destination.offset};
            SDL_DownloadFromGPUBuffer(pass, &s, &d);
        }
        void End() override
        {
            SDL_EndGPUCopyPass(pass);
            pass = nullptr;
        }
    };

    Render m_render;

private:
    SdlGpuDevice& m_device;
    SDL_GPUCommandBuffer* m_commands = nullptr;
    Copy m_copy;
};

}  // namespace

namespace detail {

SdlGpuDevice::~SdlGpuDevice()
{
    SDL_WaitForGPUIdle(m_device);
    SDL_DestroyGPUDevice(m_device);
}

CommandBuffer* SdlGpuDevice::AcquireCommandBuffer()
{
    SDL_GPUCommandBuffer* native = SDL_AcquireGPUCommandBuffer(m_device);
    if (!native) {
        Fail("SDL_AcquireGPUCommandBuffer");
        return nullptr;
    }
    std::unique_ptr<SdlCommandBuffer> commands;
    {
        std::lock_guard lock(m_poolMutex);
        if (!m_free.empty()) {
            commands = std::move(m_free.back());
            m_free.pop_back();
        }
    }
    if (!commands) {
        commands = std::make_unique<SdlCommandBuffer>(*this);
    }
    commands->Reset(native);
    return commands.release();     // back in m_free on Finish
}

void SdlGpuDevice::Finish(SdlCommandBuffer* commands)
{
    commands->Reset(nullptr);
    std::lock_guard lock(m_poolMutex);
    m_free.emplace_back(commands);
}

std::unique_ptr<Device> CreateSdlGpuDevice(const DeviceOptions& options, std::string& error)
{
    const char* debugEnv = SDL_getenv("X2_GPU_DEBUG");
    const bool debug = options.debug || (debugEnv && debugEnv[0] == '1');
    SDL_GPUDevice* device = SDL_CreateGPUDevice(ToSdl(options.shaderFormats), debug,
                                                options.driver.empty() ? nullptr : options.driver.c_str());
    if (!device) {
        error = std::string("SDL_CreateGPUDevice: ") + SDL_GetError();
        return nullptr;
    }
    return std::make_unique<SdlGpuDevice>(device);
}

}  // namespace detail

namespace sdlgpu {

SDL_GPUDevice* Native::Device(render::Device& device)
{
    auto* sdl = dynamic_cast<SdlGpuDevice*>(&device);
    return sdl ? sdl->Native() : nullptr;
}

SDL_GPUCommandBuffer* Native::CommandBuffer(render::CommandBuffer& commands)
{
    auto* sdl = dynamic_cast<SdlCommandBuffer*>(&commands);
    return sdl ? sdl->Native() : nullptr;
}

SDL_GPURenderPass* Native::RenderPass(render::RenderPass& pass)
{
    auto* sdl = dynamic_cast<SdlCommandBuffer::Render*>(&pass);
    return sdl ? sdl->pass : nullptr;
}

SDL_GPUTexture* Native::Texture(render::Device& device, render::Texture texture)
{
    return dynamic_cast<SdlGpuDevice*>(&device) ? Sdl(texture) : nullptr;
}

SDL_GPUTextureFormat Native::Format(render::TextureFormat format)
{
    return ToSdl(format);
}

}  // namespace sdlgpu

}  // namespace x2::render
