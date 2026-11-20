[Home](../README.md) › [Tools](README.md) › Database patches

# Database patches

SQL patches in `tools/database/patches/<database>/<date>_<fix>.sql` repair or extend the development databases: missing
objects, and procedures or rows that do not match what the servers expect. The databases are hand-made, not KOG's, so a
problem found in them is fixed with a patch in the task that found it (user, 2026-11-15; see
[Development database](development-database.md)). Patches can be run again (`CREATE OR ALTER`, guarded inserts) and are
applied by hand; this project does not run them when restoring a backup or starting a server.

## Game01

| Patch | Fixes |
|---|---|
| [`20261115_add_character.sql`](../../tools/database/patches/Game01/20261115_add_character.sql) | `gup_create_unit` accepts class 9 (Add); `GSkill_Default` 9000/9200 and Add's `GBase_Item` (112991–112995, 41757–41761) for class 9, whose rows were Elesis's and Ara's items. Admin accounts (`Account.dbo.MUser.AuthLevel` ≥ 2) skip the limit of 4 units created a day (2026-11-16, `client/unit-wait-delete`) |
| [`20261115_skill_pages.sql`](../../tools/database/patches/Game01/20261115_skill_pages.sql) | Skill pages: `PageNumber` in the keys of `GSkill_New`, `GSkillSlot_New`, `GSkillSlot2_New`; table `GSkill_Expand`; `P_GSkill_Expand_SEL` (was a stub answering one page), `_INS`, `_UPD`, `_UPD_Active`, `P_GSkillSlot2_New_BUY`/`_INS`, `P_GSkill_New_All_DEL_UnitClassChange_20131212` (were missing); `P_GSkill_New_UPD` and `P_GSkill_New_All_DEL_20131212` per page |
| [`20261115_unit_list_without_pvp.sql`](../../tools/database/patches/Game01/20261115_unit_list_without_pvp.sql) | `gup_get_unit_info_by_useruid`: units with no `GUnitPVP` row (10 in the backup) were missing from the character list |
| [`20261115_skill_before_list.sql`](../../tools/database/patches/Game01/20261115_skill_before_list.sql) | `P_GSkill_BeforeList_INS` (was missing): copies a unit's learned skills, all pages, into `GSkill_BeforeList` (gets `PageNumber`) before its cash skill points expire; the GameServer stops the expiry unless it answers 0 |
| [`20261116_unit_wait_delete_dates.sql`](../../tools/database/patches/Game01/20261116_unit_wait_delete_dates.sql) | `P_GUnitWaitDelete_INT`: a deleted unit waited 3 days for permanent deletion and 1 day to be restored, so character select showed both buttons disabled for a day; now 24 hours (as the client's `STR_ID_16103` says) and restore at once. Admin accounts (`Account.dbo.MUser.AuthLevel` ≥ 2, GM and above, read across databases) delete permanently at once, and `P_GUnitRestore_SET` lets them delete a restored unit again at once (others wait a day). Units already waiting get the same dates (2026-11-16, `client/unit-wait-delete`) |
| [`20261116_events_pshop_pet.sql`](../../tools/database/patches/Game01/20261116_events_pshop_pet.sql) | Procedures the GameServer called and the database lacked (V5's are stubs), each with a new table: the global mission's daily participation (`P_GEvent_GlobalMission_INS`, `GEvent_GlobalMission`), the couple event (`P_GEvent_Couple_CHK`/`_INS`/`_DEL`/`_SEL`, `GEvent_Couple`), the cumulative Christmas reward (`gup_insert_xmas_event`, `GEvent_Xmas`) and opening or extending a personal-shop agency (`P_GPshopInfo_INT`). `P_GPet_Info_DEL` wrote released pets to another database (`GameDataMgmt`), so releasing a pet failed; they go to `GPet_Info_Deleted` (2026-11-16, `tools/db-missing-procedures`) |
| [`20261116_item_socket5.sql`](../../tools/database/patches/Game01/20261116_item_socket5.sql) | The 2013 item revamp added a fifth socket (`Socket5`) and `ExpandedSocketNum` to items, and the servers read and write them through the mail, bank and agency procedures, which predate it: `P_GPost_INS` was called with two arguments too many, and `P_Gpost_SEL`, `P_GItem_GET_BankItem_New` and `P_GPShopItem_SEL` returned two columns too few. `GPost_Socket` and `GPShopItem` get both columns, and those procedures and the agency's `P_GPShopItem_INT_DEL_A/B/C_New` carry them (2026-11-16) |
| [`20261120_rank_classes.sql`](../../tools/database/patches/Game01/20261120_rank_classes.sql) | `gup_update_rank`, the daily ranking, gives each unit its character for the per-character ranks (`UClass`) and knew only Ara's first path after Chung: Ara's 2nd and 3rd paths, Elesis and Add got none (NULL). All their classes, the new paths' included, now have their character. It also failed outright, leaving staff out through `Account.dbo.MUserAuthority`, which our `Account` lacks; it reads `MUser.AuthLevel` now. Checked in a rolled-back transaction: units of 6 characters ranked, Elesis's and Add's included (2026-11-20, `server/third-paths`) |
| [`20261118_square_table.sql`](../../tools/database/patches/Statistics/20261118_square_table.sql) | `LSquareTable` (what the CenterServer opens at startup, [Squares](../original/squares.md)): six markets of type 4 (`Mercado 1`-`6`) and the type 0 test square; types 1-3 have no usable map. Run again safely |

Checked by applying each twice and by running the procedures inside rolled-back transactions (2026-11-15; the
2026-11-16 patches the same day). The databases run at compatibility level 100 (SQL Server 2008), so patches avoid
newer functions such as `TRY_CONVERT`.

### The V5 source's databases as a reference (2026-11-15)

`Elsword-V5.0318.6_3/.../DB_Backup/DB_20260908/*.bak` are SQL Server 2025 backups (version 998; ours is 2022, 958),
so they are read in a temporary `mcr.microsoft.com/mssql/server:2025-latest` container. Our Game01 is close to its base:
437 of our 448 procedures are identical to V5's. **Many of V5's procedures are stubs** (`AS BEGIN RETURN 0 END`,
user, 2026-11-15): of the 40 that our servers call and our databases lack, 33 are stubs in V5. Read a V5 procedure
before using it.

### Still missing (2026-11-16)

Called in `src/server` (`exec dbo.<name>`, comments stripped), in none of our databases, and only a stub (or nothing)
in V5. Found by comparing every call with the databases' procedures, parameters and first result sets (2026-11-16):

- Game01: `P_ESReward_SEL`, `P_ESRewardList_SET`, `P_ESUnitInfo_SET` (`GSRelayDBThread`, the Elsword reward relay;
  skipped: it needs the relay's own design).
- Ours are stubs: `P_GItem_Def_INS`/`_UPD` (Game01; the item-name translation dump, returns nothing).
- Unused here: `mup_create_user` (LoginServer's account creation; accounts come from DEB login), and the publishers'
  `mup_auth_gash_user`, `mup_create_gash_user`, `mup_create_nx_user` (other countries).
- Never sent: `lup_insert_pirate_EventInfo` (Statistics). Its only sender, `KGlobalMissionManager::WriteGlobalMissionRewardInfo`,
  is commented out.

Not missing (2026-11-16): `P_GUnitLastPosition_SEL`, `P_GUnitPVP_Season2_SEL` and `P_GUser_MER` appear only in log
messages; `gup_insert_Title` and `gup_get_item_list_pet` exist with other letter case (the collation ignores it).
Done: `P_GSkill_BeforeList_INS`, the logs ([Statistics](#statistics)), the events and agency
([Game01](#game01)), the global mission's Statistics side, and the Account tables ([Account](#account)).
Procedures that return nothing while the server reads a result (`P_LServerList_CCU_UPD`, `P_StatsStoneCnt_SET`,
`gup_update_guild_maxno`) only make the server log that it read no result; they are left as they are.

## Statistics

[`20261115_missing_logs.sql`](../../tools/database/patches/Statistics/20261115_missing_logs.sql) adds the 11 log
procedures the GameServer and CenterServer call that our Statistics lacked (`lup_insert_inventory_ED_log`,
`P_LBossFieldLog_INS`/`_Unit_INS`, `P_LChangePetLog_INS`, `P_LCoupleSystemLog_INT`, `P_LHenirHackUserLog_INS`,
`P_LItemTradeError_INT`, `P_LSkillPageExpandLog_INT`, `P_LUnlimitUnitClassChangeLog_INS`, `P_StatsFieldBoss{Kill,Spawn}_INS`),
each with a table named after it, so their calls stop failing. Production disabled several of its own logs (the
`INSERT` of `P_LDungeonStageLog_INS` is commented out), so these were likely never kept there; here they are, which
helps debugging. Checked like the Game01 patches (2026-11-15).

[`20261116_global_mission_and_server_list.sql`](../../tools/database/patches/Statistics/20261116_global_mission_and_server_list.sql)
adds GlobalServer's global mission (`LEvent_GlobalMission`, `P_LEvent_GlobalMission_SEL`/`_UPD`; V5's are stubs), a
server-wide daily mission the GameServers count. No mission is seeded: the script's header shows the row to insert to run
one. It also fixes procedures that answered the servers wrongly: `P_LServerList_SEL` returns `PublicIP` and `PrivateIP`
(the servers read both since 2013), `P_LAccusationLog_INT` takes the CenterServer's arguments in its order,
`P_StatCCUMinuteGuestUser_INS` gets its table, and `P_LTradeBank_INT` answers 0 (2026-11-16).

[`20261104_globalserver_event_features.sql`](../../tools/database/patches/Statistics/20261104_globalserver_event_features.sql) adds
the tables and procedures used by GlobalServer's timed enchant and limited exchange flows. It implements the server's result-set
contracts, single-use timed-enchant claims, exchange quota tracking, and rollback records. Its syntax targets SQL Server 2008 or
newer (compatibility level 100); run it with a script executor that treats standalone `GO` lines as batch separators.

The code establishes the single-use event claim, shared `LimitUID` quota, and rollback behavior. The SQL source does not provide
the original table definitions, so the new table and column layout is an additive model documented in the script. Configuration
rows are not seeded because the repository contains no authoritative event schedule, enchant level, item targets, or quota values.

The exchange procedure reloads the remaining quota by subtracting outstanding consumption log rows. A rollback deletes its row,
so the quota is restored both in memory and after a server restart. The script does not modify existing tables. Configure the new
tables before enabling either feature.

## Account

[`20261116_block_lists_and_authority.sql`](../../tools/database/patches/Account/20261116_block_lists_and_authority.sql)
(2026-11-16):

- **Block lists.** The LoginServer reloads a block list when its row in `ReleaseTick` grows, and `ReleaseTick` was empty,
  so no list loaded; the tables behind them (`RejectedUser`, `TradeBlock`) did not exist, and neither did the machine-ID
  list's `mup_get_add`. The patch creates them (`MachineIDBlock` for machine IDs) and seeds the ticks of types 0
  (connection), 1 (trade), 4 (GlobalServer's `MPenalty` list) and 5 (machine ID). After editing a list by hand, raise
  its tick (the script's header shows how).
- **Missing tables of existing procedures:** `SecondPWChangeLog` (changing the second password always failed) and
  `MUserComeback.RewardLevel`/`RegDate` (returning-user rewards).
- **Authority.** The user's level is `MUser.AuthLevel`; `mup_get_authority_list` and `mup_set_user_authority` (GameServer
  admin commands), `mup_update_tool_authlevel` and `mup_get_user_info` read the original's `MUserAuthority`, which this
  database does not have, and now use `MUser.AuthLevel`.
