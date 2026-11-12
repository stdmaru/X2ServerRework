[Home](../README.md) › [Tools](README.md) › Client folder

# Client folder (`tools/client/install.sh`)

The ported client runs from a folder laid out as in decision T35 (3) (the launcher's root is not ported yet):

```
<client>/data/            game root: x2 (x2.exe), data*.kom, general.ess, movie/, music/, and what the client writes
<client>/data/data/       client_data, loose files read only by X2_DEV_TOOLS builds (preferred over the .kom)
```

- **`.kom`**: packed from a client_data with `x2_assets pack <client_data> <client>/data`
  ([client_data](client-data.md)). The client_data can be the one in `<client>/data/data`.
- **`general.ess`**: the string table (UTF-16LE, XORed lines), loose in the original client; without it every
  `GET_STRING` is empty (message boxes without text).
- **Loose folders** the client reads from disk, outside the archives: `movie/` (the `.avi` of the class creation
  screen, [Client: movies](../porting/client-movie.md)) and `music/` (`.ogg`; the sound backend is silent for now).
  Both come unchanged from the original client (`_client/data`).
- **Written by the client** in the game root: `ErrorLog.txt`, `DialogLog.txt`, `OpenScriptFile.log`, `State.log`,
  `MachineId.info` and the user's settings (`GameOptions.lua`, `TutorSystem.lua`, `SelectedServerGroup.lua`). The
  first start logs `GameOptions.lua` and `TutorSystem.lua` as missing.

```bash
tools/client/install.sh <client>/data [preset]      # copies build/<preset>/src/client/x2 (default linux-debug)
```

It copies the executable and, when they are missing, `general.ess`, `movie/` and `music/` from `X2_CLIENT_DATA` (default
`_client/data`). It never touches the `.kom` or the client_data. Run it on the machine that has the folder: the
`x2-dev` container does not mount it.

The user's folder (2026-11-12): `/media/games/X2ReworkClient/data`, with its client_data in `data/data`.

## Running

From the game root (`X2_DEV_TOOLS` builds need no arguments; release builds need the patcher's `PATCHER_RUN_ONLY` last).
The first start asks (Yes/No) whether to pick the graphics options automatically, as the original did when
`GameOptions.lua` is missing. Headless, for start-up checks: `SDL_VIDEO_DRIVER=dummy X2_RENDER_BACKEND=null ./x2`
([Client: DXUT](../porting/client-dxut.md)).
