// The SDL_GPU backend without a GPU (container, CI): creation fails with a message, or works where a GPU exists.
// The escape hatch returns nothing for another backend's objects.

#include "x2/render/sdlgpu.h"

#include <doctest/doctest.h>

using namespace x2::render;

TEST_CASE("render: sdlgpu device creation fails cleanly or works")
{
    std::string error;
    auto device = Device::Create({}, error);
    if (!device) {
        CHECK_FALSE(error.empty());
        return;
    }
    CHECK(device->GetBackend() == Backend::SdlGpu);
    CHECK_FALSE(device->DriverName().empty());
    CHECK(sdlgpu::Native::Device(*device) != nullptr);
    Texture texture = device->CreateTexture({.width = 4, .height = 4});
    CHECK(texture);
    CHECK(sdlgpu::Native::Texture(*device, texture) != nullptr);
    device->ReleaseTexture(texture);
}

TEST_CASE("render: the escape hatch ignores the null backend")
{
    std::string error;
    DeviceOptions options;
    options.backend = Backend::Null;
    auto device = Device::Create(options, error);
    REQUIRE(device);
    CHECK(sdlgpu::Native::Device(*device) == nullptr);
    Texture texture = device->CreateTexture({.width = 4, .height = 4});
    CHECK(sdlgpu::Native::Texture(*device, texture) == nullptr);
    CommandBuffer* commands = device->AcquireCommandBuffer();
    CHECK(sdlgpu::Native::CommandBuffer(*commands) == nullptr);
    commands->Cancel();
    device->ReleaseTexture(texture);
}
