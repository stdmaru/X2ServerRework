[Início](../README.md) › [Código original](README.md) › Inventário do `_reference/X2Server`

# Inventário do `_reference/X2Server`

`_reference/X2Server` → clone verificado de `github.com/stdmaru/X2Server` (`_reference/X2Server-github`, commit `b8fb1380`;
14 commits de jul/2026 portando para VS2026 / toolset `v145`). Veja abaixo por que não é mais a cópia do HDD.

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

## Integridade da cópia

Em 2026-10-29, a cópia original no HDD (`/media/1tb-hdd/Storage/ElswordServer/source/X2Server`, hoje `_reference/X2Server-hdd`)
tinha **90 arquivos rastreados corrompidos**: mesmo tamanho e conteúdo virado bytes aleatórios. O `git status` não percebia por causa do cache de *stat*.
Os objetos do git estavam íntegros.

Afetados: `KncWX2Server` (35, entre eles `CenterServer.vcxproj`, configs `.lua`/`.ini` e `.dsn`), `3rdparty/boost/spirit` (33), `cximage` (21) e `KncTools` (1).
Nenhum arquivo do KNCSDK foi atingido, então o que já tinha sido importado continuou válido (o relatório saiu idêntico com o clone).

Ações:
- `_reference/X2Server` passou a apontar para um clone do GitHub, no mesmo commit, com `git fsck` limpo e os 90 arquivos conferidos.
- O importador agora confere o hash git de **todo** arquivo que lê ([Importador](../ferramentas/importador.md)).
- **Recomendação**: checar o HDD (SMART). Arquivos que só existem nele, fora do git, podem estar corrompidos também.

## Outras referências (2026-10-29)

Também em `_reference/` (somente leitura, fora do git):

| Pasta | O que é |
|---|---|
| `_original-server/` | Servidor compilado como roda hoje: `.exe`, `config_*.ini` (CP949), `.dsn` e os scripts `.lua` (UTF-8 com BOM) que ele carrega |
| `AuthServer/` | Fonte do servidor SOAP de autenticação/cobrança do DEB (`_DEB_AUTH`), feito pelo dono do projeto. Usado por enquanto; deve sair numa fase futura |

Na raiz, `_database/` (fora do git): backups do SQL Server dos bancos do jogo (`Account`, `Game01`, `ES_BILLING`, `Statistics`),
restaurados pelo [Banco de desenvolvimento](../ferramentas/banco-dev.md).

E em `_tools/kom/`: ferramenta para extrair os `.kom` do cliente (`_client/`), útil para inspecionar scripts e assets.
Formatos que ela lê e grava (2026-10-30):

| Arquivo | Entradas | Bytecode Lua | Estado |
|---|---|---|---|
| `data036.kom` do nosso cliente (2014) | Algorithm 2 (XORCRC) | `\x1bKL\x81` | ok |
| `data036.kom` de agosto de 2014 (repositório `Elsword-V5.0318.6_3`) | Algorithm 3 (XORCRC com outra chave, zlib sem o adler32 final) | `\x1bKL\x82`: LuaJIT 2.0 com os opcodes reordenados | ok |
| `data036_br.kom` (2016) | Algorithm 3 no XML, mas com outra chave ou variação | ? | não abre: as entradas decifram para lixo |

O Algorithm 3 e a tabela de opcodes do `0x82` não estão no `_reference` (só 0, 1 e 2): vêm da engenharia reversa publicada no repositório
`Elsword-V5.0318.6_3` (`KTDXCommonFunc.h`, `tools/kpcrack/`), feita em 2026 sobre um cliente de 2014, não de código da KOG. Em nenhum dos
formatos a versão do LuaJIT muda: todos são LuaJIT 2.0.

`_reference/X2Server` (o link usado pelo importador) não existia em 2026-10-29; o clone está em `_reference/X2Server-github`, e o importador usa o clone direto quando o link falta.

