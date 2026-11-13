[Home](../README.md) › [Porting](README.md) › Render

# Render: the graphics boundary (`src/render`, targets `x2::render`, `x2::render_sdlgpu`)

New code, with no equivalent in the original (which called Direct3D 9 directly). Everything that draws goes through
`x2::render`, a thin API with SDL_GPU's shape, and SDL_GPU is reached only through its backend (decision
[T36](../project/decisions.md#accepted), revising T19 and T32). Added on 2026-11-06 (`client/render-boundary`, Phase 4).

## API (`x2/render/render.h`)

Only what SDL_GPU does, nothing added in advance for a future backend (T36 (1)):

| Part | Contents |
|---|---|
| Handles | `Buffer`, `TransferBuffer`, `Texture`, `Sampler`, `Shader`, `GraphicsPipeline`: opaque, a default handle is "none", released explicitly (`Device::Release*`) as in SDL_GPU |
| Descriptions | `BufferDesc`, `TextureDesc`, `SamplerDesc`, `ShaderDesc` (code + reflection: sampler and uniform block counts), `GraphicsPipelineDesc` (vertex layout, rasterizer, depth-stencil, blend per color target) |
| Multisampling (2026-11-12, `client/msaa`) | `TextureDesc::sampleCount`, `GraphicsPipelineDesc::sampleCount` and `alphaToCoverage`, `StoreOp::Resolve`/`ResolveAndStore` with `ColorTargetInfo::resolveTexture`, `Device::TextureSupportsSampleCount`; the null backend copies a resolved target's contents into the resolve texture |
| Recording | `CommandBuffer` (uniforms pushed by slot, render and copy passes, mipmaps, debug groups, swapchain texture, `Submit`/`Cancel`), `RenderPass` (pipeline, viewport, scissor, blend constants, stencil reference, vertex/index buffers and samplers by slot, draws), `CopyPass` (uploads, copies, downloads) |
| `Device` | Created at run time for a `Backend` (`Device::Create`); window claims and swapchain parameters, format support, resource creation, transfer buffer mapping, `AcquireCommandBuffer`, `WaitIdle`, `LastError` |
| Helpers | `WindowClaim` (claims a window and sets its present mode for its lifetime), `BackendName`/`ParseBackend`, texture format sizes |

- **Conventions** (T36 (2)), as in Direct3D: clip-space depth 0..1, texture and viewport origin at the top left (row 0 is the top
  row). SDL_GPU already uses them, so the SDL_GPU backend converts nothing.
- **Left out for now** (2026-11-06), until a caller needs them: compute, storage buffers and textures, fences, blits, indirect
  draws, HDR swapchains. D3D9 features SDL_GPU lacks (triangle fans, border and mirror-once addressing) are
  the D3D9 translation's job in `KTDXLIB` (T36 (5)), not the backends'.
- Failures return a none handle, null or false, with the message in `Device::LastError()`; `Device::Create` reports through its
  `error` argument.
- **Destroy the device before `main` returns**, never from a static destructor: by then the GPU driver and its layers may have
  run their own teardown (the Vulkan validation layer aborts in `SDL_WaitForGPUIdle`, seen on 2026-11-06).
- Command buffers may be acquired and recorded on any thread, each on one; a pass belongs to its command buffer until `End()`.

## Backends

| Backend | Files | Behavior |
|---|---|---|
| `sdlgpu` | `backends/sdlgpu/sdlgpu_device.cpp` | The only real one: a near 1:1 wrapper; handles hold the SDL pointers, enums are mapped by table. Device created with SPIR-V, DXIL and MSL by default; validation layers only with `DeviceOptions::debug` or `X2_GPU_DEBUG=1` (2026-11-12: on by default in `X2_DEV_TOOLS` builds before, where the Vulkan layer took about 80% of the client's frame) |
| `null` | `backends/null/null_device.cpp` | No GPU. Validates handles and ranges (a misuse fails `Submit`), records one line per submitted command, keeps the bytes of buffers and textures so clears, uploads, copies and downloads behave (formats encoded as in DXGI/SDL: `B5G6R5` has blue in the low bits, BC data copied in whole 4×4 blocks); draws are only recorded. A claimed window gets a `B8G8R8A8Unorm` swapchain texture of its pixel size, none while minimized |

- The null backend's test view is `x2/render/null.h`: `Commands`, `ClearCommands`, `TextureBytes`, `BufferBytes`,
  `LiveResourceCount` (resources the caller created and has not released; swapchain textures are not counted).
- **Choosing a backend**: the caller picks it in `DeviceOptions::backend`. The client reads `X2_RENDER_BACKEND` (`sdlgpu`, the
  default, or `null`) in the [DXUT replacement](client-dxut.md); the tools always use `sdlgpu`.

## The SDL_GPU boundary (T36 (4))

- **SDL_GPU is used only in `src/render/backends/sdlgpu`.** `SDL.h` includes `SDL_gpu.h`, so the source scan
  (`test_boundary.cpp`) looks for the API itself: identifiers `SDL_…GPU…`, the header's name, and ImGui's `sdlgpu3` backend, in
  every source under `src/` (comments excluded).
- **Escape hatch**: `x2/render/sdlgpu.h` (`x2::render::sdlgpu::Native`) gives the SDL device, command buffer, render pass, texture
  and format behind an `sdlgpu` device's objects (null for another backend's). Only targets that link `x2::render_sdlgpu` see the
  header. Its single user is the ImGui layer (`src/app/src/imgui_layer.cpp`, the scan's one exception) until an ImGui renderer on
  `x2::render` replaces it ([Roadmap](../project/roadmap.md#phase-4--client-graphics)).
- `x2::render` links SDL3 privately; its public headers include no SDL header (`SDL_Window` is forward declared).

## Tests

`x2_render_tests` (`ctest -R render`), headless:

- `test_null.cpp`: backend names, format sizes, resource creation and release, pipelines that need both stages, a swapchain cleared
  and recorded on a dummy SDL window, texture upload and readback (top left origin), BC blocks, buffer upload and copy, misuse
  failing the submit, `Cancel` recording nothing.
- `test_sdlgpu.cpp`: device creation fails with a message without a GPU (container, CI) or works where there is one; the escape
  hatch returns nothing for the null backend.
- `test_boundary.cpp`: the source scan above.

## Pending

- The effects (T37), D3DX replacements and the ImGui renderer on `x2::render`: see the
  [Roadmap](../project/roadmap.md#phase-4--client-graphics). The D3D9 translation above the boundary is the
  [D3D9 device](client-d3d9.md) (2026-11-06).
- Shaders are compiled offline by SDL_shadercross and committed ([Shaders](../tools/shaders.md), T39).
