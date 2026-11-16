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
  warnings of KOG's tool printed; a missing one makes `Item.kim` missing. Debug and dev clients read the loose `Item.lua`/`SetItem.lua` instead ([X2Lib](../porting/client-x2lib.md#items-from-the-loose-scripts-2026-11-15-serveradd-character)).
- Source in the archives is what the **ported client** loads (T21: upstream LuaJIT 2.1). The original `x2.exe`'s LuaJIT
  has no parser, so it cannot load edited scripts; for it, `tools/kom/kom.py pack` recompiles with KOG's LuaJIT
  ([Server package](runtime.md#toolskom)).

## Packing

For each archive of the manifest, in position order: a file whose SHA-256 matches is encoded as imported (or its kept
blob is used) with its original `FileTime`; an edited file is re-encoded with its entry's algorithm at level 6 (new
`FileTime` = checksum, as KOG's packer); a missing file is dropped. When the entries still match the archive's original
header, that header is written (`ArchiveBuilder::SetHeader`), so both header layouts of the client
([XML header](../porting/kom.md#xml-header)) come back unchanged; otherwise KOG's layout is generated. Files not in the
manifest are reported and **not packed**. Every archive is rewritten. [T50](../project/decisions.md) (2026-11-15): `x2_assets`
gains a GUI and a `--cli` mode that add and remove manifest entries, so new files get an archive and are packed;
rebuilding only the changed archives stays with x2 studio's export to `.kom` ([roadmap](../project/roadmap.md#later--x2-studio)).

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

## Bringing data from other clients

Four scripts bring what our client lacks from other clients, and can be run again (a second run changes nothing). The sources and their order are on [Add character](../project/add-character.md).

```bash
tools/assets/merge_skills.py <client_data>/data/NewSkillTempletVer2.lua --skills 9000-9014,9200-9207 \
    --classes UC_ADD_NASOD_RULER,... --source <file> [--source <file> ...] --text <file> [--tree <file>] [--dry-run]
tools/assets/merge_items.py <client_data>/items --enum <client_data>/data/Enum.lua --unit-type UT_ADD \
    --source <Item.lua> [--source ...] --text <Item.lua> --luajit /usr/bin/luajit [--dry-run]
```

- `merge_skills.py`: each skill from the first `--source` that has it; its name and descriptions from `--text` where
  the source's are empty or Korean; each class's tree entries from the first of `--tree` and the sources that has them.
- `merge_items.py`: the items (by `--unit-type` or `--ids`) that our `Item.lua` lacks, from the first `--source` that
  has them, with `--text`'s name and description where the source's are Korean (items left Korean are skipped), and
  their sets from the `SetItem.lua` beside a source. It also reads V5's `ClientScript` export (list fields such as
  `m_RenderRotateList`, rotations in radians) into the original keys (`ADD_ITEM_SCALE_ROTATE` in degrees, ...). Items
  with a value our `Enum.lua` lacks (classes we do not have, `IT_CUSTOM`...) are skipped; an item whose set is in no
  source gets `m_SetID = 0` (`CX2ItemManager::GetSetItem` asserts on an unknown set). The result is compiled with
  `kim.scripts_to_kim` to check it.
- `copy_assets.py <client_data> --scripts <Item.lua> --dir <extracted archive> ... --kom <.kom folder>`: every file
  name the scripts quote that no file of the client_data has, from the first `--dir` (named after its archive, e.g.
  `data148_kom`) or `--kom` archive that has it, into the folder `x2_assets import` would choose. The meshes (`.x`/`.y`,
  MSZIP-compressed DirectX files too) and `.xet` it copies are scanned for the files they name. `--report` lists what
  came from where and what is in no source.
- `merge_quests.py <client_data> --quests <IDs> --source <data036 folder> --fallback <folder> --text <folder> --ess
  <our general.ess> --strings <their General.ess>`: quests by ID with the sub-quests, scenes (`EventScene.lua`) and
  scene objects they name that ours lacks. A `--source`'s text is kept unless Korean; a `--fallback`'s (V5, English)
  is replaced by `--text`'s. Classes our `Enum.lua` lacks are dropped from class lists. The `STR_ID`s the scenes use
  are defined in `StringID_def.lua`, and their strings are copied into `general.ess` (the client's folder, outside
  client_data) where ours lacks them. String IDs are the same in our table, BR 2017's and V5's (checked 2026-11-15,
  e.g. 1860, 8402, 30631; 2,375 of V5's scenes use the same IDs as BR 2014-08's). Items, dungeons and NPCs the
  quests name that we lack are reported (then brought with `merge_items.py --ids`).
- `fill_strings.py --ess <our general.ess> --strings <their General.ess> [--strings ...]`: every string of ours that is
  a placeholder (empty, or its own name `STR_ID_<n>`, which the client shows as is) or Korean takes, by ID, the first
  source's text that is real and not Korean; everything else of ours is kept. String IDs are stable between our
  table and BR 2017's (2026-11-16: 25,823 of the 26,389 IDs both have text for are equal, the rest revised wording or
  Korean in ours).
- `merge_player_text.py <client_data> --source <EventScene.lua> [--source ...] --ess <our general.ess> --strings
  <their General.ess>`: a scene's line picks `PLAYER_TEXT<unit type - 1>` for the player's character and falls back
  to `TEXT` (empty in most) when that index is missing, so Add (`UT_ADD` = 9) needs `PLAYER_TEXT8`. The lines up to
  `--max-index` (8) a text play of ours lacks are taken, by scene ID, sequence and text play, from the first
  `--source` that has them; ours are never changed. Their STR_IDs are defined and their strings copied as
  `merge_quests.py` does.

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
| `ui/scripts/UIAnchors.lua` (new) | `client/widescreen-anchors` (2026-11-13) | The dialogs' horizontal anchors ([T44](../project/decisions.md), [format](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)), empty at first. `client/widescreen-migration` (2026-11-13) fills the HUD: chat, quick slots, the player's portrait left; minimap, party list, quest tracker, the player's buffs and the menu buttons right; the EXP bar and loading bars STRETCH; the free training menu by rows. `client/widescreen-stretch` (2026-11-13) adds `STRETCH_ZONES` to the menu bar (its two plain middles, 49–494 and 569–1011) and the loading bars, and anchors the first loading screen's version string LEFT. A new file: dev builds read it as a loose file; `pack` does not write new files yet ([Packing](#packing)), so a release build needs that first `client/widescreen-screens` (2026-11-13) anchors character select (lists and buttons right, exit and delete left, slot dialogs `DIALOG = "RIGHT"`) and character creation (panel, video and Back right; title, info, class-change dialogs and Exit left), and stretches the name input's shade |
| `ui/scripts/UIAnchors.lua` | `client/widescreen-backgrounds` (2026-11-15) | `BACKDROPS` explained in its header; `DLG_Login_Back.lua` declares its static `Login` a backdrop with a 64-unit fade ([T51](../project/decisions.md)) |
| `ui/scripts/DLG_Login_Back.lua` | `client/widescreen-backgrounds` (2026-11-15) | The four background pictures (`DLG_Login0`, `1`, `2` ×2, 512×512 tiles of the 1024×768 screen) replaced by one, `DLG_Login_16x9.tga` key `BG01`, from (-170.67, 0) to (1194.67, 768): the 16:9 screen centered on the 1024 column |
| `ui/login/DLG_Login_16x9.tga`, `.tet` (new) | `client/widescreen-backgrounds` (2026-11-15) | The login screen at 16:9, 2560×1440, 24-bit uncompressed TGA as the originals; its `.tet` has one key, `BG01`, the whole texture. Made from the original's tiles: sides by Stable Diffusion outpainting, the original pasted back untouched, 4x-AnimeSharp. The old tiles stay (unused) |
| `ui/scripts/DLG_Unit_Select_Back_New.lua` | `client/widescreen-screens` (2026-11-13) | The pictures `Bg_ChaList` and `Bg_ChannelList` moved out of `charac` / `ChannelListStatic` into new statics `characListLabel` and `ChannelListLabel`, so the labels follow their lists to the right; the code shows them |
| `ui/scripts/DLG_Unit_Create_Back_New.lua` | `client/widescreen-screens` (2026-11-13) | `Bg_line` and `Bg_StatCategory` moved into a new static `StaticUnit_NoChange_Unit_Info_Right`, and `star_graph_noclass` into `StaticUnit_Create_Unit_Graph`, so they go right with the panel while the rest stays left |
| `ui/scripts/DLG_Unit_Create_Front_Menu_New.lua` | `client/widescreen-screens` (2026-11-13) | The daily creation notice (`Bg_Notice_4`) moved out of `Creative_Character` (the panel, right) into a new static `Creative_Character_Notice`, so it stays centered under the character |
| `ui/scripts/DLG_Menu_Button_NEW.lua` | `client/widescreen-stretch` (2026-11-13) | The vigor bar's middle (`g_pPicture_TIRED_Bg_CENTER`) `ADD_SIZE_X` 433 → 432, so it ends at 1011 where the end piece starts (569 + 10 + 432), as the EXP bar's does. Its semi-transparent inside overlapped the end piece by one unit, a darker column at the bar's right end, in the original too |
| `ui/scripts/DLG_PVP_Game_My_State_NEW.lua` | `tools/ui-editor` (2026-11-14) | The HUD portrait and bars (`SetPos`) (0, 0) → (-2, -2), so the portrait's frame touches the screen's top left corner (it started 2 units in and 2 down, ~3 px at 1080p, in the original). The hit shake now returns to this position ([X2Lib](../porting/client-x2lib.md#hud-gauge-shake-2026-11-14-toolsui-editor)) |
| `ui/textures/DLG_UI_Common_Texture{16,21,23,25,45,66,68,70,74,76,77,80,81}_NEW.tet` | `tools/ui-editor` (2026-11-14) | The HUD portraits' keys lined up by `tools/assets/crop_portraits.py` (run it on a new client_data): in 42 the frame starts 2 texels in from the key's left edge, as kept; in 12 (Ara, Elesis, Add, Code: Sariel) 3 texels and a row lower, cropped by one each; `M_KNIGHT_STATE` started a column short, inside its own band, widened by one. They did not line up from class to class in the original. Chung's frameless faces (`Texture61`) are left as they are |
| `ui/textures/DLG_UI_Common_Texture{16,21,23,25,45,66,68,70,74,76,77,80,81}_NEW.tga` | `tools/ui-editor` (2026-11-14) | Same script: fully transparent texels within 2 of a visible one take their visible neighbours' average color (they were white, and linear filtering drew a light line along the portrait's frame at the screen edge). Alpha and every visible texel unchanged; still uncompressed 32-bit TGA |
| `ui/scripts/DLG_UI_Community_Friend.lua` | `tools/ui-editor-layout` (2026-11-14) | Lines 33 and 37: the add-friend button's `SetDisableTex` and `SetDisablePoint` called on `g_pButtonFriend_add`, a typo of `g_pButtonfriend_add`. In the original it failed (the script stopped there) or, after `DLG_Search_Friend.lua` or `DLG_UI_Community_Whole.lua` had set that global, gave their button the disabled texture and point; found when the UI editor opened the dialog alone |
| `ui/scripts/DLG_Unit_Create_Front_Menu_New.lua` | `tools/ui-editor-layout` (2026-11-15) | Line 132: `g_pPromotionMovie:SetLoop(true)` → `g_pIntroMovie:SetLoop(true)`, the movie the lines above create. `g_pPromotionMovie` is the character select screen's movie (`DLG_Unit_Select_Back_New.lua`, which sets it not to loop): the call reached another dialog's object, freed or not, and failed when that script had not run; the code loops the intro movie itself (`CX2StateCreateUnit`), so nothing shown changes. Found when the UI editor opened the creation screen's scene |
| `ui/scripts/DLG_Dungeon_Ending_Event_Speech.lua` | `client/speech-shadow-stretch` (2026-11-16) | The shade picture `Shadow` moved out of `Static_Ending_Speech` into a new static `Static_Ending_Speech_Shadow`, added before it so it still draws behind the balloon; the code only reads `Static_Ending_Speech`'s texts |
| `ui/scripts/UIAnchors.lua` | `client/speech-shadow-stretch` (2026-11-16) | `DLG_Dungeon_Ending_Event_Speech.lua`: `Static_Ending_Speech_Shadow` STRETCH (no zones: a plain fill); the balloon stays centered |
| `data/NewSkillTempletVer2.lua` | `server/add-character` (2026-11-15) | Add's first path: 33 skills (9000–9014, 9200–9207, 9400–9408, 9600) from BR 2014-08, with their Portuguese text from BR 2017, and 72 tree entries of `UC_ADD_NASOD_RULER`/`PSYCHIC_TRACER`/`LUNATIC_PSYKER` from V5 (BR 2014-08 has none), by `tools/assets/merge_skills.py` |
| `ui/textures/DLG_Add_SKILL_ScreenShot_01.{tga,TET}`, `_02` (new) | `server/add-character` (2026-11-15) | The skill tree's Add screenshots: the `.tga` from BR 2017 `data063`, the `.TET` from V5 `data063.kom`. New files: dev builds only until `pack` writes them ([Packing](#packing)) |
| `items/Item.lua`, `items/SetItem.lua` | `server/add-character` (2026-11-15) | 1,706 Add items and 132 sets from V5's `ClientScript` (BR 2014-08 first; it has few), names and descriptions from BR 2017, by `tools/assets/merge_items.py`; 51,543 items, 2,557 sets. Left out: 49 items with values we lack (classes 33–36/123–126, PR 83), 119 only in Korean; 258 items whose set is in no source have `m_SetID = 0` |
| `items/equipment/**`, `items/icons/**`, `_unsorted/data148`, ... (new) | `server/add-character` (2026-11-15) | 3,058 meshes, textures and icons for the items, by `tools/assets/copy_assets.py` over `items/Item.lua`: 2,944 for the added items and what their meshes name (BR 2014-08 and 2017 extracted archives first, then V5's `.kom`), 114 that our own items named but our client lacked. 200 names are in no source (46 Add icons and meshes, e.g. `HQ_Shop_ADD_Cash_*1040`; the rest were already missing in the 2014-03 client). New files: dev builds only until `pack` writes them |
| `data/FieldQuest.lua`, `data/FieldSubQuest.lua`, `data/EventScene.lua` | `server/add-character` (2026-11-15) | 60 quests, 11 sub-quests and 25 scenes by `tools/assets/merge_quests.py`: Elesis's job changes (Free Knight 35310–35313, Saber/Pyro Knight 35320–35333 from BR 2014-08; Grand Master 35400–35404, Blazing Heart 35410–35414 from V5), her skill training (35340–35359, 35450–35454, Asura dropped from the class list) and Add's (Tracer 35460–35463, Psychic Tracer 35470–35473, Lunatic Psyker 35480–35484, from V5), V5's text replaced by BR 2017's (Tracer has none: English). Scene `35470_02` is in no source |
| `general.ess` (client folder) | `server/add-character` (2026-11-15) | 91 strings of those scenes from BR 2017's `General.ess`, by the same script; the file keeps its format (`ff fe` BOM, XORed UTF-16LE lines) |
| `items/Item.lua` | `server/add-character` (2026-11-15) | The quests' items 92392, 92393 (BR 2014-08) and 135469 (V5), by `merge_items.py --ids`, and their 2 icons |
| `items/Item.lua` | `server/add-character` (2026-11-15) | 44 skill memos of Ara's 2nd path and Elesis (607018–607035, 608001–608030; 608002–608005 are in no source) from BR 2014-08, text from BR 2017, by `merge_items.py --ids`, for the server's `ClassChangeItem/Memo.lua`; their icons were already in our client |
| `items/Item.lua`, `items/ManufactureItem.lua` | `server/add-character` (2026-11-15) | 47 recipes of Add's items (materials and cost; 55 result rows, 52 village rows: BR 2014-08 first, V5's for the villages, which BR 2014-08 lacks) by `tools/runtime/merge_server_tables.py --client-manufacture`, and 3 of their materials (61538, 133536, 133537; BR 2014-08) by `merge_items.py --ids` |
| `DungeonData.lua` | `server/add-character` (2026-11-15) | Add's tutorial (39608, `Dungeon_Battle_Field_Tutorial_Add.lua`, already in the client_data) after Elesis's, from V5: without it the GameServer had no such dungeon and Add's tutorial stayed black |
| `general.ess` (client folder) | `server/add-character` (2026-11-16) | 2,510 placeholders (`STR_ID_<n>` shown as text: Add's dungeon lines 29599–29602 and DP/Dynamo messages 29419–29421, 29939, among others) and 299 Korean strings replaced by BR 2017's `General.ess`, by `tools/assets/fill_strings.py`; 3,235 placeholders are in no source (2026-11-16) |
| `data/EventScene.lua` | `server/add-character` (2026-11-16) | Add's lines (`PLAYER_TEXT8`) in 104 text plays (Lowe's after the tutorial, among others) from BR 2014-08 (BR 2017 as fallback), by `tools/assets/merge_player_text.py`; their strings were already in `general.ess`. 22 text plays stop at `PLAYER_TEXT5` in every source (Ara, Elesis and Add see `TEXT`) |
