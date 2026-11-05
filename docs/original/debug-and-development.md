[Home](../README.md) › [Original code](README.md) › Debug, development, and administration tools

# Debug, development, and administration tools

Goal (decision T10): the base build is **`_SERVICE_`**, but **retain** everything useful for administration,
development, or debugging. It may be redesigned; it need not reproduce the original.

## How the original controls this

Environment is selected by a define in the vcxproj, interpreted in `KTDXLIB/KTDX.h`:

| Macro | Meaning (`KTDX.h` comments) |
|---|---|
| `_SERVICE_` | Production patch |
| `_SERVICE_` + `_OPEN_TEST_` | Open test server |
| `_IN_HOUSE_` | Internal build **with** test code |
| `_IN_HOUSE_SERVICE_READY_QA_` | Internal build **without** test code (QA) |

Debug functionality appears in three forms:

1. **Derived macros**: each environment block in `KTDX.h` defines feature macros; `_IN_HOUSE_` includes
   `InHouseEtc.h` and `InHouse1..6.h`, one file per developer.
2. **Direct checks** in code: `#ifndef _SERVICE_` / `#ifdef _IN_HOUSE_` / `_OPEN_TEST_` (~82 / 68 / 40 client files).
3. **Server**: admin commands are **packets** (`EGS_ADMIN_*`, 33 types: kick, change ED/level/class, create NPC,
   complete quest, observer mode, auth level...) checked against authorization level at runtime.
   They barely depend on `#ifdef`, so they remain in the regular port. In the original, the server's MFC window also
   had operational functions; those become console commands.

## Problem: `_IN_HOUSE_` is not just debug

`InHouse*.h` mixes development tools with **unreleased gameplay changes** and experimental fixes.
Examples: `BALANCE_PATCH_20131107`, `UPGRADE_TRADE_SYSTEM`, `OPTION_UI_RENEWAL`, `FIX_MEMORY_LEAK`, `REGULATE_STATE_CHANGE_LOGIC`.
Enabling all of `_IN_HOUSE_` would change the game and break compatibility with the original `x2.exe` (T4).
Therefore, the rule cannot be "enable all in-house features."

## Importer strategy

Our flag, **`X2_DEV_TOOLS`**: enabled in `Development`, disabled in `Release`.

1. Resolve everything else as in the US `_SERVICE_` build.
2. **Whitelist debug macros**: do not resolve them; rewrite their checks to `X2_DEV_TOOLS`.
   Initial candidates (to review):
   - items/inventory: `ITEM_CHEAT_POPUP_TEST` (cheat item-purchase window), `ITEM_VIEW_CHEAT`, `AUTH_AUTO_SELL_ITEM`, `AUTH_DELETE_ITEM_ALL`, `JUNK_AVATAR`
   - character/buffs: `ERASE_BUFF_CHEAT`, `CHEAT_CLASS_CHANGE`, `VILLAGE_FLASH_MAN_DEV_ONLY`, `DAMAGE_ZERO`
   - commands: `CHEAT_COMMAND_SHORT_CUT_KEY_TEST`, `ADD_SHORT_CUT_KEY_IN_HOUSE`
   - monsters/maps: `SUMMON_MONSTER_BY_PRESS_KEY_AT_TRAINING_GAME`, `MONSTER_TEST_DUNGEON`, `CREATE_MONSTER_LIST_TEST`, `MONSTER_STATE_LIST_TEST`, `FREE_SELECT_PVP_MAP`, `ENABLE_SET_UNIT_POSITION_TEST`
   - visualization: `SHOW_ATTACK_BOX_DUMMY`, `IN_GAME_MANUAL_CAMERA_POSITION_TEST`, `SHOW_ONLY_MY_DAMAGE`
   - logs: `ADD_DAMAGE_LOG`, `WRITE_SCRIPT_LOG_AT_CONSOLE_WINDOW`, `LOG_LOAD_DLG_SCRIPT`, `ADD_RESOURCE_ERROR_LOG`, `EFFECT_USE_LOG`
   - scripts: `REALTIME_SCRIPT_PATCH` (hot-reload scripts), `EXPAND_DEVELOPER_SCRIPT`
   - convenience: `AUTO_LOGIN_IN_HOUSE`, `KHELP_SYSTEM_TEST`, `X2OPTIMIZE_INDICATE_LOCAL_MASSFILE`/`MASS_FILE_FIRST` (read loose file before `.kom`)
3. **Direct checks** (`#ifndef _SERVICE_`, `#ifdef _IN_HOUSE_`) become `#if X2_DEV_TOOLS` by default.
   The importer generates a **report** of every converted block for manual triage. Blocks that are not debug
   (gameplay, another country, anti-cheat) resolve as `_SERVICE_`.
4. Everything in `InHouse*.h` that is **not** whitelisted is discarded, leaving the `_SERVICE_` behavior.
5. Requirement: with `X2_DEV_TOOLS` enabled, the client **remains compatible** with the server and protocol. Debug must not change packet format.
   If a feature requires that, keep it out of the whitelist and redesign it later.

## Client dev-only names outside the whitelist (2026-11-05)

The client flag set ([Flags](../tools/flags.md#client-results-2026-11-05)) finds 74 names defined only in the development build;
28 are whitelisted, and the other **46 resolve as in the service build (false)**. Almost all come from the per-developer
`KTDXLIB/InHouse*.h` / `InHouseEtc.h` headers, which `KTDX.h` includes under `_IN_HOUSE_`; four come from `Always.h`,
`Always_Global.h` and `KTDX.h` (under `_IN_HOUSE_`, `_OPEN_TEST_` or `#ifndef _SERVICE_`). Counted by `#if` lines that test them
in `KTDXLIB`, `X2Lib` and `X2`:

| Kind | Names (uses) | Whitelist? |
|---|---|---|
| Not used by any code | `ADD_XET_CRASH_LOG`, `CODE_ANALYSIS_FIX`, `DISCONNECT_DISENABLE_DLGUI`, `FIX_CHANGE_NOT_DOWN_STATE_BY_STUN`, `FIX_FIELDMAPUI_PARTYICON`, `FIX_TOOLTIP_SIZE`, `OPTION_UI_RENEWAL`, `PROCESS_DLL_ERROR_LOG`, `RATE_MODIFIER_INHOUSE_TEST`, `REGULATE_STATE_CHANGE_LOGIC`, `UPGRADE_TRADE_SYSTEM`, `USE_PARTICLE_TRACE` (0 each) | no: nothing to decide |
| Developer/QA tools | `ALWAYS_SCREEN_SHOT_TEST` (64: screenshot test mode in PvP/NPC code), `NEXON_QA_CHEAT_REQ` (27: QA cheat commands requested by the publisher), `FIELD_CHARACTER_SCREEN_SHOT` (10: character/NPC screenshots in fields, PNG/TGA capture), `VILLAGE_MAP_FAST_RELOADING` (3: reload village map UI, gated by `XUAL_DEV`), `NOT_RENDER_NPC_GAME_EDIT` (9: hide NPCs from the game-edit console), `CHECK_PROCESS_MEMORY_USAGE` (2: HOME key shows memory use), `MODIFY_FIND_SHOP_IMAGE_FILE_NAME` (3: writes a shop image list file), `NEW_CHECK_OPENED_DEVICE` (4: dumps opened devices to a file), `FIRST_SCREEN_TEST` (7: an alternative start-up screen render), `DROP_FILE_TEST` (1) | candidates |
| Extra logs and error checks | `LEAVE_LAST_ERROR_LOG_TEST` (22), `NO_MORE_ERROR_TEST` (15: `InHouseLog` on bad line maps, nameless particles), `ADD_RAID_FIELD_LOG` (10: test logs and message boxes), `DELETE_ERROR_LOG` (3), `PREVENT_SUBSTR_ERROR` (1), `CHECK_INVALIDE_CASH_SHOP_ITEM` (1) | candidates (logs only) |
| Unreleased gameplay, UI or fixes | `RESET_INVALID_UNIT_POSITION_TEST` (15), `FIX_ENCHANT_FROZEN_TRACE_BUG_BY_MAGICAL_MAKEUP` (9), `FIX_TEMP_MESH_BILLBOARD` (9: a new mesh token), `CLIFF_CLIMBING_TEST` (8), `MARIO_LIKE_BLOCK_TEST` (6), `ADJUST_RIDINGPET_SKILLSLOT` (5), `PLAY_EMOTION_BY_DAMAGE_EFFECT` (4: a new `LAUGHTER_DAMAGE` script field), `PRE_EQUIP_PERSONAL_SHOP_BOARD` (4: try on items from shop search), `EVE_CLOAKING_ADJUST` (3), `SKILL_FOR_CANCEL` (3), `NEW_VILLAGE_AUTO_SD_UNIT` (3), `CORRECTION_DAMAGE_FREE_CHANNEL` (1), `FIX_MEMORY_LEAK` (1), `FIX_MEMORY_VIOLATE` (2), `HIDE_SYSTEM_MESSAGE` (1), `WINDOW_SYSTEM_BUTTON_FUNC` (1), `NOT_USE_UDP_CHECK_INHOUSE` (1) | no: they change the game (T4/T5); evident bug fixes among them belong in Phase 5 |
| Build experiment | `REDUCED_PRECOMPILED_HEADER_TEST` (11: alternative includes in `X2Main.h` and `X2NPCAI.h`) | no |

Leaving them false keeps the client identical to the shipped `elsword.exe`. Whitelisting a candidate later means rewriting its
`#ifdef` to `#if X2_DEV_TOOLS` by hand in `src/client` (the modules are not reimported once edited).

## After the port (redesign, Phase 5+)

- Bring scattered cheats into a single client **dev console/panel** (commands + windows: items, monsters, position, buffs).
- Server: admin console (kick, announcements, script reload, status) instead of the MFC UI.
- `EGS_ADMIN_*` packets remain the source of truth on the server, always checking authorization level.
- Evaluate ImGui for client development windows.

## To verify

- Full list of `#ifndef _SERVICE_` blocks in X2Lib (from the importer report).
- Whether the client decides to show cheats by macro or also by the auth level received from the server.
- Mixed encoding confirmed: `KTDX.h` is UTF-8, while `InHouse*.h` are CP949. Detection must be per file.
