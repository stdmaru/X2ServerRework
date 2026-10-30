[Início](../README.md) › [Porte](README.md) › KNCSDK

# KNCSDK (`src/kncsdk`, alvos `x2::kncsdk` e `x2::kncsdk_lua`)

SDK interno, do qual todos os servidores e o cliente dependem. Partes:

| Pasta | Conteúdo |
|---|---|
| `dbg/` | log |
| `Serializer/` | serialização (formato de rede) |
| `KncSecurity/` | cripto de pacotes |
| `Thread/` | threads e locks |
| `ObjectPool/` | pools de objetos |
| raiz | `KncUtil`, macros, `lua_tinker` |

- Origem: `KNCSDK_Server`, unido ao que só existe em `KNCSDK_Client`.
  Importado em 2026-10-29 (62 de 742 arquivos, [relatório](../../tools/import/reports/kncsdk/REPORT.md)) e portado à mão no mesmo dia.
- O módulo está **congelado** no importador (`frozen = true`): toda edição agora é feita direto em `src/kncsdk`.
- Análise do original: [Bibliotecas › KNCSDK](../original/bibliotecas.md).
- Depende de: [Plataforma](platform.md), Crypto++ 8.9, zlib, LuaJIT 2.1 (só `kncsdk_lua`).

## Estado (2026-10-29)

| Parte | Estado |
|---|---|
| Compilação Linux (GCC 16 Debug, Clang 23 Release) | ok, sem warnings |
| Compilação Windows MinGW-w64 (GCC 16, UCRT), testes no Wine 11 | ok, sem warnings; `kncsdk_tests` passa (2026-10-29) e `kncsdk_lua_tests` também, com o LuaJIT 2.1 compilado pelo CMake (T21, 2026-10-30) |
| Compilação Windows (MSVC) | **não testada** |
| União com `KNCSDK_Client` | feita (`SecureBuffer`, `KCSLockType`), ver abaixo |
| Testes | 28 casos / 964 asserções (`kncsdk_tests`) + 3 (`kncsdk_lua_tests`), todos passando |
| Validação contra bytes reais do cliente original | **pendente** (ver Pendências) |

## Mudanças em relação ao original

Interface pública (nomes, macros, assinaturas) mantida; mudou o que está listado aqui.

### Intencionais

| Onde | Mudança | Motivo |
|---|---|---|
| `dbg/logfile` | Linhas montadas por thread e gravadas sob lock, em arquivo **texto UTF-8** (`log.log`, ou `log/log_<data>.log`) + console com cor | O original gravava **HTML** de vários threads sem lock (condição de corrida) |
| `dbg/profiler` | Removido | Desligado no original (`TB_USE_PROFILER`), sem uso |
| `KncUtil` | Strings estreitas são **UTF-8** em todas as plataformas | O original usava a code page ANSI do Windows (`CP_ACP`) |
| `KncUtil::GetTimeStampByName*` | Data de modificação do executável | O original lia o *link time* do cabeçalho PE |
| `KncSecurity` | Gerador de chaves/SPI semeado pelo SO, um por thread | O original usava `boost::mt19937` **sem semente** (mesmas chaves em toda execução) e compartilhado sem lock |
| `ObjectPool` | `BlockAllocator`/`BoostAllocator` usam um pool próprio (`FixedBlockPool.h`) | O original guardava ponteiros de **32 bits** nos blocos livres; sem Boost |
| `Import*.h` | Esvaziados | Só faziam `#pragma comment(lib)`; o link agora é do CMake |
| `lua_interpreter` | Novo (`lua_interpreter.cpp`): prompt Lua simples em stdin, usado por `KBaseServer::Run` | O original vinha de um `lua.c` modificado do Lua 5.1.4, que o LuaJIT não traz; o terminal da T15 vai substituí-lo |
| Testes originais (`UnitTest/`) | Removidos, substituídos por doctest | Eram demos (um era cópia dos exemplos do Crypto++) |

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `KSecureBuffer::GetPayload*` e similares | `_JIF( cond, false )`: a "ação" `false;` não faz nada, e o código **continuava** após falhar, por exemplo decifrando um pacote com HMAC inválido | `_JIF( cond, return false )`, padrão já usado 1.404× no servidor. Só rejeita pacotes que já eram inválidos |
| `lua_tinker::dobuffer/dostring` | Condição invertida: o script nunca executava | Corrigida (o servidor não usava essas funções) |
| `lua_tinker` `tostring_s64/u64` | `%I64d` só existe no MSVC | `%lld`/`%llu` |
| `KncUtil::EraseFrontWord` | Dependia de *overflow* de `npos` em 32 bits | Mesmo resultado, explícito |
| `SerBuffer::Compress/UnCompress` | Passava `DWORD*` onde zlib espera `uLongf*` (8 bytes no Linux) | Variáveis do tipo certo; o tamanho na rede continua `DWORD` |
| `SerBuffer::UnCompress` | Ignorava a falha ao ler o tamanho original e descomprimia com um valor não inicializado | Retorna `false` (warning do GCC em Release) |

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

## Pendências

- **Validar o protocolo com bytes reais**: capturar pacotes do `x2.exe` original e do servidor original e comparar com o `Serializer`/`KSecureBuffer` portados.
  Hoje os bytes esperados dos testes vêm da leitura do código.
- O IV do `KSecureBuffer` é `'A' + rand() % 40` repetido (fraco). Mudar isso mexe no protocolo; decidir depois.
- O `lua_tinker` do cliente (`LuaCommon`) é outra variante; unificar ao portar o cliente.
- No cliente original, o log fica desligado nas builds de produção (`KNCSDK_Client/dbg/logfile.hpp`); reproduzir via nível de log ao portar o cliente.
- `KByteStream` herda de `std::basic_string<unsigned char>`, que a libc++ (Clang no macOS) não suporta mais. Não afeta Linux/Windows.
- LuaJIT no MSVC: a receita do CMake (T21) chama o `msvcbuild.bat` do LuaJIT (T25).
