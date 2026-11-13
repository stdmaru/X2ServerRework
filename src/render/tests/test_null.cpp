// x2::render on the null backend: handles, swapchain, clears, transfers and readback, with no GPU.

#include "x2/render/null.h"

#include <SDL3/SDL.h>
#include <doctest/doctest.h>

#include <algorithm>
#include <cstring>
#include <vector>

using namespace x2::render;

namespace {

std::unique_ptr<Device> NullDevice()
{
    std::string error;
    DeviceOptions options;
    options.backend = Backend::Null;
    auto device = Device::Create(options, error);
    REQUIRE_MESSAGE(device, error);
    return device;
}

bool Contains(const std::vector<std::string>& lines, const std::string& text)
{
    return std::any_of(lines.begin(), lines.end(), [&](const std::string& l) { return l.find(text) != std::string::npos; });
}

struct DummyWindow {
    DummyWindow(int w, int h)
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));
        window = SDL_CreateWindow("render", w, h, 0);
        REQUIRE(window);
    }
    ~DummyWindow()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    SDL_Window* window = nullptr;
};

void Upload(Device& device, Texture texture, std::uint32_t w, std::uint32_t h, const std::vector<std::byte>& bytes)
{
    TransferBuffer transfer = device.CreateTransferBuffer({TransferBufferUsage::Upload, static_cast<std::uint32_t>(bytes.size())});
    REQUIRE(transfer);
    std::memcpy(device.MapTransferBuffer(transfer, false), bytes.data(), bytes.size());
    device.UnmapTransferBuffer(transfer);
    CommandBuffer* commands = device.AcquireCommandBuffer();
    CopyPass* copy = commands->BeginCopyPass();
    REQUIRE(copy);
    copy->UploadToTexture({transfer}, {.texture = texture, .w = w, .h = h}, false);
    copy->End();
    REQUIRE(commands->Submit());
    device.ReleaseTransferBuffer(transfer);
}

std::vector<std::byte> Download(Device& device, const TextureRegion& region, std::uint32_t size)
{
    TransferBuffer transfer = device.CreateTransferBuffer({TransferBufferUsage::Download, size});
    CommandBuffer* commands = device.AcquireCommandBuffer();
    CopyPass* copy = commands->BeginCopyPass();
    copy->DownloadFromTexture(region, {transfer});
    copy->End();
    REQUIRE(commands->Submit());
    REQUIRE(device.WaitIdle());
    const auto* data = static_cast<const std::byte*>(device.MapTransferBuffer(transfer, false));
    std::vector<std::byte> out(data, data + size);
    device.UnmapTransferBuffer(transfer);
    device.ReleaseTransferBuffer(transfer);
    return out;
}

std::vector<std::byte> Bytes(std::initializer_list<int> values)
{
    std::vector<std::byte> out;
    for (int v : values)
        out.push_back(static_cast<std::byte>(v));
    return out;
}

}  // namespace

TEST_CASE("render: backend names and format sizes")
{
    Backend backend = Backend::SdlGpu;
    CHECK(ParseBackend("null", backend));
    CHECK(backend == Backend::Null);
    CHECK(ParseBackend(BackendName(Backend::SdlGpu), backend));
    CHECK(backend == Backend::SdlGpu);
    CHECK_FALSE(ParseBackend("d3d9", backend));

    CHECK(TextureFormatSize(TextureFormat::B8G8R8A8Unorm, 3, 2, 1) == 24);
    CHECK(TextureFormatSize(TextureFormat::B5G6R5Unorm, 4, 4, 1) == 32);
    CHECK(TextureFormatSize(TextureFormat::Bc1Unorm, 8, 8, 1) == 32);     // 2x2 blocks of 8 bytes
    CHECK(TextureFormatSize(TextureFormat::Bc3Unorm, 2, 2, 1) == 16);     // one block, even below 4x4
    CHECK(IsBlockCompressed(TextureFormat::Bc2Unorm));
    CHECK(IsDepthFormat(TextureFormat::D24UnormS8Uint));
    CHECK_FALSE(IsDepthFormat(TextureFormat::R32Float));
}

TEST_CASE("render: null device creates and releases resources")
{
    auto device = NullDevice();
    CHECK(device->GetBackend() == Backend::Null);
    CHECK(std::string(device->DriverName()) == "null");

    Buffer vertices = device->CreateBuffer({BufferUsage::Vertex, 256, "vertices"});
    Texture texture = device->CreateTexture({.width = 4, .height = 4});
    Sampler sampler = device->CreateSampler({});
    CHECK(vertices);
    CHECK(texture);
    CHECK(sampler);
    CHECK(null::LiveResourceCount(*device) == 3);
    CHECK(null::BufferBytes(*device, vertices).size() == 256);

    CHECK_FALSE(device->CreateBuffer({BufferUsage::Vertex, 0}));
    CHECK_FALSE(device->LastError().empty());
    CHECK_FALSE(device->CreateTexture({.format = TextureFormat::Bc1Unorm, .usage = TextureUsage::ColorTarget,
                                       .width = 4, .height = 4}));

    device->ReleaseBuffer(vertices);
    device->ReleaseTexture(texture);
    device->ReleaseSampler(sampler);
    CHECK(null::LiveResourceCount(*device) == 0);
}

TEST_CASE("render: null pipelines need a vertex and a fragment shader")
{
    auto device = NullDevice();
    const std::byte code[4] = {};
    Shader vs = device->CreateShader({.code = code, .stage = ShaderStage::Vertex, .uniformBufferCount = 1});
    Shader fs = device->CreateShader({.code = code, .stage = ShaderStage::Fragment, .samplerCount = 1});
    REQUIRE(vs);
    REQUIRE(fs);
    CHECK_FALSE(device->CreateShader({.code = {}, .stage = ShaderStage::Vertex}));

    const VertexBufferDesc buffer{0, 24, VertexInputRate::Vertex};
    const VertexAttribute attributes[] = {{0, 0, VertexElementFormat::Float3, 0}, {1, 0, VertexElementFormat::UByte4Norm, 12}};
    const ColorTargetDesc target{TextureFormat::B8G8R8A8Unorm, {}};
    GraphicsPipelineDesc desc;
    desc.vertexShader = vs;
    desc.fragmentShader = fs;
    desc.vertexBuffers = {&buffer, 1};
    desc.vertexAttributes = attributes;
    desc.colorTargets = {&target, 1};
    GraphicsPipeline pipeline = device->CreateGraphicsPipeline(desc);
    CHECK(pipeline);

    desc.fragmentShader = vs;
    CHECK_FALSE(device->CreateGraphicsPipeline(desc));
    device->ReleaseGraphicsPipeline(pipeline);
    device->ReleaseShader(vs);
    device->ReleaseShader(fs);
}

TEST_CASE("render: null swapchain is cleared and recorded")
{
    DummyWindow w(64, 32);
    auto device = NullDevice();
    REQUIRE(device->ClaimWindow(w.window));
    CHECK(device->SetSwapchainParameters(w.window, PresentMode::Vsync));
    CHECK(device->SwapchainTextureFormat(w.window) == TextureFormat::B8G8R8A8Unorm);

    CommandBuffer* commands = device->AcquireCommandBuffer();
    REQUIRE(commands);
    SwapchainTexture swapchain;
    REQUIRE(commands->WaitAndAcquireSwapchainTexture(w.window, swapchain));
    REQUIRE(swapchain.texture);
    CHECK(swapchain.width == 64);
    CHECK(swapchain.height == 32);
    const ColorTargetInfo target{.texture = swapchain.texture, .clearColor = {1.0f, 0.5f, 0.0f, 1.0f},
                                 .loadOp = LoadOp::Clear};
    RenderPass* pass = commands->BeginRenderPass({&target, 1}, nullptr);
    REQUIRE(pass);
    pass->End();
    CHECK(commands->Submit());

    const auto& lines = null::Commands(*device);
    CHECK(Contains(lines, "AcquireSwapchainTexture"));
    CHECK(Contains(lines, "load=clear(1,0.5,0,1)"));
    CHECK(lines.back() == "Submit");
    const auto bytes = null::TextureBytes(*device, swapchain.texture);
    REQUIRE(bytes.size() == 64 * 32 * 4);
    CHECK(bytes[0] == std::byte{0});      // B
    CHECK(bytes[1] == std::byte{128});    // G
    CHECK(bytes[2] == std::byte{255});    // R
    CHECK(bytes[3] == std::byte{255});    // A
    CHECK(null::LiveResourceCount(*device) == 0);   // the swapchain is the window's, not the caller's
    device->ReleaseWindow(w.window);
}

TEST_CASE("render: null multisampled target resolves into a single-sampled one")
{
    auto device = NullDevice();
    CHECK(device->TextureSupportsSampleCount(TextureFormat::B8G8R8A8Unorm, SampleCount::X4));
    CHECK_FALSE(device->TextureSupportsSampleCount(TextureFormat::Bc1Unorm, SampleCount::X4));
    Texture msaa = device->CreateTexture({.format = TextureFormat::B8G8R8A8Unorm, .usage = TextureUsage::ColorTarget,
                                          .width = 4, .height = 2, .sampleCount = SampleCount::X4});
    Texture resolved = device->CreateTexture({.format = TextureFormat::B8G8R8A8Unorm,
                                              .usage = TextureUsage::ColorTarget | TextureUsage::Sampler,
                                              .width = 4, .height = 2});
    REQUIRE(msaa);
    REQUIRE(resolved);
    CommandBuffer* commands = device->AcquireCommandBuffer();
    const ColorTargetInfo target{.texture = msaa, .clearColor = {0.0f, 0.0f, 1.0f, 1.0f}, .loadOp = LoadOp::Clear,
                                 .storeOp = StoreOp::Resolve, .resolveTexture = resolved};
    RenderPass* pass = commands->BeginRenderPass({&target, 1}, nullptr);
    REQUIRE(pass);
    pass->End();
    CHECK(commands->Submit());
    CHECK(Contains(null::Commands(*device), "store=resolve->"));
    const auto bytes = null::TextureBytes(*device, resolved);
    REQUIRE(bytes.size() == 4 * 2 * 4);
    CHECK(bytes[0] == std::byte{255});    // B
    CHECK(bytes[2] == std::byte{0});      // R
    device->ReleaseTexture(msaa);
    device->ReleaseTexture(resolved);
}

TEST_CASE("render: null blit scales a texture into another")
{
    auto device = NullDevice();
    Texture big = device->CreateTexture({.format = TextureFormat::R8G8B8A8Unorm, .width = 4, .height = 2});
    Texture small = device->CreateTexture({.format = TextureFormat::R8G8B8A8Unorm,
                                           .usage = TextureUsage::Sampler | TextureUsage::ColorTarget,
                                           .width = 2, .height = 1});
    REQUIRE(big);
    REQUIRE(small);
    Upload(*device, big, 4, 2, Bytes({1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4,
                                      5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8}));
    CommandBuffer* commands = device->AcquireCommandBuffer();
    commands->BlitTexture({.texture = big, .w = 4, .h = 2}, {.texture = small, .w = 2, .h = 1}, Filter::Linear);
    CHECK(commands->Submit());
    CHECK(Contains(null::Commands(*device), "BlitTexture"));
    const auto bytes = null::TextureBytes(*device, small);
    CHECK(std::vector<std::byte>(bytes.begin(), bytes.end()) == Bytes({1, 1, 1, 1, 3, 3, 3, 3}));    // nearest texels
    device->ReleaseTexture(big);
    device->ReleaseTexture(small);
}

TEST_CASE("render: null texture upload and readback round trip")
{
    auto device = NullDevice();
    Texture texture = device->CreateTexture({.format = TextureFormat::R8G8B8A8Unorm, .width = 2, .height = 2});
    const auto pixels = Bytes({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    Upload(*device, texture, 2, 2, pixels);
    CHECK(Download(*device, {.texture = texture, .w = 2, .h = 2}, 16) == pixels);
    // The bottom right texel alone: row 1 is the bottom row (top left origin).
    CHECK(Download(*device, {.texture = texture, .x = 1, .y = 1, .w = 1, .h = 1}, 4) == Bytes({13, 14, 15, 16}));
    device->ReleaseTexture(texture);
}

TEST_CASE("render: null BC textures copy whole blocks")
{
    auto device = NullDevice();
    Texture texture = device->CreateTexture({.format = TextureFormat::Bc1Unorm, .width = 8, .height = 4});
    std::vector<std::byte> blocks(16);
    for (size_t i = 0; i < blocks.size(); ++i)
        blocks[i] = static_cast<std::byte>(i);
    Upload(*device, texture, 8, 4, blocks);
    const auto second = Download(*device, {.texture = texture, .x = 4, .w = 4, .h = 4}, 8);
    CHECK(second == std::vector<std::byte>(blocks.begin() + 8, blocks.end()));
    device->ReleaseTexture(texture);
}

TEST_CASE("render: null buffer upload and copy")
{
    auto device = NullDevice();
    Buffer a = device->CreateBuffer({BufferUsage::Vertex, 8});
    Buffer b = device->CreateBuffer({BufferUsage::Index, 8});
    TransferBuffer transfer = device->CreateTransferBuffer({TransferBufferUsage::Upload, 8});
    const auto data = Bytes({9, 8, 7, 6, 5, 4, 3, 2});
    std::memcpy(device->MapTransferBuffer(transfer, true), data.data(), data.size());
    device->UnmapTransferBuffer(transfer);

    CommandBuffer* commands = device->AcquireCommandBuffer();
    CopyPass* copy = commands->BeginCopyPass();
    copy->UploadToBuffer({transfer, 0}, {a, 0, 8}, false);
    copy->CopyBufferToBuffer({a, 4}, {b, 0}, 4, false);
    copy->End();
    CHECK(commands->Submit());
    const auto bytesA = null::BufferBytes(*device, a);
    const auto bytesB = null::BufferBytes(*device, b);
    CHECK(std::vector<std::byte>(bytesA.begin(), bytesA.end()) == data);
    CHECK(std::vector<std::byte>(bytesB.begin(), bytesB.begin() + 4) == Bytes({5, 4, 3, 2}));
    device->ReleaseTransferBuffer(transfer);
    device->ReleaseBuffer(a);
    device->ReleaseBuffer(b);
}

TEST_CASE("render: null misuse fails the submit, cancel records nothing")
{
    auto device = NullDevice();
    Texture texture = device->CreateTexture({.width = 2, .height = 2});
    TransferBuffer transfer = device->CreateTransferBuffer({TransferBufferUsage::Upload, 4});

    CommandBuffer* commands = device->AcquireCommandBuffer();
    CopyPass* copy = commands->BeginCopyPass();
    copy->UploadToTexture({transfer}, {.texture = texture, .w = 2, .h = 2}, false);   // 16 bytes from a 4-byte buffer
    copy->End();
    CHECK_FALSE(commands->Submit());
    CHECK(device->LastError().find("out of range") != std::string::npos);
    CHECK(null::Commands(*device).back() == "Submit failed");

    null::ClearCommands(*device);
    commands = device->AcquireCommandBuffer();
    commands->InsertDebugLabel("dropped");
    commands->Cancel();
    CHECK(null::Commands(*device).empty());

    commands = device->AcquireCommandBuffer();
    CHECK(commands->BeginRenderPass({}, nullptr) == nullptr);
    CHECK_FALSE(commands->Submit());
    device->ReleaseTransferBuffer(transfer);
    device->ReleaseTexture(texture);
}
