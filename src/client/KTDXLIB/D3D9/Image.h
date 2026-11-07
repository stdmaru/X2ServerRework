#pragma once
// D3DX's image files for KTDXLIB's device: .dds, .tga, and the .png/.jpg/.bmp some of the client's textures turn out to
// be (decision T37: the assets load as they are). Files are recognized by their contents, as D3DX did, not by name.
// docs/porting/client-d3d9.md#image-files

#include "D3DXTypes.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace x2::d3d9::image {

struct Level {
    UINT width = 0, height = 0;
    std::vector<std::byte> bytes;       // tightly packed in Image::format (Formats.h)
};

struct Image {
    D3DXIMAGE_INFO info{};              // what D3DXGetImageInfoFromFileInMemory reports
    // The levels' format: the file's, except formats a texture is not created in, which become the format D3DX created
    // them as: R8G8B8 -> X8R8G8B8, P8 -> A8R8G8B8, A8B8G8R8/X8B8G8R8 -> A8R8G8B8/X8R8G8B8.
    D3DFORMAT format = D3DFMT_UNKNOWN;
    // The file's levels (fewer than info.MipLevels when the file is cut short). Volume textures keep their first slice,
    // cube maps their first face.
    std::vector<Level> levels;
};

// The file's header only. False for a file that is not an image this loader reads.
bool ReadInfo(std::span<const std::byte> file, D3DXIMAGE_INFO& info);
std::optional<Image> Decode(std::span<const std::byte> file);

// --- Conversions, through 8-bit B, G, R, A (A8R8G8B8's bytes) ---------------------------------------------------------

// Any format Info() knows (Formats.h), compressed ones included, into width * height * 4 bytes.
void ToBgra8(D3DFORMAT format, std::span<const std::byte> level, UINT width, UINT height, std::uint8_t* out);
// BGRA8 into any format Info() knows; DXT1/3/5 through stb_dxt (DXT1 without its 1-bit alpha).
std::vector<std::byte> FromBgra8(D3DFORMAT format, const std::uint8_t* bgra, UINT width, UINT height);

enum class Filter { None, Point, Linear, Box };
// D3DX_FILTER_* (D3DX_DEFAULT is TRIANGLE, which becomes Box).
Filter FilterFromD3DX(DWORD filter);
// D3DX's filters: None copies the top-left corner (transparent black outside the source), Point takes the nearest
// texel, Linear the four nearest, Box averages the texels each destination texel covers (Linear when enlarging).
std::vector<std::uint8_t> Resize(const std::uint8_t* bgra, UINT width, UINT height, UINT newWidth, UINT newHeight,
                                 Filter filter);

}  // namespace x2::d3d9::image
