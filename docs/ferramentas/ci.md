[Início](../README.md) › [Ferramentas](README.md) › Integração contínua

# Integração contínua (GitHub Actions)

`.github/workflows/ci.yml`, criado em 2026-10-30. Roda em todo push, em pull request e à mão (`workflow_dispatch`).
Um push novo no mesmo branch cancela a execução anterior.

| Job | O que faz |
|---|---|
| GCC Debug, GCC Release, Clang Release | container `archlinux:latest` (o mesmo GCC 16/Clang/CMake 4 do desenvolvimento), build dos presets `linux-debug`/`linux-release` e de um Clang Release em `build/clang`, `ctest` |
| MinGW Debug + Wine | preset `mingw-debug`, `wineboot` num prefixo próprio e `ctest` pelo Wine (T18) |
| Importer and wiki | `tools/import/tests/test_condres.py`, `test_flagsim.py` e `tools/docs/check_links.py` |

- **Sem warnings**: o log do build é salvo e qualquer `warning:` reprova o job (regra do AGENTS.md).
- **Dependências**: LuaJIT, zlib e unixODBC vêm dos pacotes do Arch (no MinGW, LuaJIT e zlib são compilados pela receita do CMake, T21);
  Asio, Crypto++ e doctest pelo `FetchContent` com versão fixa (T13). `ccache` guardado entre execuções (`actions/cache`).
- **Fora do CI**: a suíte `db` do `KODBC` (precisa de um SQL Server: `X2_TEST_ODBC`, ver [Banco de desenvolvimento](banco-dev.md)),
  a geração do `ClientScript` (precisa do cliente, ver [Pastas de execução](runtime.md)) e o MSVC (D5). O repositório não tem
  `_reference/`, `_client/` nem `_database/`, e nada do CI depende deles.

Ver o resultado: `gh run list`, `gh run view --log-failed`.
