#pragma once

// x2::render: the graphics boundary (decision T36). A thin API with SDL_GPU's shape and nothing SDL_GPU lacks: device,
// buffers, textures, samplers, shaders, graphics pipelines, command buffers, render and copy passes, bindings and
// uniforms by slot, texture readback. Backends sit behind `Device`, chosen at run time: `SdlGpu` (the only real one) and
// `Null` (records commands, keeps texture and buffer bytes, rasterizes nothing; for headless tests).
//
// Conventions at the boundary, the same as Direct3D: clip-space depth 0..1, texture and viewport origin at the top
// left, row 0 of a texture is its top row. Each backend adapts to them.
//
// Nothing here includes SDL: SDL_gpu.h belongs to src/render/backends/sdlgpu only (a source scan checks it). Tools that
// still need the SDL_GPU objects (ImGui's sdlgpu3 backend) use x2/render/sdlgpu.h. See docs/porting/render.md.

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

struct SDL_Window;

namespace x2::render {

// --- Handles ----------------------------------------------------------------------------------------------------------
// Opaque and owned by the device that created them; a default-constructed handle is "none". Released explicitly
// (Device::Release*), as in SDL_GPU: a resource still used by a command buffer in flight lives until it completes.

template <typename Tag>
struct Handle {
    std::uint64_t value = 0;
    explicit operator bool() const { return value != 0; }
    friend bool operator==(Handle, Handle) = default;
};

using Buffer = Handle<struct BufferTag>;
using TransferBuffer = Handle<struct TransferBufferTag>;
using Texture = Handle<struct TextureTag>;
using Sampler = Handle<struct SamplerTag>;
using Shader = Handle<struct ShaderTag>;
using GraphicsPipeline = Handle<struct GraphicsPipelineTag>;

// --- Enumerations -----------------------------------------------------------------------------------------------------

enum class Backend : std::uint8_t { Null, SdlGpu };

// Bit flags.
enum class ShaderFormat : std::uint32_t { None = 0, SpirV = 1u << 0, Dxil = 1u << 1, Msl = 1u << 2 };

enum class TextureFormat : std::uint8_t {
    Invalid,
    A8Unorm,
    R8Unorm,
    R8G8Unorm,
    R8G8B8A8Unorm,
    R8G8B8A8UnormSrgb,
    B8G8R8A8Unorm,
    B8G8R8A8UnormSrgb,
    B5G6R5Unorm,
    B5G5R5A1Unorm,
    B4G4R4A4Unorm,
    R16G16B16A16Float,
    R32Float,
    Bc1Unorm,   // DXT1
    Bc2Unorm,   // DXT2/3
    Bc3Unorm,   // DXT4/5
    D16Unorm,
    D24Unorm,
    D32Float,
    D24UnormS8Uint,
    D32FloatS8Uint,
};

enum class TextureType : std::uint8_t { Texture2D, Texture2DArray, Texture3D, Cube, CubeArray };

// Bit flags.
enum class TextureUsage : std::uint32_t { None = 0, Sampler = 1u << 0, ColorTarget = 1u << 1, DepthStencilTarget = 1u << 2 };

enum class SampleCount : std::uint8_t { X1, X2, X4, X8 };

// Bit flags.
enum class BufferUsage : std::uint32_t { None = 0, Vertex = 1u << 0, Index = 1u << 1, Indirect = 1u << 2 };

enum class TransferBufferUsage : std::uint8_t { Upload, Download };

enum class ShaderStage : std::uint8_t { Vertex, Fragment };

enum class VertexElementFormat : std::uint8_t {
    Invalid,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
    Float, Float2, Float3, Float4,
    Byte2, Byte4, UByte2, UByte4,
    Byte2Norm, Byte4Norm, UByte2Norm, UByte4Norm,
    Short2, Short4, UShort2, UShort4,
    Short2Norm, Short4Norm, UShort2Norm, UShort4Norm,
    Half2, Half4,
};

enum class VertexInputRate : std::uint8_t { Vertex, Instance };
enum class PrimitiveType : std::uint8_t { TriangleList, TriangleStrip, LineList, LineStrip, PointList };
enum class FillMode : std::uint8_t { Fill, Line };
enum class CullMode : std::uint8_t { None, Front, Back };
enum class FrontFace : std::uint8_t { CounterClockwise, Clockwise };

enum class CompareOp : std::uint8_t { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always };

enum class StencilOp : std::uint8_t {
    Keep, Zero, Replace, IncrementAndClamp, DecrementAndClamp, Invert, IncrementAndWrap, DecrementAndWrap,
};

enum class BlendFactor : std::uint8_t {
    Zero, One,
    SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor,
    SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha,
    ConstantColor, OneMinusConstantColor,
    SrcAlphaSaturate,
};

enum class BlendOp : std::uint8_t { Add, Subtract, ReverseSubtract, Min, Max };

// Bit flags.
enum class ColorComponent : std::uint8_t { None = 0, R = 1u << 0, G = 1u << 1, B = 1u << 2, A = 1u << 3, All = 0xF };

enum class Filter : std::uint8_t { Nearest, Linear };
enum class MipmapMode : std::uint8_t { Nearest, Linear };
enum class AddressMode : std::uint8_t { Repeat, MirroredRepeat, ClampToEdge };
enum class IndexElementSize : std::uint8_t { U16, U32 };
enum class LoadOp : std::uint8_t { Load, Clear, DontCare };
// Resolve: the multisampled contents are resolved into ColorTargetInfo::resolveTexture and then undefined;
// ResolveAndStore also keeps them.
enum class StoreOp : std::uint8_t { Store, DontCare, Resolve, ResolveAndStore };
enum class PresentMode : std::uint8_t { Vsync, Immediate, Mailbox };

template <typename E>
inline constexpr bool IsFlagEnum = false;
template <> inline constexpr bool IsFlagEnum<ShaderFormat> = true;
template <> inline constexpr bool IsFlagEnum<TextureUsage> = true;
template <> inline constexpr bool IsFlagEnum<BufferUsage> = true;
template <> inline constexpr bool IsFlagEnum<ColorComponent> = true;

template <typename E> requires IsFlagEnum<E>
constexpr E operator|(E a, E b)
{
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(static_cast<U>(a) | static_cast<U>(b)));
}

template <typename E> requires IsFlagEnum<E>
constexpr bool HasFlag(E set, E flag)
{
    using U = std::underlying_type_t<E>;
    return (static_cast<U>(set) & static_cast<U>(flag)) == static_cast<U>(flag) && static_cast<U>(flag) != 0;
}

// --- Descriptions -----------------------------------------------------------------------------------------------------

struct Color {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
};

struct Rect {
    std::int32_t x = 0, y = 0, w = 0, h = 0;
};

struct Viewport {
    float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;
    float minDepth = 0.0f, maxDepth = 1.0f;
};

struct BufferDesc {
    BufferUsage usage = BufferUsage::None;
    std::uint32_t size = 0;
    std::string_view name = {};      // debug name; may be empty
};

struct TransferBufferDesc {
    TransferBufferUsage usage = TransferBufferUsage::Upload;
    std::uint32_t size = 0;
};

struct TextureDesc {
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::R8G8B8A8Unorm;
    TextureUsage usage = TextureUsage::Sampler;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t layerCountOrDepth = 1;
    std::uint32_t levelCount = 1;
    SampleCount sampleCount = SampleCount::X1;
    std::string_view name = {};
};

struct SamplerDesc {
    Filter minFilter = Filter::Nearest;
    Filter magFilter = Filter::Nearest;
    MipmapMode mipmapMode = MipmapMode::Nearest;
    AddressMode addressU = AddressMode::Repeat;
    AddressMode addressV = AddressMode::Repeat;
    AddressMode addressW = AddressMode::Repeat;
    float mipLodBias = 0.0f;
    float maxAnisotropy = 0.0f;         // 0: anisotropic filtering off
    bool enableCompare = false;
    CompareOp compareOp = CompareOp::Never;
    float minLod = 0.0f;
    float maxLod = 1000.0f;
};

// One stage's code with its reflection: how many samplers and uniform blocks it binds (slots 0..n-1).
struct ShaderDesc {
    std::span<const std::byte> code;
    std::string_view entryPoint = "main";
    ShaderFormat format = ShaderFormat::SpirV;      // exactly one format
    ShaderStage stage = ShaderStage::Vertex;
    std::uint32_t samplerCount = 0;
    std::uint32_t uniformBufferCount = 0;
};

struct VertexBufferDesc {
    std::uint32_t slot = 0;
    std::uint32_t pitch = 0;
    VertexInputRate inputRate = VertexInputRate::Vertex;
};

struct VertexAttribute {
    std::uint32_t location = 0;
    std::uint32_t bufferSlot = 0;
    VertexElementFormat format = VertexElementFormat::Invalid;
    std::uint32_t offset = 0;
};

struct RasterizerState {
    FillMode fillMode = FillMode::Fill;
    CullMode cullMode = CullMode::None;
    FrontFace frontFace = FrontFace::CounterClockwise;
    bool enableDepthBias = false;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    bool enableDepthClip = true;
};

struct StencilOpState {
    StencilOp failOp = StencilOp::Keep;
    StencilOp passOp = StencilOp::Keep;
    StencilOp depthFailOp = StencilOp::Keep;
    CompareOp compareOp = CompareOp::Always;
};

struct DepthStencilState {
    bool enableDepthTest = false;
    bool enableDepthWrite = false;
    CompareOp compareOp = CompareOp::LessOrEqual;
    bool enableStencilTest = false;
    StencilOpState backStencil = {};
    StencilOpState frontStencil = {};
    std::uint8_t compareMask = 0xFF;
    std::uint8_t writeMask = 0xFF;
};

struct BlendState {
    bool enableBlend = false;
    BlendFactor srcColor = BlendFactor::One;
    BlendFactor dstColor = BlendFactor::Zero;
    BlendOp colorOp = BlendOp::Add;
    BlendFactor srcAlpha = BlendFactor::One;
    BlendFactor dstAlpha = BlendFactor::Zero;
    BlendOp alphaOp = BlendOp::Add;
    ColorComponent writeMask = ColorComponent::All;
};

struct ColorTargetDesc {
    TextureFormat format = TextureFormat::Invalid;
    BlendState blend = {};
};

struct GraphicsPipelineDesc {
    Shader vertexShader = {};
    Shader fragmentShader = {};
    std::span<const VertexBufferDesc> vertexBuffers = {};
    std::span<const VertexAttribute> vertexAttributes = {};
    PrimitiveType primitiveType = PrimitiveType::TriangleList;
    RasterizerState rasterizer = {};
    SampleCount sampleCount = SampleCount::X1;
    bool alphaToCoverage = false;   // the fragment's alpha sets the covered samples (multisampled targets)
    DepthStencilState depthStencil = {};
    std::span<const ColorTargetDesc> colorTargets = {};
    TextureFormat depthStencilFormat = TextureFormat::Invalid;     // Invalid: no depth-stencil target
    std::string_view name = {};
};

// --- Pass and transfer arguments --------------------------------------------------------------------------------------

struct ColorTargetInfo {
    Texture texture = {};
    std::uint32_t mipLevel = 0;
    std::uint32_t layerOrDepthPlane = 0;
    Color clearColor = {};
    LoadOp loadOp = LoadOp::Load;
    StoreOp storeOp = StoreOp::Store;
    bool cycle = false;         // SDL_GPU cycling: write to a fresh copy if the texture is still in use
    Texture resolveTexture = {};    // single-sampled; with StoreOp::Resolve or ResolveAndStore
};

// A region of one level of a 2D texture, for BlitTexture.
struct BlitRegion {
    Texture texture = {};
    std::uint32_t mipLevel = 0;
    std::uint32_t x = 0, y = 0, w = 0, h = 0;
};

struct DepthStencilTargetInfo {
    Texture texture = {};
    float clearDepth = 1.0f;
    LoadOp loadOp = LoadOp::Load;
    StoreOp storeOp = StoreOp::Store;
    LoadOp stencilLoadOp = LoadOp::DontCare;
    StoreOp stencilStoreOp = StoreOp::DontCare;
    bool cycle = false;
    std::uint8_t clearStencil = 0;
};

struct BufferBinding {
    Buffer buffer = {};
    std::uint32_t offset = 0;
};

struct TextureSamplerBinding {
    Texture texture = {};
    Sampler sampler = {};
};

// Texel data in a transfer buffer. 0 for pixelsPerRow / rowsPerLayer means tightly packed (the region's width/height).
struct TextureTransferInfo {
    TransferBuffer transferBuffer = {};
    std::uint32_t offset = 0;
    std::uint32_t pixelsPerRow = 0;
    std::uint32_t rowsPerLayer = 0;
};

struct TextureRegion {
    Texture texture = {};
    std::uint32_t mipLevel = 0;
    std::uint32_t layer = 0;
    std::uint32_t x = 0, y = 0, z = 0;
    std::uint32_t w = 0, h = 0, d = 1;
};

struct TextureLocation {
    Texture texture = {};
    std::uint32_t mipLevel = 0;
    std::uint32_t layer = 0;
    std::uint32_t x = 0, y = 0, z = 0;
};

struct TransferBufferLocation {
    TransferBuffer transferBuffer = {};
    std::uint32_t offset = 0;
};

struct BufferRegion {
    Buffer buffer = {};
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
};

struct BufferLocation {
    Buffer buffer = {};
    std::uint32_t offset = 0;
};

// The window's back buffer for this frame. `texture` is none when there is nothing to draw into (minimized window,
// swapchain not ready): record nothing into it, but still submit the command buffer.
struct SwapchainTexture {
    Texture texture = {};
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

// --- Command recording ------------------------------------------------------------------------------------------------
// Owned by the command buffer that began them; valid until End(). One pass at a time per command buffer.

class RenderPass {
public:
    virtual void BindGraphicsPipeline(GraphicsPipeline pipeline) = 0;
    virtual void SetViewport(const Viewport& viewport) = 0;
    virtual void SetScissor(const Rect& scissor) = 0;
    virtual void SetBlendConstants(const Color& color) = 0;
    virtual void SetStencilReference(std::uint8_t reference) = 0;
    virtual void BindVertexBuffers(std::uint32_t firstSlot, std::span<const BufferBinding> bindings) = 0;
    virtual void BindIndexBuffer(const BufferBinding& binding, IndexElementSize size) = 0;
    virtual void BindVertexSamplers(std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings) = 0;
    virtual void BindFragmentSamplers(std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings) = 0;
    virtual void DrawPrimitives(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
                                std::uint32_t firstInstance) = 0;
    virtual void DrawIndexedPrimitives(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex,
                                       std::int32_t vertexOffset, std::uint32_t firstInstance) = 0;
    virtual void End() = 0;

protected:
    ~RenderPass() = default;
};

class CopyPass {
public:
    virtual void UploadToTexture(const TextureTransferInfo& source, const TextureRegion& destination, bool cycle) = 0;
    virtual void UploadToBuffer(const TransferBufferLocation& source, const BufferRegion& destination, bool cycle) = 0;
    virtual void CopyTextureToTexture(const TextureLocation& source, const TextureLocation& destination, std::uint32_t w,
                                      std::uint32_t h, std::uint32_t d, bool cycle) = 0;
    virtual void CopyBufferToBuffer(const BufferLocation& source, const BufferLocation& destination, std::uint32_t size,
                                    bool cycle) = 0;
    // Readback: the data is in the transfer buffer once the command buffer has completed (Device::WaitIdle).
    virtual void DownloadFromTexture(const TextureRegion& source, const TextureTransferInfo& destination) = 0;
    virtual void DownloadFromBuffer(const BufferRegion& source, const TransferBufferLocation& destination) = 0;
    virtual void End() = 0;

protected:
    ~CopyPass() = default;
};

// Acquired from the device, valid until Submit() or Cancel(). Uniform data is pushed by slot outside passes and stays
// bound for the rest of the command buffer, as in SDL_GPU.
class CommandBuffer {
public:
    virtual void PushVertexUniformData(std::uint32_t slot, std::span<const std::byte> data) = 0;
    virtual void PushFragmentUniformData(std::uint32_t slot, std::span<const std::byte> data) = 0;

    // Null on failure (Device::LastError()).
    virtual RenderPass* BeginRenderPass(std::span<const ColorTargetInfo> colorTargets,
                                        const DepthStencilTargetInfo* depthStencilTarget) = 0;
    virtual CopyPass* BeginCopyPass() = 0;
    virtual void GenerateMipmaps(Texture texture) = 0;
    // Copies and scales `source` into `destination` (outside any pass); the destination must be a color target.
    virtual void BlitTexture(const BlitRegion& source, const BlitRegion& destination, Filter filter) = 0;

    virtual void PushDebugGroup(std::string_view name) = 0;
    virtual void PopDebugGroup() = 0;
    virtual void InsertDebugLabel(std::string_view text) = 0;

    // Waits for the window's next back buffer (it must have been claimed). False only if the device failed.
    virtual bool WaitAndAcquireSwapchainTexture(SDL_Window* window, SwapchainTexture& swapchain) = 0;

    virtual bool Submit() = 0;
    virtual void Cancel() = 0;

protected:
    ~CommandBuffer() = default;
};

// --- Device -----------------------------------------------------------------------------------------------------------

struct DeviceOptions {
    Backend backend = Backend::SdlGpu;
    ShaderFormat shaderFormats = ShaderFormat::SpirV | ShaderFormat::Dxil | ShaderFormat::Msl;
    // Validation layers where available; X2_GPU_DEBUG=1 also turns them on. Off by default even with dev tools: the
    // Vulkan layer checks every draw and took about 80% of the client's frame (2026-11-12).
    bool debug = false;
    std::string driver;         // SDL_GPU driver name ("vulkan", "direct3d12", "metal"); empty: the best one
};

class Device {
public:
    // Null with `error` set when the backend cannot create a device (no GPU, no driver for the shader formats).
    static std::unique_ptr<Device> Create(const DeviceOptions& options, std::string& error);
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    virtual Backend GetBackend() const = 0;
    virtual std::string_view DriverName() const = 0;
    virtual ShaderFormat ShaderFormats() const = 0;
    // The last failure's message on this device, for calls that return a none handle, null or false.
    virtual const std::string& LastError() const = 0;

    // Windows and their swapchains. A claimed window must be released before it is destroyed.
    virtual bool ClaimWindow(SDL_Window* window) = 0;
    virtual void ReleaseWindow(SDL_Window* window) = 0;
    virtual bool WindowSupportsPresentMode(SDL_Window* window, PresentMode mode) = 0;
    virtual bool SetSwapchainParameters(SDL_Window* window, PresentMode mode) = 0;
    virtual TextureFormat SwapchainTextureFormat(SDL_Window* window) = 0;

    virtual bool TextureSupportsFormat(TextureFormat format, TextureType type, TextureUsage usage) = 0;
    // Whether a 2D texture of this format can have this many samples.
    virtual bool TextureSupportsSampleCount(TextureFormat format, SampleCount count) = 0;

    virtual Buffer CreateBuffer(const BufferDesc& desc) = 0;
    virtual TransferBuffer CreateTransferBuffer(const TransferBufferDesc& desc) = 0;
    virtual Texture CreateTexture(const TextureDesc& desc) = 0;
    virtual Sampler CreateSampler(const SamplerDesc& desc) = 0;
    virtual Shader CreateShader(const ShaderDesc& desc) = 0;
    virtual GraphicsPipeline CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;

    virtual void ReleaseBuffer(Buffer buffer) = 0;
    virtual void ReleaseTransferBuffer(TransferBuffer buffer) = 0;
    virtual void ReleaseTexture(Texture texture) = 0;
    virtual void ReleaseSampler(Sampler sampler) = 0;
    virtual void ReleaseShader(Shader shader) = 0;
    virtual void ReleaseGraphicsPipeline(GraphicsPipeline pipeline) = 0;

    // The transfer buffer's memory, until Unmap. `cycle`: map a fresh copy if the buffer is still in use.
    virtual void* MapTransferBuffer(TransferBuffer buffer, bool cycle) = 0;
    virtual void UnmapTransferBuffer(TransferBuffer buffer) = 0;

    // Null on failure. Command buffers may be acquired and recorded on any thread; each one on a single thread.
    virtual CommandBuffer* AcquireCommandBuffer() = 0;
    virtual bool WaitIdle() = 0;

protected:
    Device() = default;
};

// Claims a window for the device for its lifetime and sets its present mode; releases it (after WaitIdle) on
// destruction. Destroy it before the window and after the last frame.
class WindowClaim {
public:
    WindowClaim(Device& device, SDL_Window* window, PresentMode mode = PresentMode::Vsync);
    ~WindowClaim();

    WindowClaim(const WindowClaim&) = delete;
    WindowClaim& operator=(const WindowClaim&) = delete;

    // False when the claim failed: see Device::LastError().
    bool Ok() const { return m_claimed; }

private:
    Device& m_device;
    SDL_Window* m_window;
    bool m_claimed = false;
};

// "null" / "sdlgpu" (the names configuration and tests use).
std::string_view BackendName(Backend backend);
bool ParseBackend(std::string_view name, Backend& backend);

// Bytes per texel block (a 4x4 block for BC formats, one texel otherwise); 0 for Invalid.
std::uint32_t TexelBlockSize(TextureFormat format);
bool IsBlockCompressed(TextureFormat format);
bool IsDepthFormat(TextureFormat format);
// Bytes of a w x h x d region in `format`, tightly packed.
std::uint32_t TextureFormatSize(TextureFormat format, std::uint32_t w, std::uint32_t h, std::uint32_t d);

}  // namespace x2::render
