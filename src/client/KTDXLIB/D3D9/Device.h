#pragma once

// KTDXLIB's D3D9 device (decision T38): the D3D9 calls the engine makes, translated onto x2::render (T36 (5)). Not
// COM: no reference counts, no HRESULTs, no surfaces as objects, no Reset or lost devices. Methods keep D3D9's names
// and arguments where the semantics are the same, so the engine's calls are rewritten with little change, and D3D9's
// type names (LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9...) alias these classes.
//
// How it works:
//  - Render, texture stage and sampler states are cached as D3D9 values. A draw turns the states that live in a
//    pipeline (blending, depth/stencil, culling, the vertex layout, the targets' formats) into a key and takes the
//    pipeline from a cache; the rest is dynamic state (viewport, stencil reference, blend factor) or uniforms.
//  - Fixed function runs on generated shaders (D3D9/shaders): a vertex shader per FVF shape and one fragment shader
//    that interprets two texture stages and the alpha test from uniforms.
//  - D3D9's pixel centers are on integers; the vertex shader moves them half a pixel.
//  - Triangle fans become indexed triangle lists (a shared fan index buffer).
//  - A frame is two command buffers: uploads (texture and vertex buffer unlocks, DrawPrimitiveUP data) and draws,
//    submitted in that order at Present. Unlocks record their upload at once, cycling the resource when D3D9 would
//    give the caller fresh memory, so draws recorded earlier keep the old contents.
//
// See docs/porting/client-d3d9.md for what is emulated and what is not yet.

#include "D3D9Types.h"

#include "x2/render/render.h"

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct SDL_Window;

namespace x2::d3d9 {

class Device;

inline constexpr DWORD kTextureStages = 2;      // stages the fixed-function shader runs (D3D9 has 8)

// IDirect3DTexture9 without COM. Lockable textures (managed, system memory, dynamic) keep a CPU copy of every level
// in their D3D9 format; UnlockRect uploads the level. Render targets have no CPU copy.
class Texture {
public:
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    DWORD GetLevelCount() const { return static_cast<DWORD>(m_levels.size()); }
    bool GetLevelDesc(UINT level, D3DSURFACE_DESC* desc) const;
    // rect: null for the whole level. D3DLOCK_READONLY: no upload at UnlockRect.
    bool LockRect(UINT level, D3DLOCKED_RECT* locked, const RECT* rect, DWORD flags);
    bool UnlockRect(UINT level);

    D3DFORMAT Format() const { return m_format; }
    DWORD Usage() const { return m_usage; }
    UINT Width() const { return m_width; }
    UINT Height() const { return m_height; }
    // The x2::render texture behind it (tests, readback).
    render::Texture RenderTexture() const { return m_texture; }
    render::TextureFormat RenderFormat() const { return m_renderFormat; }

private:
    friend class Device;
    Texture() = default;

    struct Level {
        UINT width = 0, height = 0;
        std::vector<std::byte> bytes;       // D3D9 format, tightly packed (lockable textures only)
        bool locked = false;
        bool readOnly = false;
    };

    Device* m_device = nullptr;
    render::Texture m_texture;
    render::TextureFormat m_renderFormat = render::TextureFormat::Invalid;
    D3DFORMAT m_format = D3DFMT_UNKNOWN;
    DWORD m_usage = 0;
    D3DPOOL m_pool = D3DPOOL_MANAGED;
    UINT m_width = 0, m_height = 0;
    std::vector<Level> m_levels;
};

// IDirect3DVertexBuffer9 without COM: a GPU buffer with a CPU copy. Unlock uploads what was locked; D3DLOCK_DISCARD
// (and a lock with no flags, which must keep the rest) cycles the GPU buffer, D3DLOCK_NOOVERWRITE does not.
class VertexBuffer {
public:
    ~VertexBuffer();
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    bool Lock(UINT offset, UINT size, void** data, DWORD flags);
    bool Unlock();
    UINT Size() const { return static_cast<UINT>(m_bytes.size()); }
    DWORD Fvf() const { return m_fvf; }
    render::Buffer RenderBuffer() const { return m_buffer; }

private:
    friend class Device;
    VertexBuffer() = default;

    Device* m_device = nullptr;
    render::Buffer m_buffer;
    std::vector<std::byte> m_bytes;
    DWORD m_usage = 0;
    DWORD m_fvf = 0;
    UINT m_lockOffset = 0, m_lockSize = 0;
    DWORD m_lockFlags = 0;
    bool m_locked = false;
};

// The pipeline state of a draw as D3D9 values, before translation. Public for tests.
struct PipelineKey {
    std::array<std::uint32_t, 28> words{};
    friend bool operator==(const PipelineKey&, const PipelineKey&) = default;
};

struct PipelineKeyHash {
    std::size_t operator()(const PipelineKey& key) const;
};

class Device {
public:
    // The window must already be claimed by `render` (DXUT's WindowClaim); null for a device that only renders into
    // its own render targets (tests).
    Device(render::Device& render, SDL_Window* window);
    ~Device();
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    // False if the shaders or the device's own resources could not be created (LastError()).
    bool Ok() const { return m_ok; }
    const std::string& LastError() const { return m_error; }
    render::Device& RenderDevice() { return m_render; }

    // --- Frame ---
    // BeginScene acquires the frame's command buffer and back buffer; Present submits the frame. Present without a
    // scene still presents a frame cleared to black. Draws outside a scene fail, as in D3D9.
    bool BeginScene();
    bool EndScene();
    bool Present();
    // rects: null or count 0 for the whole viewport. A clear of the whole target becomes the next render pass's load
    // operation; anything smaller is drawn.
    bool Clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);

    // --- States ---
    void SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
    DWORD GetRenderState(D3DRENDERSTATETYPE state) const;
    void SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);
    DWORD GetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type) const;
    void SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value);
    DWORD GetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type) const;
    void SetTexture(DWORD stage, Texture* texture);
    Texture* GetTexture(DWORD stage) const;
    void SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix);
    void GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX* matrix) const;
    void SetViewport(const D3DVIEWPORT9* viewport);
    void GetViewport(D3DVIEWPORT9* viewport) const;
    void SetFVF(DWORD fvf);
    DWORD GetFVF() const { return m_fvf; }
    void SetStreamSource(UINT stream, VertexBuffer* buffer, UINT offset, UINT stride);

    // Null: the back buffer. Resets the viewport to the whole target, as D3D9 does.
    bool SetRenderTarget(DWORD index, Texture* target);
    Texture* GetRenderTarget() const { return m_target; }

    // --- Draws ---
    bool DrawPrimitive(D3DPRIMITIVETYPE type, UINT startVertex, UINT primitiveCount);
    // Leaves stream 0 unset afterwards, as D3D9 does.
    bool DrawPrimitiveUP(D3DPRIMITIVETYPE type, UINT primitiveCount, const void* vertices, UINT stride);

    // --- Resources ---
    // levels 0: the whole mip chain. Null on failure (LastError()).
    std::unique_ptr<Texture> CreateTexture(UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format,
                                           D3DPOOL pool);
    std::unique_ptr<VertexBuffer> CreateVertexBuffer(UINT length, DWORD usage, DWORD fvf, D3DPOOL pool);

    // The back buffer's size this frame (0 x 0 while minimized or before the first scene).
    UINT BackBufferWidth() const { return m_backBuffer.width; }
    UINT BackBufferHeight() const { return m_backBuffer.height; }
    std::size_t PipelineCount() const { return m_pipelines.size(); }

    // Builds the key a draw with these states would use (tests).
    PipelineKey MakePipelineKey(D3DPRIMITIVETYPE type, DWORD fvf, UINT stride) const;

private:
    friend class Texture;
    friend class VertexBuffer;

    struct Target {
        render::Texture color;
        render::TextureFormat colorFormat = render::TextureFormat::Invalid;
        UINT width = 0, height = 0;
    };

    struct PendingClear {
        bool color = false, depth = false, stencil = false;
        render::Color colorValue;
        float depthValue = 1.0f;
        std::uint8_t stencilValue = 0;
    };

    struct UploadChunk {
        render::TransferBuffer buffer;
        std::uint32_t size = 0;
        std::uint32_t used = 0;
        bool mappedThisBatch = false;
    };

    // Uploads
    bool UploadToBuffer(render::Buffer buffer, std::uint32_t offset, std::span<const std::byte> bytes, bool cycle);
    bool UploadToTexture(render::Texture texture, std::uint32_t level, std::uint32_t width, std::uint32_t height,
                         std::span<const std::byte> bytes, bool cycle);
    std::optional<render::TransferBufferLocation> Stage(std::span<const std::byte> bytes);
    render::CopyPass* UploadPass();
    bool FlushUploads();

    // Frame
    bool EnsureFrame();
    render::RenderPass* EnsurePass();
    void EndPass();
    Target CurrentTarget() const;
    bool SubmitFrame();

    // Draws
    bool Draw(D3DPRIMITIVETYPE type, UINT primitiveCount, render::Buffer buffer, UINT offset, UINT stride,
              UINT firstVertex, DWORD fvf);
    render::GraphicsPipeline Pipeline(const PipelineKey& key, DWORD fvf, UINT stride);
    render::Sampler SamplerFor(DWORD stage);
    void PushUniforms(DWORD fvf);
    bool ClearRects(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);

    void ForgetTexture(Texture* texture);
    void ForgetVertexBuffer(VertexBuffer* buffer);
    bool Fail(std::string message);

    render::Device& m_render;
    SDL_Window* m_window;
    bool m_ok = false;
    std::string m_error;
    render::ShaderFormat m_shaderFormat = render::ShaderFormat::None;

    // States (D3D9 values)
    std::array<DWORD, X2_D3DRS_COUNT> m_renderStates{};
    std::array<std::array<DWORD, X2_D3DTSS_COUNT>, kTextureStages> m_stageStates{};
    std::array<std::array<DWORD, X2_D3DSAMP_COUNT>, kTextureStages> m_samplerStates{};
    std::array<Texture*, kTextureStages> m_textures{};
    D3DMATRIX m_world{}, m_view{}, m_projection{};
    D3DVIEWPORT9 m_viewport{};
    DWORD m_fvf = 0;
    VertexBuffer* m_stream = nullptr;
    UINT m_streamOffset = 0, m_streamStride = 0;
    Texture* m_target = nullptr;

    // Frame
    render::CommandBuffer* m_frame = nullptr;
    render::RenderPass* m_pass = nullptr;
    render::SwapchainTexture m_backBuffer;
    render::TextureFormat m_backBufferFormat = render::TextureFormat::Invalid;
    render::Texture m_depth;
    render::TextureFormat m_depthFormat = render::TextureFormat::Invalid;
    UINT m_depthWidth = 0, m_depthHeight = 0;
    PendingClear m_pendingClear;
    bool m_inScene = false;

    // Uploads
    render::CommandBuffer* m_uploads = nullptr;
    render::CopyPass* m_uploadPass = nullptr;
    std::vector<UploadChunk> m_chunks;
    render::Buffer m_dynamicVertices;           // DrawPrimitiveUP ring
    std::uint32_t m_dynamicOffset = 0;

    // Device resources
    std::vector<render::Shader> m_vertexShaders;   // GeneratedShader order (kFixedFunctionVertex)
    render::Shader m_fragmentShader;
    render::Texture m_white;
    render::Buffer m_fanIndices;
    std::unordered_map<PipelineKey, render::GraphicsPipeline, PipelineKeyHash> m_pipelines;
    std::unordered_map<std::uint64_t, render::Sampler> m_samplers;
    std::unordered_set<Texture*> m_liveTextures;         // detached if they outlive the device
    std::unordered_set<VertexBuffer*> m_liveVertexBuffers;
};

}  // namespace x2::d3d9

// D3D9's type names, for the engine's declarations (T38).
using IDirect3DDevice9 = x2::d3d9::Device;
using LPDIRECT3DDEVICE9 = x2::d3d9::Device*;
using IDirect3DTexture9 = x2::d3d9::Texture;
using LPDIRECT3DTEXTURE9 = x2::d3d9::Texture*;
using IDirect3DBaseTexture9 = x2::d3d9::Texture;    // there are only 2D textures (no cube or volume textures)
using LPDIRECT3DBASETEXTURE9 = x2::d3d9::Texture*;
using IDirect3DVertexBuffer9 = x2::d3d9::VertexBuffer;
using LPDIRECT3DVERTEXBUFFER9 = x2::d3d9::VertexBuffer*;
