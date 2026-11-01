[Início](../README.md) › [Ferramentas](README.md) › Integração contínua

# Integração contínua (GitHub Actions)

`.github/workflows/ci.yml`, criado em 2026-10-30 (container `archlinux`; no mesmo dia passou para a imagem Ubuntu, T24, e o MinGW + Wine deu lugar ao clang-cl no Windows, T26).

## Minutos (orçamento)

O repositório é privado: **2.000 minutos/mês** de Actions, com os do Windows contando **2×**. Em 2026-10-30 uma rodada completa
custava ~80 a 110 minutos cobrados (jobs Linux de 16 a 22 min com o ccache frio, Windows 14 min × 2). Por isso:

- **Automático só no push da `main`** (ao integrar), e não quando o push só muda `docs/`, `*.md` ou `.claude/`.
- **Branches de trabalho:** a matriz Linux roda local, de graça (`docker/build.sh`, [Ambiente de build](build-environment.md)). O CI é disparado
  à mão só quando precisa, por padrão **só o Windows**, quando a máquina Windows de testes não estiver disponível:
  `gh workflow run ci.yml --ref <branch>` (ou `-f jobs=linux` / `-f jobs=all`). Depois, `gh run list --branch <branch>`.
- **Antes de integrar,** a matriz Linux local completa passa e o Windows passa na máquina de testes (`tools/win/winbuild.sh`,
  [Ambiente de build](build-environment.md#windows-clang-cl-por-ssh)); o disparo do Windows no CI fica para quando ela não estiver disponível.
- **No CI, Linux com 2 jobs:** Clang Debug (dev tools ON) e GCC Release (dev tools OFF). O Clang Release, o build distribuído, fica na matriz local.
- **`timeout-minutes`** em todos os jobs; um push novo no mesmo branch cancela a rodada anterior.
- O cache do ccache de um branch só herda o da `main`: depois que a `main` roda, os branches começam quentes.

Os jobs de Linux rodam na imagem do [ambiente de build](build-environment.md) (Ubuntu, T24), a mesma do `docker/build.sh`; o de Windows, num runner Windows (T26).

| Job | O que faz |
|---|---|
| Build image | publica a imagem em `ghcr.io/<dono>/x2-build:<hash do Dockerfile>` (privada, como o repositório); só reconstrói quando o `Dockerfile` muda |
| Clang Debug, GCC Release (check) | build dos presets `linux-debug` (Clang) e `linux-gcc` (GCC, só verificação), `ctest` |
| clang-cl Debug | runner `windows-latest` (fora da imagem Ubuntu): ambiente do Visual Studio x64 (`ilammy/msvc-dev-cmd`) e o clang-cl do LLVM do runner, preset `windows-debug` (com `-k 0`: todos os erros numa rodada), `ctest`. O build de Windows distribuído (T26) e o único lugar onde o Windows é compilado; LuaJIT pelo `msvcbuild.bat` (T25). Minutos de Windows contam 2× em repositório privado |
| Importer and wiki | `tools/import/tests/test_condres.py`, `test_flagsim.py` e `tools/docs/check_links.py` |

- **Sem warnings**: o log do build é salvo e qualquer `warning:` (no `cl`/`link`, `warning C####:`/`LNK####:`) reprova o job (regra do AGENTS.md).
- **Dependências**: as da imagem; LuaJIT pela receita do CMake com commit fixo (T21); Asio, zlib e doctest pelo `FetchContent`
  com versão fixa (T13). `ccache` guardado entre execuções (`actions/cache`).
- **Fora do CI**: a suíte `db` do `KODBC` (precisa de um SQL Server: `X2_TEST_ODBC`, ver [Banco de desenvolvimento](development-database.md)),
  a geração do `ClientScript` (precisa do cliente, ver [Pastas de execução](runtime.md)) O repositório não tem
  `_reference/`, `_client/` nem `_database/`, e nada do CI depende deles.

Ver o resultado: `gh run list`, `gh run view --log-failed`.
