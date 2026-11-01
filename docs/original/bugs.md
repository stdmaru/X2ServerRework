[Home](../README.md) › [Original code](README.md) › Original bugs

# Original bugs (Phase 5 backlog)

Incorrect behavior that **already existed in the original** and that the port **intentionally preserves**, to avoid changing
the game mid-port. These are fixed in [Phase 5](../project/roadmap.md#phase-5--deep-cleanup), each with a decision about the new behavior.

**Rule (user request, 2026-10-31):** fix an obvious bug as soon as it is found, recording the fix on the module page.
Only bugs that are more convenient to handle later or whose correct behavior is uncertain (game balance, database data) remain here.

Bugs already fixed by the port (out-of-bounds reads, `%s` with `std::wstring`...) are not listed here: see the "Original bugs fixed"
section of the relevant module page in [Porting](../porting/README.md).

## Code

| Location | Bug | In-game effect | Found |
|---|---|---|---|
| `Common/X2Data/XSLSkillTree.cpp`, `LoadSkillTemplet_Stat` | Magic defense per level is read from the `m_DefPhysic` table instead of `m_DefMagic` (the block was copied and only the vector changed) | Skills with stat bonuses grant magic defense equal to physical defense. Guild skills 500 and 501 have their own `m_DefMagic`, which is ignored | 2026-10-31, `server/game-run` |
| `Common/X2Data/XSLSkillTree.cpp`, `LoadSkillTemplet_Stat` | Size check is inverted (`empty() != false && size() != SLV_MAX_LEVEL`): it rejects only an empty table, allowing a table with fewer than 30 levels | The original read out of bounds for missing levels. The port uses 0 for those levels ([GameServer](../porting/server-game.md#original-bugs-fixed)); still need to decide whether a short table is valid (guild skills have 10 levels) | 2026-10-31, `server/game-run` |
| `GameServer/NewPartyListManager.cpp`, `KPartyListManager::IsPossibleLowLevel` | `OUT bool isPartyNULL` is passed by value | Caller never receives the value | 2026-10-30, `server/game-build` |
| `GameServer`, guild grade | Checks `== -1` on a `u_char`, which is always false (the check was removed in the port, with the same effect) | Verify in `server/game-run`: what the procedure returns when the character has no guild | 2026-10-30, `server/game-build` |
| `GameServer`, skill level and active skill page | Check `unsigned < 0`, which is always false (the checks were removed in the port, with the same effect) | Negative-value validation never runs | 2026-10-30, `server/game-build` |
| `CenterServer/DungeonRoom.cpp`, "sub-stage cleared in 1 s" check (`GetSubStageNpcDieCheck`) | The active NPC count for the sub-stage is computed and discarded: `m_mapSubStageActiveNpcNum` receives the number of NPCs that grant EXP, and `m_iSubStageActiveNpc` (which triggers the check from 5 onward) also comes from the EXP map. The active-NPC map is never read | Anti-hack check (`HCT_CHECK_NPC_DIE_TIME` request to GameServer) counts EXP-granting NPCs instead of active NPCs: may trigger or fail to trigger unexpectedly. The dead count was removed in the port; behavior is unchanged | 2026-10-31, `server/center-build` |

## Data (`ServerResource`, `ClientScript`)

Errors loading production scripts with `ClientScript` generated from the client (GameServer counts from 2026-10-31, after
[T28](../project/decisions.md): same counts as the 2026-07 production logs in `_original-server/GameServer/log`). The item set (49,837)
and dungeon scripts were checked: they match those in the original server's `ClientScript`, so item and dungeon errors already occurred
in production. The server starts while ignoring invalid entries:

- Items referenced by `ServerResource` but absent from `Item.lua`: 437 attraction items (`AddAttractionItemInfo`),
  167 random item groups (`AddRandomItemGroup`, with total probability exceeding 100%), 172 cash packages with duplicate items,
  27 sealed items, and 13 result-notification items.
- 1,081 crafting results (`AddManufactureResultTemplet`, "잘못된 제조ID") whose crafting ID is absent from `ClientScript`
  (production, with its own `ClientScript` from a newer client, had 1,223).
- 12 pets and 1 mount (ID 11) referenced by the creation script but missing from the template.
- 24 dungeon scripts referenced by `DungeonData.lua` that are in neither the client nor the original `ClientScript`
  (`ElTutorial_*` tutorials, `Training_*` training, `Besma_MINE2_*`, `Elder_NewYear`); `Dungeon_Event_KidDay.lua` exists only in the original `ClientScript`.
- 2 field line maps (`Field_Velder_Stage2/3_LineMap.lua`) that do not exist.
- Procedure `dbo.P_GItem_Def_GET` (GM tool) is absent from the development database.
- GlobalServer calls three groups of `Statistics` procedures absent from all backups (2026-10-31, `server/global-run`):
  `P_LEvent_GlobalMission_SEL/UPD` (global missions), `P_LEvent_RandomEnchant_SEL/UPD` (timed enchantment event), and
  `P_LExchangeLimitInfo_GET`/`P_LExchangeLimitItem_GET`/`P_LExchangeLimitLog_*` (limited exchange). These features start without data.
  Crystal-defense time is also missing ("디펜스 던전 시간 셋팅 값이 존재 하지 않습니다", `SetTodayMissionTime`).
- `MapData.lua` does not register local maps Ruben (10000) and Elder (10001) through `AddLocalMapInfo` (entries start at Altera):
  when the client requests the automatic-party bonus in these towns (`EGS_REQUEST_GET_AUTO_PARTY_BONUS_INFO_NOT`), GameServer reports
  "등록되지 않은 맵ID입니다!" and sends an empty list (2026-10-31, `server/global-run`).
