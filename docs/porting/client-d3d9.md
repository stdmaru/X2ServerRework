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
| `Resources.cpp` | `Texture` (levels, `LockRect`/`UnlockRect`, format conversion), `VertexBuffer` and `IndexBuffer` (`Lock`/`Unlock`, `D3DFMT_INDEX16`/`INDEX32`) |
| `D3DXTypes.h`, `D3DXTexture.cpp`, `D3DX.cpp` | D3DX's types for the engine (interfaces as plain classes, T38; the D3DX objects keep `AddRef`/`Release`, held by a minimal `CComPtr`) and its functions on the device: `D3DXCheckTextureRequirements` follows `Device::SupportsTextureFormat`, `D3DXCreateTexture` creates on the device, `D3DXGetImageInfoFromFileInMemory`/`D3DXCreateTextureFromFileInMemoryEx` read [image files](#image-files-2026-11-07-clientd3dx-textures). `D3DX.cpp`: reference-counted `D3DXCreateBuffer`, `D3DXFrameDestroy`/`Find`/`NumNamedMatrices`/`CalculateBoundingSphere`; the effect creators fail with `E_NOTIMPL` until their step (2026-11-07). `ID3DXMesh`, `ID3DXSkinInfo`, `ID3DXAnimationSet`/`Keyframed`/`Compressed` and `ID3DXEffect` are declared with the methods the engine calls; `ID3DXLine` is gone (no user) |
| `Mesh.h` / `Mesh.cpp` | [`ID3DXMesh`](#meshes-2026-11-07-clientx-loader) on the device (`x2::d3d9::Mesh`), FVF element offsets (`DescribeVertex`), point representatives and adjacency; `D3DXCreateMeshFVF`, `D3DXWeldVertices`, `D3DXIntersect` |
| `XFile.h` / `XFile.cpp` | The [.x file](#x-files-2026-11-07-clientx-loader) reader (`x2::d3d9::xfile`): text, binary, MSZIP, as a tree of data objects |
| `XMesh.h` / `XMesh.cpp` | A `Mesh` data object as D3DX lays it out (`ReadXMesh`), the materials buffer, `D3DXLoadMeshFromXInMemory`, `D3DXLoadMeshHierarchyFromXInMemory` |
| `SkinInfo.h` / `SkinInfo.cpp` | [`ID3DXSkinInfo`](#skinned-meshes-2026-11-07-clientx-skin) (`x2::d3d9::SkinInfo`), `D3DXCreateSkinInfoFVF`, `ConvertToIndexedBlendedMesh` |
| `Animation.h` / `Animation.cpp` | [Animation](#animation-2026-11-07-clientx-animation): keyframed and compressed sets, `x2::d3d9::AnimationController`, `D3DXCreateAnimationController`, `D3DXFrameRegisterNamedMatrices`, `D3DXCreateKeyframedAnimationSet`, `D3DXCreateCompressedAnimationSet` |
| `Formats.h` | Texel and block sizes, pitch and rows of the D3D9 formats the device takes (shared by the device and the loaders) |
| `Image.h`, `Image.cpp`, `ImageConvert.cpp` | The [image files](#image-files-2026-11-07-clientd3dx-textures) (`x2::d3d9::image`) and the texel conversions: BGRA8 to and from every device format, BC1–3 decoding, BC encoding (stb_dxt), D3DX's resize filters. The device's upload conversion uses them too |
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
- **Indexed draws** (2026-11-07): `SetIndices` and `DrawIndexedPrimitive` bind the index buffer as 16- or 32-bit and
  pass `BaseVertexIndex` as the vertex offset; `MinIndex`/`NumVertices` are not needed. Indices past the buffer fail;
  indexed fans fail (no caller).
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
`SupportsTextureFormat(format, usage)` says whether `CreateTexture` takes a format: render targets are A8R8G8B8 or
X8R8G8B8, and DXT cannot generate mips.

## State kept, not drawn with (2026-11-06)

What the engine sets and reads back but the fixed-function shaders do not use yet (T39):

- **Material and lights**: `SetMaterial`/`GetMaterial` (zeroed at first), `SetLight`/`GetLight`/`LightEnable`/
  `GetLightEnable` for up to 1,024 lights; enabling a light never set gives D3D9's default (white, directional, +z).
- **Shaders and vertex declarations**: no such objects exist (the effects become KTDXLIB's own shaders, T37); setting one
  is ignored, getting one gives null. `IDirect3DVertexDeclaration9`/`VertexShader9`/`PixelShader9` are declared only as
  interfaces.
- `GetAvailableTextureMem` reports a fixed 512 MB (the backend does not say); `EvictManagedResources` does nothing.
- `UpdateTexture(source, destination)` copies the levels both textures have from the source's CPU copy and uploads them
  (same size and format); `Texture::AddDirtyRect` is accepted (whole levels are copied).

## Image files (2026-11-07, `client/d3dx-textures`)

`CKTDXDeviceTexture` loads every texture through `D3DXGetImageInfoFromFileInMemory` and
`D3DXCreateTextureFromFileInMemoryEx`. Files are recognized by their contents, as D3DX did, not by name (T37: the assets
load as they are):

| File | Read by | D3DX's format (`D3DXIMAGE_INFO`) | Texture created as (format `UNKNOWN`) |
|---|---|---|---|
| DDS, FourCC `DXT1`–`DXT5` | `Image.cpp` | `DXT1`–`DXT5` | the same, blocks copied (native BC1–3) |
| DDS, RGB masks | `Image.cpp` | `A8R8G8B8`, `X8R8G8B8`, `A8B8G8R8`, `X8B8G8R8`, `R8G8B8`, `R5G6B5`, `A1R5G5B5`, `X1R5G5B5`, `A4R4G4B4`, `X4R4G4B4` | the same, but `R8G8B8` → `X8R8G8B8` and `A/X8B8G8R8` → `A/X8R8G8B8` |
| DDS, luminance/alpha, a D3DFORMAT number as FourCC | `Image.cpp` | `L8`, `A8L8`, `A8`, that number if the device takes it | the same |
| TGA 24/32-bit, gray, color-mapped (raw and RLE) | stb_image | `R8G8B8`, `A8R8G8B8`, `L8`, `P8` | `X8R8G8B8`, `A8R8G8B8`, `L8`, `A8R8G8B8` |
| TGA 15/16-bit (raw and RLE) | `Image.cpp` (stb_image drops the alpha bit) | `A1R5G5B5` with attribute bits, else `X1R5G5B5` | the same |
| PNG, JPEG, BMP | stb_image | PNG `A8R8G8B8`/`X8R8G8B8`, JPEG `X8R8G8B8`, BMP `R8G8B8`/`A8R8G8B8` | as the TGA row |

- **Levels**: a DDS's chain is read as stored; a volume texture keeps the first slice of each level and a cube map its
  first face (none in the client; 49 DXT1 files are volumes of depth 1). Levels the texture has and the file does not
  are generated from the level above with the mip filter (box when it is `D3DX_FILTER_NONE`).
- **Size, levels and format asked for**: 0, `D3DX_DEFAULT` and `D3DX_DEFAULT_NONPOW2` keep the file's size (the device
  takes any size); 0/`D3DX_DEFAULT` levels give the whole chain, `D3DX_FROM_FILE` the file's; `UNKNOWN`,
  `D3DFMT_FROM_FILE` and `D3DX_DEFAULT` the format of the last column. A format the device does not take becomes
  `A8R8G8B8`. When the size and format are the file's and there is no color key, levels are copied; otherwise they go
  through BGRA8: decoded (BC1–3 in software), color-keyed (that exact ARGB becomes transparent black), resized with
  `filter`, encoded (BC through stb_dxt, the others by `FromBgra8`).
- **Filters**: `NONE` copies the top-left corner (transparent black outside the source), `POINT` takes the nearest
  texel, `LINEAR` the four nearest, `TRIANGLE`/`BOX`/`D3DX_DEFAULT` average the texels each destination texel covers
  (linear when enlarging). Mirror and dither flags are ignored.
- **The texture detail option** (`CX2GameOption`, through `CKTDXDeviceManager::SetTextureDetailLevel`): High loads the file as it is (one level);
  Medium halves textures of 128 texels and up and makes `R8G8B8`/`R5G6B5` files DXT1; Low quarters them and makes the
  other files DXT3, so most DXT1/DXT5 textures are decoded, resized and encoded again at load. Medium and Low cost CPU
  at load time, as they did with D3DX.
- **stb**: [nothings/stb](https://github.com/nothings/stb) (public domain) pinned to commit `2c980bb` (2026-08-02,
  stb_image 2.30, stb_dxt 1.12) as `x2::stb` in `cmake/X2Dependencies.cmake`, compiled without the project's warning
  options like pl_mpeg; only the TGA, PNG, JPEG and BMP decoders, from memory.

What the client's archives hold (2026-11-07, 57,793 unique `.dds`/`.tga` names, first-wins as the client mounts them):
DXT1 28,633, DXT5 1,775, DXT3 1,277, R8G8B8 778, A8R8G8B8 438, X8R8G8B8 15 and A4R4G4B4 12 `.dds` (11,299 with mip
chains); 24-bit 21,113, 32-bit 3,704, 16-bit 24 (all with the alpha bit), color-mapped 6 and gray 3 `.tga` (291 RLE).
Seven `.dds` are PNGs and one a JPEG, one `.tga` is a JPEG and another (`UIEdge.tga`) a BMP. Three are not read, as D3DX
could not have read them: `Velder_Field_001.dds` and `Velder_Field_003.dds` (empty) and
`field_Peita_stage1_bluegate.dds` (`Q8W8V8U8`, a bump map format the device does not have).

### Behavior changes and assumptions

| Where | D3DX | Port | Why |
|---|---|---|---|
| A DDS cut short | Assumed to fail | Its whole levels load; missing ones are generated | No client file is cut short; tolerant |
| `D3DX_DEFAULT` size | Rounded up to a power of two | The file's size | The device takes any size; the client passes explicit sizes |
| A volume/cube DDS as a 2D texture | Not checked | First slice/face | The 49 volumes have depth 1 |
| Encoding to DXT1 | Alpha below 128 became the transparent color | Alpha ignored | Only opaque formats (`R8G8B8`, `R5G6B5`) are made DXT1 by the client |
| 32-bit TGA with no attribute bits (755 files) | *Assumed* `A8R8G8B8` | `A8R8G8B8`, alpha kept | To confirm against the original client when the engine renders |
| Exact texels after resizing or encoding | D3DX's filters and BC compressor | Ours and stb_dxt's | Close, not identical |

## X files (2026-11-07, `client/x-loader`)

`xfile::Read` reads DirectX `.x` files into a tree of data objects (T37: the assets load as they are, through our own
parser). The 16-byte header (`xof 0303` + `txt `/`bin `/`tzip`/`bzip` + `0032`/`0064`) selects the encoding; files are
recognized by it, not by name (`.x` and `.y` alike; KOG's `KME `/`KSM ` meshes are read by the engine itself).

- **MSZIP** (`tzip`/`bzip`): after the header, a DWORD with the uncompressed size (header included), then blocks of
  {WORD uncompressed size, WORD packed size + 2, `CK`, raw deflate}; each block is inflated (zlib) with the previous
  32 KB of output as its dictionary. Some of the client's blocks end in a stored deflate block that has its length but
  not the length's complement (NLEN); zlib rejects it, so such a block is inflated again with the complement put back
  (found 2026-11-07 in a few dozen files; D3DX read them).
- **Binary**: tokens `NAME`, `STRING` (with its terminator token), `INTEGER`, `GUID`, `INTEGER_LIST`, `FLOAT_LIST`
  (32- or 64-bit floats), braces; punctuation and template keywords carry no payload.
- **Text**: names, numbers (`std::from_chars`), quoted strings, GUIDs, `,`/`;` separators, `//` and `#` comments.
- **Objects** keep their type, name, data members (numbers as `double`, strings apart) and children in file order;
  `{ Name }` is a reference (`IsReference`). Template definitions are skipped: the loaders know the templates they read
  and read members in their order (`xfile::Cursor`), so D3DX's template checks are not done.

## Meshes (2026-11-07, `client/x-loader`)

`x2::d3d9::Mesh` implements `ID3DXMesh` with a vertex buffer, a 16- or 32-bit index buffer (`D3DXMESH_32BIT`) and a
per-face attribute buffer in CPU memory; `GetVertexBuffer`/`GetIndexBuffer` add no reference (buffers are not counted,
T38). What the engine calls (`CKTDXDeviceXMesh` for `.x` and KOG's `.kme`, `KTDXCollision.h`):

- **`D3DXLoadMeshFromXInMemory`**: every `Mesh` in the file, top level or in `Frame`s, merged into one mesh, each moved
  by its frames' `FrameTransformMatrix` (normals by the inverse transpose, normalized); the FVF is `XYZ` plus `NORMAL`,
  `DIFFUSE` (`MeshVertexColors`) and `TEX1` (`MeshTextureCoords`) when any mesh has them. A vertex is split when a face
  corner gives it a different normal (`MeshNormals` has its own indices); polygons become triangle fans. Materials come
  from each `MeshMaterialList` (inline or `{ Name }` references), in order; a short face list repeats its last index;
  a mesh without one gets D3DX's default material (diffuse 0.5 gray, specular 0.5). The materials buffer holds the
  `D3DXMATERIAL` entries followed by their texture names; effect instances are empty; adjacency is computed with exact
  positions. More than 65,535 vertices makes the mesh 32-bit.
- **`D3DXCreateMeshFVF`**, `Lock*`/`Unlock*`, `GetDeclaration` (from the FVF), `UpdateSemantics` (accepted; the device
  draws by FVF), `CloneMeshFVF` (elements copied by meaning, new ones zero, diffuse white).
- **`GenerateAdjacency`**: faces share an edge through point representatives (vertices at the same position, within
  epsilon on each axis); edges used by more than two faces pair in order.
- **`Optimize`**: `ATTRSORT`, `VERTEXCACHE` and `STRIPREORDER` sort faces by attribute (stable) and build the attribute
  table (one range per attribute, `VertexStart` = lowest vertex, `VertexCount` = highest − lowest + 1); `COMPACT`,
  `VERTEXCACHE` and `STRIPREORDER` renumber vertices in first-use order and drop unused ones, unless `IGNOREVERTS`.
  Face and vertex remaps and the adjacency follow.
- **`D3DXWeldVertices`**: vertices at the same position within `Position` weld to the first one whose other elements are
  within their epsilons (position only with `WELDALL`); welded vertices are removed unless `DONOTREMOVEVERTICES`. The
  attribute table's vertex ranges are recomputed.
- **`DrawSubset`**: the subset's ranges from the attribute table (or each run of the attribute when there is none) as
  indexed triangle lists, with the mesh's FVF, stream and indices set on the device.
- **`D3DXIntersect`**: every face, both sides (Möller–Trumbore); the closest hit, the hit count and all hits.

### Behavior changes and assumptions

| Where | D3DX | Port | Why |
|---|---|---|---|
| Template checks | Members checked against the templates | Not checked; members read in order | The client's files use the standard templates |
| `Optimize` with `VERTEXCACHE`/`STRIPREORDER` | Faces reordered for the GPU's vertex cache | Attribute sort and first-use vertex order only | Same contents; only draw speed differs |
| `D3DXWeldVertices` | Also splits and partially matches (`WELDPARTIALMATCHES`, `DONOTSPLIT`) | Whole-vertex welds only | The engine welds with zero epsilons and no flags |
| Order of a loaded mesh's vertices | D3DX's own | The file's, split vertices appended | Same mesh; the engine welds and optimizes after loading |

## Skinned meshes (2026-11-07, `client/x-skin`)

`CKTDXDeviceXSkinMesh` loads skinned `.x` files through `D3DXLoadMeshHierarchyFromXInMemory` and its own allocator,
and KOG's KSM files with `D3DXCreateMeshFVF` + `D3DXCreateSkinInfoFVF`; both then go through `Optimize` (with the
vertex remap), `ID3DXSkinInfo::Remap` and `ConvertToIndexedBlendedMesh` (a palette of 20), and are cloned to
`XYZB4 | NORMAL | TEX1 | LASTBETA_UBYTE4`. Drawing them uses the matrix palette in the effects (T37), which is a later
step; this one gives the engine its data.

- **`D3DXLoadMeshHierarchyFromXInMemory`**: one frame per `Frame` (`CreateFrame`, `FrameTransformMatrix` or identity,
  children in file order), one mesh container per `Mesh` with faces (`CreateMeshContainer`: the mesh in the file's FVF,
  materials with their names — D3DX's default one when there are none —, empty effect instances, adjacency, and skin
  info when the mesh has `SkinWeights`). A single top-level frame is the root; otherwise a nameless root holds the
  top-level frames and meshes. The allocator takes its own references. The file's animation sets come on an
  [animation controller](#animation-2026-11-07-clientx-animation) (none without them); the user data loader is not
  called.
- **Skin info**: one bone per `SkinWeights` (name, offset matrix, influences); the file's vertex indices go to every
  vertex split from that vertex (`XMesh::source`). `XSkinMeshHeader` is not needed. `Remap` takes Optimize's
  new-to-old remap (a vertex copied twice gets its influences twice; dropped vertices lose theirs).
- **`ConvertToIndexedBlendedMesh`**: faces sorted by attribute (stable), then grouped in order into bone combinations
  while their bones fit the palette; each combination's vertices are copied after the previous one's (a vertex shared
  by two combinations is copied twice). Each vertex keeps its four strongest influences, sorted by weight; the FVF is
  `XYZB<n> | LASTBETA_UBYTE4` plus the mesh's other elements, n the largest influence count: n − 1 weights are stored
  (the last one is what they leave of 1) and the last DWORD holds the palette slots. The mesh's attribute of each face
  is its combination's index, and its attribute table is the combinations; the bone combination buffer holds the
  entries and their palettes (unused slots `0xFFFFFFFF`). Face and vertex remaps and the adjacency follow. A face
  whose bones do not fit the palette fails.
- **Blend weights and indices in vertex layouts**: `LASTBETA_UBYTE4`/`D3DCOLOR` make the position's last DWORD the
  blend indices, so `CloneMeshFVF` copies weights as weights and indices as indices (from `XYZB2` to `XYZB4`, the
  indices move to the last DWORD), and `GetDeclaration` describes both.
- **`D3DXFrameCalculateBoundingSphere`**: the sphere around the centroid of every mesh vertex of the frame, its
  siblings and children, moved by the frames' matrices, in the given frame's parent space; zero without meshes.

### Behavior changes and assumptions

| Where | D3DX | Port | Why |
|---|---|---|---|
| A vertex no bone influences | *Not known* | Follows bone 0 with weight 1 | So its palette slot is a real bone |
| More than four influences on a vertex | *Assumed* the four strongest | The four strongest; the last weight is implied | The indices are four bytes |
| Vertex order in a combination | D3DX's own | First use by the faces | Same mesh; the engine only uses the ranges |
| `D3DXFrameCalculateBoundingSphere` | *Assumed* frames' matrices applied, siblings included (as the other frame functions) | The same | To confirm against the original when collision spheres are used |

## Animation (2026-11-07, `client/x-animation`)

`CKTDXDeviceXSkinMesh` gets its animation controller from `D3DXLoadMeshHierarchyFromXInMemory` (`.x`) or builds it
from the KSM file's KAC data (`KTDXAnimationController::ReadFromMemory`: `D3DXCreateAnimationController` with an
output per named frame, 2 tracks and 30 events, a keyframed set per KAC set filled with `RegisterAnimationSRTKeys`,
then `D3DXFrameRegisterNamedMatrices`). Each `CKTDGXSkinAnim` clones it (`CloneAnimationController`, also to add the
sets of another mesh) and drives tracks 0 and 1: `ChangeAnim` keys linear speed, weight and enable events from global
time 0 for a transition, `UpdateAnimationFrameStructure` calls `AdvanceTime(dt, NULL)` and reads the track position
back with `GetTrackDesc`; the sets give `GetName` and `GetPeriod`. The controller writes the frames of the shared
hierarchy, which `CKTDGXSkinAnim` then copies to its own frames.

In the client (2026-11-07): the 1,083 KSM controllers hold 17,980 sets, **all keyframed** (looped, 4,800 ticks per
second; no D3DX compressed data); 5,466 `.x` files have 7,295 animation sets (keys of type 0 rotation, 1 scale,
2 position and 4 matrix).

- **Keyframed sets**: per animation, the frame name and its scale, rotation and translation keys (copied and sorted by
  time). `GetSRT` takes seconds (× ticks per second): scale and translation interpolated linearly, rotation by slerp,
  held at the first and last keys; no keys of a kind give scale 1, no rotation, no translation. The period is the last
  key time over the ticks per second; `GetPeriodicPosition` wraps (loop), clamps (once) or mirrors (ping-pong) a track
  position into it. More animations than declared are refused. `Compress` stores the keys as they are.
- **Rotation keys are the conjugates of the rotations**, as `.x` files store them (`w, x, y, z`): the frame matrix is
  scale × rotation of the conjugate × translation. The client's own data decides it: at time 0, 31,329 KSM rotation
  keys match their frame's rest matrix only conjugated (7 only as they are), and in the `.x` files 90,885 animated
  frames are at rest at time 0 (34,389 elsewhere) against 63,122 (62,152) the other way. Matrix keys (type 4, and 3)
  are decomposed into scale, rotation and translation keys at the same time (a mirrored matrix gets a negative x
  scale).
- **`.x` animation sets**: `AnimTicksPerSecond` (4,800 without it), one keyframed set per top-level `AnimationSet`
  (looped), one animation per `Animation` naming a frame (by reference or inline `Frame`; one naming none is left out),
  its `AnimationKey`s. A key of another type fails the load. The controller has an output per named frame, a set per
  `AnimationSet` (file order), 2 tracks and 30 events, and the first set on track 0.
- **The controller**: outputs are frame matrices by name (`D3DXFrameRegisterNamedMatrices` registers every named frame;
  a second frame of a name already registered is not animated); tracks start enabled with weight 1, speed 1,
  position 0, low priority and no set. `AdvanceTime` adds the time, runs the events due, moves every enabled track by
  `delta × speed` (the position is not wrapped; the set wraps it), and blends: every enabled track with a set and a
  positive weight gives each animated output its SRT, and an output several tracks move gets their weighted average
  (rotations by slerp); outputs no track moves keep their matrix. Events: an enable event applies at its start; a speed
  or weight event goes from the track's value at its start to its value over its duration (linear, or smoothstep for
  ease-in-ease-out) and is then removed. `KeyTrack*` return a handle, 0 when the track does not exist or the event
  limit is reached; `UnkeyAllTrackEvents` drops a track's events. `ResetTime` sets the time to 0 and moves pending
  events by as much. `CloneAnimationController` copies outputs, sets, tracks, events and time within the new limits
  (fewer outputs or sets than registered are refused).
- **Compressed sets**: `D3DXCreateCompressedAnimationSet` takes only data from `Compress` (our lossless format:
  names and keys); `GetCompressedData` returns it.

### Behavior changes and assumptions

| Where | D3DX | Port | Why |
|---|---|---|---|
| Compressed animation data | D3DX's own lossy format | Our lossless format; D3DX's data is refused | No client file has any (2026-11-07); only `KTDXAnimationController`'s writer reads it back |
| Blending tracks | *Not known* whether weights are normalized; priorities blended by a priority blend | Weighted average of the tracks moving an output; priorities not told apart | The engine keys weights that add up to 1 and uses only low priority |
| Speed changes inside one `AdvanceTime` | *Assumed* the speed after the events | Events first, then `position += delta × speed` | Differs from integrating the speed by part of a frame during a transition |
| Callback keys | Reported to the handler | Kept, not reported | The engine passes no handler |
| Order of `.x` sets, track 0 | *Not known* (possibly reversed) | File order, the first on track 0 | The engine finds sets by name |
| Tracks and events of the `.x` controller | *Assumed* 2 tracks, 30 events | The same as the KSM path | The engine uses tracks 0 and 1 |
| A second frame of the same name | *Not known* | The first one is the output | The load would otherwise fail |

## DXUT and the skeleton

The [DXUT replacement](client-dxut.md) creates the device with the window, passes it to the `FrameMove`/`FrameRender`
callbacks and calls `Present` after `FrameRender`, as DXUT did; `DXUTGetD3DDevice()` returns it. The skeleton
(`X2/X2Skeleton.cpp`) draws a test card through it: a textured, alpha-blended quad and a turning triangle fan.

## Not emulated yet (2026-11-07)

Lighting with materials (stored, see above; `D3DRS_LIGHTING` is on by default, but lit geometry goes through the effects, T37), fog, specular,
point sprites and point sizes, texture transforms, stages 2–7, `D3DTA_TEMP`/`RESULTARG`, bump mapping, border and
mirror-once addressing (mapped to clamp and mirror), the scissor test, `DrawIndexedPrimitiveUP` and indexed fans,
vertex declarations and programmable shaders (`SetVertexShader`/`SetPixelShader`), `StretchRect`,
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
- `D3DXCheckTextureRequirements`: defaults, mip chains, formats a render target cannot take; a file that is not an image
  fails cleanly.
- `test_d3dx_texture.cpp` (`-tc="d3dx*"`): `.dds`/`.tga`/`.png`/`.bmp` built in the test — the info of every format and of
  files that are rejected, TGA orientation, RLE, 16-bit alpha, gray and color maps; BC1–3 decoding against hand-made
  blocks and round trips through stb_dxt; every format to and from BGRA8; the four filters; textures copied, converted,
  generated, color-keyed and cut short, volumes and cube maps; `CKTDXDeviceTexture`'s High/Medium/Low loads. With
  `X2_MASSFILE_CLIENT_DATA` set to the client's `data` folder, every texture of its archives is decoded (8 threads, about
  4 s in the container, 2026-11-07), the three unreadable ones are exactly the ones expected, and a few go through the
  device at every detail level.
- `test_d3dx.cpp` (`-tc="d3dx*"`, and the two `d3d9:` cases below): the material and lights with D3D9's default light,
  `UpdateTexture`'s copy and refusals, buffers and `CComPtr` reference counts, frame search/count/destroy through the
  allocator, `D3DXCreateTexture`'s mip chain, the effect creators failing with null outputs.
- `test_d3dx_mesh.cpp` (`-tc="d3d9: indexed*,xfile*,d3dx*"`): indexed draws (16/32-bit, base vertex, bounds, a released
  index buffer unset); `.x` files written in the test as text, binary and MSZIP (one block and blocks leaning on the
  previous one's dictionary), broken ones; `D3DXLoadMeshFromXInMemory` on a framed quad (normal split, materials inline
  and by reference, texture names, adjacency) and on a mesh with no materials; `D3DXCreateMeshFVF`, `CloneMeshFVF`,
  `GetDeclaration`, adjacency, `D3DXWeldVertices`, `Optimize` and its attribute table, `D3DXIntersect`, `DrawSubset`.
  With `X2_MASSFILE_CLIENT_DATA`, every `.x`/`.y` of the client's archives is read and every one with a mesh goes
  through `CKTDXDeviceXMesh`'s steps (load, clone, weld, optimize).
  On 2026-11-07: 26,874 unique `.x`/`.y` names, 25,679 `xof` files (the rest are KOG's `KME`/`KSM`), all read; the
  25,588 with a mesh all load and pass, with every attribute below the material count (a few minutes in the debug
  build, the device calls serialized).
- `test_d3dx_skin.cpp` (`-tc="d3dx*"`): skin info (names, offsets, influences, `Remap` with copies and drops),
  `ConvertToIndexedBlendedMesh` on a strip of five bones with a palette of three (combinations split by palette and
  material, shared vertices copied, weights and slots, adjacency, the clone to the engine's format and its
  declaration, a palette too small), `D3DXLoadMeshHierarchyFromXInMemory` (frames, skinned and unskinned containers,
  weights on split vertices, a nameless root, a file with no frame or mesh) and `D3DXFrameCalculateBoundingSphere`.
- `test_d3dx_animation.cpp` (`-tc="d3dx*"`): keyframed sets (sorting, interpolation, held ends, missing keys,
  playback, the animation limit), compressed sets (round trip, foreign data refused), the controller (outputs, tracks,
  speed, disabled tracks, a `ChangeAnim` transition with its blend, events waiting for their start and after
  `ResetTime`, the event limit, clones that register more sets, unregistering), and `.x` animation sets (rotation keys
  at the rest matrix, interpolation, matrix keys and a mirror, no controller without sets, an unknown key type). With
  `X2_MASSFILE_CLIENT_DATA`, every `.x` with animation sets loads with its controller and every set plays (about
  2 minutes in the debug build): 5,466 files, 7,295 sets on 2026-11-07, and the rest-matrix count above.
- `test_ktdxlib.cpp`, with `X2_MASSFILE_CLIENT_DATA`: every skinned mesh of the client loaded by
  `CKTDXDeviceXSkinMesh` itself, textures included, and every set of a clone of its controller played. On
  2026-11-07: 8,687 skinned meshes (7,600 `.x` with `SkinWeights`, 1,087 KSM — named `.x` too), all loaded (hierarchy
  or KSM frames and KAC controller, optimize, remap, indexed blended mesh); 1,576 animated, 19,926 sets. The engine logs
  `OpenXET` failures for meshes with no `.xet` side file (most of them; it is optional) into `ErrorLog.txt` in the
  working folder.
- **On a GPU** (skipped without video or GPU, as in the container and CI): renders into a 16×16 target and reads it back —
  texels mapped 1:1 to pixels with the half-pixel rule, the vertex color with no texture, 50% blending, the alpha test, a
  fan. Passed on the user's Linux desktop (Vulkan, 2026-11-06).
