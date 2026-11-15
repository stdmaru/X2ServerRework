[Home](../README.md) › [Project](README.md) › Add character

# Add character: sources and gaps (survey, 2026-11-15)

What making Add playable needs (`UT_ADD` = 9: Nasod Ruler 9, Psychic Tracer 32, Lunatic Psyker 122), where each piece
comes from, and what is out of scope. Progress is on the [Roadmap](roadmap.md); the sources are described on
[Inventory](../original/inventory.md#our-clients-assets-and-the-brazilian-sources-user-2026-11-15).

**Our files are the superset** (user, 2026-11-15): what another source has and ours lacks is added to ours, entry by
entry; nothing of ours is replaced by another client's file. Text in Portuguese wherever a source has it.

**Source order** (user, 2026-11-15): `_reference` first (little modified, and only by the user); then the Brazilian
scripts in `kom_BR`; the V5 source (`Elsword-V5.0318.6_3`, edited by a third party) only where nothing else has it.

## Code: already in `_reference`, already imported

- Every Add path of `_reference` is in `src/`: `X2GUAdd.cpp/.h`, `X2Unit`, `X2GUUser`, `X2StateCreateUnit`,
  `X2UISkillTreeNew`, `X2CashShop`, `X2SquareUnit`, `X2GageUI`, `X2Data`, `X2Main`, and the server's `XSLUnit`. Add's
  conditionals (`SERV_9TH_NEW_CHARACTER`, `SERV_ADD_LUNATIC_PSYKER`, `ADD_MEMO_1ST_CLASS`) were resolved true on both
  sides. The only Add lines left out belong to `SERV_ELISIS_PREVIOUS_SIS_EVENT`, an event (off).
- The skill enums cover the first path: `SI_SA_AN_*`/`SI_SA_APT_*`/`SI_SA_ALP_*` 9000–9014, `SI_A_*` 9200–9207,
  `SI_P_*` 9400–9408 and `SI_HA_ALP_DOOMS_DAY` 9600, in `X2SkillTree.h` and `XSLSkillTree.h`.
- **Nothing from V5 is needed for the code** of the first path.

## Client data

| Data | Ours (2014-03 US) | Source |
|---|---|---|
| Skills (`NewSkillTempletVer2.lua`) | none of Add's | BR 2014-08: the same fields as ours, IDs 9000–9601; its Add text is still Korean (Add had not launched in Brazil). BR 2017 has the same IDs with Portuguese names, but after the skill revamp (new fields) |
| Skill sockets (`SkillLevelUpSocketGroup.lua`) | 100 Add lines | BR 2014-08: 185 |
| Items (`Item.kim`) | 17 Add items | BR 2014-08 has only 53, mostly Korean, and **not** Add's basic (84–89) or promotion (41759–41773) items. BR 2017 has them (3,467 Add items, `kim.py` layout `2017`, Portuguese) |
| `UnitTemplet.lua`, `CharacterStatTable.lua`, `ComboTree.lua`, `Add_NasodRuler.lua`, `EffectSet.lua`, `DamageEffect.lua`, `CorrelateUnitClassTemplet.lua` | partly | BR 2014-08 (a superset; it also has the second path) |
| Job change quests (`FieldQuest.lua`, `m_eChangeUnitClass`) | none | only BR 2017 (Psychic Tracer, Lunatic Psyker) |

**Skills, decided (user, 2026-11-15):** BR 2014-08 first, V5 where it lacks a skill; the text (names, descriptions,
per-level text) from BR 2017 by skill ID where BR 2014-08 has none or has it in Korean.

## Server data and database

- `runtime/ServerResource` and `_reference`'s `ServerResource` have nothing for classes 9/32/122 (`StatTable`,
  `ClassChangeSkill`, `ClassChangeTable`). V5's has them (`StatTable` 9/32/122, `ClassChangeSkill`, `ClassChangeTable`):
  the only source.
- Job change quests reach the servers through `ClientScript`, generated from the client.
- `Game01` (not KOG's, see [Development database](../tools/development-database.md)): `gup_create_unit` refuses
  classes outside 1–8; `GSkill_Default` has no class 9; `GBase_Item`'s class 9 rows are Ara's and Elesis's items. The
  reference's `DataBase/` procedure is older (classes 1–4). Fixes go in `tools/database/patches/Game01/`.

## Also synced on the way (user, 2026-11-15)

What the current game should have but our data lacks or has out of sync is brought in with Add:

- **Elesis's jobs** (Saber Knight 29, Pyro Knight 30, Grand Master 120, Blazing Heart 121) should all be playable:
  our client has **no** Elesis job change quest (`FieldQuest.lua`, `m_eChangeUnitClass`); BR 2014-08 has the first
  jobs', BR 2017 all four. `runtime/ServerResource/StatTable.lua` has no rows for 120/121; `_reference`'s has.
- **Ara's second path** (Little Devil 31, Yama Raja 119): our client has its job change quests, but
  `runtime/ServerResource/StatTable.lua` has no rows for 31/119; `_reference`'s has.

## Toward three paths for every class (survey for PR 83, 2026-11-15)

The goal is **every character with its three paths** (user, 2026-11-15). Elsword to Chung have them; Ara's first two
and Elesis's first two (with the sync above) and Add's first (this PR) come next. What is left, all present in V5's
`X2Unit.h`, `XSLUnit.h` and `ServerResource/StatTable.lua`:

| Path | Classes | V5 code (lines under its flag; skills are mostly outside the flags) | Skill IDs |
|---|---|---|---|
| Add 2nd | Arc Tracer 33, Mastermind 123 | `SERV_ADD_ARC_TRACER` 4,261 (3,827 in `X2GUAdd.cpp`), `SERV_ADD_MASTERMIND` 2,179 | 9015–9022, 9208–9211, 9409–9413, 9601 |
| Add 3rd | Time Tracer 36, Diabolic Esper 126 | `SERV_ADD_TIME_TRACER` 218, `SERV_ADD_DIABOLIC_ESPER` 343 | 9023–9030, 9212–9214, 9414–9418, 9602 |
| Ara 3rd | Little Specter 34, Asura 124 | `SERV_ARA_LITTLE_SPECTER` 116, `SERV_ARA_ASURA` 106 | 7017–7025, 7220–7224, 7407, 7413–7417, 7602 |
| Elesis 3rd | Dark Knight 35, Crimson Avenger 125 | `SERV_ELESIS_DARK_KNIGHT` 122, `SERV_ELESIS_CRIMSON_AVENGER` 145 | 8021–8028, 8220–8224, 8410–8415, 8602 |

- **Code: only V5 has it** (`_reference` has none of these classes), so it is ported from V5, checked against ours,
  behind switches. Against `_reference`, V5's `X2GUAdd.cpp` is 7,017 lines longer (18,279), `X2GUAra.cpp` 2,367
  (19,381) and `X2GUEL.cpp` 2,317 (12,053). Each skill above is handled in its character's `X2GU*.cpp` (e.g.
  `SI_SA_AAT_PANZER_BUSTER` 61 uses). Many other V5-only skill names are renames or third-party `_X2DUP` copies of
  skills we have: compare by ID and behavior, not by name.
- **Client data**: the BR 2014-08 scripts have Add's second path (skills, `UnitTemplet`, `CharacterStatTable`,
  `CorrelateUnitClassTemplet`) but not the third paths; BR 2017 has all of them (and their job change quests in
  `FieldQuest.lua`), after the skill revamp; V5's `ClientScript` has the skills in our format.
- **Server data**: V5's `StatTable` has 33–36 and 123–126; `ClassChangeSkill`/`ClassChangeTable` have Add's paths.
- **Still to check in PR 83**: packets or structs these classes need (T4: the original `x2.exe` must still connect);
  `Game01` (`gup_create_unit` and the job change procedures for the new classes); the character switches of PR 81.

## Out of scope of this PR (2026-11-15)

- **Add's second and third paths, Ara's and Elesis's third paths**: PR 83 (above). Filtered out of what this PR imports.
- **Hyper actives** (`x600`, one per character): the code has them (`SI_HA_*`), our client's data does not.
