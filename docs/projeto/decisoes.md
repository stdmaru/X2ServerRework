[Início](../README.md) › [Projeto](README.md) › Decisões

# Decisões

## Tomadas

| # | Decisão | Motivo |
|---|---|---|
| T1 | Assets do cliente não mudam; o código se adapta aos formatos atuais | Pedido do projeto |
| T2 | Ferramentas (`X2Tool`, patcher, `*DBManager`...) ficam fora do porte inicial | Foco em servidor + cliente + libs |
| T3 | Bibliotecas de terceiros entram pela versão **upstream**, não a cópia do `_reference` | As cópias são antigas, modificadas e só-Windows |
| T4 | Compatibilidade de rede com o `x2.exe`/`elsword.exe` original (antiga D1) | Validar o servidor sem ter o cliente portado |
| T5 | Variante **US / `_SERVICE_`** (antiga D2) | Config já usada no porte VS2026 do `_reference` |
| T6 | Alvo **64 bits** (antiga D5) | — |
| T7 | Manter **MSSQL via ODBC** por enquanto (antiga D6) | Migrar SGBD é projeto à parte |
| T8 | Ordem: **bibliotecas → servidor → cliente** | Servidor é menor e testável com o cliente original |
| T9 | `_reference` fica **intocado**; código novo do zero em `src/`, alimentado por um **importador automático** (seleção por vcxproj + CP949→UTF-8 + resolução de `#if` + exclusões) | Limpar o original exigiria compilar no Windows/MSVC x86 a cada passo; copiar à mão 2 M linhas é inviável e perderia a limpeza das flags |
| T10 | Base `_SERVICE_`, **mantendo ferramentas de debug/dev/admin** sob uma flag nova `X2_DEV_TOOLS` (build `Development`). Descartar o resto: flags não usadas, legado, outros países, anti-cheat | `_IN_HOUSE_` mistura debug com gameplay não lançado; whitelist + relatório em [Debug e dev](../original/debug-e-dev.md) |
| T11 | **Camada de compatibilidade** `x2::platform`: os nomes Win32/ATL usados em massa (`CTime`, `DWORD`, `CString`, `CRITICAL_SECTION`, eventos, `GetTickCount`...) são oferecidos com a mesma semântica; no Windows vêm do `windows.h` real. Modernização para `std::` só na Fase 5 | `CTime` aparece ~1.700×, `DWORD` ~900× e macros de log ~13.500× no servidor; reescrever durante o porte multiplicaria o risco. Ver [Plataforma](../porte/platform.md) |
| T12 | **arm64** é alvo futuro: nada de asm/intrínsecos x86 no código novo, `char` com sinal (`-fsigned-char`), dependências que suportam arm64 | Pedido do projeto (2026-10-29) |
| T13 | Dependências: `find_package` primeiro, `FetchContent` com versão e hash fixos como reserva (antiga D3). Crypto++ via `cryptopp-cmake`, com o include restrito a `<cryptopp/...>`; doctest só como header | Funciona igual em Linux e Windows sem gerenciador de pacotes obrigatório |
| T14 | Layout: `src/platform`, `src/kncsdk`, depois `src/server/...` e `src/client/...`; cada módulo importado espelha a estrutura original até a Fase 5 | Não precisa reescrever includes na importação |
| T15 | **UI do servidor = terminal em todas as plataformas** (o MFC sai, inclusive no Windows). O terminal reproduz a janela original: log ao vivo, prompt que executa **Lua** (como o campo de entrada do `CServerUIDlg`) com histórico, status `_Loading`/`_Ready` no título do terminal, confirmação ao encerrar e modo sem stdin para rodar como serviço. Um `--gui` portável (ImGui + SDL3, igual nas duas plataformas) fica como front-end opcional futuro, sobre a mesma API de comandos/log | Um caminho de código só; o MFC não é portável. Decisão delegada ao agente pelo usuário (2026-10-29) |
| T16 | **Log**: o arquivo mantém o conteúdo completo do original (nível, função, data, dados, `BUILD_LOG`, arquivo/linha). O console tem renderização própria, compacta e colorida (`HH:MM:SS NÍVEL Função: mensagem`, campos `BUILD_LOG` alinhados abaixo), com nível de filtro independente do arquivo. `START_LOG` passa a entregar metadados estruturados ao logger em vez de texto pronto | O conteúdo do original é útil para diagnóstico, mas confuso no console. Decisão delegada ao agente pelo usuário (2026-10-29) |
| T17 | **MSSQL via API ODBC**, driver **Microsoft ODBC Driver 18** (Windows e Linux, x64/arm64) e FreeTDS como alternativa sem mudar código. Os `.dsn` viram *connection strings* na configuração. No unixODBC, `SQLWCHAR` é UTF-16: a camada ODBC converte de/para `wchar_t` | O wrapper original (`KODBC`/`StoredProcedure`) é ODBC; o driver oficial é o mais completo (TLS, tipos, desempenho). Detalha a T7. Delegado ao agente pelo usuário (2026-10-29) |
| T18 | **MinGW-w64 (GCC, runtime UCRT) cruzado a partir do Linux**, com testes rodando no **Wine**, como verificação contínua dos caminhos `_WIN32`. Presets `mingw-debug`/`mingw-release`, toolchain em `cmake/toolchains/mingw-w64-x86_64.cmake`, runtime estático. O build de Windows que vale para distribuição continua sendo o **MSVC**; o MinGW não substitui a validação nele | Não há máquina Windows; sem isso o código Windows só seria compilado na hora de validar no MSVC, acumulando erros. O MinGW já achou um bug real (`printf` ISO no MinGW). Delegado ao agente pelo usuário (2026-10-29) |
| T19 | **SDL3** para janela/input/áudio e **SDL_GPU** para render no cliente (antiga D4), com shaders via SDL_shadercross. Vulkan direto e ponte DXVK descartados como alvo | Vulkan/D3D12/Metal de graça com muito menos código; o D3D9 está concentrado no `KTDXLIB`. Escolhido pelo usuário com a recomendação do agente (2026-10-29) |

## Pendentes

### D5 — LuaJIT no Windows
O LuaJIT não tem CMake. Opções:
- receita própria em CMake, que compila `buildvm` e gera os headers;
- vcpkg só para ele;
- binário pré-compilado.

Decidir quando houver uma máquina Windows para validar.
