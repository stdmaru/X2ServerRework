[Início](../README.md) › [Ferramentas](README.md) › Pastas de execução (`runtime/`)

# Pastas de execução (`runtime/`, `x2_run_*`)

Decisão T22 em [Decisões](../project/decisions.md). Criado em 2026-10-30 no passo `server/channel-run`. Nada aqui depende do
`_reference`, que será apagado: o que os servidores leem está versionado em `runtime/` ou é gerado a partir do cliente (`_client/`).

## Uso

```bash
docker compose -f docker/docker-compose.yml up -d --wait && docker/mssql/restore.sh   # banco (ver Banco de desenvolvimento)
cmake --build --preset linux-debug                                                   # compila e monta todas as pastas
cmake --build --preset linux-debug --target x2_run_channel                           # ou só uma
cd build/linux-debug/run/ChannelServer && ./ChannelServer DEV 0                       # tag DEV, grupo de servidores 0
cmake --build --preset linux-debug --target x2_run_game                              # GameServer (porta 9300)
cd build/linux-debug/run/GameServer && ./GameServer DEV 0
cmake --build --preset linux-debug --target x2_run_login                             # LoginServer (porta 9200)
cd build/linux-debug/run/LoginServer && ./LoginServer DEV 0
cmake --build --preset linux-debug --target x2_run_center                            # CenterServer (porta 9100)
cd build/linux-debug/run/CenterServer && ./CenterServer DEV 0
cmake --build --preset linux-debug --target x2_run_global                            # GlobalServer (porta 9500)
cd build/linux-debug/run/GlobalServer && ./GlobalServer DEV 0
```

O login passa pelo EPublisherServer ([Servidor: EPublisherServer](../porting/server-epublisher.md)), que precisa estar rodando:

```bash
cmake --build --preset linux-debug --target x2_run_epublisher
cd build/linux-debug/run/EPublisherServer && ./EPublisherServer DEV     # porta 8000 (X2_RUN_EPUBLISHER_PORT)
```

Desde 2026-10-31 o build normal monta as pastas de todos os servidores (`X2_RUN_FOLDERS`, ligada por padrão, põe os `x2_run_*`
no `ALL`), em todos os presets. É incremental: o `ClientScript` tem um arquivo de controle e só é regerado quando o cliente muda, e os
arquivos só são copiados quando mudam; um build sem mudanças continua levando menos de um segundo. Sem o cliente (`X2_CLIENT_DATA`), o
`ClientScript` é pulado com um aviso. O CI configura com `-DX2_RUN_FOLDERS=OFF` (não tem cliente nem banco).

Estas pastas são um **ambiente de desenvolvimento** (banco local, driver ODBC e cliente da máquina que compilou), não um pacote de
implantação: o pacote de release, com as configurações como templates para preencher, é um passo à parte (`cmake --install`, ainda não existe).

## Layout montado

| Pasta | De onde vem |
|---|---|
| `build/<preset>/run/ServerResource/` | `runtime/ServerResource/` (cópia) |
| `build/<preset>/run/ClientScript/` | gerado dos `.kom` de `X2_CLIENT_DATA` por `tools/runtime/gen_clientscript.py` |
| `build/<preset>/run/<Servidor>/` | o executável, os arquivos de `runtime/<Servidor>/` e os templates `*.in` preenchidos; `Statistics/` vazia |

Os configs apontam para `../ServerResource` e `../ClientScript` (`SimLayer:AddPath`), como o original apontava para `C:\Elsword\...`.

## O que está versionado (`runtime/`)

- `ServerResource/`: os 84 scripts do servidor em produção (`_original-server/ServerResource`, 20 MB), com os bytes intactos
  (`-text` no `.gitattributes`: mistura de CRLF/LF e BOM, como estavam), **com as versões da pasta de cada servidor no lugar das
  do `ServerResource`** (abaixo, T28). Escolha do usuário em 2026-10-30, no lugar da cópia do
  repositório `X2Server` (85 MB, com pastas por país e os valores da KOG: o servidor em produção tem nível máximo 67, URL de anúncio própria e arquivos a mais).
- `<Servidor>/`: nenhum `.lua` de dados (só os configs); arquivos do original sem segredo (`config_ch.ini`) e templates `*.in`: `config_<x>_DEV.lua.in` (derivado do
  `config_*_US_SERVICE.lua`, comentários traduzidos) e um `.dsn.in` por banco.

### Valores dos templates (cache do CMake)

| Variável | Padrão | Onde entra |
|---|---|---|
| `X2_RUN_DIR` | `build/<preset>/run` | destino |
| `X2_CLIENT_DATA` | `_client/data` | origem do `ClientScript` |
| `X2_RUN_DB_SERVER`, `X2_RUN_DB_USER`, `X2_RUN_DB_PASSWORD` | `127.0.0.1,<X2_DB_PORT ou 1433>`, `sa`, a senha de desenvolvimento do `docker-compose.yml` | `.dsn` |
| `X2_RUN_DB_NAME_PREFIX` | vazio | prefixo dos nomes dos bancos nos `.dsn` (2026-10-31): um servidor com os bancos restaurados como `Elsword-Account`, `Elsword-Statistics`... usa `Elsword-` |
| `X2_RUN_ODBC_DRIVER`, `X2_RUN_ODBC_OPTIONS` | FreeTDS (`libtdsodbc.so`, achado pelo `find_library` em `/usr/lib` no Arch ou `/usr/lib/<triplet>/odbc` no Ubuntu; `TDS_Version=7.4;ClientCharset=UTF-8`) se existir, senão ODBC Driver 18 (`Encrypt=yes;TrustServerCertificate=yes`) | `DBLayer:SetODBCDriver/SetODBCOptions` |
| `X2_RUN_SERVER_IP` | `127.0.0.1` | `SetLocalIP` (endereço com que o servidor se identifica no banco) |
| `X2_RUN_EPUBLISHER_PORT`, `X2_RUN_EPUBLISHER_URL` | `8000`, `http://127.0.0.1:8000/` | porta do EPublisherServer; `DEBAuthManager:AddAuthServiceAddress` dos servidores |

A senha fica só no cache e nos arquivos gerados em `build/`, nunca no git.

**Os arquivos de `run/<Servidor>/` são regenerados a cada build** (`copy_if_different`): uma edição à mão num `.dsn` ou config de lá
volta ao template no build seguinte (aconteceu na VM Windows em 2026-10-31). Para outro banco ou endereço, mude as variáveis acima e
rode o configure de novo. No Windows o `ClientScript` não é gerado (precisa do `luajit` e do cliente): copie o de um build Linux para
`run/ClientScript`.

**Pelo ambiente ou pelo `docker/.env`** (2026-10-31): cada variável da tabela também pode vir de uma variável de ambiente de mesmo nome, que
**vence o cache a cada configure** (`x2_env_cache` em `cmake/X2Runtime.cmake`); sem ela, vale o que está no cache (o `-D` ou o padrão).
Três têm o nome que o `docker compose` e o `restore.sh` também leem: a senha vem de `X2_DB_PASSWORD`, o endereço de `X2_SERVER_IP` e
a porta do `X2_RUN_DB_SERVER` de `X2_DB_PORT`. Valor vazio é ignorado; caminho relativo é tomado a partir da raiz do repositório. O
`docker/build.sh` repassa ao container todas as variáveis `X2_*` do ambiente e do `docker/.env` (não versionado; modelo com todas as
variáveis em `docker/.env.example`). A mudança vale no próximo configure (`cmake --preset <preset>`) e depois fica no cache.

**Rodar no host** (para testes, sem Docker): o binário compilado na imagem roda direto numa distribuição com as mesmas bibliotecas
(unixODBC, FreeTDS); só o caminho do driver muda. Configure com `-DX2_RUN_ODBC_DRIVER=/usr/lib/libtdsodbc.so` (Arch, 2026-10-31) e
remonte as pastas (`x2_run_*`). O `gdb` do host dá o *backtrace* de um crash: `gdb -batch -ex run -ex bt --args ./GameServer DEV 0`.

## `ClientScript` gerado do cliente

`tools/runtime/gen_clientscript.py <data> <saída>`: decifra cada `.lua` dos `.kom` e decompila o bytecode da KOG (`tools/kom/kom.py`),
e transforma o `Item.kim` em `Item.lua` + `SetItem.lua` (`tools/kom/kim.py`, que usa um `luajit` para ler os enums: o do sistema ou, sem ele, como na imagem de build, o interpretador do LuaJIT de commit fixo da T21, compilado pelo CMake).
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

O ChannelServer só lê `Enum.lua` e `DungeonEnum.lua` do `ClientScript`. O GameServer lê quase tudo; os scripts grandes demais para o
LuaJIT passam pelo `lua_dofile` ([KNCSDK](../porting/kncsdk.md)), e os erros dos dados de produção ao carregar estão em
[Bugs do original](../original/bugs.md#dados-serverresource-clientscript).

**GameServer** (`runtime/GameServer`, 2026-10-31): `config_gs_DEV.lua.in` derivado do `config_gs_US_SERVICE.lua` de produção (que já não
tinha `CF_XTRAP` nem HackShield), com a estatística por FTP desligada; `.dsn` do `Account`, `Game01`, `Statistics` e `ES_BILLING`
(o de produção tinha `Game01`/`Game02` por grupo, os dois no mesmo banco); `config_gs.ini` como estava (a versão `V4.0326.5`).
Os `.lua` soltos na pasta de produção do GameServer **eram lidos** e foram para o `ServerResource` (abaixo).

**LoginServer** (`runtime/LoginServer`, 2026-10-31): `config_lg_DEV.lua.in` derivado do `config_lg_US_SERVICE.lua` (já sem a Nexon, que
estava comentada), `.dsn` do `Account`, `Game01`, `Statistics` e `ES_BILLING` e o `config_lg.ini` como estava. O `RankingData.lua` da
pasta de produção do LoginServer (recompensa de Henir até o 6º lugar, contra 3 no `ServerResource`) era o lido e foi para o `ServerResource`. O `RefreshTimeBackup.ini`
(horário do próximo reset de cada ranking de Henir) é criado pelo servidor ao encerrar, na própria pasta, e não é versionado.

**CenterServer** (`runtime/CenterServer`, 2026-10-31): `config_cn_DEV.lua.in` derivado do `config_cn_US_SERVICE.lua` (o SMS já estava
desligado), `.dsn` do `Account`, `Game01` e `Statistics` e o `config_cn.ini` como estava. A pasta `Config/` de produção (configs de
outros países) não vem.

**GlobalServer** (`runtime/GlobalServer`, 2026-10-31): `config_gb_DEV.lua.in` derivado do `config_gb_US_SERVICE.lua` (SMS e NXWeb já
estavam comentados), `.dsn` do `Account` e do `Statistics` (os dois bancos da config US) e o `config_gb.ini` como estava. O
`GlobalMissionConfig.lua` e o `MissEliosConfig.lua` da pasta de produção não são lidos por nada (T28).

## Scripts da pasta de cada servidor (2026-10-31, T28)

O `KAutoPath` (`Common/KAutoPath.cpp`) procura um script **primeiro na pasta atual** e nas subpastas dela, e só depois nos `AddPath`.
Em produção cada serviço rodava com a pasta atual na própria pasta (`nssm set ... AppDirectory C:\Elsword\<Servidor>\`,
`_original-server/Install-Services.ps1`), então os `.lua` soltos ali **valiam no lugar** dos do `ServerResource`. O porte mantém a
ordem de busca; os scripts ficam todos no `ServerResource`, com a versão da pasta do servidor, para o mesmo comportamento da produção.

Comparação de 2026-10-31 (pasta do servidor × `ServerResource` × `KncWX2Server/ServerResource/US` do repositório `X2Server`, a base):

| Situação | Arquivos | Feito |
|---|---|---|
| Diferentes do `ServerResource` | GameServer: `CashItemPrice`, `ClassChangeItem`, `ClassChangeMemo`, `ClassChangeQuest`, `ClassChangeSkill`, `ClassChangeTable`, `FieldData`, `ManufactureResultTable`, `MapData`, `PetData`, `RandomItemData`, `RandomItemMapping`, `RandomItemTable`, `RecommendUserTable`, `StatTable`; CenterServer: `DropTable`, `PaymentItemTable`, `SecretDungeonData`, `StaticDropTable`; LoginServer: `RankingData`; GlobalServer: `WorldMissionConfig` (só o fim de linha) | a versão da pasta do servidor substitui a do `ServerResource` (bytes intactos) |
| Iguais ao `ServerResource` | os outros 11 do GameServer e 13 do CenterServer | nada |
| Só na pasta do servidor, sem uso | `BoardBox`, `BoardBox2`, `CardTable`, `GlobalMissionConfig`, `MissEliosConfig` (o do GameServer e o do GlobalServer diferem) | não vêm: nenhum código, config ou script cita esses nomes. Rever o `MissEliosConfig` do GlobalServer quando ele for portado |

Nem sempre a versão da pasta do servidor é a mais nova: em 9 arquivos o `ServerResource` era a base intacta e só a pasta do servidor tinha
edições (`CashItemPrice` de 2012-02 contra 2011-06), mas no `StatTable` (classes da Ara, 2013-08 contra 2012-05), no `RandomItemData`,
no `RandomItemMapping` (eventos até 2014-08) e no `PaymentItemTable` o `ServerResource` era mais novo e **nunca foi carregado** em
produção. As versões substituídas ficam no histórico do git (o commit inicial do `runtime/ServerResource`) e no repositório `X2Server`.

## `tools/kom`

`kom.py` (`list`, `unpack`, `pack`, `decompile`) e `kim.py` (`decompile`, `compile`, `info`), trazidos de `_tools/kom` em 2026-10-30.
Formatos em [Inventário](../original/inventory.md). Para **empacotar** (`pack`/`compile`) eles precisam do LuaJIT 2.0.2 da KOG
(`luajit-kog/luajit` ao lado do script, ou `$KOM_LUAJIT`/`--luajit`), que hoje é compilado a partir das fontes do `_reference`: pendente
trazer essa receita para o repositório antes de o `_reference` sair. Extrair e decompilar não precisam dele.
