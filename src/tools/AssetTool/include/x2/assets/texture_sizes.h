// The original size of every image in the client's archives (docs/tools/client-data.md#original-texture-sizes): the
// client draws an upscaled texture at the size its .tet and dialogs were made for (CKTDXDeviceTexture).
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

namespace x2::assets {

// The size of an image file from its header, as D3DX recognizes it by its contents: DDS, BMP, PNG, JPEG and TGA (TGA
// last: it has no signature). False if it is none of them.
bool ImageSize(const std::uint8_t* data, std::size_t size, std::uint32_t& width, std::uint32_t& height);

struct TextureSizeStats
{
    std::size_t images = 0;   // written
    std::size_t skipped = 0;  // .dds/.tga/.bmp/.png/.jpg entries that are no image (empty, or unreadable)
};

// Writes `output`: a header comment, then one line per image name of the client's archives in `source` (its data
// folder; first wins, as the client mounts them), sorted: "<name>\t<width>\t<height>". jobs 0: one per hardware thread.
bool WriteTextureSizes(const std::filesystem::path& source, const std::filesystem::path& output, unsigned jobs,
                       TextureSizeStats& stats, std::string& error);

} // namespace x2::assets
