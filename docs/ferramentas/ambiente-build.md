[Início](../README.md) › [Ferramentas](README.md) › Ambiente de build

# Ambiente de build (Ubuntu em Docker)

Desde 2026-10-30 os builds e testes de Linux rodam numa imagem **Ubuntu** própria (decisão T24), a mesma no desenvolvimento e no
[CI](ci.md). O host (qualquer distribuição com Docker) só monta o repositório. O Windows (clang-cl, T26) não é compilado aqui:
só no job Windows do CI.

- Imagem: `docker/build/Dockerfile`, sobre `ubuntu:26.04` (26.04.1 LTS). Em 2026-10-30: Clang 21.1 (presets `linux-*`), GCC 15.2
  (preset `linux-gcc`, verificação), CMake 4.2, Ninja 1.13, unixODBC 2.3 + FreeTDS (`tdsodbc`), Python 3, ccache.
- Sem pacote do LuaJIT: o do Ubuntu é o fork OpenResty, então o CMake compila o commit upstream fixo (T21) também no Linux.
- Todo preset passa pelo `ccache` (`CMAKE_<LANG>_COMPILER_LAUNCHER` na imagem).

## Uso

`docker/build.sh` roda um comando dentro do container (sem argumentos, abre um shell). Ele constrói a imagem na primeira vez
(e quando o `Dockerfile` muda; senão o cache do Docker torna isso instantâneo).

```bash
docker/build.sh cmake --preset linux-debug
docker/build.sh cmake --build --preset linux-debug
docker/build.sh ctest --preset linux-debug
docker/build.sh cmake --preset linux-gcc && docker/build.sh cmake --build --preset linux-gcc   # verificação com GCC
```

- O repositório é montado **no mesmo caminho** do host: `build/` e o `compile_commands.json` servem ao editor do host.
- Roda com o usuário do host (os arquivos de `build/` não ficam do root). O `HOME` do container, com o cache do ccache,
  fica em `~/.cache/x2-build` (`X2_BUILD_HOME` muda).
- Rede do host (`--network host`): os testes da suíte `db` e os servidores acham o SQL Server do [banco de desenvolvimento](banco-dev.md)
  em `127.0.0.1`, e o cliente original acha os servidores como antes.
- Um diretório de `build/` configurado fora do container (outro compilador) deve ser apagado antes: o Ninja não recompila
  objetos só porque a versão do compilador mudou.

## Windows (clang-cl) por SSH

O Windows não roda na imagem. Além do job do [CI](ci.md), há uma **máquina Windows de testes** (VM do usuário, desde 2026-10-30)
onde `tools/win/winbuild.sh` compila e testa sem gastar minutos do Actions:

```bash
X2_WIN_HOST=usuario@host tools/win/winbuild.sh                        # windows-debug, 6 jobs
X2_WIN_HOST=usuario@host tools/win/winbuild.sh windows-release 4
```

- Manda os arquivos versionados **com as edições ainda não commitadas** (`git ls-files` + `tar` pelo SSH) para `C:/x2/src`
  (`X2_WIN_DIR` muda) e roda `cmake`/`ctest` no ambiente do Visual Studio achado pelo `vswhere`. O `build/` fica na máquina e é incremental.
- A máquina precisa de Visual Studio Build Tools (carga C++, componentes Clang e CMake, que trazem CMake e Ninja), Python 3 e
  OpenSSH com login por chave; Git não. Em 2026-10-30: Windows Server 2022, Build Tools 18.11.3 (MSVC 14.51), 12 núcleos, 8 GB
  (por isso 6 jobs: os arquivos de pacotes são pesados).
- Só para build e testes.

## O que o Ubuntu mudou em relação ao Arch (2026-10-30)

| Achado | Tratamento |
|---|---|
| MinGW do Ubuntu: threads `win32` por padrão e "file too big" (limite de seções do COFF) nos pacotes | resolvido com `-posix` e `-Wa,-mbig-obj`; depois o MinGW saiu (T26) |
| GCC 15 Release: `-Wmaybe-uninitialized` falso em `KTCInfo::m_wstrUDPRelayIP` nos testes de ida e volta | pragma só em volta do `CheckRoundTrip` (`src/server/Common/tests/packet_test.h`); o pragma no ponto de chamada não funciona |
| Pacote do LuaJIT é o fork OpenResty | não instalado; receita do commit fixo (T21) |
