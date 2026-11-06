[Home](../README.md) › [Tools](README.md) › Shaders

# Shaders (`tools/shaders/compile.py`)

The project's HLSL shaders are compiled **offline** and their outputs committed (decision
[T39](../project/decisions.md#accepted)): builds never run a shader compiler, so they work the same on Linux x86_64 and
arm64 and on Windows. Added on 2026-11-06 (`client/ktdxlib-device`).

## Compiler

[SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross) (T19) on Microsoft's prebuilt DXC, both in the
[build image](build-environment.md) under `/opt/shadercross` (x86_64 only; DXC is not published for Linux arm64). Pinned
in `docker/build/Dockerfile`: shadercross `1ff05bec` (2026-09-03), DXC `v1.9.2609` (Linux tarball checked by SHA-256),
SPIRV-Cross from Ubuntu's package. For each shader it writes SPIR-V (Vulkan), DXIL (D3D12, signed with DXC's `libdxil.so`)
and MSL (Metal, entry point `main0`), and reads shadercross's JSON reflection (sampler and uniform block counts).

## Use

```bash
docker/build.sh python3 tools/shaders/compile.py src/client/KTDXLIB/D3D9/shaders
```

```bash
docker/build.sh python3 tools/shaders/compile.py --check src/client/KTDXLIB/D3D9/shaders
```

`--check` only compares the hash (no compiler needed). Commit the regenerated `.inc` with the `.hlsl` change.

## Shader directories

Each has a `shaders.json`:

| Key | Meaning |
|---|---|
| `output` | The generated include file, relative to the directory |
| `shaders[].name`, `source`, `stage` | Table name, `.hlsl` file, `vertex` or `fragment` |
| `shaders[].variants` | Optional `{"DEFINE": [values]}`: one shader per combination, in the defines' order, the last fastest |

HLSL follows SDL_GPU's binding rules: vertex uniforms `b<n>, space1`; fragment textures and samplers `t<n>`/`s<n>, space2`;
fragment uniforms `b<n>, space3`. Vertex inputs use `TEXCOORD<n>` with `[[vk::location(n)]]` (D3D12 matches by semantic,
Vulkan by location).

The generated file has one `x2::GeneratedShader` table per shader (`KTDXLIB/D3D9/GeneratedShader.h`), the byte arrays,
`kShaderSourceFiles` and `kShaderSourceHash`: FNV-1a 64 over each file's name, a NUL, its bytes and a NUL, in order
(`shaders.json` first). A test recomputes it from the sources and fails when they changed without regenerating
(`d3d9: the generated shaders match their sources`). Generated files are marked `linguist-generated -diff` in
`.gitattributes`.

| Directory | Shaders | Used by |
|---|---|---|
| `src/client/KTDXLIB/D3D9/shaders` | Fixed-function vertex (12 variants) and fragment; 924 KB generated (2026-11-06) | [D3D9 device](../porting/client-d3d9.md) |
