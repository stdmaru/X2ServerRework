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

`EGS_SQUARE_UNIT_SYNC_DATA_REQ`/`_NOT` (state, position, line, facing) is the older square sync of the CenterServer
square room; `XPT_SQUARE_UNIT_SYNC*`, a P2P version, is commented out in the original.

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

## What the server already has

- `CXSLLineMap` (`src/server/Common/LineMap`) loads a line map from its Lua (`AddLine_LUA`...) and answers
  `IsOnLine`, `GetNearest`, `GetLineData`; the GameServer uses it only for battle field start positions
  (`CXSLBattleField`).
- The servers already read every client `.lua` from `ClientScript` ([Server package](../tools/runtime.md)), so the
  village line map scripts reach them with no new tool.
