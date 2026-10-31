[Início](README.md) › Compilar no Linux

# Compilar no Linux

Há dois caminhos. O **recomendado é o container** (`docker/build.sh`): é a mesma imagem Ubuntu do CI (decisão T24), e
o host só precisa do Docker. O build nativo também funciona, mas não é verificado continuamente.

Os dois produzem os mesmos presets:

| Preset | Compilador | Tipo | Ferramentas de dev (`X2_DEV_TOOLS`) |
|---|---|---|---|
| `linux-debug` | Clang | Debug | sim |
| `linux-development` | Clang | RelWithDebInfo | sim |
| `linux-release` | Clang | Release | não |
| `linux-gcc` | GCC | Release | não (só verificação) |

As ferramentas de dev são os comandos de debug e de administração do original ([Debug e dev](original/debug-e-dev.md)).
Sem elas, o build é o de produção.

## 1. Pegar o código

```bash
git clone https://github.com/stdmaru/X2ServerRework.git
cd X2ServerRework
```

O **cliente original** (`_client/`) e os **backups do banco** (`_database/`) não fazem parte do repositório. O build não
precisa deles, mas para rodar, sim ([Rodar os servidores](running.md)). Sem o `_client/`, o build avisa que pulou o `ClientScript`.

## 2a. Com Docker (recomendado)

Pacotes do host: Docker com o plugin `compose`, e Git.

| Distribuição | Comando |
|---|---|
| Ubuntu / Debian | `sudo apt install docker.io docker-compose-v2 git` |
| Arch | `sudo pacman -S docker docker-compose git` |
| Fedora | `sudo dnf install moby-engine docker-compose git` |

O usuário precisa poder rodar o Docker sem `sudo`:

```bash
sudo systemctl enable --now docker
sudo usermod -aG docker "$USER"     # vale a partir do próximo login
```

Configurar, compilar e testar:

```bash
docker/build.sh cmake --preset linux-debug
docker/build.sh cmake --build --preset linux-debug
docker/build.sh ctest --preset linux-debug
```

- Na primeira vez, o `docker/build.sh` constrói a imagem (`docker/build/Dockerfile`). Depois, o cache do Docker torna isso instantâneo.
  Sem argumentos, o script abre um shell dentro dela.
- O repositório é montado no mesmo caminho do host, então `build/` e `compile_commands.json` servem ao editor normalmente.
- O container roda com o seu usuário. O cache do ccache fica em `~/.cache/x2-build`.
- As variáveis `X2_*` do shell e do `docker/.env` são repassadas ao container ([Rodar os servidores](running.md#2-configurações-locais)).

## 2b. Nativo (sem Docker)

Pacotes:

| Distribuição | Comando |
|---|---|
| Ubuntu 26.04 | `sudo apt install build-essential clang cmake ninja-build pkg-config git python3 unixodbc-dev tdsodbc` |
| Arch | `sudo pacman -S base-devel clang cmake ninja pkgconf git python unixodbc freetds` |

É o que a imagem instala. Precisa de CMake 3.28 ou mais novo e de um Clang com C++20. O ccache é opcional.
O zlib, o LuaJIT, o Asio e o doctest vêm pelo próprio CMake: ele usa o pacote do sistema quando existe e, quando não, baixa a versão fixa.
O LuaJIT só aceita o upstream 2.1. O pacote do Ubuntu é o fork do OpenResty e é ignorado: nesse caso o CMake compila o commit fixo.

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
```

**Observação:** um diretório `build/<preset>` configurado de um jeito (nativo ou container) deve ser apagado antes de
configurar do outro. O cache guarda caminhos de bibliotecas que só existem num dos dois, e o Ninja não recompila só
porque o compilador mudou.

## 3. O resultado

- Executáveis em `build/<preset>/src/server/<Servidor>/`.
- **Pastas prontas para rodar** em `build/<preset>/run/<Servidor>/`: o executável, os configs preenchidos e os scripts.
  O build normal monta essas pastas (`X2_RUN_FOLDERS`). Ver [Rodar os servidores](running.md).
- Um build sem mudanças leva menos de um segundo, e cada build seguinte só refaz o que mudou.

## Problemas comuns

| Sintoma | Causa e solução |
|---|---|
| `permission denied ... docker.sock` | o usuário não está no grupo `docker`: rode `usermod -aG docker`, entre de novo na sessão |
| `Imported target "PkgConfig::LUAJIT" includes non-existent path` | `build/<preset>` configurado fora do container: apague a pasta e configure de novo |
| `ClientScript is not generated` | falta o `_client/data` (ou o `X2_CLIENT_DATA` aponta para outro lugar): o build segue, mas os servidores precisam dele para rodar |

Mais detalhes da imagem e do CI: [Ambiente de build](ferramentas/ambiente-build.md), [Integração contínua](ferramentas/ci.md).
