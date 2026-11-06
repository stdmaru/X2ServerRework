#pragma once

// The escape hatch to SDL_GPU (decision T36): the SDL objects behind an x2::render SdlGpu device, for code that must
// talk to SDL_GPU directly. Today only ImGui's sdlgpu3 renderer (x2::app_imgui), until an ImGui renderer on x2::render
// replaces it. Linking x2::render_sdlgpu is what makes this header available; anything else stays on x2/render/render.h.

#include "x2/render/render.h"

#include <SDL3/SDL_gpu.h>

namespace x2::render::sdlgpu {

// Each function returns null (or SDL_GPU_TEXTUREFORMAT_INVALID) when the object does not belong to an SdlGpu device.
struct Native {
    static SDL_GPUDevice* Device(render::Device& device);
    static SDL_GPUCommandBuffer* CommandBuffer(render::CommandBuffer& commands);
    static SDL_GPURenderPass* RenderPass(render::RenderPass& pass);
    static SDL_GPUTexture* Texture(render::Device& device, render::Texture texture);
    static SDL_GPUTextureFormat Format(render::TextureFormat format);
};

}  // namespace x2::render::sdlgpu
