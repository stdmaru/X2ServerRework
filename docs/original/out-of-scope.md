[Home](../README.md) › [Original code](README.md) › Out-of-scope projects and tools

# Out-of-scope projects and tools

Projects and folders in `_reference/X2Server` that **are not currently being ported**. This page documents what is present in the
reference code.

## Tools (editors, converters, utilities)

`X2Tool`, `X2DungeonTool`, `X2EffectTool`, `X2EventSceneTool`, `X2ItemTool`, `X2LineMapTool`, `X2WorldTool`, `X2ScenarioTool`,
`X2Viewer`, `X2KomFileViewer`, `X2MassFileTool`, `X2ScriptPreprocessingTool`, `X2TETXETPreprocessingTool` (in the `.sln`),
`XFileCompare`, `XFileExp`, `KOGExport`, `the game exporter`, `KOGSkinMesh`, `KOGGamePerformanceCheck`,
`LuaCompileTool`, `AnsiToUTF8`, `ContentSelector`, `CreateCheckInfo`, `DungeonStatistics`, `KncTools`, `ReleaseTool`, `TestEmpty`.

Potentially useful as **reference** later:
- `X2KomFileViewer` / `X2MassFileTool` — `.kom` format.
- `LuaCompileTool` — indicates whether scripts in `.kom` files are bytecode.
- `X2TETXETPreprocessingTool` / `KTDXDeviceTET_Preprocessing` — preprocessed texture/mesh formats.

## DB / web / billing managers

`BillingTool`, `NetErrorDBManager`, `ScriptDBManager`, `StringDBManager`, `TextureStringManager`, `TransDBManagerForWeb`.

## Launchers / patcher

`X2Patcher` (in the `.sln`), `X2ChinaLauncher` (303 MB).

## Third-party integrations

`HShield`, `XTRAP`, `NexonModule`, `Steamworks`, `IGA` — see [Proprietary binaries without source (`.lib` / `.dll`)](proprietary-binaries.md).

## Data / spreadsheets

`ScriptData/` (`.xls/.xlsm/.mdb` design files), `Localization/`, `Backup/`.

## Duplicates / legacy

- `KncWX2Server/KncWX2Server_2003.sln`, `*_2003.vcxproj`, `.vcproj`, `.dsp/.dsw`, `.vspscc/.vssscc`.
- `KncWX2Server/GameClient/` (38 files) — likely a test client/bot; verify before discarding.
- `luaLib/HK_SERVICE/`, `luaLib/Release_IN/` — versioned build artifacts.
