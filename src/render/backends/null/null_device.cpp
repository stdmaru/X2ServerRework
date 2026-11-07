// The null backend (decision T36): no GPU. It validates handles, records one line per submitted command, and keeps the
// bytes of buffers and textures so that clears, uploads, copies and downloads behave; draws change nothing.
// The test view is x2/render/null.h. See docs/porting/render.md.

#include "x2/render/null.h"

#include "../../src/backends.h"

#include <SDL3/SDL_video.h>

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace x2::render {

namespace {

enum class Kind : std::uint8_t { Buffer, TransferBuffer, Texture, Sampler, Shader, Pipeline };

const char* KindName(Kind kind)
{
    switch (kind) {
    case Kind::Buffer:
        return "buffer";
    case Kind::TransferBuffer:
        return "transfer";
    case Kind::Texture:
        return "texture";
    case Kind::Sampler:
        return "sampler";
    case Kind::Shader:
        return "shader";
    case Kind::Pipeline:
        return "pipeline";
    }
    return "?";
}

struct Resource {
    Kind kind = Kind::Buffer;
    std::string name;
    bool swapchain = false;
    // Buffers and transfer buffers.
    std::vector<std::byte> bytes;
    TransferBufferUsage transferUsage = TransferBufferUsage::Upload;
    bool mapped = false;
    // Textures: one byte vector per (mip level, layer), tightly packed.
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::Invalid;
    TextureUsage usage = TextureUsage::None;
    std::uint32_t width = 0, height = 0, layersOrDepth = 1, levels = 1;
    std::vector<std::vector<std::byte>> subresources;
    // Shaders.
    ShaderStage stage = ShaderStage::Vertex;

    std::uint32_t Layers() const { return type == TextureType::Texture3D ? 1 : layersOrDepth; }
    std::uint32_t LevelWidth(std::uint32_t level) const { return std::max(1u, width >> level); }
    std::uint32_t LevelHeight(std::uint32_t level) const { return std::max(1u, height >> level); }
    std::uint32_t LevelDepth(std::uint32_t level) const
    {
        return type == TextureType::Texture3D ? std::max(1u, layersOrDepth >> level) : 1;
    }
    std::vector<std::byte>* Subresource(std::uint32_t level, std::uint32_t layer)
    {
        if (level >= levels || layer >= Layers()) {
            return nullptr;
        }
        return &subresources[level * Layers() + layer];
    }
};

std::string Id(Kind kind, std::uint64_t value)
{
    return std::string(KindName(kind)) + "#" + std::to_string(value);
}

std::string Num(float value)
{
    char text[32];
    std::snprintf(text, sizeof(text), "%g", static_cast<double>(value));
    return text;
}

std::string ColorText(const Color& c)
{
    return "(" + Num(c.r) + "," + Num(c.g) + "," + Num(c.b) + "," + Num(c.a) + ")";
}

const char* LoadName(LoadOp op)
{
    switch (op) {
    case LoadOp::Load:
        return "load";
    case LoadOp::Clear:
        return "clear";
    case LoadOp::DontCare:
        return "dontcare";
    }
    return "?";
}

const char* StoreName(StoreOp op)
{
    return op == StoreOp::Store ? "store" : "dontcare";
}

// --- Texel encoding for clears ----------------------------------------------------------------------------------------

std::uint32_t Unorm(float value, std::uint32_t max)
{
    const float clamped = std::clamp(value, 0.0f, 1.0f);
    return static_cast<std::uint32_t>(std::lround(clamped * static_cast<float>(max)));
}

std::uint16_t Half(float value)
{
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(value);
    const std::uint32_t sign = (bits >> 16) & 0x8000u;
    const int exponent = static_cast<int>((bits >> 23) & 0xFF) - 127 + 15;
    const std::uint32_t mantissa = bits & 0x7FFFFFu;
    if (exponent <= 0) {
        return static_cast<std::uint16_t>(sign);   // zero (denormals flush)
    }
    if (exponent >= 31) {
        return static_cast<std::uint16_t>(sign | 0x7C00u);
    }
    return static_cast<std::uint16_t>(sign | (static_cast<std::uint32_t>(exponent) << 10) | (mantissa >> 13));
}

template <typename T>
void Put(std::vector<std::byte>& out, T value)
{
    const auto* p = reinterpret_cast<const std::byte*>(&value);
    out.insert(out.end(), p, p + sizeof(T));
}

// One texel of `format` holding the color (or depth/stencil), little endian; empty if the format cannot be a target.
std::vector<std::byte> EncodeColor(TextureFormat format, const Color& c)
{
    std::vector<std::byte> t;
    auto u8 = [&](float v) { t.push_back(static_cast<std::byte>(Unorm(v, 255))); };
    switch (format) {
    case TextureFormat::A8Unorm:
        u8(c.a);
        break;
    case TextureFormat::R8Unorm:
        u8(c.r);
        break;
    case TextureFormat::R8G8Unorm:
        u8(c.r);
        u8(c.g);
        break;
    case TextureFormat::R8G8B8A8Unorm:
    case TextureFormat::R8G8B8A8UnormSrgb:
        u8(c.r);
        u8(c.g);
        u8(c.b);
        u8(c.a);
        break;
    case TextureFormat::B8G8R8A8Unorm:
    case TextureFormat::B8G8R8A8UnormSrgb:
        u8(c.b);
        u8(c.g);
        u8(c.r);
        u8(c.a);
        break;
    case TextureFormat::B5G6R5Unorm:
        Put(t, static_cast<std::uint16_t>(Unorm(c.r, 31) << 11 | Unorm(c.g, 63) << 5 | Unorm(c.b, 31)));
        break;
    case TextureFormat::B5G5R5A1Unorm:
        Put(t, static_cast<std::uint16_t>(Unorm(c.a, 1) << 15 | Unorm(c.r, 31) << 10 | Unorm(c.g, 31) << 5 |
                                          Unorm(c.b, 31)));
        break;
    case TextureFormat::B4G4R4A4Unorm:
        Put(t, static_cast<std::uint16_t>(Unorm(c.a, 15) << 12 | Unorm(c.r, 15) << 8 | Unorm(c.g, 15) << 4 |
                                          Unorm(c.b, 15)));
        break;
    case TextureFormat::R16G16B16A16Float:
        Put(t, Half(c.r));
        Put(t, Half(c.g));
        Put(t, Half(c.b));
        Put(t, Half(c.a));
        break;
    case TextureFormat::R32Float:
        Put(t, c.r);
        break;
    default:
        break;
    }
    return t;
}

std::vector<std::byte> EncodeDepth(TextureFormat format, float depth, std::uint8_t stencil)
{
    std::vector<std::byte> t;
    switch (format) {
    case TextureFormat::D16Unorm:
        Put(t, static_cast<std::uint16_t>(Unorm(depth, 0xFFFF)));
        break;
    case TextureFormat::D24Unorm:
        Put(t, Unorm(depth, 0xFFFFFF));
        break;
    case TextureFormat::D24UnormS8Uint:
        Put(t, Unorm(depth, 0xFFFFFF) | static_cast<std::uint32_t>(stencil) << 24);
        break;
    case TextureFormat::D32Float:
        Put(t, depth);
        break;
    case TextureFormat::D32FloatS8Uint:
        Put(t, depth);
        Put(t, static_cast<std::uint32_t>(stencil));
        break;
    default:
        break;
    }
    return t;
}

void Fill(std::vector<std::byte>& bytes, const std::vector<std::byte>& texel)
{
    if (texel.empty()) {
        return;
    }
    for (std::size_t i = 0; i + texel.size() <= bytes.size(); i += texel.size()) {
        std::memcpy(bytes.data() + i, texel.data(), texel.size());
    }
}

// --- Region copies between a texture subresource and linear memory ----------------------------------------------------

struct RegionLayout {
    std::uint32_t blockBytes = 0;
    std::size_t texOffsetFirst = 0;
    std::size_t texRowBytes = 0, texRows = 0;
    std::size_t linRowBytes = 0, linRowsPerLayer = 0;
    std::size_t copyBytes = 0, copyRows = 0, depth = 0;
};

// False when the region does not fit the subresource.
bool Layout(const Resource& texture, std::uint32_t level, std::uint32_t x, std::uint32_t y, std::uint32_t z,
            std::uint32_t w, std::uint32_t h, std::uint32_t d, std::uint32_t pixelsPerRow, std::uint32_t rowsPerLayer,
            RegionLayout& out)
{
    const std::uint32_t block = IsBlockCompressed(texture.format) ? 4 : 1;
    const std::uint32_t lw = texture.LevelWidth(level), lh = texture.LevelHeight(level), ld = texture.LevelDepth(level);
    if (w == 0 || h == 0 || d == 0 || x + w > lw || y + h > lh || z + d > ld || x % block || y % block) {
        return false;
    }
    auto blocks = [block](std::uint32_t n) { return static_cast<std::size_t>((n + block - 1) / block); };
    out.blockBytes = TexelBlockSize(texture.format);
    out.texRowBytes = blocks(lw) * out.blockBytes;
    out.texRows = blocks(lh);
    out.texOffsetFirst = (static_cast<std::size_t>(z) * out.texRows + y / block) * out.texRowBytes +
                         static_cast<std::size_t>(x / block) * out.blockBytes;
    out.linRowBytes = blocks(pixelsPerRow ? pixelsPerRow : w) * out.blockBytes;
    out.linRowsPerLayer = blocks(rowsPerLayer ? rowsPerLayer : h);
    out.copyBytes = blocks(w) * out.blockBytes;
    out.copyRows = blocks(h);
    out.depth = d;
    return out.copyBytes <= out.linRowBytes && out.copyRows <= out.linRowsPerLayer;
}

std::size_t LinearSize(const RegionLayout& l)
{
    return ((l.depth - 1) * l.linRowsPerLayer + (l.copyRows - 1)) * l.linRowBytes + l.copyBytes;
}

void CopyRegion(std::vector<std::byte>& sub, std::byte* linear, const RegionLayout& l, bool toTexture)
{
    for (std::size_t zz = 0; zz < l.depth; ++zz) {
        for (std::size_t row = 0; row < l.copyRows; ++row) {
            std::byte* tex = sub.data() + l.texOffsetFirst + (zz * l.texRows + row) * l.texRowBytes;
            std::byte* lin = linear + (zz * l.linRowsPerLayer + row) * l.linRowBytes;
            if (toTexture) {
                std::memcpy(tex, lin, l.copyBytes);
            } else {
                std::memcpy(lin, tex, l.copyBytes);
            }
        }
    }
}

class NullCommandBuffer;

}  // namespace

namespace detail {

class NullDevice final : public Device {
public:
    explicit NullDevice(const DeviceOptions& options) : m_shaderFormats(options.shaderFormats) {}
    ~NullDevice() override;

    Backend GetBackend() const override { return Backend::Null; }
    std::string_view DriverName() const override { return "null"; }
    ShaderFormat ShaderFormats() const override { return m_shaderFormats; }
    const std::string& LastError() const override { return m_lastError; }

    bool ClaimWindow(SDL_Window* window) override;
    void ReleaseWindow(SDL_Window* window) override;
    bool WindowSupportsPresentMode(SDL_Window* window, PresentMode mode) override;
    bool SetSwapchainParameters(SDL_Window* window, PresentMode mode) override;
    TextureFormat SwapchainTextureFormat(SDL_Window* window) override;
    bool TextureSupportsFormat(TextureFormat format, TextureType type, TextureUsage usage) override;

    Buffer CreateBuffer(const BufferDesc& desc) override;
    TransferBuffer CreateTransferBuffer(const TransferBufferDesc& desc) override;
    Texture CreateTexture(const TextureDesc& desc) override;
    Sampler CreateSampler(const SamplerDesc& desc) override;
    Shader CreateShader(const ShaderDesc& desc) override;
    GraphicsPipeline CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override;

    void ReleaseBuffer(Buffer buffer) override { Release(Kind::Buffer, buffer.value); }
    void ReleaseTransferBuffer(TransferBuffer buffer) override { Release(Kind::TransferBuffer, buffer.value); }
    void ReleaseTexture(Texture texture) override { Release(Kind::Texture, texture.value); }
    void ReleaseSampler(Sampler sampler) override { Release(Kind::Sampler, sampler.value); }
    void ReleaseShader(Shader shader) override { Release(Kind::Shader, shader.value); }
    void ReleaseGraphicsPipeline(GraphicsPipeline pipeline) override { Release(Kind::Pipeline, pipeline.value); }

    void* MapTransferBuffer(TransferBuffer buffer, bool cycle) override;
    void UnmapTransferBuffer(TransferBuffer buffer) override;

    CommandBuffer* AcquireCommandBuffer() override;
    bool WaitIdle() override { return true; }

    // For NullCommandBuffer and the test view (null.h). Callers hold m_mutex where noted.
    std::mutex& Mutex() { return m_mutex; }
    Resource* FindLocked(Kind kind, std::uint64_t id);
    void FailLocked(std::string message) { m_lastError = std::move(message); }
    void Fail(std::string message)
    {
        std::lock_guard lock(m_mutex);
        FailLocked(std::move(message));
    }
    // Validates a handle at record time; false (and LastError) if it is not a live resource of that kind.
    bool Check(Kind kind, std::uint64_t id, const char* what);
    bool AcquireSwapchainTexture(SDL_Window* window, SwapchainTexture& swapchain);
    void Finish(NullCommandBuffer* commands);

    std::vector<std::string> m_commands;
    std::vector<null::PushedUniform> m_uniforms;
    std::unordered_map<std::uint64_t, Resource> m_resources;

private:
    std::uint64_t AddLocked(Resource resource);
    void Release(Kind kind, std::uint64_t id);

    ShaderFormat m_shaderFormats;
    std::mutex m_mutex;
    std::string m_lastError;
    std::uint64_t m_nextId = 1;
    std::unordered_map<SDL_Window*, std::uint64_t> m_windows;     // claimed window -> its swapchain texture (0: none)
    std::vector<std::unique_ptr<NullCommandBuffer>> m_free;
};

}  // namespace detail

namespace {

using detail::NullDevice;

// Commands recorded as (line, effect) and applied on Submit, in order.
class NullCommandBuffer final : public CommandBuffer {
public:
    explicit NullCommandBuffer(NullDevice& device) : m_device(device), m_render(*this), m_copy(*this) {}

    void Reset()
    {
        m_ops.clear();
        m_failed = false;
        m_inPass = false;
    }

    void Record(std::string line, std::function<void(NullDevice&)> effect = {})
    {
        m_ops.push_back({std::move(line), std::move(effect)});
    }
    // A misuse: recorded, and Submit fails.
    void Error(const std::string& message)
    {
        m_failed = true;
        m_device.Fail(message);
        Record("error: " + message);
    }
    bool Check(Kind kind, std::uint64_t id, const char* what)
    {
        if (m_device.Check(kind, id, what)) {
            return true;
        }
        Error(m_device.LastError());
        return false;
    }

    void PushVertexUniformData(std::uint32_t slot, std::span<const std::byte> data) override
    {
        PushUniform("PushVertexUniformData", ShaderStage::Vertex, slot, data);
    }
    void PushFragmentUniformData(std::uint32_t slot, std::span<const std::byte> data) override
    {
        PushUniform("PushFragmentUniformData", ShaderStage::Fragment, slot, data);
    }
    void PushUniform(const char* what, ShaderStage stage, std::uint32_t slot, std::span<const std::byte> data)
    {
        Record(std::string(what) + " slot=" + std::to_string(slot) + " bytes=" + std::to_string(data.size()),
               [uniform = null::PushedUniform{stage, slot, {data.begin(), data.end()}}](NullDevice& device) {
                   device.m_uniforms.push_back(uniform);
               });
    }

    RenderPass* BeginRenderPass(std::span<const ColorTargetInfo> colorTargets,
                                const DepthStencilTargetInfo* depthStencilTarget) override;
    CopyPass* BeginCopyPass() override;

    void GenerateMipmaps(Texture texture) override
    {
        if (Check(Kind::Texture, texture.value, "GenerateMipmaps")) {
            Record("GenerateMipmaps " + Id(Kind::Texture, texture.value));
        }
    }
    void PushDebugGroup(std::string_view name) override { Record("PushDebugGroup " + std::string(name)); }
    void PopDebugGroup() override { Record("PopDebugGroup"); }
    void InsertDebugLabel(std::string_view text) override { Record("InsertDebugLabel " + std::string(text)); }

    bool WaitAndAcquireSwapchainTexture(SDL_Window* window, SwapchainTexture& swapchain) override
    {
        if (!m_device.AcquireSwapchainTexture(window, swapchain)) {
            return false;
        }
        Record("AcquireSwapchainTexture " +
               (swapchain.texture ? Id(Kind::Texture, swapchain.texture.value) + " " + std::to_string(swapchain.width) +
                                        "x" + std::to_string(swapchain.height)
                                  : std::string("none")));
        return true;
    }

    bool Submit() override
    {
        if (m_inPass) {
            Error("Submit inside a pass");
        }
        const bool ok = !m_failed;
        {
            std::lock_guard lock(m_device.Mutex());
            for (auto& op : m_ops) {
                if (!op.line.empty()) {
                    m_device.m_commands.push_back(op.line);
                }
                if (ok && op.effect) {
                    op.effect(m_device);
                }
            }
            m_device.m_commands.push_back(ok ? "Submit" : "Submit failed");
        }
        m_device.Finish(this);
        return ok;
    }
    void Cancel() override { m_device.Finish(this); }

    class Render final : public RenderPass {
    public:
        explicit Render(NullCommandBuffer& owner) : m(owner) {}
        void BindGraphicsPipeline(GraphicsPipeline pipeline) override
        {
            if (m.Check(Kind::Pipeline, pipeline.value, "BindGraphicsPipeline")) {
                m.Record("BindGraphicsPipeline " + Id(Kind::Pipeline, pipeline.value));
            }
        }
        void SetViewport(const Viewport& v) override
        {
            m.Record("SetViewport " + Num(v.x) + "," + Num(v.y) + " " + Num(v.w) + "x" + Num(v.h) + " depth " +
                     Num(v.minDepth) + ".." + Num(v.maxDepth));
        }
        void SetScissor(const Rect& r) override
        {
            m.Record("SetScissor " + std::to_string(r.x) + "," + std::to_string(r.y) + " " + std::to_string(r.w) + "x" +
                     std::to_string(r.h));
        }
        void SetBlendConstants(const Color& color) override { m.Record("SetBlendConstants " + ColorText(color)); }
        void SetStencilReference(std::uint8_t reference) override
        {
            m.Record("SetStencilReference " + std::to_string(reference));
        }
        void BindVertexBuffers(std::uint32_t firstSlot, std::span<const BufferBinding> bindings) override
        {
            std::string line = "BindVertexBuffers";
            for (std::size_t i = 0; i < bindings.size(); ++i) {
                if (!m.Check(Kind::Buffer, bindings[i].buffer.value, "BindVertexBuffers")) {
                    return;
                }
                line += " [" + std::to_string(firstSlot + i) + "]=" + Id(Kind::Buffer, bindings[i].buffer.value) + "+" +
                        std::to_string(bindings[i].offset);
            }
            m.Record(line);
        }
        void BindIndexBuffer(const BufferBinding& binding, IndexElementSize size) override
        {
            if (m.Check(Kind::Buffer, binding.buffer.value, "BindIndexBuffer")) {
                m.Record("BindIndexBuffer " + Id(Kind::Buffer, binding.buffer.value) + "+" +
                         std::to_string(binding.offset) + (size == IndexElementSize::U16 ? " u16" : " u32"));
            }
        }
        void BindVertexSamplers(std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings) override
        {
            BindSamplers("BindVertexSamplers", firstSlot, bindings);
        }
        void BindFragmentSamplers(std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings) override
        {
            BindSamplers("BindFragmentSamplers", firstSlot, bindings);
        }
        void DrawPrimitives(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
                            std::uint32_t firstInstance) override
        {
            m.Record("DrawPrimitives vertices=" + std::to_string(vertexCount) + " instances=" +
                     std::to_string(instanceCount) + " first=" + std::to_string(firstVertex) + " firstInstance=" +
                     std::to_string(firstInstance));
        }
        void DrawIndexedPrimitives(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex,
                                   std::int32_t vertexOffset, std::uint32_t firstInstance) override
        {
            m.Record("DrawIndexedPrimitives indices=" + std::to_string(indexCount) + " instances=" +
                     std::to_string(instanceCount) + " first=" + std::to_string(firstIndex) + " vertexOffset=" +
                     std::to_string(vertexOffset) + " firstInstance=" + std::to_string(firstInstance));
        }
        void End() override
        {
            m.Record("EndRenderPass");
            m.m_inPass = false;
        }

    private:
        void BindSamplers(const char* what, std::uint32_t firstSlot, std::span<const TextureSamplerBinding> bindings)
        {
            std::string line = what;
            for (std::size_t i = 0; i < bindings.size(); ++i) {
                if (!m.Check(Kind::Texture, bindings[i].texture.value, what) ||
                    !m.Check(Kind::Sampler, bindings[i].sampler.value, what)) {
                    return;
                }
                line += " [" + std::to_string(firstSlot + i) + "]=" + Id(Kind::Texture, bindings[i].texture.value) +
                        "/" + Id(Kind::Sampler, bindings[i].sampler.value);
            }
            m.Record(line);
        }
        NullCommandBuffer& m;
    };

    class Copy final : public CopyPass {
    public:
        explicit Copy(NullCommandBuffer& owner) : m(owner) {}

        void UploadToTexture(const TextureTransferInfo& source, const TextureRegion& destination, bool cycle) override
        {
            m.Record(std::string("UploadToTexture ") + Id(Kind::TransferBuffer, source.transferBuffer.value) + "+" +
                     std::to_string(source.offset) + " -> " + Region(destination) + (cycle ? " cycle" : ""));
            TextureTransfer("UploadToTexture", source, destination, true);
        }
        void UploadToBuffer(const TransferBufferLocation& source, const BufferRegion& destination, bool cycle) override
        {
            m.Record(std::string("UploadToBuffer ") + Id(Kind::TransferBuffer, source.transferBuffer.value) + "+" +
                     std::to_string(source.offset) + " -> " + Id(Kind::Buffer, destination.buffer.value) + "+" +
                     std::to_string(destination.offset) + " size=" + std::to_string(destination.size) +
                     (cycle ? " cycle" : ""));
            BufferCopy("UploadToBuffer", Kind::TransferBuffer, source.transferBuffer.value, source.offset, Kind::Buffer,
                       destination.buffer.value, destination.offset, destination.size);
        }
        void CopyTextureToTexture(const TextureLocation& source, const TextureLocation& destination, std::uint32_t w,
                                  std::uint32_t h, std::uint32_t d, bool cycle) override
        {
            m.Record("CopyTextureToTexture " + Id(Kind::Texture, source.texture.value) + " -> " +
                     Id(Kind::Texture, destination.texture.value) + " " + std::to_string(w) + "x" + std::to_string(h) +
                     "x" + std::to_string(d) + (cycle ? " cycle" : ""));
            if (!m.Check(Kind::Texture, source.texture.value, "CopyTextureToTexture") ||
                !m.Check(Kind::Texture, destination.texture.value, "CopyTextureToTexture")) {
                return;
            }
            m.Record("", [=](NullDevice& device) {
                Resource* src = device.FindLocked(Kind::Texture, source.texture.value);
                Resource* dst = device.FindLocked(Kind::Texture, destination.texture.value);
                RegionLayout ls, ld;
                std::vector<std::byte>* subS = src ? src->Subresource(source.mipLevel, source.layer) : nullptr;
                std::vector<std::byte>* subD = dst ? dst->Subresource(destination.mipLevel, destination.layer) : nullptr;
                if (!subS || !subD || src->format != dst->format ||
                    !Layout(*src, source.mipLevel, source.x, source.y, source.z, w, h, d, 0, 0, ls) ||
                    !Layout(*dst, destination.mipLevel, destination.x, destination.y, destination.z, w, h, d, 0, 0, ld)) {
                    device.FailLocked("CopyTextureToTexture: region out of range or formats differ");
                    return;
                }
                std::vector<std::byte> temp(LinearSize(ls));
                CopyRegion(*subS, temp.data(), ls, false);
                CopyRegion(*subD, temp.data(), ld, true);
            });
        }
        void CopyBufferToBuffer(const BufferLocation& source, const BufferLocation& destination, std::uint32_t size,
                                bool cycle) override
        {
            m.Record("CopyBufferToBuffer " + Id(Kind::Buffer, source.buffer.value) + "+" + std::to_string(source.offset) +
                     " -> " + Id(Kind::Buffer, destination.buffer.value) + "+" + std::to_string(destination.offset) +
                     " size=" + std::to_string(size) + (cycle ? " cycle" : ""));
            BufferCopy("CopyBufferToBuffer", Kind::Buffer, source.buffer.value, source.offset, Kind::Buffer,
                       destination.buffer.value, destination.offset, size);
        }
        void DownloadFromTexture(const TextureRegion& source, const TextureTransferInfo& destination) override
        {
            m.Record("DownloadFromTexture " + Region(source) + " -> " +
                     Id(Kind::TransferBuffer, destination.transferBuffer.value) + "+" +
                     std::to_string(destination.offset));
            TextureTransfer("DownloadFromTexture", destination, source, false);
        }
        void DownloadFromBuffer(const BufferRegion& source, const TransferBufferLocation& destination) override
        {
            m.Record("DownloadFromBuffer " + Id(Kind::Buffer, source.buffer.value) + "+" + std::to_string(source.offset) +
                     " size=" + std::to_string(source.size) + " -> " +
                     Id(Kind::TransferBuffer, destination.transferBuffer.value) + "+" +
                     std::to_string(destination.offset));
            BufferCopy("DownloadFromBuffer", Kind::Buffer, source.buffer.value, source.offset, Kind::TransferBuffer,
                       destination.transferBuffer.value, destination.offset, source.size);
        }
        void End() override
        {
            m.Record("EndCopyPass");
            m.m_inPass = false;
        }

    private:
        static std::string Region(const TextureRegion& r)
        {
            return Id(Kind::Texture, r.texture.value) + " mip=" + std::to_string(r.mipLevel) + " layer=" +
                   std::to_string(r.layer) + " " + std::to_string(r.x) + "," + std::to_string(r.y) + "," +
                   std::to_string(r.z) + " " + std::to_string(r.w) + "x" + std::to_string(r.h) + "x" +
                   std::to_string(r.d);
        }

        void TextureTransfer(const char* what, const TextureTransferInfo& linear, const TextureRegion& region,
                             bool toTexture)
        {
            if (!m.Check(Kind::TransferBuffer, linear.transferBuffer.value, what) ||
                !m.Check(Kind::Texture, region.texture.value, what)) {
                return;
            }
            {
                // Validate now, so that misuse fails the submit instead of being skipped silently.
                std::lock_guard lock(m.m_device.Mutex());
                Resource* transfer = m.m_device.FindLocked(Kind::TransferBuffer, linear.transferBuffer.value);
                Resource* texture = m.m_device.FindLocked(Kind::Texture, region.texture.value);
                RegionLayout l;
                const bool fits = texture->Subresource(region.mipLevel, region.layer) &&
                                  Layout(*texture, region.mipLevel, region.x, region.y, region.z, region.w, region.h,
                                         region.d, linear.pixelsPerRow, linear.rowsPerLayer, l) &&
                                  linear.offset + LinearSize(l) <= transfer->bytes.size();
                const bool usage = transfer->transferUsage ==
                                   (toTexture ? TransferBufferUsage::Upload : TransferBufferUsage::Download);
                if (!fits) {
                    m.m_failed = true;
                    m.m_device.FailLocked(std::string(what) + ": region out of range");
                    m.Record(std::string("error: ") + what + ": region out of range");
                    return;
                }
                if (!usage) {
                    m.m_failed = true;
                    m.m_device.FailLocked(std::string(what) + ": wrong transfer buffer usage");
                    m.Record(std::string("error: ") + what + ": wrong transfer buffer usage");
                    return;
                }
            }
            m.Record("", [=](NullDevice& device) {
                Resource* transfer = device.FindLocked(Kind::TransferBuffer, linear.transferBuffer.value);
                Resource* texture = device.FindLocked(Kind::Texture, region.texture.value);
                if (!transfer || !texture) {
                    return;
                }
                RegionLayout l;
                Layout(*texture, region.mipLevel, region.x, region.y, region.z, region.w, region.h, region.d,
                       linear.pixelsPerRow, linear.rowsPerLayer, l);
                CopyRegion(*texture->Subresource(region.mipLevel, region.layer), transfer->bytes.data() + linear.offset,
                           l, toTexture);
            });
        }

        void BufferCopy(const char* what, Kind fromKind, std::uint64_t from, std::uint32_t fromOffset, Kind toKind,
                        std::uint64_t to, std::uint32_t toOffset, std::uint32_t size)
        {
            if (!m.Check(fromKind, from, what) || !m.Check(toKind, to, what)) {
                return;
            }
            {
                std::lock_guard lock(m.m_device.Mutex());
                const Resource* a = m.m_device.FindLocked(fromKind, from);
                const Resource* b = m.m_device.FindLocked(toKind, to);
                if (std::size_t(fromOffset) + size > a->bytes.size() || std::size_t(toOffset) + size > b->bytes.size()) {
                    m.m_failed = true;
                    m.m_device.FailLocked(std::string(what) + ": range out of bounds");
                    m.Record(std::string("error: ") + what + ": range out of bounds");
                    return;
                }
            }
            m.Record("", [=](NullDevice& device) {
                Resource* a = device.FindLocked(fromKind, from);
                Resource* b = device.FindLocked(toKind, to);
                if (a && b) {
                    std::memmove(b->bytes.data() + toOffset, a->bytes.data() + fromOffset, size);
                }
            });
        }

        NullCommandBuffer& m;
    };

private:
    struct Op {
        std::string line;
        std::function<void(NullDevice&)> effect;
    };

    NullDevice& m_device;
    Render m_render;
    Copy m_copy;
    std::vector<Op> m_ops;
    bool m_failed = false;
    bool m_inPass = false;
};

RenderPass* NullCommandBuffer::BeginRenderPass(std::span<const ColorTargetInfo> colorTargets,
                                               const DepthStencilTargetInfo* depthStencilTarget)
{
    if (m_inPass) {
        Error("BeginRenderPass inside a pass");
        return nullptr;
    }
    if (colorTargets.empty() && !depthStencilTarget) {
        Error("BeginRenderPass without targets");
        return nullptr;
    }
    std::string line = "BeginRenderPass";
    for (std::size_t i = 0; i < colorTargets.size(); ++i) {
        const ColorTargetInfo target = colorTargets[i];
        if (!Check(Kind::Texture, target.texture.value, "BeginRenderPass")) {
            return nullptr;
        }
        line += " color[" + std::to_string(i) + "]=" + Id(Kind::Texture, target.texture.value) + " load=" +
                LoadName(target.loadOp) + (target.loadOp == LoadOp::Clear ? ColorText(target.clearColor) : "") +
                " store=" + StoreName(target.storeOp);
    }
    if (depthStencilTarget) {
        if (!Check(Kind::Texture, depthStencilTarget->texture.value, "BeginRenderPass")) {
            return nullptr;
        }
        line += " depth=" + Id(Kind::Texture, depthStencilTarget->texture.value) + " load=" +
                LoadName(depthStencilTarget->loadOp) +
                (depthStencilTarget->loadOp == LoadOp::Clear ? "(" + Num(depthStencilTarget->clearDepth) + ")" : "") +
                " store=" + StoreName(depthStencilTarget->storeOp);
    }
    std::vector<ColorTargetInfo> colors(colorTargets.begin(), colorTargets.end());
    std::optional<DepthStencilTargetInfo> depth;
    if (depthStencilTarget) {
        depth = *depthStencilTarget;
    }
    Record(line, [colors = std::move(colors), depth](NullDevice& device) {
        for (const ColorTargetInfo& target : colors) {
            if (target.loadOp != LoadOp::Clear) {
                continue;
            }
            Resource* texture = device.FindLocked(Kind::Texture, target.texture.value);
            const bool volume = texture && texture->type == TextureType::Texture3D;
            std::vector<std::byte>* sub =
                texture ? texture->Subresource(target.mipLevel, volume ? 0 : target.layerOrDepthPlane) : nullptr;
            if (!sub) {
                continue;
            }
            const std::vector<std::byte> texel = EncodeColor(texture->format, target.clearColor);
            if (volume) {
                RegionLayout l;
                const std::uint32_t lw = texture->LevelWidth(target.mipLevel), lh = texture->LevelHeight(target.mipLevel);
                if (Layout(*texture, target.mipLevel, 0, 0, target.layerOrDepthPlane, lw, lh, 1, 0, 0, l)) {
                    std::vector<std::byte> slice(LinearSize(l));
                    Fill(slice, texel);
                    CopyRegion(*sub, slice.data(), l, true);
                }
            } else {
                Fill(*sub, texel);
            }
        }
        if (depth && (depth->loadOp == LoadOp::Clear || depth->stencilLoadOp == LoadOp::Clear)) {
            Resource* texture = device.FindLocked(Kind::Texture, depth->texture.value);
            if (std::vector<std::byte>* sub = texture ? texture->Subresource(0, 0) : nullptr) {
                Fill(*sub, EncodeDepth(texture->format, depth->clearDepth, depth->clearStencil));
            }
        }
    });
    m_inPass = true;
    return &m_render;
}

CopyPass* NullCommandBuffer::BeginCopyPass()
{
    if (m_inPass) {
        Error("BeginCopyPass inside a pass");
        return nullptr;
    }
    Record("BeginCopyPass");
    m_inPass = true;
    return &m_copy;
}

}  // namespace

namespace detail {

NullDevice::~NullDevice() = default;

std::uint64_t NullDevice::AddLocked(Resource resource)
{
    const std::uint64_t id = m_nextId++;
    m_resources.emplace(id, std::move(resource));
    return id;
}

Resource* NullDevice::FindLocked(Kind kind, std::uint64_t id)
{
    auto it = m_resources.find(id);
    return it != m_resources.end() && it->second.kind == kind ? &it->second : nullptr;
}

bool NullDevice::Check(Kind kind, std::uint64_t id, const char* what)
{
    std::lock_guard lock(m_mutex);
    if (FindLocked(kind, id)) {
        return true;
    }
    FailLocked(std::string(what) + ": " + Id(kind, id) + " is not a live " + KindName(kind));
    return false;
}

void NullDevice::Release(Kind kind, std::uint64_t id)
{
    if (id == 0) {
        return;
    }
    std::lock_guard lock(m_mutex);
    Resource* resource = FindLocked(kind, id);
    if (!resource || resource->swapchain) {
        FailLocked("Release: " + Id(kind, id) + " is not a live " + KindName(kind));
        return;
    }
    m_resources.erase(id);
}

bool NullDevice::ClaimWindow(SDL_Window* window)
{
    std::lock_guard lock(m_mutex);
    if (!window) {
        FailLocked("ClaimWindow: no window");
        return false;
    }
    if (!m_windows.emplace(window, 0).second) {
        FailLocked("ClaimWindow: window already claimed");
        return false;
    }
    return true;
}

void NullDevice::ReleaseWindow(SDL_Window* window)
{
    std::lock_guard lock(m_mutex);
    auto it = m_windows.find(window);
    if (it == m_windows.end()) {
        return;
    }
    m_resources.erase(it->second);
    m_windows.erase(it);
}

bool NullDevice::WindowSupportsPresentMode(SDL_Window* window, PresentMode /*mode*/)
{
    std::lock_guard lock(m_mutex);
    return m_windows.contains(window);
}

bool NullDevice::SetSwapchainParameters(SDL_Window* window, PresentMode /*mode*/)
{
    std::lock_guard lock(m_mutex);
    if (!m_windows.contains(window)) {
        FailLocked("SetSwapchainParameters: window not claimed");
        return false;
    }
    return true;
}

TextureFormat NullDevice::SwapchainTextureFormat(SDL_Window* window)
{
    std::lock_guard lock(m_mutex);
    return m_windows.contains(window) ? TextureFormat::B8G8R8A8Unorm : TextureFormat::Invalid;
}

bool NullDevice::TextureSupportsFormat(TextureFormat format, TextureType type, TextureUsage usage)
{
    if (format == TextureFormat::Invalid) {
        return false;
    }
    if (HasFlag(usage, TextureUsage::ColorTarget) && (IsBlockCompressed(format) || IsDepthFormat(format))) {
        return false;
    }
    if (HasFlag(usage, TextureUsage::DepthStencilTarget) && (!IsDepthFormat(format) || type == TextureType::Texture3D)) {
        return false;
    }
    return true;
}

bool NullDevice::AcquireSwapchainTexture(SDL_Window* window, SwapchainTexture& swapchain)
{
    swapchain = {};
    int w = 0, h = 0;
    const bool minimized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) != 0;
    if (!SDL_GetWindowSizeInPixels(window, &w, &h)) {
        w = h = 0;
    }
    std::lock_guard lock(m_mutex);
    auto it = m_windows.find(window);
    if (it == m_windows.end()) {
        FailLocked("WaitAndAcquireSwapchainTexture: window not claimed");
        return false;
    }
    if (minimized || w <= 0 || h <= 0) {
        return true;
    }
    Resource* texture = FindLocked(Kind::Texture, it->second);
    if (!texture || texture->width != static_cast<std::uint32_t>(w) || texture->height != static_cast<std::uint32_t>(h)) {
        m_resources.erase(it->second);
        Resource r;
        r.kind = Kind::Texture;
        r.name = "swapchain";
        r.swapchain = true;
        r.format = TextureFormat::B8G8R8A8Unorm;
        r.usage = TextureUsage::ColorTarget;
        r.width = static_cast<std::uint32_t>(w);
        r.height = static_cast<std::uint32_t>(h);
        r.subresources.emplace_back(TextureFormatSize(r.format, r.width, r.height, 1));
        it->second = AddLocked(std::move(r));
    }
    swapchain.texture.value = it->second;
    swapchain.width = static_cast<std::uint32_t>(w);
    swapchain.height = static_cast<std::uint32_t>(h);
    return true;
}

Buffer NullDevice::CreateBuffer(const BufferDesc& desc)
{
    std::lock_guard lock(m_mutex);
    if (desc.size == 0 || desc.usage == BufferUsage::None) {
        FailLocked("CreateBuffer: no size or usage");
        return {};
    }
    Resource r;
    r.kind = Kind::Buffer;
    r.name = desc.name;
    r.bytes.resize(desc.size);
    return {AddLocked(std::move(r))};
}

TransferBuffer NullDevice::CreateTransferBuffer(const TransferBufferDesc& desc)
{
    std::lock_guard lock(m_mutex);
    if (desc.size == 0) {
        FailLocked("CreateTransferBuffer: no size");
        return {};
    }
    Resource r;
    r.kind = Kind::TransferBuffer;
    r.transferUsage = desc.usage;
    r.bytes.resize(desc.size);
    return {AddLocked(std::move(r))};
}

Texture NullDevice::CreateTexture(const TextureDesc& desc)
{
    if (desc.width == 0 || desc.height == 0 || desc.layerCountOrDepth == 0 || desc.levelCount == 0 ||
        !TextureSupportsFormat(desc.format, desc.type, desc.usage) ||
        (desc.type == TextureType::Cube && desc.layerCountOrDepth != 6) ||
        (desc.type == TextureType::CubeArray && desc.layerCountOrDepth % 6 != 0) ||
        (desc.type == TextureType::Texture2D && desc.layerCountOrDepth != 1)) {
        Fail("CreateTexture: invalid description");
        return {};
    }
    std::lock_guard lock(m_mutex);
    Resource r;
    r.kind = Kind::Texture;
    r.name = desc.name;
    r.type = desc.type;
    r.format = desc.format;
    r.usage = desc.usage;
    r.width = desc.width;
    r.height = desc.height;
    r.layersOrDepth = desc.layerCountOrDepth;
    r.levels = desc.levelCount;
    for (std::uint32_t level = 0; level < r.levels; ++level) {
        for (std::uint32_t layer = 0; layer < r.Layers(); ++layer) {
            r.subresources.emplace_back(
                TextureFormatSize(r.format, r.LevelWidth(level), r.LevelHeight(level), r.LevelDepth(level)));
        }
    }
    return {AddLocked(std::move(r))};
}

Sampler NullDevice::CreateSampler(const SamplerDesc& /*desc*/)
{
    std::lock_guard lock(m_mutex);
    Resource r;
    r.kind = Kind::Sampler;
    return {AddLocked(std::move(r))};
}

Shader NullDevice::CreateShader(const ShaderDesc& desc)
{
    std::lock_guard lock(m_mutex);
    const auto format = static_cast<std::uint32_t>(desc.format);
    if (desc.code.empty() || !std::has_single_bit(format) || !HasFlag(m_shaderFormats, desc.format)) {
        FailLocked("CreateShader: no code, or a format the device was not created for");
        return {};
    }
    Resource r;
    r.kind = Kind::Shader;
    r.stage = desc.stage;
    return {AddLocked(std::move(r))};
}

GraphicsPipeline NullDevice::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
{
    std::lock_guard lock(m_mutex);
    const Resource* vs = FindLocked(Kind::Shader, desc.vertexShader.value);
    const Resource* fs = FindLocked(Kind::Shader, desc.fragmentShader.value);
    if (!vs || !fs || vs->stage != ShaderStage::Vertex || fs->stage != ShaderStage::Fragment) {
        FailLocked("CreateGraphicsPipeline: needs a vertex and a fragment shader");
        return {};
    }
    for (const VertexAttribute& attribute : desc.vertexAttributes) {
        const bool slot = std::any_of(desc.vertexBuffers.begin(), desc.vertexBuffers.end(),
                                      [&](const VertexBufferDesc& b) { return b.slot == attribute.bufferSlot; });
        if (!slot || attribute.format == VertexElementFormat::Invalid) {
            FailLocked("CreateGraphicsPipeline: attribute without a vertex buffer or a format");
            return {};
        }
    }
    Resource r;
    r.kind = Kind::Pipeline;
    r.name = desc.name;
    return {AddLocked(std::move(r))};
}

void* NullDevice::MapTransferBuffer(TransferBuffer buffer, bool /*cycle*/)
{
    std::lock_guard lock(m_mutex);
    Resource* r = FindLocked(Kind::TransferBuffer, buffer.value);
    if (!r) {
        FailLocked("MapTransferBuffer: " + Id(Kind::TransferBuffer, buffer.value) + " is not a live transfer buffer");
        return nullptr;
    }
    r->mapped = true;
    return r->bytes.data();
}

void NullDevice::UnmapTransferBuffer(TransferBuffer buffer)
{
    std::lock_guard lock(m_mutex);
    if (Resource* r = FindLocked(Kind::TransferBuffer, buffer.value)) {
        r->mapped = false;
    }
}

CommandBuffer* NullDevice::AcquireCommandBuffer()
{
    std::lock_guard lock(m_mutex);
    std::unique_ptr<NullCommandBuffer> commands;
    if (m_free.empty()) {
        commands = std::make_unique<NullCommandBuffer>(*this);
    } else {
        commands = std::move(m_free.back());
        m_free.pop_back();
    }
    commands->Reset();
    return commands.release();     // back in m_free on Finish
}

void NullDevice::Finish(NullCommandBuffer* commands)
{
    std::lock_guard lock(m_mutex);
    m_free.emplace_back(commands);
}

std::unique_ptr<Device> CreateNullDevice(const DeviceOptions& options, std::string& /*error*/)
{
    return std::make_unique<NullDevice>(options);
}

}  // namespace detail

namespace null {

namespace {

detail::NullDevice* AsNull(Device& device)
{
    return dynamic_cast<detail::NullDevice*>(&device);
}

}  // namespace

const std::vector<std::string>& Commands(Device& device)
{
    static const std::vector<std::string> none;
    detail::NullDevice* null = AsNull(device);
    return null ? null->m_commands : none;
}

void ClearCommands(Device& device)
{
    if (detail::NullDevice* null = AsNull(device)) {
        std::lock_guard lock(null->Mutex());
        null->m_commands.clear();
        null->m_uniforms.clear();
    }
}

const std::vector<PushedUniform>& PushedUniforms(Device& device)
{
    static const std::vector<PushedUniform> none;
    detail::NullDevice* null = AsNull(device);
    return null ? null->m_uniforms : none;
}

std::span<const std::byte> TextureBytes(Device& device, Texture texture)
{
    detail::NullDevice* null = AsNull(device);
    if (!null) {
        return {};
    }
    std::lock_guard lock(null->Mutex());
    Resource* r = null->FindLocked(Kind::Texture, texture.value);
    return r ? std::span<const std::byte>(r->subresources.front()) : std::span<const std::byte>();
}

std::span<const std::byte> BufferBytes(Device& device, Buffer buffer)
{
    detail::NullDevice* null = AsNull(device);
    if (!null) {
        return {};
    }
    std::lock_guard lock(null->Mutex());
    Resource* r = null->FindLocked(Kind::Buffer, buffer.value);
    return r ? std::span<const std::byte>(r->bytes) : std::span<const std::byte>();
}

std::size_t LiveResourceCount(Device& device)
{
    detail::NullDevice* null = AsNull(device);
    if (!null) {
        return 0;
    }
    std::lock_guard lock(null->Mutex());
    return static_cast<std::size_t>(std::count_if(null->m_resources.begin(), null->m_resources.end(),
                                                  [](const auto& entry) { return !entry.second.swapchain; }));
}

}  // namespace null

}  // namespace x2::render
