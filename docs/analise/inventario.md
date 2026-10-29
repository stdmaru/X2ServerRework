# Inventário do `_reference/X2Server`

`_reference/X2Server` → symlink para `/media/1tb-hdd/Storage/ElswordServer/source/X2Server`
(repositório git próprio, 14 commits de jul/2026 portando para VS2026 / toolset `v145`).

Solução: `ProjectX2.sln`. Linhas = `.c/.cpp/.h/.hpp/.inl` brutas (inclui comentários e código morto).

## Em escopo

| Módulo | Arquivos | Linhas | Papel |
|---|---:|---:|---|
| `KncWX2Server/` | 1.423 | ~1,0 M | 5 servidores + `Common/` (729 arquivos) |
| `X2Lib/` | 609 | ~934 k | Lógica de jogo do cliente |
| `KTDXLIB/` | 338 | ~216 k | Engine do cliente (D3D9, som, input, UI, rede) |
| `X2/` | 7 | ~3,7 k | `main`/executável do cliente |
| `KNCSDK_Server/` | 742 | ~226 k | SDK interno (log, serializer, crypto, Lua) — **maior parte é Crypto++ e Lua embutidos** |
| `KNCSDK_Client/` | 740 | ~225 k | Quase cópia do de servidor (36 arquivos diferentes) |
| `X2ServerProtocol/` | 38 | ~5,5 k | Socket/IOCP/Actor do lado cliente (duplica `Common/Socket`) |
| `LuaCommon/` | 7 | ~5,6 k | `lua_tinker` + `KLuaManager` |
| `luaLib/` | 63 | ~23 k | Lua 5.1.4 + `lua_tinker` (mais uma cópia) |
| `luajitLib/` | 147 | ~75 k | LuaJIT 2.0.2 |
| `libxml/` | 102 | ~217 k | libxml2 antigo |
| `cximage/` | 433 | ~289 k | CxImage + jpeg + png |
| `Profiler/` | 18 | ~3 k | Profiler próprio |
| `3rdparty/` | — | 2,6 GB | boost 1.91 (+libs pré-compiladas), DXSDK, freetype, IntelTBB 2.2 |

Linhas de código **nosso** relevantes (sem 3rdparty embutido): ~2,2 M antes da limpeza de flags.

## Projetos da solução e grafo de dependência

```
Servidores (Login/Center/Channel/Game/Global)
  └─ KncWX2Server/Common ── KNCSDK_Server (lib + headers) ── boost, TBB(Game), DXSDK headers (!)
                         └─ libs binárias: CrashRpt, MailMIME/SMTP, HackShield, nProtect, XTRAP, AuthCheck(Nexon), billing

X2 (exe)
  └─ X2Lib ── KTDXLIB ── KNCSDK_Client, X2ServerProtocol, libxml, CxImage, freetype, DXSDK(d3d9/d3dx9/dinput8/dsound), FMOD
         │            └─ BandiVideo, KncFirewall, CrashRpt/BugTrap
         ├─ luajitLib / luaLib / LuaCommon
         ├─ KncWX2Server/Common  (pacotes e enums compartilhados com o servidor — fonte única do protocolo)
         └─ NexonModule, Steamworks, IGA, XTRAP, HackShield
```

Observações:
- Servidores incluem headers do **DXSDK** (13 arquivos usam `D3DXVECTOR3`/matrizes) — só matemática, fácil de trocar.
- `GameServer` inclui `../../X2/dat/Script` (dados do cliente).
- `X2Lib` inclui `KncWX2Server/Common` diretamente → `ClientPacket.h`, `CommonPacket.h`, `Enum/`, `EventID_*.h` são compartilhados.
- `X2Lib` tem 605 entradas no diretório raiz — organização praticamente plana.

## Dados

- `KncWX2Server/ServerResource/*.lua` — tabelas lidas pelo servidor.
- `DataBase/` — 1.475 `.sql` (projeto SSMS/DBProj) — schema + stored procedures MSSQL.
- `ScriptData/` — planilhas `.xls/.xlsm/.mdb` fonte de tabelas (insumo de ferramentas; fora de escopo).
- `_client/data/` — cliente pronto: `x2.exe`, `*.kom` (header `KOG GC TEAM MASSFILE V.0.3`, conteúdo zlib), DLLs (d3dx9_43, fmodex, bugtrap, steam...), `x2.fx`.
