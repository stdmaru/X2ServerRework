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

## After the port (redesign, Phase 5+)

- Bring scattered cheats into a single client **dev console/panel** (commands + windows: items, monsters, position, buffs).
- Server: admin console (kick, announcements, script reload, status) instead of the MFC UI.
- `EGS_ADMIN_*` packets remain the source of truth on the server, always checking authorization level.
- Evaluate ImGui for client development windows.

## To verify

- Full list of `#ifndef _SERVICE_` blocks in X2Lib (from the importer report).
- Whether the client decides to show cheats by macro or also by the auth level received from the server.
- Mixed encoding confirmed: `KTDX.h` is UTF-8, while `InHouse*.h` are CP949. Detection must be per file.
