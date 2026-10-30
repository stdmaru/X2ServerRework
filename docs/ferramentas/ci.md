[Início](../README.md) › [Ferramentas](README.md) › Integração contínua

# Integração contínua (GitHub Actions)

`.github/workflows/ci.yml`, criado em 2026-10-30 (container `archlinux`; no mesmo dia passou para a imagem Ubuntu, T24, e o MinGW + Wine deu lugar ao clang-cl no Windows, T26). Roda em todo push, em pull request e à mão (`workflow_dispatch`).
Um push novo no mesmo branch cancela a execução anterior.

Os jobs de Linux rodam na imagem do [ambiente de build](ambiente-build.md) (Ubuntu, T24), a mesma do `docker/build.sh`; o de Windows, num runner Windows (T26).

| Job | O que faz |
|---|---|
| Build image | publica a imagem em `ghcr.io/<dono>/x2-build:<hash do Dockerfile>` (privada, como o repositório); só reconstrói quando o `Dockerfile` muda |
| Clang Debug, Clang Release, GCC Release (check) | build dos presets `linux-debug`, `linux-release` (Clang) e `linux-gcc` (GCC, só verificação), `ctest` |
| clang-cl Debug | runner `windows-latest` (fora da imagem Ubuntu): ambiente do Visual Studio x64 (`ilammy/msvc-dev-cmd`) e o clang-cl do LLVM do runner, preset `windows-debug` (com `-k 0`: todos os erros numa rodada), `ctest`. O build de Windows distribuído (T26) e o único lugar onde o Windows é compilado; LuaJIT pelo `msvcbuild.bat` (T25). Minutos de Windows contam 2× em repositório privado |
| Importer and wiki | `tools/import/tests/test_condres.py`, `test_flagsim.py` e `tools/docs/check_links.py` |

- **Sem warnings**: o log do build é salvo e qualquer `warning:` (no `cl`/`link`, `warning C####:`/`LNK####:`) reprova o job (regra do AGENTS.md).
- **Dependências**: as da imagem; LuaJIT pela receita do CMake com commit fixo (T21); Asio, zlib e doctest pelo `FetchContent`
  com versão fixa (T13). `ccache` guardado entre execuções (`actions/cache`).
- **Fora do CI**: a suíte `db` do `KODBC` (precisa de um SQL Server: `X2_TEST_ODBC`, ver [Banco de desenvolvimento](banco-dev.md)),
  a geração do `ClientScript` (precisa do cliente, ver [Pastas de execução](runtime.md)) O repositório não tem
  `_reference/`, `_client/` nem `_database/`, e nada do CI depende deles.

Ver o resultado: `gh run list`, `gh run view --log-failed`.
