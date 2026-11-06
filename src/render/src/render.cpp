#include "x2/render/render.h"

#include "backends.h"

namespace x2::render {

std::unique_ptr<Device> Device::Create(const DeviceOptions& options, std::string& error)
{
    switch (options.backend) {
    case Backend::Null:
        return detail::CreateNullDevice(options, error);
    case Backend::SdlGpu:
        return detail::CreateSdlGpuDevice(options, error);
    }
    error = "x2::render: unknown backend";
    return nullptr;
}

std::string_view BackendName(Backend backend)
{
    switch (backend) {
    case Backend::Null:
        return "null";
    case Backend::SdlGpu:
        return "sdlgpu";
    }
    return "?";
}

bool ParseBackend(std::string_view name, Backend& backend)
{
    for (Backend candidate : {Backend::Null, Backend::SdlGpu}) {
        if (name == BackendName(candidate)) {
            backend = candidate;
            return true;
        }
    }
    return false;
}

std::uint32_t TexelBlockSize(TextureFormat format)
{
    switch (format) {
    case TextureFormat::Invalid:
        return 0;
    case TextureFormat::A8Unorm:
    case TextureFormat::R8Unorm:
        return 1;
    case TextureFormat::R8G8Unorm:
    case TextureFormat::B5G6R5Unorm:
    case TextureFormat::B5G5R5A1Unorm:
    case TextureFormat::B4G4R4A4Unorm:
    case TextureFormat::D16Unorm:
        return 2;
    case TextureFormat::R8G8B8A8Unorm:
    case TextureFormat::R8G8B8A8UnormSrgb:
    case TextureFormat::B8G8R8A8Unorm:
    case TextureFormat::B8G8R8A8UnormSrgb:
    case TextureFormat::R32Float:
    case TextureFormat::D24Unorm:
    case TextureFormat::D32Float:
    case TextureFormat::D24UnormS8Uint:
        return 4;
    case TextureFormat::R16G16B16A16Float:
    case TextureFormat::D32FloatS8Uint:
    case TextureFormat::Bc1Unorm:
        return 8;
    case TextureFormat::Bc2Unorm:
    case TextureFormat::Bc3Unorm:
        return 16;
    }
    return 0;
}

bool IsBlockCompressed(TextureFormat format)
{
    return format == TextureFormat::Bc1Unorm || format == TextureFormat::Bc2Unorm || format == TextureFormat::Bc3Unorm;
}

bool IsDepthFormat(TextureFormat format)
{
    switch (format) {
    case TextureFormat::D16Unorm:
    case TextureFormat::D24Unorm:
    case TextureFormat::D32Float:
    case TextureFormat::D24UnormS8Uint:
    case TextureFormat::D32FloatS8Uint:
        return true;
    default:
        return false;
    }
}

std::uint32_t TextureFormatSize(TextureFormat format, std::uint32_t w, std::uint32_t h, std::uint32_t d)
{
    const std::uint32_t block = IsBlockCompressed(format) ? 4 : 1;
    const std::uint32_t columns = (w + block - 1) / block;
    const std::uint32_t rows = (h + block - 1) / block;
    return columns * rows * d * TexelBlockSize(format);
}

}  // namespace x2::render
