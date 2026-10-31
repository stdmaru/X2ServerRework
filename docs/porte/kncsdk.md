[Início](../README.md) › [Porte](README.md) › KNCSDK

# KNCSDK (`src/kncsdk`, alvos `x2::kncsdk` e `x2::kncsdk_lua`)

SDK interno, do qual todos os servidores e o cliente dependem. Partes:

| Pasta | Conteúdo |
|---|---|
| `dbg/` | log |
| `Serializer/` | serialização (formato de rede) |
| `KncSecurity/` | cripto de pacotes |
| `Crypto/` | **novo** (T27): DES-CBC, HMAC-MD5, MD5, SHA-256 e hex (`x2::crypto`), no lugar do Crypto++ |
| `Thread/` | threads e locks |
| `ObjectPool/` | pools de objetos |
| raiz | `KncUtil`, macros, `lua_tinker` |

- Origem: `KNCSDK_Server`, unido ao que só existe em `KNCSDK_Client`.
  Importado em 2026-10-29 (62 de 742 arquivos, [relatório](../../tools/import/reports/kncsdk/REPORT.md)) e portado à mão no mesmo dia.
- O módulo está **congelado** no importador (`frozen = true`): toda edição agora é feita direto em `src/kncsdk`.
- Análise do original: [Bibliotecas › KNCSDK](../original/bibliotecas.md).
- Depende de: [Plataforma](platform.md), zlib, LuaJIT 2.1 (só `kncsdk_lua`).

## Estado (2026-10-29)

| Parte | Estado |
|---|---|
| Compilação Linux (GCC 16 Debug, Clang 23 Release) | ok, sem warnings |
| Compilação Windows MinGW-w64 (GCC 16, UCRT), testes no Wine 11 | ok, sem warnings; `kncsdk_tests` passa (2026-10-29) e `kncsdk_lua_tests` também, com o LuaJIT 2.1 compilado pelo CMake (T21, 2026-10-30) |
| Compilação Windows (clang-cl, VS 2026) | ok, sem warnings; `kncsdk_tests` e `kncsdk_lua_tests` passam num Windows real (T26, 2026-10-30) |
| União com `KNCSDK_Client` | feita (`SecureBuffer`, `KCSLockType`), ver abaixo |
| Testes | 39 casos / 1.029 asserções (`kncsdk_tests`, contagem de 2026-10-31) + 3 (`kncsdk_lua_tests`), todos passando |
| Validação contra bytes reais do cliente original | **pendente** (ver Pendências) |

## Mudanças em relação ao original

Interface pública (nomes, macros, assinaturas) mantida; mudou o que está listado aqui.

### Intencionais

| Onde | Mudança | Motivo |
|---|---|---|
| `dbg/logfile` | Cada instrução de log monta um **registro** (`dbg::entry`) com os metadados (nível, função, hora, arquivo/linha, campos do `BUILD_LOG`) ao lado do texto, gravado sob lock no fim da instrução. O arquivo, **texto UTF-8** (`log.log`, ou `log/log_<data>.log`), recebe o texto do original sem mudança; o console tem renderização própria (ver [Log](#log-t16)) e nível próprio | O original gravava **HTML** de vários threads sem lock (condição de corrida); o console repetia o texto do arquivo (decisão T16) |
| `dbg/logfile` | Uma instrução sem `dbg::endl` (`dbg::cerr << L"..." << id;`) termina a linha no arquivo | O original deixava a linha aberta, e o log seguinte (de qualquer thread) colava nela |
| `dbg/profiler` | Removido | Desligado no original (`TB_USE_PROFILER`), sem uso |
| `KncUtil` | Strings estreitas são **UTF-8** em todas as plataformas | O original usava a code page ANSI do Windows (`CP_ACP`) |
| `KncUtil::GetTimeStampByName*` | Data de modificação do executável | O original lia o *link time* do cabeçalho PE |
| `KncSecurity`, `KncUtil::GetMD5` | Cripto pelo `x2::crypto` (`Crypto/`, T27) no lugar do Crypto++; mesmos bytes (vetores oficiais + comparação com o Crypto++). `GetRandomGenerator()` (um `AutoSeededRandomPool` sem uso fora de código comentado) removido | O Crypto++ não compila com clang-cl nem com a STL do VS 2026 |
| `KncSecurity` | Gerador de chaves/SPI semeado pelo SO, um por thread | O original usava `boost::mt19937` **sem semente** (mesmas chaves em toda execução) e compartilhado sem lock |
| `ObjectPool` | `BlockAllocator`/`BoostAllocator` usam um pool próprio (`FixedBlockPool.h`) | O original guardava ponteiros de **32 bits** nos blocos livres; sem Boost |
| `Import*.h` | Esvaziados | Só faziam `#pragma comment(lib)`; o link agora é do CMake |
| `lua_interpreter` | Novo (`lua_interpreter.cpp`): o terminal dos servidores (ver [Terminal](#terminal-t15)), usado por `KBaseServer::Run` | O original vinha de um `lua.c` modificado do Lua 5.1.4, que o LuaJIT não traz, e a janela MFC (`CServerUIDlg`) tinha o próprio campo de comando (T15) |
| `LUA_DOFILE` | Era `luaL_dofile`; agora `lua_dofile` (`lua_dofile.cpp`, 2026-10-31). Igual ao `luaL_dofile`, menos para arquivo que passa do limite do LuaJIT de 65.536 constantes numa função (`Item.lua` gerado do cliente, `ArcadeResultTable.lua`): ele é dividido nos comandos do topo (identificador na coluna 0) e as partes, compiladas antes, rodam em ordem, com os números de linha do arquivo nas mensagens. Só divide sem `local` no topo e sem string longa (`[[`); se uma parte não compila, devolve o erro original | O original rodava num Lua 5.1, que aceita 2^18 constantes; o LuaJIT recusava os dois arquivos (T21) |
| Testes originais (`UnitTest/`) | Removidos, substituídos por doctest | Eram demos (um era cópia dos exemplos do Crypto++) |

### Log (T16)

Feito em 2026-10-31 (`kncsdk/log-structured`). As macros (`dbg.hpp`) mantêm nomes e uso, só mudaram por dentro:

| Macro | Entrega ao logger | Texto no arquivo (igual ao original) |
|---|---|---|
| `START_LOG( type, data )` | `logfile::Start`: nível, nome do tipo, função, hora, arquivo e linha; conta o ponto de chamada (`LogStat`) | `type : Função(), AAAA-MM-DD hh:mm:ss, data` |
| `BUILD_LOG*( x )` | `dbg::field`: um campo (nome = expressão, valor) | `    x : valor` |
| `END_LOG` | `dbg::location`: arquivo e linha | `    (arquivo, linha)` |
| `LOG_HEADER_STR`, `_JIF`/`LIF`, `LOG_CONDITION` | `dbg::header`: função (e hora) | `Função(), data, ` / `Função(), in x` |

O primeiro `<<` num `dbg::cout/cerr/cwarn/clog` (ou o `Start`) cria um `dbg::record` temporário, que grava o registro quando a
instrução termina; se o nível não vai para nenhum destino, nada é formatado. Console, uma linha por registro e os campos alinhados embaixo:

```
18:44:03 NOTE  SetLanBugOutCheckOption_Lua: 랜선렉 방지 코드: 인자 정보 수정
               m_bLanBugOutCheck            = false
               m_byteLanBugOutCheckMaxCount = 0
```

- Níveis: `ERROR` (`cerr`, vermelho), `WARN` (`cwarn`, amarelo), `INFO` (`clog`, ciano), `NOTE` (`cout`, verde); os `*2` iguais aos de base.
  Hora e nomes dos campos esmaecidos, função em negrito; sem cor quando a saída não é terminal (`IsColorTerminal`).
- O console não mostra arquivo/linha, separadores (`lbreak`) nem linhas vazias; texto solto (dumps, tabelas) sai como está.
- Nível do console: `logfile::SetConsoleLogLevel` (Lua: `consolelog( n )`), independente do `log( n )` do arquivo; `-1` volta a seguir o arquivo, que é o padrão.
- `logfile::SetConsoleWriter` troca o destino do console (o terminal da T15 usa).

### Terminal (T15)

Feito em 2026-10-31 (`server/terminal`), no lugar da janela MFC (`Common/ui/ServerUIDlg`, ver [Servidor](../original/servidor.md)).
O `lua_interpreter( L, LuaPromptOptions )` usa o `x2::platform::Terminal` ([Plataforma](platform.md)) e tem três modos:

| Modo | Quando | Comportamento |
|---|---|---|
| Interativo | stdin e stdout são um terminal | Entrada crua e editor de linha (←/→, Home/End, Ctrl+A/E/U/K/W, Ctrl+←/→, ↑/↓ no histórico, Ctrl+L limpa a tela). O log (`SetConsoleWriter`) e tudo o que vai para stdout/stderr (um pipe, como o `CHookStdio` do original) saem acima do prompt, que é redesenhado. Título `<título>_Loading`, depois `_Ready` (o `ready` é consultado a cada ~100 ms, como o laço de 5 ms do original). Ctrl+C limpa a linha; com a linha vazia, Ctrl+C ou Ctrl+D perguntam `Are you sure you want to exit? [y/N]` (texto do original) |
| Linhas | stdin é pipe ou arquivo | Cada linha é um comando, sem prompt nem eco; o fim do stdin encerra. É o que o ServerManager (T30) vai usar |
| Serviço | `--no-stdin` | Não lê nada; espera SIGTERM/SIGINT/SIGHUP (Windows: Ctrl+C, fechamento do console, logoff, desligamento) |

- Linha de comando como no `lua.c`: uma expressão imprime o valor (`1+1` → `2`), o resto roda como instrução; o erro sai em vermelho.
  O original rodava `luaL_dostring` e descartava o erro.
- O `KServerMain` abre o terminal antes do `Init` (a janela original existia durante a carga), com o nome do executável como título
  (a janela se chamava `GameServer`...), e o fecha depois do `ShutDown`. Teclas digitadas durante a carga esperam o prompt; um Ctrl+C
  nesse intervalo abre a pergunta assim que o prompt aparece.
- SIGTERM e o fechamento do console encerram sem perguntar. No Windows, o fechamento dá ~5 s para o `ShutDown` (limite do sistema).
- Num terminal, uma falha fatal (SIGSEGV, SIGABRT...) devolve o modo do terminal antes de terminar, para não deixar o shell em modo cru.
- Linux: `termios` sem `ECHO`/`ICANON`/`ISIG` (o processamento da saída fica). Windows: `ReadConsoleW` com `ENABLE_VIRTUAL_TERMINAL_INPUT`
  (as teclas chegam como texto e sequências VT, o mesmo decodificador do Linux), porque o UTF-8 pelo stdin do conhost não é confiável.
- Sem biblioteca de edição de linha (T13): as opções (readline, linenoise, replxx) não cobrem o console do Windows ou trazem bem mais
  do que o necessário; o editor é pequeno e testado (`test_terminal.cpp`).

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `KSecureBuffer::GetPayload*` e similares | `_JIF( cond, false )`: a "ação" `false;` não faz nada, e o código **continuava** após falhar, por exemplo decifrando um pacote com HMAC inválido | `_JIF( cond, return false )`, padrão já usado 1.404× no servidor. Só rejeita pacotes que já eram inválidos |
| `lua_tinker::dobuffer/dostring` | Condição invertida: o script nunca executava | Corrigida (o servidor não usava essas funções) |
| `lua_tinker` `tostring_s64/u64` | `%I64d` só existe no MSVC | `%lld`/`%llu` |
| `KncUtil::EraseFrontWord` | Dependia de *overflow* de `npos` em 32 bits | Mesmo resultado, explícito |
| `SerBuffer::Compress/UnCompress` | Passava `DWORD*` onde zlib espera `uLongf*` (8 bytes no Linux) | Variáveis do tipo certo; o tamanho na rede continua `DWORD` |
| `SerBuffer::UnCompress` | Ignorava a falha ao ler o tamanho original e descomprimia com um valor não inicializado | Retorna `false` (warning do GCC em Release) |
| `SerBuffer::UnCompress` | Um evento sem dados também é comprimido (tamanho 0 + fluxo zlib vazio), e a volta pegava `&vec[0]` do vetor vazio e chamava `Write` com 0 bytes (`assert( len > 0 )`): comportamento indefinido, que no Release do MSVC passava | `data()` e o `Write` pulado: o mesmo buffer vazio. Derrubava o GameServer (Debug) no relay TCP (`KTRUser`) ao trocar de personagem (2026-10-31, `server/center-run`); teste em `test_serializer.cpp` |

### Portabilidade (sem mudança de comportamento)

- **Chaves padrão do SA (SPI 0)** (achado com o cliente original em 2026-10-30): o original escrevia `"임시로만드는키"` e
  `"해커들절대모를키"` num fonte CP949 e usava os 8 primeiros bytes. Com o fonte convertido para UTF-8 pelo importador, os bytes
  mudaram e o cliente recusava o primeiro pacote do servidor (`IsValidICV_Local` com SPI 0, "falha na conexão com o servidor de canal").
  Agora são os bytes CP949 escritos à mão; `test_security.cpp` confere a chave e um HMAC-MD5 calculado fora. **Cuidado geral:**
  literal `char` não ASCII que vá para a rede, cripto ou hash tem de ser escrito em bytes (varredura em 2026-10-30: só este caso).

- `Serializer` com tamanhos fixos na rede, iguais ao build MSVC x86. Detalhes em [Protocolo](../original/protocolo.md):
  - `int`, `unsigned`, `long` e `unsigned long` viram 4 bytes; um `long` fora da faixa de 32 bits **falha**, em vez de truncar;
  - `wchar_t` vira 2 bytes;
  - `std::wstring` vira UTF-16LE.
- `Serializer`: as funções `SerializeHelper::ComputeTag/PutInto/GetFrom` ganharam um parâmetro-etiqueta `SerializeHelper::Adl`, e o `KSerializer` as chama sem qualificação.
  Sem isso, GCC/Clang não enxergam as sobrecargas declaradas depois do header (os pacotes); o MSVC antigo enxergava.
  As macros `SERIALIZE_*` absorvem a mudança, e o código dos pacotes não muda.
- `KCSLockType`: sem Boost e sem especializações dentro da classe; mesmas macros (`KCSLOCK_BEGIN`...).
- `lua_tinker`: faltavam `typename`/`template`, havia especializações dentro da classe (agora `if constexpr`) e sobrecargas de `push_functor` impossíveis de instanciar (removidas).
  Lua via `<lua.hpp>`, porque o LuaJIT é C; o original compilava a própria cópia do Lua como C++.
- Macros com `L#x`, `classname##::` e `Si##Name##()`, que só funcionavam no MSVC: reescritas (`WIDENE(x)` = `L ## #x`).
- `RTTI.h`: `DynamicCast` devolvia `false` como `void*` (agora `nullptr`) e `L#classname` virou `L ## #classname`.
- `logfile.hpp` inclui `<ostream>` (o `operator<<` genérico precisava dele).
- `KncUtil::NumberShuffle`: as mesmas escolhas do original (o `rand()`-ésimo dos que sobram, na ordem), feitas no próprio vetor com `std::rotate`; a cópia com `erase` disparava um falso positivo do GCC 15 em Release (`-Wfree-nonheap-object`). Teste em `test_util.cpp`.

## Pendências

- **Validar o protocolo com bytes reais**: capturar pacotes do `x2.exe` original e do servidor original e comparar com o `Serializer`/`KSecureBuffer` portados.
  Hoje os bytes esperados dos testes vêm da leitura do código.
- O IV do `KSecureBuffer` é `'A' + rand() % 40` repetido (fraco). Mudar isso mexe no protocolo; decidir depois.
- O `lua_tinker` do cliente (`LuaCommon`) é outra variante; unificar ao portar o cliente.
- No cliente original, o log fica desligado nas builds de produção (`KNCSDK_Client/dbg/logfile.hpp`); reproduzir via nível de log ao portar o cliente.
- `KByteStream` herda de `std::basic_string<unsigned char>`, que a libc++ (Clang no macOS) não suporta mais. Não afeta Linux/Windows.
- LuaJIT no MSVC: a receita do CMake (T21) chama o `msvcbuild.bat` do LuaJIT (T25).
