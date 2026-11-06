[Home](../README.md) › [Porting](README.md) › Client: D3D9 device

# Client: KTDXLIB's D3D9 device (`src/client/KTDXLIB/D3D9`, target `x2::client_d3d9`)

New code. The original engine called Direct3D 9 directly; the port keeps the engine's D3D9 *calls* and translates them
onto [`x2::render`](render.md), above the graphics boundary (decisions [T36 (5) and T38](../project/decisions.md#accepted)).
There is no `d3d9.h`, no DirectX runtime and no COM: `x2::d3d9::Device`, `Texture` and `VertexBuffer` are KTDXLIB's
own classes, and D3D9's type names (`IDirect3DDevice9`, `LPDIRECT3DDEVICE9`, `LPDIRECT3DTEXTURE9`,
`LPDIRECT3DVERTEXBUFFER9`) alias them. Added on 2026-11-06 (`client/ktdxlib-device`, Phase 4). Overview of the client
port: [Client](client.md).

**Why a translation and not a rewrite now**: the engine speaks D3D9 in about 70 files and in its data (render states,
FVFs, texture stage ops stored as numbers). Keeping that vocabulary lets the engine compile as it is while the platform
changes underneath; rewriting the callers onto `x2::render`'s own concepts is possible later, file by file, once
everything runs through this layer ([Phase 5](../project/roadmap.md#phase-5--deep-cleanup)).

## Files

| File | Contents |
|---|---|
| `D3D9Types.h` | The vocabulary: `D3DRS_*`, `D3DTSS_*`, `D3DSAMP_*`, `D3DTOP_*`, `D3DTA_*`, `D3DBLEND*`, `D3DCMP_*`, `D3DCULL_*`, `D3DSTENCILOP_*`, `D3DFVF_*`, `D3DPT_*`, `D3DFMT_*`, `D3DPOOL_*`, `D3DUSAGE_*`, `D3DLOCK_*`, `D3DCLEAR_*`, `D3DTS_*`, `D3DLOCKED_RECT`, `D3DRECT`, `D3DSURFACE_DESC`, with d3d9types.h's names and values (enumerations as C enums, flags as macros). The math types come from [`x2/platform/d3dx_math.h`](platform.md) |
| `Device.h` / `Device.cpp` | `x2::d3d9::Device`: states, frame, clears, draws, pipeline and sampler caches, uploads |
| `Resources.cpp` | `Texture` (levels, `LockRect`/`UnlockRect`, format conversion) and `VertexBuffer` (`Lock`/`Unlock`) |
| `Fvf.h` | FVF → element offsets and the vertex shader variant |
| `GeneratedShader.h`, `shaders/` | The fixed-function HLSL, its manifest and the generated blobs (`generated/FixedFunctionShaders.inc`, [Shaders](../tools/shaders.md), T39) |

## How a draw is translated

- **States** are cached as D3D9 values (D3D9's defaults at creation). A draw builds a `PipelineKey` from what lives in an
  SDL_GPU pipeline: primitive class, FVF and stride, target formats, cull and fill mode, depth and stencil states, blend
  states, color write mask, depth bias. Pipelines are cached by key; blend factors are left out of the key while blending
  is off, stencil states while stencil is off. Viewport, stencil reference and blend factor are dynamic state; texture
  stage states, the texture factor and the alpha test are uniforms (no new pipeline).
- **Fixed function** runs on two generated shaders: `FixedFunction.vert.hlsl` (12 variants: pre-transformed `XYZRHW` or
  `XYZ` through world × view × projection, diffuse or not, 0–2 texture coordinate sets) and `FixedFunction.frag.hlsl`
  (one shader that interprets texture stages 0 and 1 — every `D3DTOP_*` except bump mapping and `PREMODULATE`, the
  `D3DTA_*` arguments with `COMPLEMENT`/`ALPHAREPLICATE`, `TEXCOORDINDEX` — and the alpha test on 8-bit values). Vertex
  inputs use fixed locations (`TEXCOORD<n>` = location n): 0 position, 1 diffuse (`D3DCOLOR` read as `UByte4Norm`, swizzled
  from B, G, R, A), 2 and 3 texture coordinates; other FVF elements (normal, point size, specular, blend weights) are
  skipped by offset.
- **Half-pixel offset**: D3D9 puts pixel centers on integers, modern APIs on halves. Pre-transformed vertices move
  +0.5 pixel before going back to clip space; transformed ones move by (1/width, −1/height) × w in clip space. The GPU test
  checks that a D3D9-style quad at −0.5 maps texels to pixels exactly.
- **Culling**: D3D9's front faces are clockwise; `D3DCULL_CCW` (the default) culls back faces.
- **Triangle fans** (SDL_GPU has none) are drawn as indexed lists from a shared index buffer (0, i, i+1), up to 16,384
  triangles per draw.
- **A stage with no texture** samples a 1×1 white texture, so `MODULATE` with no texture gives the vertex color.
  *Assumed* to be D3D9's behavior (it is wined3d's); to confirm against the original client when the engine renders.
- **Clears**: a clear of the whole target becomes the next render pass's load operation; a smaller one (rectangles, or a
  viewport smaller than the target) is drawn as quads with states that write exactly the cleared values (depth test
  always, stencil replace, color write mask per flag), clipped to the viewport; the caller's states are restored.

## Frames and uploads

- `BeginScene` acquires the frame's command buffer and the back buffer (the device's own depth-stencil buffer,
  `D24UnormS8Uint` or `D32FloatS8Uint`, follows the back buffer's size); `Present` submits. Draws outside a scene fail, as
  in D3D9; clears do not need one. `Present` without a scene presents a frame cleared to black. A minimized window has no
  back buffer: draws are dropped, uploads still happen.
- **Two command buffers per frame**: uploads, then draws, submitted in that order at `Present`. Each unlock records its
  upload at once, so the resource is cycled at the right moment: draws recorded earlier keep the old contents.
  - `VertexBuffer::Unlock`: `D3DLOCK_NOOVERWRITE` uploads the locked range in place; `D3DLOCK_DISCARD` cycles and uploads
    the range; no flags cycles and uploads the whole CPU copy (D3D9 keeps the rest).
  - `Texture::UnlockRect` uploads the level; single-level textures cycle, textures with mip levels do not (cycling would
    lose the other levels).
  - `DrawPrimitiveUP` appends to a 4 MB ring and cycles when it wraps; afterwards stream 0 is unset, as in D3D9.
  - Staged data lives in 4 MB transfer buffers reused per batch (mapped with cycling on the first write of a batch);
    past 64 MB staged the uploads are submitted early, which keeps them ahead of the frame.
- **Render targets**: `SetRenderTarget(0, texture)` with a `D3DUSAGE_RENDERTARGET` texture (`A8R8G8B8`/`X8R8G8B8`), null for
  the back buffer; the viewport resets to the whole target, as in D3D9. The depth buffer is attached only when the target
  has the back buffer's size.
- Resources destroyed while bound are unbound; resources that outlive the device are detached (their calls fail).

## Textures

| D3D9 format | Uploaded as | Notes |
|---|---|---|
| `A8R8G8B8` | `B8G8R8A8Unorm` | as is |
| `X8R8G8B8`, `R8G8B8`, `A8`, `L8`, `A8L8` | `B8G8R8A8Unorm` | converted on upload (alpha 255 for X and R8G8B8) |
| `R5G6B5`, `A1R5G5B5`, `A4R4G4B4` | `B5G6R5`, `B5G5R5A1`, `B4G4R4A4` | native where the GPU takes them, else converted to `B8G8R8A8` |
| `X1R5G5B5`, `X4R4G4B4` | as their A formats | the alpha bits set on upload |
| `DXT1`, `DXT2/3`, `DXT4/5` | `Bc1`, `Bc2`, `Bc3` | native (T37) |

Levels 0 means the whole chain; `D3DUSAGE_AUTOGENMIPMAP` shows one level and generates the rest after level 0 is uploaded.
Lockable textures keep a CPU copy of every level in their D3D9 format (`LockRect` with a rectangle points into it,
by 4×4 blocks for DXT); render targets have none and cannot be locked.

## DXUT and the skeleton

The [DXUT replacement](client-dxut.md) creates the device with the window, passes it to the `FrameMove`/`FrameRender`
callbacks and calls `Present` after `FrameRender`, as DXUT did; `DXUTGetD3DDevice()` returns it. The skeleton
(`X2/X2Skeleton.cpp`) draws a test card through it: a textured, alpha-blended quad and a turning triangle fan.

## Not emulated yet (2026-11-06)

Lighting and materials (`D3DRS_LIGHTING` is on by default, but lit geometry goes through the effects, T37), fog, specular,
point sprites and point sizes, texture transforms, stages 2–7, `D3DTA_TEMP`/`RESULTARG`, bump mapping, border and
mirror-once addressing (mapped to clamp and mirror), the scissor test, index buffers and `DrawIndexedPrimitive*`,
vertex declarations and programmable shaders (`SetVertexShader`/`SetPixelShader`), `StretchRect`, `UpdateTexture`,
`GetRenderTargetData`, surfaces, `SetDepthStencilSurface`, `D3DPOOL_DEFAULT` semantics (every non-target texture is
lockable), state blocks. Each comes with the engine file that needs it.

## Tests

`x2_client_tests -tc="d3d9*"` ([source](../../src/client/tests/test_d3d9.cpp)):

- The generated shaders match their sources (the hash, [Shaders](../tools/shaders.md)) and have the expected variants and
  reflection.
- FVF layouts: screen, mesh, skinned (`XYZB4`), three-component texture coordinates; invalid FVFs.
- On the null backend: a frame (uploads submitted before the draws, the whole-target clear as the load op, fans as indexed
  lists), the pipeline cache (uniform and dynamic state do not create pipelines; blending and culling do), texture formats
  and locked rectangles (A8R8G8B8 with mips, X8R8G8B8, A4R4G4B4, A8L8, DXT1), vertex buffer lock flags and the bytes they
  leave, partial clears drawn with the states restored, resources destroyed before and after the device.
- **On a GPU** (skipped without video or GPU, as in the container and CI): renders into a 16×16 target and reads it back —
  texels mapped 1:1 to pixels with the half-pixel rule, the vertex color with no texture, 50% blending, the alpha test, a
  fan. Passed on the user's Linux desktop (Vulkan, 2026-11-06).
