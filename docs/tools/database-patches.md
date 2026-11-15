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
| [`20261115_add_character.sql`](../../tools/database/patches/Game01/20261115_add_character.sql) | `gup_create_unit` accepts class 9 (Add); `GSkill_Default` 9000/9200 and Add's `GBase_Item` (112991–112995, 41757–41761) for class 9, whose rows were Elesis's and Ara's items |
| [`20261115_skill_pages.sql`](../../tools/database/patches/Game01/20261115_skill_pages.sql) | Skill pages: `PageNumber` in the keys of `GSkill_New`, `GSkillSlot_New`, `GSkillSlot2_New`; table `GSkill_Expand`; `P_GSkill_Expand_SEL` (was a stub answering one page), `_INS`, `_UPD`, `_UPD_Active`, `P_GSkillSlot2_New_BUY`/`_INS`, `P_GSkill_New_All_DEL_UnitClassChange_20131212` (were missing); `P_GSkill_New_UPD` and `P_GSkill_New_All_DEL_20131212` per page |
| [`20261115_unit_list_without_pvp.sql`](../../tools/database/patches/Game01/20261115_unit_list_without_pvp.sql) | `gup_get_unit_info_by_useruid`: units with no `GUnitPVP` row (10 in the backup) were missing from the character list |

Checked by applying each twice and by running the procedures inside rolled-back transactions (2026-11-15).

### The V5 source's databases as a reference (2026-11-15)

`Elsword-V5.0318.6_3/.../DB_Backup/DB_20260908/*.bak` are SQL Server 2025 backups (version 998; ours is 2022, 958),
so they are read in a temporary `mcr.microsoft.com/mssql/server:2025-latest` container. Our Game01 is close to its base:
437 of our 448 procedures are identical to V5's. **Many of V5's procedures are stubs** (`AS BEGIN RETURN 0 END`,
user, 2026-11-15): of the 40 that our servers call and our databases lack, 33 are stubs in V5. Read a V5 procedure
before using it.

### Still missing (2026-11-15)

Called by active server code, in none of our databases, and only a stub (or nothing) in V5. Each needs a table design
and a procedure written from the server's call and result handling:

- Game01: `P_GSkill_BeforeList_INS`, `P_GUnitLastPosition_SEL`, `P_GUnitPVP_Season2_SEL`, `P_GUser_MER`,
  `P_GPshopInfo_INT`, `P_GEvent_Couple_CHK`/`_DEL`/`_INS`, `P_GEvent_GlobalMission_INS`, `P_ESReward_SEL`,
  `P_ESRewardList_SET`, `P_ESUnitInfo_SET`, `gup_insert_xmas_event`, `gup_insert_Title`, `gup_get_item_list_pet`
  (ours is `gup_get_Item_list_pet`: check the collation).
- Statistics (logs): `lup_insert_inventory_ED_log`, `lup_insert_pirate_EventInfo`, `P_LBossFieldLog_INS`,
  `P_LBossFieldLog_Unit_INS`, `P_LChangePetLog_INS`, `P_LCoupleSystemLog_INT`, `P_LEvent_GlobalMission_SEL`
  (real in V5), `P_LEvent_GlobalMission_UPD`, `P_LHenirHackUserLog_INS`, `P_LItemTradeError_INT`,
  `P_LSkillPageExpandLog_INT`, `P_LUnlimitUnitClassChangeLog_INS`, `P_StatsFieldBossKill_INS`, `P_StatsFieldBossSpawn_INS`.
- Account: `mup_create_user` (and the publishers' `mup_*_gash_user`, `mup_create_nx_user`, `mup_get_add`, likely unused).
- Ours are stubs: `P_GItem_Def_INS`/`_UPD` (Game01).

## Statistics

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
