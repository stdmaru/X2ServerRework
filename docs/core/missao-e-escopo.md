# Missão e escopo

## Missão

Portar **servidor** e **cliente** de `_reference/X2Server` para Windows + Linux
(CMake, SDL, Vulkan/abstração gráfica moderna), removendo código morto, variantes de país,
anti-cheats e integrações proprietárias, e reorganizando o código.

## Dentro do escopo

**Servidores** (`KncWX2Server/`): `LoginServer`, `CenterServer`, `ChannelServer`, `GameServer`, `GlobalServer`
e o código compartilhado `KncWX2Server/Common`.

**Cliente**: `X2` (executável), `X2Lib` (lógica de jogo), `KTDXLIB` (engine: render, som, input, rede, UI).

**Bibliotecas que eles usam**: `KNCSDK_Server` / `KNCSDK_Client`, `X2ServerProtocol`, `LuaCommon`, `luaLib`,
`luajitLib`, `libxml`, `cximage`, `Profiler`, `3rdparty/{boost,freetype,IntelTBB,DXSDK}`.
Detalhes: [../analise/bibliotecas.md](../analise/bibliotecas.md).

**Dados de servidor que o código lê** (não são "assets do cliente", mas também não vamos reescrever):
`KncWX2Server/ServerResource/*.lua`, `DataBase/` (1.475 `.sql` de MSSQL — schema e stored procedures).

## Fora do escopo (por enquanto)

- Assets do cliente (`_client/data/*.kom`, `.dds`, `.x`, `.fx`, música, vídeo). O código novo tem de ler os formatos atuais.
- Todas as ferramentas (`X2Tool`, `X2DungeonTool`, `X2Patcher`, `*DBManager`, ...). Lista: [../ruido/fora-de-escopo.md](../ruido/fora-de-escopo.md).
- Anti-cheat (HackShield, XTRAP, nProtect), billing/auth de publishers, Steam, IGA, Nexon — serão removidos ou viram stub.
  Ver [../analise/binarios-proprietarios.md](../analise/binarios-proprietarios.md).

## Critério de "pronto" (alto nível)

1. Servidores compilam e rodam em Linux e Windows a partir de um único `CMakeLists.txt`, sem MFC, sem IOCP direto, sem `windows.h` fora de uma camada de plataforma.
2. Um cliente conecta, loga, entra em vila e em dungeon contra o servidor portado.
3. Cliente portado roda em Linux e Windows com os assets atuais, sem D3D9/D3DX.
4. Uma única variante de país — US (ver [decisoes.md](decisoes.md)); flags `SERV_*` resolvidas e removidas do código.
