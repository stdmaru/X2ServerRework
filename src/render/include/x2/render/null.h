#pragma once

// The null backend's view for tests (decision T36): what a Null device recorded, and the bytes its resources hold.
// It rasterizes nothing: clears, uploads, copies and downloads change bytes; draws are only recorded.

#include "x2/render/render.h"

#include <span>
#include <string>
#include <vector>

namespace x2::render::null {

// One line per call, in submission order, e.g. "BeginRenderPass color[0]=texture#3 load=clear(0,0,0,1)".
// Only submitted command buffers appear; Cancel drops what was recorded.
const std::vector<std::string>& Commands(Device& device);
void ClearCommands(Device& device);

// The bytes of mip level 0, layer 0 of a texture (tightly packed rows), or a buffer's bytes. Empty for another device's
// handles or a handle that is not alive.
std::span<const std::byte> TextureBytes(Device& device, Texture texture);
std::span<const std::byte> BufferBytes(Device& device, Buffer buffer);

// Resources alive on the device (created and not released), to check that callers release what they create.
std::size_t LiveResourceCount(Device& device);

}  // namespace x2::render::null
