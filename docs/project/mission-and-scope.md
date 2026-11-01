[Home](../README.md) › [Project](README.md) › Mission and scope

# Mission and scope

## Mission

Port the **server** and **client** from `_reference/X2Server` to Windows and Linux
(CMake, SDL, Vulkan/modern graphics abstraction), removing dead code, country variants,
anti-cheat systems, and proprietary integrations, while reorganizing the code.

## In scope

**Servers** (`KncWX2Server/`): `LoginServer`, `CenterServer`, `ChannelServer`, `GameServer`, `GlobalServer`,
and the shared `KncWX2Server/Common` code.

**Client**: `X2` (executable), `X2Lib` (game logic), `KTDXLIB` (engine: rendering, audio, input, networking, UI).

**Libraries they use**: `KNCSDK_Server` / `KNCSDK_Client`, `X2ServerProtocol`, `LuaCommon`, `luaLib`,
`luajitLib`, `libxml`, `cximage`, `Profiler`, `3rdparty/{boost,freetype,IntelTBB,DXSDK}`.
See [Libraries: recommended actions](../original/libraries.md) for details.

**Runtime data consumed by the server** (not client assets, and not being rewritten):
`KncWX2Server/ServerResource/*.lua`, `DataBase/` (1,475 MSSQL `.sql` files — schema and stored procedures).

## Out of scope (for now)

- Client assets (`_client/data/*.kom`, `.dds`, `.x`, `.fx`, music, video). New code must read the existing formats.
- All original tools (`X2Tool`, `X2DungeonTool`, `X2Patcher`, `*DBManager`, ...). See [Out-of-scope projects and tools](../original/out-of-scope.md).
- Anti-cheat (HackShield, XTRAP, nProtect), publisher billing/auth, Steam, IGA, and Nexon — these will be removed or stubbed.
  See [Proprietary binaries without source (`.lib` / `.dll`)](../original/proprietary-binaries.md).

## Definition of done (high level)

1. Servers build and run on Linux and Windows from a single `CMakeLists.txt`, without MFC, direct IOCP, or `windows.h`
   outside a platform layer.
2. A client can connect, log in, and enter a town and a dungeon on the ported server.
3. The ported client runs on Linux and Windows with the existing assets, without D3D9/D3DX.
4. One country variant — US (see [decisions.md](decisions.md)); `SERV_*` flags are resolved and removed from the code.
