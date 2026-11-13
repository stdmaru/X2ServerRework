[Home](../README.md) › [Tools](README.md) › client_data

# client_data (`src/tools/AssetTool`, `x2_assets`)

Decision T35: tools never edit `.kom` directly. `x2_assets` imports the client's archives into **client_data**, an
organized folder of ordinary files plus a manifest (not versioned: it holds the client's assets), and packs it back. An
unchanged client_data packs back into every original `.kom` **byte for byte**. The `.kom` format itself is on
[`.kom` archives](../porting/kom.md).

| Target | For |
|---|---|
| `x2::assets` (`x2/assets/client_data.h`, `manifest.h`, `layout.h`) | import, pack and status; tested headless |
| `x2_assets` | its command line |

```bash
x2_assets import _client/data <client_data> --layout=tools/assets/layout.txt   # -j<N>: threads (default: all); --no-lua
x2_assets status <client_data>                       # changed, missing and new files
x2_assets pack <client_data> <output folder>         # every .kom of the manifest
x2_assets layout _client/data --layout=tools/assets/layout.txt   # folder counts only, nothing written
```

In the build image: `build/linux-debug/src/tools/AssetTool/x2_assets`. Import refuses a destination that is not empty;
pack refuses to write into client_data itself.

## What client_data holds

```
client_data/
  .kom/manifest.tsv          what packs the folder back (below)
  .kom/headers/<x>.kom.hdr   each archive's original fixed + XML header (~13 MB in all)
  .kom/blobs/<name>          original stored blobs that re-encoding does not reproduce (11.5%, 763 MB)
  .kom/originals/<name>      the bytecode of decompiled scripts and the original Item.kim (below)
  ui/ characters/ items/ ... the files, decoded, named as in the archives
```

- **Files are found by name, never by path**: pack indexes client_data recursively (skipping `.kom/`) by file name,
  ASCII case-insensitive, so files and whole folders can be moved or renamed freely. A name in two folders is an error.
- Files are the entries **decoded** (algorithms 0, 2 and 3), with the Lua made readable ([below](#lua)). Entries of
  algorithms `x2::kom` does not decode (1; none in the client) are written as their stored blob.
- The manifest (`x2/assets/manifest.h` has the exact format) is a tab-separated text file: one `archive` record per
  `.kom` in load order (name, entry count) and one `file` record per entry: name, archive, position, algorithm, decoded
  and stored sizes, original checksum, `FileTime`, **encoding**, **form** and the SHA-256 of the file's content
  (manifest version 2; a client_data of version 1 must be imported again). Encodings:
  - `zlib`: encoding the content with the entry's algorithm at **zlib level 6** gives the original blob;
  - `blob`: it does not (another compressor, [Writing](../porting/kom.md#writing)); the original is kept in
    `.kom/blobs/` and used while the content's SHA-256 still matches;
  - `stored`: the file is the stored blob itself.

  Forms: `raw` (the file is the entry), `lua` (the file is the decompiled source of a bytecode entry), `kim` (the entry
  is `Item.kim`, held as `Item.lua` + `SetItem.lua`; its SHA-256 is that of the two scripts' hex hashes concatenated).

## Lua

<a id="lua"></a>Import makes the client's Lua readable with [`x2::lua`](../porting/lua.md) (`--no-lua` keeps it as it is):

- Every `.lua` entry that is KOG bytecode is **decompiled** to UTF-8 source with BOM, form `lua`, if LuaJIT reads the
  output back; the bytecode goes to `.kom/originals/`. A script that does not decompile stays bytecode (form `raw`) and
  import names it with the reason. All 4,553 of the client's do (2026-11-05); 9 `.lua` entries were source already.
- `Item.kim` becomes **`Item.lua` + `SetItem.lua`** in its folder (form `kim`), with the enum names of `Enum.lua` when
  that one is source; the original goes to `.kom/originals/`. If an archive already has an entry of either name, it
  stays `Item.kim`.
- Pack writes an **unchanged** file's original from `.kom/originals/`, so an unchanged client_data still packs back byte
  for byte. An **edited script** is packed as **UTF-8 Lua source with BOM** (user, 2026-11-05; a BOM is added if
  missing), after LuaJIT checks its syntax: a script that does not load stops the pack. Bytecode put back into
  client_data is packed as it is. Edited `Item.lua`/`SetItem.lua` are **compiled into a new `Item.kim`**, with the
  warnings of KOG's tool printed; a missing one makes `Item.kim` missing.
- Source in the archives is what the **ported client** loads (T21: upstream LuaJIT 2.1). The original `x2.exe`'s LuaJIT
  has no parser, so it cannot load edited scripts; for it, `tools/kom/kom.py pack` recompiles with KOG's LuaJIT
  ([Server package](runtime.md#toolskom)).

## Packing

For each archive of the manifest, in position order: a file whose SHA-256 matches is encoded as imported (or its kept
blob is used) with its original `FileTime`; an edited file is re-encoded with its entry's algorithm at level 6 (new
`FileTime` = checksum, as KOG's packer); a missing file is dropped. When the entries still match the archive's original
header, that header is written (`ArchiveBuilder::SetHeader`), so both header layouts of the client
([XML header](../porting/kom.md#xml-header)) come back unchanged; otherwise KOG's layout is generated. Files not in the
manifest are reported and **not packed**. Every archive is rewritten. Rebuilding only the changed archives and choosing
an archive for new files belong to x2 studio's export to `.kom` ([roadmap](../project/roadmap.md#later--x2-studio)).

**Byte identity needs stock zlib 1.3.1**: the project's pinned FetchContent version (Windows) and the Ubuntu image's
system zlib. Other deflate implementations write other, equally valid streams: Arch's zlib-ng gives different bytes for
almost every entry (2026-11-05). Pack then still writes valid archives and reports each such entry as "not reproduced".

## Layout

<a id="layout"></a>Where import puts each file comes from versioned rules, `tools/assets/layout.txt` (`x2/assets/layout.h`
describes the syntax): `<folder> <name pattern> [<archive pattern>]`, case-insensitive globs, first match wins,
`{archive}` for the source archive; unmatched files go to `_unsorted/<archive>`. The layout is a **living layout, not a
decision** (user, 2026-11-05): refine it as needs show up; it never changes what pack writes. Template, by domain rather
than by extension:

| Folder | Contents |
|---|---|
| `ui/` | `DLG_*` scripts and textures, `.tet`, minimap, titles, loading, class change, NPC portraits, cut-ins, fonts |
| `characters/<character>/` | `motions/` (`Motion_<character>*.x`), `skills/` (the per-character skill effect archives) |
| `characters/scripts/` | unit scripts (`data037`) |
| `items/` | `Item.kim` and item tables, `equipment/<character>/` (`Mesh_<character>*`), `icons/` (`HQ_Shop*`), `drops/` |
| `monsters/<archive>/`, `npcs/`, `pets/` | by name (`NUI_*`, `GOD_*`, `NPC_*`, `PET_*`) and by archive |
| `world/<region>/` | dungeon and stage scripts, region meshes and textures by name (`El_Forest*`, `Velder*`, ...) |
| `effects/` | particle tables, effect textures with no clear owner |
| `sound/` | `bgm/`, `ambient/`, `voice/`, `sfx/` |
| `shaders/`, `strings/`, `data/` | `.fx`; string tables; the other data tables (`data036`, `datai015`) |
| `_unsorted/<archive>/` | whatever no rule classified |

Coverage (2026-11-05, `_client/data`): 101,826 files, 1,563 (1.5%) in `_unsorted`; `x2_assets layout` lists the most
common unsorted prefixes. The archives group by subject, which several rules use: `data015`–`020`, `107`, `131`, `143`,
`148` equipment; `data116`–`147` character skills; `data005`–`012`, `100`, `111`, `136` monsters; `data069`–`150` mostly
sound; `data067`, `130`, `datai023` icons. Later: a dependency graph (Lua and `.xet` name meshes and textures).

Measured on `_client/data` (2026-11-05, `linux-debug`, `-j8` on 16 threads): import 74 s (5.19 GB of files, 11,723 kept
blobs, client_data 5.9 GB in all); pack 52 s, all 173 archives identical to the originals, all with their original header.

## Tests (`ctest -R assets`)

`x2_assets_tests`, headless, on archives the tests build: glob and rule parsing, manifest round trip and malformed
manifests, import of both header layouts with every encoding (`zlib`, a blob compressed at another level, an opaque
entry, a non-ASCII name), byte-identical pack, moved files, edited/missing/new files, and the refusals (non-empty
destination, a name in two archives or two folders, an edited opaque entry). Lua: a bytecode script decompiled, a
truncated one kept, an Item.kim converted with Enum.lua's names, an unchanged pack byte for byte, edited scripts packed
as source and Item.kim compiled again, scripts that do not load refused, and `--no-lua`.

**Opt-in**: `X2_ASSETS_CLIENT_DATA=<client data folder> x2_assets_tests -tc="*client*"` imports with
`tools/assets/layout.txt`, checks that every script decompiled and Item.kim was converted, that the status is clean, packs and compares every archive byte for byte. It needs about
6 GB free for `_client/data` in `X2_ASSETS_WORK_DIR` or the system temporary folder. 2026-11-05, `linux-debug`, with
readable Lua: 4,553 scripts decompiled, none kept as bytecode, Item.kim converted, 173 of 173 archives identical,
7 min 14 s (6 min 22 s without Lua).

## Content changes (T42)

Since 2026-11-12 the client's content may change ([T42](../project/decisions.md)). Every file changed in the client_data
is listed here with the step that changed it and why.

| File | Step | Change |
|---|---|---|
| `ui/scripts/DLG_UI_Option_Graphic.lua` | `client/msaa` (2026-11-12) | Three rows after the resolution: anti-aliasing, anisotropic filtering, frame limit and supersampling combo boxes and the vertical sync check box (helpers `AddGraphicLabel`/`AddGraphicCombo` in the script, labels in Portuguese with `STRING`) |
| `ui/scripts/DLG_UI_Option_Etc.lua` | `client/msaa` (2026-11-12) | Vertical sync row removed (moved to the graphics tab); the rows below it moved up 26 points |
| `ui/scripts/DLG_UI_Option_Back_New.lua` | `client/msaa` (2026-11-12) | Option window 87 points taller: the nine background pieces' `ADD_SIZE_Y` 26 → 113 |
| `data/Enum.lua` | `client/msaa` (2026-11-12) | `STATE_OPTION_UI_CUSTOM_MSG`: `SOUCM_OPTION_GRAPHIC_MULTISAMPLE` 2520, `ANISOTROPY` 2521, `FRAME_LIMIT` 2522, `SUPERSAMPLE` 2523 |
| `ui/scripts/DLG_UI_Option_Graphic.lua` | `client/resolution` (2026-11-12) | Window mode check box: hit box `RIGHT_BOTTOM` (282, 275) → (282, 285). The original's was a 15×5 strip at the top of the 15×15 box, so only its upper edge took clicks; the other check boxes of the dialog are 15×15 |
| `ui/scripts/UIAnchors.lua` (new) | `client/widescreen-anchors` (2026-11-13) | The dialogs' horizontal anchors ([T44](../project/decisions.md), [format](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)), empty at first. `client/widescreen-migration` (2026-11-13) fills the HUD: chat, quick slots, the player's portrait left; minimap, party list, quest tracker, the player's buffs and the menu buttons right; the EXP bar and loading bars STRETCH (drawn centered until `client/widescreen-stretch`); the free training menu by rows. A new file: dev builds read it as a loose file; `pack` does not write new files yet ([Packing](#packing)), so a release build needs that first |
