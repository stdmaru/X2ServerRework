[Home](../README.md) › [Original code](README.md) › Squares

# Squares and personal shops (2026-11-18)

A **square** (`광장`, the "trade plaza") is a room where players meet and open **personal shops** physically, as opposed to
the auction. Found while testing it in game on 2026-11-18; this page is reference for the original's behavior.

## Where it runs

- Squares are `KSquare` rooms (`CXSLRoom::RT_SQUARE`) of the **CenterServer**, in `KCnRoomManager`
  (`src/server/CenterServer/Square.cpp`). The GameServer only forwards (`GSUserPersonalShop.cpp`, `SquareList.cpp`
  lists them for `EGS_SQUARE_LIST_REQ`).
- The CenterServer creates them **when it starts**: `DBE_SERVER_ON_REQ` calls `P_LSquareTable_SEL(@iServerSetID,
  @strServerIP)` on the **Statistics** database, which returns `[Type], [NAME]` from `dbo.LSquareTable WHERE IP = @strServerIP`.
  `@strServerIP` is the CenterServer's own address (`NetCommon::GetLocalIPW()`), the one its `LServerList` row (server
  type 1) has as `PublicIP`. So the `IP` column of `LSquareTable` is **the CenterServer's IP** (in the development setup,
  the same as `LServerList.PublicIP` of "Center"). A row is a square; restart the CenterServer to apply changes.
- `RoomManager:SetNumSquare(n)` in `config_cn.lua` must be at least the number of rows. `OpenSquare(type, name)` in the
  same Lua opens an extra square (the administrator's one).
- The `P_LSquareTable_SEL` of `Account`, `Game01` and `ES_BILLING` are empty stubs; only the Statistics one is used.

## Types (`CXSLSquareGame::SQUARE_TYPE`)

The type does **not** filter the list (every square is listed; `KSquareList::GetSquareInfoPage` ignores it) or change the
rules. It only chooses the map the client loads (`X2StateField.cpp`, `EGS_JOIN_SQUARE_NOT`):

| Type | Value | World the client loads |
|---|---|---|
| `ST_NONE` (and anything else) | 0 | `WI_SQUARE` (`Square.lua`) |
| `ST_AREA` | 1 | `WI_VILLAGE_BESMA` |
| `ST_AGE` | 2 | `WI_DUNGEON_GATE_RUBEN` |
| `ST_FREE` | 3 | `WI_DUNGEON_GATE_ELDER` |
| `ST_TRADE` | 4 | `WI_SQUARE_MARKET` (`Square_Market_*`) |

The client asks for the list with `ST_TRADE`.

Only types 0 and 4 have a map made for a square (checked in game on 2026-11-18). A square spawns its players at
`GetRandomStartPosIndex()` of the world's line map (`X2SquareUnit.cpp`), and the others are not made for it: type 1
(Besma village) has one start position, the village's own, and the player lands in water, unable to move; type 3
(Elder gate) has one, (0, 1241, 0), away from the walkable lines; type 2 (Ruben gate) **crashes the client**: its world
script `Ruben_Dungeon_Gate.lua` is in no archive (only its line map is), so `GetLineMap()` is null at
`CX2SquareGame::CX2SquareGame` (`X2SquareGame.cpp:84`, SIGSEGV). Keep `LSquareTable` rows to types 0 and 4.

## Entering

A village start position with `IsMarket = TRUE` in `DLG_Map_Village.lua` is an entrance: within 150 units the client
shows the `MarketInArea` arrow (`Get_In_Up.dds`), the interact key sends `EGS_SQUARE_LIST_REQ`, the list (refreshed every
10 s) joins with `EGS_JOIN_SQUARE_REQ`. See [client_data](../tools/client-data.md#content-changes-t42).

## Opening a shop

A personal shop can only be created **inside a square** (`EGS_CREATE_PERSONAL_SHOP_REQ` refuses any other room,
`ERR_PERSONAL_SHOP_01`). In game: the shop button of the inventory (`UIM_CREATE_PERSONAL_SHOP`,
`DLG_UI_Inventory*.lua`) opens `CX2UIPersonalShop` (`OPENFORSELL`); the player picks the tradeable items, prices
(`UI_MAX_PSHOP_ED` per unit at most) and a name, and `EGS_REG_PERSONAL_SHOP_ITEM_REQ` registers them
(`REG_CHARGE_ED_PER_ONE_ITEM` = 1000 ED fee per item). Refused for guest accounts, trade-blocked users and units, with
the inventory locked, while trading, or with another shop already open. The shop shows over the character's head to the
square, and others buy by picking the character (`X2Community.cpp`). The shop board (`X2UIPersonalShopBoard`, from the
shop NPCs) searches the open shops.

Opening a shop in the 2014 scripts crashes the client (`CX2UIPersonalShop::SetDialog`, `XPSS_SELL`, found 2026-11-18): it
reads the `Static_Personal_Shop_Name_Up`/`_Down` controls of `DLG_UI_Seller.lua`, which none of our script versions
define, and calls `GetString(0)` on the null result; the original code does the same. The inventory's shop button was
removed in the newest layout too, so the personal shop looks switched off in that build ([client_data](../tools/client-data.md#content-changes-t42)).

The shop's name is typed by the owner: the original read an `IME_Editname` edit box (2 to 20 characters) when the owner
confirmed, but only in the `#ifndef SERV_UPGRADE_TRADE_SYSTEM` branch, which the port had dropped, so the regular shop
sent an empty name (empty balloon over the stall) while only the agency shop built "Loja pessoal de <nick>". The port
restores that branch in `CX2UIPersonalShop` (`UPSCM_SELL_BEGIN_CONFIRM`; behavior change, 2026-11-18): 2 characters
minimum (`STR_ID_863`), 100 at most (the server does not limit regular shop names; the agency's limit is 100 bytes), and
`DLG_UI_Seller.lua` now carries the edit box, prefilled with `STR_ID_864` ("Loja pessoal de <nick>").

## Flows the port had dropped (2026-11-18)

The US build defines `SERV_UPGRADE_TRADE_SYSTEM`, which moved buying to the trade board and left the plaza's own flow
half-removed. The `#ifndef` side was dropped by the port, so the shop in a plaza lacked the following; they are back
(behavior change; `CX2UIPersonalShop`, `CX2UIInventory::UpdateUIButton`):

| Flow | What was wrong | Now |
|---|---|---|
| Owner's shop name | The regular shop sent an empty name (empty balloon) | `IME_Editname` read on confirm, 2 to 100 characters |
| Entering a shop by picking its owner | Answered "product sold" and left, with no item picked | Opens the buyer's window (`JoinShop`); only the board buys straight from a picked item |
| Leaving a shop | State stayed `XPSS_WAIT`, so the next entry said "the shop is already open" | Back to `XPSS_CLOSE` (buyer's window closed) |
| A sale inside the open shop | The refresh branch tested `NULL == g_pData && ...`, never true | Buyer's and seller's slot lists refresh |
| Inventory shop button | Always shown | Shown only in the plaza |
| Buyer's window title and name bar | Showed the script's "enter the shop name" and an empty bar | Keeper and shop name set in `SetDialog` (`XPSS_BUY`) |
| Everything sold (`ERM_BREAK_PERSONAL_SHOP_NOT`) | Windows stayed open, empty; the owner's next close answered NetError 347 (`ERR_PERSONAL_SHOP_12`, the server no longer had the shop) | Whoever has the window open sees it close, with the reason |

Audit (2026-11-18): every `SERV_UPGRADE_TRADE_SYSTEM` side the port dropped, in the client, the GameServer, the
CenterServer and the LoginServer, was read against the port, and the functions of the shop files were compared by name.
What is left out on purpose: the trade board's own pieces, replaced by the upgraded board (see below); the agency's
remaining-time texts for the NPC (`X2TFieldNpc.cpp`); `PRE_EQUIP_PERSONAL_SHOP_BOARD` (try an item on from the board's
search), which only the in-house build (`InHouse2.h`) defines. A mismatch that was the original's and is fixed (`server/shop-item-limit`, 2026-11-18): a regular owner's window offers 2 pages
(6 items) while `KPersonalShop` accepted 3, so registering a 4th item failed; it now accepts 6 (9 with the premium item).

Still dropped, not restored: the trade board's "cheapest first" check box and the board's per-item tooltips
(`X2UIPersonalShopBoard.cpp`), which the upgraded board replaced.

