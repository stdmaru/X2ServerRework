[Home](../README.md) › [Original code](README.md) › PvP and ally NPCs

# PvP NPCs and ally NPCs (2026-11-19)

The original has two ways for an NPC to stand in for a player: **PvP NPCs**, which take a whole team's place in a
matchmaking game, and **ally NPCs**, which fight on the players' side in a dungeon. Both are simulated by the room host's
client, like every NPC: the servers only pick them and track them in the room. They are the starting point for any future
bot system ([D13](../project/decisions.md#pending)).

## PvP NPCs

### When they appear

The GlobalServer's matchmaking first forms a match between real players (`KMatchMakingManager::MatchMakingForWaitUser`), then,
once every member's `KRoomUserInfo` is in, decides per team whether it plays NPCs instead (`KMatch`, on
`EGB_GET_ROOM_USER_INFO_FOR_MATCH_ACK`):

| Mode | Condition | Chance |
|---|---|---|
| `PNT_BEGINNER_NPC` | every player in the match has rating 0 (`KMatchUser::IsBeginner`) | always, both teams |
| `PNT_HERO_NPC` | no player is still in placement (`IsArrangeRank`: fewer than 10 official games) | per team, `rating × 0.001 × 0.01 × factor` percent, halved for 3 players (`KPvpMatchManager::DecisionNpcPvpMode`) |

A team that gets NPCs plays them in its own room (it is always the red team, the NPCs blue); the other team, if it got no NPCs,
is sent back to the queue (`EGB_REMATCH_MAKING_USER_NOT`). NPC games use their own map list (`AddPvpMapForNpcMatch`: Forest of El,
Wally's castle roof, Summer 2008, Peita).

The queue only starts matching with `SetMatchMakingBeginCount` entries waiting (6; the original's in-house test value, commented
out, is 2), and the first entry still needs an opponent, so a single player never gets an NPC game.

### Configuration

`runtime/ServerResource/PvpMatchData.lua` (loaded by the GlobalServer, CenterServer and GameServer): `SetHeroPvpNpcFactor(80)`
(the commented test value is 99999999), the NPC map list, the matching thresholds. `PvpNpcData.lua`: the NPC names drawn at
random (`AddPvpNpcName`) and one `AddPvpNpcInfo` per NPC (NPC ID, type, unit class, level, rating, rank). Counted on 2026-11-19:
25 hero NPCs, with ratings from 1400 (Lowe, Elsword Lord Knight) through Apple 1600, Amelia 1700, Lime 1900, Spika 2000, ...
up to 3300 (Balak, Code Q Proto 00).

### Which NPC comes

`KPvpMatchManager::GetPvpNpcInfo` sorts the NPCs of the mode by the gap between their rating and the team's weighted rating, and
fills a same-probability lottery with 32 copies of the nearest, 16 of the next, then 8, 4, 2 and 1 for each of the rest, without
repeating an NPC within a team. With 25 heroes and a team near rating 0, that is about 39% Lowe, 20% Apple, 10% Amelia and 31% for
the other 22: a low-rated player sees mostly the same two or three heroes.

NPC games count for rating like any other (`ResultProcess.cpp`, `GetRedTeamRating`/`GetBlueTeamRating`): a beginner who beats a
beginner NPC leaves rating 0 and no longer gets beginner NPCs.

The NPCs' strength comes from their own NPC templates and AI scripts, not from the opponent: hero NPCs were meant for players
already rated 1400 or more, so a fresh character loses to them. The only difficulty control is which NPC the rating draws;
there is no per-NPC skill setting (reaction, accuracy, evasion).

### Tested in the port (2026-11-19)

With a temporary patch (a lone team skipped the queue size and the opponent search, `TeamMemberSelection` made it the red team,
and the match forced `PNT_HERO_NPC`; reverted the same day), one player queued alone: beginner NPCs and hero NPCs both load,
attack and follow the player. The user's notes: the beginner NPC is simple but works; hero NPCs beat a new character every
time; the draw gave Amelia once and then Lowe every time, as the weights above predict.

## Ally NPCs in dungeons

The event-monster system (`EventMonsterData.lua`, `GameEvent:AddEventMonsterInfo`; which events are active comes from the
database since 2012) can spawn an NPC on the players' side: with `m_bAllyNpc = True`, `KDungeonRoom::EventDropMonster` adds it
to a random sub-stage of the loaded stage as team red with AI type `NAT_ALLY`, up to `m_iMonsterDropCount` times per dungeon,
filtered by dungeon and stage tables. All shipped entries have `m_bAllyNpc` commented out. Summoned allies (a skill or item with
an owner) go through `KRoomUserManager::AddAllyNPC` instead.

On the client, `CX2GUNPC` gives a `NAT_ALLY` NPC a `CX2AllyNPCAI`, which follows its owner unit and picks targets by a
`TargetData` table: priority, interval, ranges (`targetRange`, `targetNearRange`, `targetLostRange`) and rates
(`targetSuccessRate`, `targetAttakerRate`, `targetPreserveRate`). These are the closest thing the original has to a skill setting.

Not yet tested: spawning a hero PvP NPC as a dungeon ally (an `EventMonsterData.lua` entry with its NPC ID and `m_bAllyNpc`, plus
the event turned on in the database). Open questions for that test: whether the hero's AI script has what `CX2AllyNPCAI` reads,
whom it follows with no owner, and how it behaves against monsters rather than players. An ally NPC joins inside the dungeon; it
does not fill the auto-party queue (`AutoParty*` in the GlobalServer), which would need code like the PvP test patch.
