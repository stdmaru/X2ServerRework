[Início](../README.md) › [Ferramentas](README.md) › Integração contínua

# Integração contínua (GitHub Actions)

`.github/workflows/ci.yml`, criado em 2026-10-30 (container `archlinux`; passou para a imagem Ubuntu no mesmo dia).
O repositório é privado (2.000 minutos/mês, Windows 2×): roda sozinho só no push da `main` (fora pushes só de docs) e, nos branches,
à mão (`gh workflow run ci.yml --ref <branch>`), depois de a matriz passar localmente. Um push novo no mesmo branch cancela a execução anterior.

Todos os jobs rodam na imagem do [ambiente de build](ambiente-build.md) (Ubuntu, T24), a mesma do `docker/build.sh`.

| Job | O que faz |
|---|---|
| Build image | publica a imagem em `ghcr.io/<dono>/x2-build:<hash do Dockerfile>` (privada, como o repositório); só reconstrói quando o `Dockerfile` muda |
| GCC Debug, GCC Release, Clang Release | build dos presets `linux-debug`/`linux-release` e de um Clang Release em `build/clang`, `ctest` |
| MinGW Debug + Wine | preset `mingw-debug`, `wineboot` num prefixo do root (`/root/.wine`: o workspace e o `HOME` montados pelo GitHub não são dele) e `ctest` pelo Wine (T18) |
| MSVC Debug | runner `windows-latest` (fora da imagem Ubuntu): ambiente do Visual Studio x64 (`ilammy/msvc-dev-cmd`), preset `windows-debug`, `ctest`. O build de Windows que vale para distribuição (T18); LuaJIT pelo `msvcbuild.bat` (T25). Minutos de Windows contam 2× em repositório privado |
| Importer and wiki | `tools/import/tests/test_condres.py`, `test_flagsim.py` e `tools/docs/check_links.py` |

- **Sem warnings**: o log do build é salvo e qualquer `warning:` (no MSVC, `warning C####:`) reprova o job (regra do AGENTS.md).
- **Dependências**: as da imagem; LuaJIT pela receita do CMake com commit fixo (T21); Asio, Crypto++ e doctest pelo `FetchContent`
  com versão fixa (T13). `ccache` guardado entre execuções (`actions/cache`).
- **Fora do CI**: a suíte `db` do `KODBC` (precisa de um SQL Server: `X2_TEST_ODBC`, ver [Banco de desenvolvimento](banco-dev.md)),
  a geração do `ClientScript` (precisa do cliente, ver [Pastas de execução](runtime.md)) O repositório não tem
  `_reference/`, `_client/` nem `_database/`, e nada do CI depende deles.

Ver o resultado: `gh run list`, `gh run view --log-failed`.
