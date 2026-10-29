[Início](../README.md) › [Código original](README.md) › Fora de escopo (ruído)

# Fora de escopo (ruído)

Projetos e pastas do `_reference/X2Server` que **não** vamos portar agora. Mantido só para saber o que existe.

## Ferramentas (editores, conversores, utilitários)

`X2Tool`, `X2DungeonTool`, `X2EffectTool`, `X2EventSceneTool`, `X2ItemTool`, `X2LineMapTool`, `X2WorldTool`, `X2ScenarioTool`,
`X2Viewer`, `X2KomFileViewer`, `X2MassFileTool`, `X2ScriptPreprocessingTool`, `X2TETXETPreprocessingTool` (está na `.sln`),
`XFileCompare`, `XFileExp`, `KOGExport`, `ElswordExport`, `KOGSkinMesh`, `KOGGamePerformanceCheck`,
`LuaCompileTool`, `AnsiToUTF8`, `ContentSelector`, `CreateCheckInfo`, `DungeonStatistics`, `KncTools`, `ReleaseTool`, `TestEmpty`.

Possivelmente úteis como **referência** mais tarde:
- `X2KomFileViewer` / `X2MassFileTool` — formato `.kom`.
- `LuaCompileTool` — indica se scripts nos `.kom` são bytecode.
- `X2TETXETPreprocessingTool` / `KTDXDeviceTET_Preprocessing` — formatos de textura/mesh pré-processados.

## Gerenciadores de DB / web / billing

`BillingTool`, `NetErrorDBManager`, `ScriptDBManager`, `StringDBManager`, `TextureStringManager`, `TransDBManagerForWeb`.

## Launchers / patcher

`X2Patcher` (está na `.sln`), `X2ChinaLauncher` (303 MB).

## Integrações de terceiros

`HShield`, `XTRAP`, `NexonModule`, `Steamworks`, `IGA` — ver [Binários sem fonte (`.lib` / `.dll`)](binarios-proprietarios.md).

## Dados/planilhas

`ScriptData/` (`.xls/.xlsm/.mdb` de design), `Localization/`, `Backup/`.

## Duplicatas / legado

- `KncWX2Server/KncWX2Server_2003.sln`, `*_2003.vcxproj`, `.vcproj`, `.dsp/.dsw`, `.vspscc/.vssscc`.
- `KncWX2Server/GameClient/` (38 arquivos) — provável cliente de teste/bot; verificar antes de descartar.
- `luaLib/HK_SERVICE/`, `luaLib/Release_IN/` — artefatos de build versionados.
