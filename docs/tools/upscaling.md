[Home](../README.md) › [Tools](README.md) › Upscaling

# Upscaling workspace

The client's art is redrawn at higher resolution outside the repository, in a portable workspace on the user's
machine, `/home/maru/upscale` (set up 2026-11-15). Only the results enter the game, through the
[client_data](client-data.md#content-changes-t42), in the original formats (T1). Its own `README.md` has the
commands; this page records what it is and what was learned.

| Folder | What |
| --- | --- |
| `chaiNNer/` | chaiNNer 0.26.1, portable, with PyTorch (CUDA); native, not in Docker. |
| `chainner-agent/` | CLI (`chn`) and MCP server (`claude mcp get chainner`, user scope) that build, validate and run `.chn` chains without the GUI. Made for Windows; on Linux it needs `CHAINNER_EXE`, `CHAINNER_BACKEND`, `CHAINNER_PYTHON`, `CHAINNER_USER_DATA` and `CHAINNER_VERSION=0.26.1`. |
| `forge/` | Stable Diffusion WebUI Forge in Docker (`docker compose up -d`, NVIDIA runtime), API at `127.0.0.1:7860`, the Automatic1111 API that chaiNNer's "Stable Diffusion (external)" nodes call. Model: Animagine XL 4.0. About 13 GB with the model. |
| `chains/` | The chains and the scripts that generate them. |
| `tools/texconv.py` | Game DDS/TGA to PNG and back, each file keeping its original format. |
| `workspace/` | Inputs (`original/`, `png/<preset>`) and outputs (`wide/`, `upscaled/<preset>`, `out/<preset>`). |
| `4x-AnimeSharp.pth`, `4x-UltraSharp.pth` | The upscaling models. |

## Texture pipeline

1. `texconv.py to-png <game folder> workspace/png/<preset>`: decodes DDS and TGA to PNG (alpha kept) and writes a
   `manifest.json` with each file's format (header, DXT kind, bit masks, mipmaps, TGA depth/RLE/palette). Fake
   `.tga` files (BMP or JPEG inside) are skipped.
2. `chn run chains/upscale-<preset>.chn` (or the MCP's `chainner_run_chain`): every PNG of the tree through the
   preset's model, the tree kept, existing outputs skipped.
3. `texconv.py from-png workspace/upscaled/<preset> workspace/out/<preset> --manifest ...`: back to each file's
   original format; `texconv.py check <original> <converted>` confirms it. chaiNNer cannot write DDS on Linux, hence
   this script: DXT1 (1-bit alpha kept), DXT3 and DXT5 through Pillow 13 (ImageMagick drops DXT1 alpha and writes
   DXT5 for DXT3), uncompressed DDS with the original's bit masks, TGA with its own depth, RLE and palette, and the
   full mipmap chain when the original had one; `--mips` gives every DDS a full chain (model and world textures:
   the client loads a file's chain, [D3D9](../porting/client-d3d9.md); atlases with a `.tet` load one level). A round trip of one file of every format kept the format
   (2026-11-15).

Formats in the client_data (2026-11-15): DXT1 29,673, DXT5 1,808, DXT3 1,292, RGB24 778, RGB32 with alpha 438, DXT1
volumes 49, RGB32 15, RGB16 with alpha 12; about a third have mipmaps.

Presets (`chains/build_batch_chain.py <preset>`; `--scale 2` makes a 2× set from the 4× result):

| Preset | Model | Edges | For |
| --- | --- | --- | --- |
| `ui` | 4x-AnimeSharp | mirrored | UI atlases (TGA, alpha) |
| `model` | 4x-AnimeSharp | wrapped | character, item, monster, NPC and pet textures |
| `world` | 4x-AnimeSharp | wrapped | map and terrain textures (tileable) |
| `effect` | 4x-UltraSharp | mirrored | effect textures (fire, smoke, sparks) |
| `background` | 4x-AnimeSharp | replicated | full-screen art already at 16:9, resized to 2560×1440 |

Model choice, compared on the client's own textures (2026-11-15): the game is hand-painted anime art, and
4x-AnimeSharp keeps its line work and painted detail (UI text, a monster's veins, terrain) where 4x-UltraSharp
smooths it or keeps the old compression noise; on soft effect textures 4x-UltraSharp keeps the brushed grain that
AnimeSharp flattens. Alpha is upscaled by the model too ("Separate Alpha"), so cut-outs keep clean edges.

## Full-screen 4:3 backgrounds to 16:9

For `client/widescreen-backgrounds` ([T51](../project/decisions.md),
[backdrops](../porting/client-ktdxlib.md#backdrops-2026-11-15-clientwidescreen-backgrounds-t51)); the login screen
was the first (2026-11-15).

1. `chains/build_outpaint_chain.py` → `login-16x9-outpaint.chn` (needs Forge up): assembles the 4:3 screen from its
   tiles, mirrors it out to 1368×768, Stable Diffusion inpaints the sides plus 128 px of the original (mask blur 32,
   fill "Original", denoise 0.75; prompt, negative and seeds at the top of the script), and the untouched original
   is pasted back with a feathered edge (96 px, blur 48), so the center is the original pixel for pixel.
2. `login-16x9-upscale.chn`: the chosen variant through 4x-AnimeSharp, cropped to exact 16:9 (5460×3072 master)
   and resized to 2560×1440.
3. In the client_data: one 2560×1440 TGA, a `.tet` key covering it, the picture from x -170.67 to 1194.67, and
   `BACKDROPS = { <static> = 64 }` in `UIAnchors.lua`.

Learned on the way: chaiNNer's Gaussian Blur returns the image unchanged when either radius is 0 (the first seams
came from that); a hard mask or a strong prompt leaves visible cuts between original and generated art.

## UI textures

Not done yet: the UI atlases (`DLG_Login3.TGA` and the rest) are still the originals, and look soft next to a
2560×1440 background (user, 2026-11-15, login screen). They cannot simply be replaced, because control sizes come
from the `.tet` keys in pixels; that is the roadmap step `client/ui-hd-assets`
([roadmap](../project/roadmap.md#phase-4--client-graphics)).
