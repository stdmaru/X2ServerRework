[Início](../README.md) › [Ferramentas](README.md) › Ambiente de build

# Ambiente de build (Ubuntu em Docker)

Desde 2026-10-30 os builds e testes rodam numa imagem **Ubuntu** própria (decisão T24), a mesma no desenvolvimento e no
[CI](ci.md). O host (qualquer distribuição com Docker) só monta o repositório.

- Imagem: `docker/build/Dockerfile`, sobre `ubuntu:26.04` (26.04.1 LTS). Em 2026-10-30: GCC 15.2, Clang 21.1, CMake 4.2,
  Ninja 1.13, MinGW-w64 13 (GCC 13.2, modelo de threads `posix`), Wine 10.0, unixODBC 2.3 + FreeTDS (`tdsodbc`), Python 3, ccache.
- Sem pacote do LuaJIT: o do Ubuntu é o fork OpenResty, então o CMake compila o commit upstream fixo (T21) também no Linux.
- Todo preset passa pelo `ccache` (`CMAKE_<LANG>_COMPILER_LAUNCHER` na imagem).

## Uso

`docker/build.sh` roda um comando dentro do container (sem argumentos, abre um shell). Ele constrói a imagem na primeira vez
(e quando o `Dockerfile` muda; senão o cache do Docker torna isso instantâneo).

```bash
docker/build.sh cmake --preset linux-debug
docker/build.sh cmake --build --preset linux-debug
docker/build.sh ctest --preset linux-debug
docker/build.sh cmake --preset mingw-debug && docker/build.sh cmake --build --preset mingw-debug
docker/build.sh ctest --preset mingw-debug
```

- O repositório é montado **no mesmo caminho** do host: `build/` e o `compile_commands.json` servem ao editor do host.
- Roda com o usuário do host (os arquivos de `build/` não ficam do root). O `HOME` do container, com o cache do ccache e o
  prefixo do Wine (criado na primeira execução), fica em `~/.cache/x2-build` (`X2_BUILD_HOME` muda).
- Rede do host (`--network host`): os testes da suíte `db` e os servidores acham o SQL Server do [banco de desenvolvimento](banco-dev.md)
  em `127.0.0.1`, e o cliente original acha os servidores como antes.
- Um diretório de `build/` configurado fora do container (outro compilador) deve ser apagado antes: o Ninja não recompila
  objetos só porque a versão do compilador mudou.

## O que o Ubuntu mudou em relação ao Arch (2026-10-30)

| Achado | Tratamento |
|---|---|
| MinGW do Ubuntu usa por padrão o modelo de threads `win32` (sem `std::thread`/`std::mutex` completos) | a imagem seleciona as variantes `-posix` por `update-alternatives` (T18) |
| GCC 13 do MinGW: `ServerPacket.cpp` e os testes de pacotes passam do limite de seções do COFF ("file too big") | `-Wa,-mbig-obj` no MinGW e `/bigobj` no MSVC (`cmake/X2Compiler.cmake`) |
| GCC 15 Release: `-Wmaybe-uninitialized` falso em `KTCInfo::m_wstrUDPRelayIP` nos testes de ida e volta | pragma só em volta do `CheckRoundTrip` (`src/server/Common/tests/packet_test.h`); o pragma no ponto de chamada não funciona |
| Pacote do LuaJIT é o fork OpenResty | não instalado; receita do commit fixo (T21) |
