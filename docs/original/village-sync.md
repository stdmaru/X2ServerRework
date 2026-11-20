[Home](../README.md) › [Original code](README.md) › Village movement sync

# Village movement sync (2026-11-19)

How the original shows other players moving in a village (and in the dungeon lounge, which uses the same packets).
Analysed on 2026-11-19 after testing with two clients: dungeons, PvP and battle fields stay in sync, while in a village
another player's movement shows up about 3 to 5 seconds late, with jumps, characters vanishing and wrong positions. The
replacement is decision [T53](../project/decisions.md).

## Two models

| | Dungeon, PvP, battle field | Village, dungeon lounge |
|---|---|---|
| Transport | P2P UDP (`CKTDNUDP`, relayed when needed) | TCP: GameServer user → `KField` → TR worker thread → broadcast |
| Batch | `GetFrameBufferNum` steps (7, ~117 ms at T43's 60 steps) | **2.0 s** (`CX2TFieldGame::m_fStateSyncTime`, `X2TFieldGame.cpp`) |
| Content | Frame state | **Input history**: runs of (key mask, frame count) and one position |
| Server | Relays | Drops a packet that comes less than **1.5 s** after the previous one (`GSUserField.cpp`, `TM_FIELD_SYNC_GAP`) |

## The village flow

1. **Sender.** Every simulation step, `CX2SquareUnit::AddHistoryUnitState` records the pressed keys as a `SUS_*` bit
   mask (left, right, dash left/right, up, down, double up/down). Every 2 s, `CX2TFieldGame::SyncUnitFrame` asks
   `GetHistorySummary` for at most 29 runs (`KFieldFrameInfo`: `u_char` state, `u_char` frame count; a 30th `SUS_W` is
   appended and the rest **dropped** when there are more) and sends `EGS_FIELD_UNIT_SYNC_DATA_NOT` with them, the facing,
   the line index, the position on the line as a half float, and `m_LandPosition`, the position of the **last landing**,
   not the current one. When every run is `SUS_W` (standing still) nothing is sent, except once after moving
   (`m_bForceSync`).
2. **GameServer.** `EGS_FIELD_UNIT_SYNC_DATA_NOT` is forwarded to the user's `KField` (`ERM_FIELD_UNIT_SYNC_DATA_NOT`),
   or to the CenterServer room for a dungeon lounge. The field moves the user between **sectors**
   (`KFieldUserManager::MoveUser`, `SectorID.h`), sends the sector changes (`ETR_CHANGE_USER_LIST_IN_SECTOR_NOT` →
   `EGS_CHANGE_USER_LIST_IN_SECTOR_NOT`: users who appear and disappear) and broadcasts the packet unchanged to the
   users of the adjacent sectors, plus party members and friends (`KFieldUser::GetBroadcastDestination`), through
   `ETR_FIELD_UNIT_SYNC_DATA_BROAD_NOT` → `EGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT` (`TRWorkerThread.cpp`). The server checks
   nothing about the position or the inputs.
3. **Receiver.** `CX2SquareUnit::RecvTRPacket` keeps the new runs, and `SetSyncEventProcess` replays them one step at a
   time as if they were the remote player's keys, starting from the position of the **previous** packet
   (`m_SyncOUStartPos_Old`). The local simulation (line map, walls, jumps) moves the unit.

## Squares and wedding halls

Squares and wedding halls are CenterServer rooms (`KSquare`, `KWeddingHall`) with their own, older sync:
`CX2SquareGame` and `CX2WeddingGame` send `EGS_SQUARE_UNIT_SYNC_DATA_REQ` (state, state change count, position, line,
facing) every **5 s** (`m_fP2PSendGap`), and the room broadcasts `EGS_SQUARE_UNIT_SYNC_DATA_NOT` to **every** user in
it, with no sectors and no checks. The receiver (`CX2SquareUnit::RecvPacket`) sets the position and changes state when
the count differs. `XPT_SQUARE_UNIT_SYNC*`, a P2P version, is commented out in the original. Wedding halls also
send their units over P2P (`CX2SquareUnit::SendPacketP2PForWedding`, `KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL`).

## Why it looks broken

- **Delay of 2 to 4 s by design.** An action waits up to 2 s to be sent, then its replay starts from the previous
  packet's position and lasts the 2 s of the window, so a remote player shows an action about 3 s after it happened
  on average, plus network jitter.
- **Jumps.** A packet that arrives before the previous replay ends cuts it, and the unit jumps forward; one that arrives
  late leaves the unit repeating the last run (e.g. running) until the next packet snaps it back. The replay is not
  deterministic (walls, lines, jumps are simulated again on another machine), so the error grows during the window and
  is corrected by a snap at each packet. In the air the start position is the landing position.
- **Lost input.** More than 29 key changes in a window drop the rest.
- **Vanishing.** Sector membership is computed from a position that is up to 2 s old, so a player crossing a sector
  border is added or removed late. The visible user limit (`KFieldOption`: 10, 25, 35 per screen) also hides users.
- **No validation.** Whatever position the client sends is broadcast, so a modified client can fly, stand in the air or
  teleport.
- Minor: the server accepts its TCP connections with Nagle on (`KAccepter::Init`, `bNagleAlgOn = true` from
  `KNetLayer`), tens of milliseconds at most.

## Movement limits (2026-11-19)

Every class uses one script for its village unit, `SQUARE_Unit.lua` (`CX2SquareUnit::GetSquareUnitScriptFileName`).
`INIT_PHYSIC`: `WALK_SPEED` 500, `RUN_SPEED` 700, `JUMP_SPEED` 1500, `DASH_JUMP_SPEED` 1700, `G_ACCEL` 4000,
`MAX_G_SPEED` −2000, `RELOAD_ACCEL` 2000 (units per second). Each state sets its speeds (`SPEED_X`/`SPEED_Y`, or
`PASSIVE_SPEED_X` while a key is held): walking and jumping move at `WALK_SPEED`, dash and dash jump at `RUN_SPEED`,
the dash jump rises at `JUMP_SPEED * 1.1`, riding states at up to 1.3 times those (`RIDING_WALK`, `RIDING_DASH`),
emotions and waits stand still. A line can replace the horizontal speed (`SPEED`, moving walkways) or launch a jump
(`LT_JUMP_UP_REL` with `JUMP_SPEED_X`/`_Y`); nothing else changes a unit's speed (`CX2SquareUnit`). In `X2_DEBUG` an operator holding left Shift moves 6 times faster (`CX2SquareUnit`, the original's
`VILLAGE_FLASH_MAN_DEV_ONLY`). With no line under it a unit falls at `G_ACCEL` up to `MAX_G_SPEED`; the highest jump
is about (1500 × 1.1)² / (2 × 4000) ≈ 340 units above its line.

A state's ID, which the client sends, is its index in `INIT_STATE` (`CX2SquareUnit::InitState`, from 1); a name the
list does not have maps to 0. On 2026-11-19 the list had 21 states: `WAIT` 1, `WALK` 2, `JUMP_UP` 3, `JUMP_DOWN` 4,
`DASH` 5, `DASH_JUMP_UP` 6, `DASH_JUMP_DOWN` 7, `RIDING_ON` 8 to `RIDING_DASH_JUMP_DOWN` 17, `EMOTION_STATE` 18 (every
emotion: `CX2Unit` gives that name), `EMOTION_SITREADY` 19, `EMOTION_SITWAIT` 20, `EMOTION_STANDUP` 21. The jump and
fall states are the only ones with `LAND_CONNECT = FALSE`. The fastest horizontal speed is 910 (`RIDING_DASH`), the
fastest rise 1650 (`DASH_JUMP_UP`). The port reads them with `KSquareUnitPhysics`
([Server: Common](../porting/server-common.md#intentional)).

## Line maps on the servers (2026-11-19)

The client finds a world's line map through four scripts, all in `ClientScript`: `DLG_Map_Village.lua` gives each
`VILLAGE_MAP_ID` a `WORLD_ID`, `WorldData.lua` gives each `WORLD_ID` a world script, and the world script calls
`g_pWorld:SetLineMap("...")` with the line map script, which calls `g_pLineMap:AddLine_LUA{...}`. Squares and
wedding halls are not in the village list; their worlds are `WI_SQUARE`, `WI_SQUARE_MARKET` and `WI_*_WEDDING`.
On 2026-11-19 every village, gate, lounge and rest area resolved to a line map (27 village map IDs, 574 lines in
all) except `VMI_DUNGEON_GATE_RUBEN`, whose world script `Ruben_Dungeon_Gate.lua` is in no archive (see
[Squares](squares.md#types-cxslsquaregamesquare_type)), and `VMI_DUNGEON_LOUNGE_HAMEL`, whose world has no
`WorldData.lua` entry. The line map scripts call only `AddLine_LUA`, `AddStartPos`, `SetTeamStartPos`,
`AddCameraData_LUA` and `SetLandHeight`. The port loads them with `KWorldLineMaps`
([Server: Common](../porting/server-common.md#intentional)).

What they hold (2026-11-19): line types 0 (`LT_NORMAL`), 2 (`LT_POTAL`), 4 (`LT_JUMP_UP_REL`) and 9
(`LT_OTHER_ROAD`); no `LT_WALL` in any village or room. Walls are made another way: `CX2SquareUnit` cancels a
horizontal step that leaves the unit with no line below it within `LINE_RADIUS` (`CKTDGLineMap::CanDown`, down to
y 0), on the ground or in the air, so the end of a line with nothing under it stops the unit (the edges of Elder,
user, 2026-11-19); and a line end marked `BEFORE_LINE`/`NEXT_LINE` −2 keeps a unit that left that line within it
even with a line below (`LINEMAP_Altera_Village.lua`, `Besma_Village_Linemap.lua`), by putting it back over the highest
line below it (`GetLandPosition`): once a line higher than that end is under the unit (a jump up past the end, onto the
ledge beside it), that line is its line and the unit is free (2026-11-20). `LT_WALL` lines, where they
exist, stop a step that crosses them (`CrossedWall`, a 3D segment test with radius 1). World 491 has 4 jump lines (`JUMP_SPEED_X` 5000,
`JUMP_SPEED_Y` 3000) and 5 lines with `SPEED` 5000. A jump line throws the unit without changing its state, so a
unit in a state that keeps it on a line can be in the air. Portal lines do not teleport village units. The port
checks reported movement against these lines with `KVillageMoveValidator`
([Server: Common](../porting/server-common.md#intentional)).

## What the server already had

- `CXSLLineMap` (`src/server/Common/LineMap`) answers `IsOnLine`, `GetNearest`, `GetLineData`, but could not load
  a line map: `LoadLineMap` was a stub, and `AddLine_LUA` read the line's `START_POS`/`END_POS` as globals. The
  GameServer's battle fields (`CXSLBattleField`) read only start positions and leave `AddLine_LUA` empty.
- The servers already read every client `.lua` from `ClientScript` ([Server package](../tools/runtime.md)), so the
  village line map scripts reach them with no new tool.
