#pragma once

// One shader compiled offline by tools/shaders/compile.py (decision T39), in every format SDL_GPU takes, with the
// reflection x2::render needs. The generated .inc files hold arrays of these.

#include "x2/render/render.h"

#include <cstdint>
#include <span>

namespace x2 {

struct GeneratedShader {
    const char* name;
    const char* variant;            // its defines, "NAME=value ..."; empty without variants
    render::ShaderStage stage;
    std::span<const unsigned char> spirv;
    std::span<const unsigned char> dxil;
    std::span<const unsigned char> msl;
    const char* mslEntryPoint;      // SPIRV-Cross renames main to main0
    std::uint32_t samplerCount;
    std::uint32_t uniformBufferCount;
};

}  // namespace x2
