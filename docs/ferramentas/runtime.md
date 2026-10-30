[Início](../README.md) › [Ferramentas](README.md) › Pastas de execução (`runtime/`)

# Pastas de execução (`runtime/`, `x2_run_*`)

Decisão T22 em [Decisões](../projeto/decisoes.md). Criado em 2026-10-30 no passo `server/channel-run`. Nada aqui depende do
`_reference`, que será apagado: o que os servidores leem está versionado em `runtime/` ou é gerado a partir do cliente (`_client/`).

## Uso

```bash
docker compose -f docker/docker-compose.yml up -d --wait && docker/mssql/restore.sh   # banco (ver Banco de desenvolvimento)
cmake --build --preset linux-debug --target x2_run_channel                           # compila e monta a pasta
cd build/linux-debug/run/ChannelServer && ./ChannelServer DEV 0                       # tag DEV, grupo de servidores 0
```

O servidor precisa do serviço de autenticação DEB para o login ([Servidor: ChannelServer](../porte/server-channel.md)).

## Layout montado

| Pasta | De onde vem |
|---|---|
| `build/<preset>/run/ServerResource/` | `runtime/ServerResource/` (cópia) |
| `build/<preset>/run/ClientScript/` | gerado dos `.kom` de `X2_CLIENT_DATA` por `tools/runtime/gen_clientscript.py` |
| `build/<preset>/run/<Servidor>/` | o executável, os arquivos de `runtime/<Servidor>/` e os templates `*.in` preenchidos; `Statistics/` vazia |

Os configs apontam para `../ServerResource` e `../ClientScript` (`SimLayer:AddPath`), como o original apontava para `C:\Elsword\...`.

## O que está versionado (`runtime/`)

- `ServerResource/`: os 84 scripts do servidor em produção (`_original-server/ServerResource`, 20 MB), com os bytes intactos
  (`-text` no `.gitattributes`: mistura de CRLF/LF e BOM, como estavam). Escolha do usuário em 2026-10-30, no lugar da cópia do
  repositório `X2Server` (85 MB, com pastas por país e os valores da KOG: o servidor em produção tem nível máximo 67, URL de anúncio própria e arquivos a mais).
- `<Servidor>/`: arquivos do original sem segredo (`config_ch.ini`) e templates `*.in`: `config_<x>_DEV.lua.in` (derivado do
  `config_*_US_SERVICE.lua`, comentários traduzidos) e um `.dsn.in` por banco.

### Valores dos templates (cache do CMake)

| Variável | Padrão | Onde entra |
|---|---|---|
| `X2_RUN_DIR` | `build/<preset>/run` | destino |
| `X2_CLIENT_DATA` | `_client/data` | origem do `ClientScript` |
| `X2_RUN_DB_SERVER`, `X2_RUN_DB_USER`, `X2_RUN_DB_PASSWORD` | `127.0.0.1,1433`, `sa`, `$X2_DB_PASSWORD` ou a senha de desenvolvimento do `docker-compose.yml` | `.dsn` |
| `X2_RUN_ODBC_DRIVER`, `X2_RUN_ODBC_OPTIONS` | FreeTDS (`/usr/lib/libtdsodbc.so`, `TDS_Version=7.4;ClientCharset=UTF-8`) se existir, senão ODBC Driver 18 (`Encrypt=yes;TrustServerCertificate=yes`) | `DBLayer:SetODBCDriver/SetODBCOptions` |
| `X2_RUN_SERVER_IP` | `127.0.0.1` | `SetLocalIP` (endereço com que o servidor se identifica no banco) |
| `X2_RUN_AUTH_URL` | `http://127.0.0.1:8000/` | `DEBAuthManager:AddAuthServiceAddress` |

A senha fica só no cache e nos arquivos gerados em `build/`, nunca no git.

## `ClientScript` gerado do cliente

`tools/runtime/gen_clientscript.py <data> <saída>`: decifra cada `.lua` dos `.kom` e decompila o bytecode da KOG (`tools/kom/kom.py`),
e transforma o `Item.kim` em `Item.lua` + `SetItem.lua` (`tools/kom/kim.py`, que usa o `luajit` do sistema para ler os enums).
Saída plana (o `KAutoPath` procura nas subpastas por nome, e nenhum nome se repete entre os `.kom`). Medido em 2026-10-30 com o
cliente em `_client/`: 4.564 scripts de 173 `.kom`, 45 s. Um carimbo na saída evita refazer quando nada mudou.

**Comparação com o `ClientScript` do servidor original** (2026-10-30, por comportamento com `tools/runtime/lua_trace.lua`, que
registra chamadas, argumentos e globais definidas): 2.198 equivalentes (2 iguais byte a byte), 1.800 diferentes, 68 que o cliente não tem.

- **Versão**: o `ClientScript` do servidor original é de um cliente mais novo (o `DungeonEnum.lua` tem masmorras de 2016 a 2019).
  Os IDs de masmorra usados pelo `ServerResource` existem nas duas versões.
- **Formato**: o do servidor está antes do pré-processador da KOG (`X2ScriptPreprocessingTool`): vetores como texto dentro das
  tabelas (`"START_POS = D3DXVECTOR3(0, 19200, 0)"`); o do cliente tem as chamadas (`START_POS = D3DXVECTOR3(0, 19200, 0)`). Aparece nos
  linemaps (`World/`) e diálogos. **A conferir no GameServer**, que é quem lê esses arquivos (`lua_tinker::get<D3DXVECTOR3>`).
- **Ausentes no cliente**: scripts de evento (`*_Festival`, `Halloween_2013`, `Mission_Impossible`...), de tradução (`*Trans.lua`) e o `NewSkillTemplet.lua`.

O ChannelServer só lê `Enum.lua` e `DungeonEnum.lua` do `ClientScript`.

## `tools/kom`

`kom.py` (`list`, `unpack`, `pack`, `decompile`) e `kim.py` (`decompile`, `compile`, `info`), trazidos de `_tools/kom` em 2026-10-30.
Formatos em [Inventário](../original/inventario.md). Para **empacotar** (`pack`/`compile`) eles precisam do LuaJIT 2.0.2 da KOG
(`luajit-kog/luajit` ao lado do script, ou `$KOM_LUAJIT`/`--luajit`), que hoje é compilado a partir das fontes do `_reference`: pendente
trazer essa receita para o repositório antes de o `_reference` sair. Extrair e decompilar não precisam dele.
