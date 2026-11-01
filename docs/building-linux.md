[Home](README.md) › Build on Linux

# Build on Linux

There are two options. The **recommended option is the container** (`docker/build.sh`): it uses the same Ubuntu image as CI
(decision T24), and the host only needs Docker. Native builds also work, but are not continuously verified.

Both use the same presets:

| Preset | Compiler | Type | Dev tools (`X2_DEV_TOOLS`) |
|---|---|---|---|
| `linux-debug` | Clang | Debug | yes |
| `linux-development` | Clang | RelWithDebInfo | yes |
| `linux-release` | Clang | Release | no |
| `linux-gcc` | GCC | Release | no (verification only) |

Dev tools are original debug and administration commands ([Debug and development](original/debug-and-development.md)).
Without them, the build is for production.

## 1. Get the code

```bash
git clone https://github.com/stdmaru/X2ServerRework.git
cd X2ServerRework
```

The **original client** (`_client/`) and **database backups** (`_database/`) are not part of repository. Build does not need them,
but running does ([Run the servers](running.md)). Without `_client/`, build warns that it skipped `ClientScript`.

## 2a. Docker (recommended)

Host packages: Docker with `compose` plugin, and Git.

| Distribution | Command |
|---|---|
| Ubuntu / Debian | `sudo apt install docker.io docker-compose-v2 git` |
| Arch | `sudo pacman -S docker docker-compose git` |
| Fedora | `sudo dnf install moby-engine docker-compose git` |

User must be able to run Docker without `sudo`:

```bash
sudo systemctl enable --now docker
sudo usermod -aG docker "$USER"     # takes effect on next login
```

Configure, build, and test:

```bash
docker/build.sh cmake --preset linux-debug
docker/build.sh cmake --build --preset linux-debug
docker/build.sh ctest --preset linux-debug
```

- On first run, `docker/build.sh` builds image (`docker/build/Dockerfile`). Later Docker cache makes it immediate.
  Without arguments, script opens a shell inside image.
- Repository is mounted at same host path, so `build/` and `compile_commands.json` are available to editor.
- Container runs as your user. ccache is in `~/.cache/x2-build`.
- Shell and `docker/.env` `X2_*` variables are passed into container ([Run the servers](running.md#2-local-settings)).

## 2b. Native (without Docker)

Packages:

| Distribution | Command |
|---|---|
| Ubuntu 26.04 | `sudo apt install build-essential clang cmake ninja-build pkg-config git python3 unixodbc-dev tdsodbc` |
| Arch | `sudo pacman -S base-devel clang cmake ninja pkgconf git python unixodbc freetds` |

These are installed in the image. Requires CMake 3.28+ and Clang with C++20. ccache is optional.
zlib, LuaJIT, Asio, and doctest come through CMake: it uses system package when available, otherwise downloads pinned version.
LuaJIT accepts upstream 2.1 only. Ubuntu package is OpenResty fork and ignored; in that case CMake builds pinned commit.

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
```

**Note:** a `build/<preset>` directory configured one way (native or container) must be removed before configuring the other way.
Cache stores library paths that exist in only one environment, and Ninja does not rebuild merely because compiler changed.

## 3. Output

- Executables in `build/<preset>/src/server/<Server>/`.
- **Ready-to-run folders** in `build/<preset>/run/<Server>/`: executable, filled configs, and scripts.
  Normal build assembles these (`X2_RUN_FOLDERS`). See [Run the servers](running.md).
- An unchanged build takes less than a second; each subsequent build only rebuilds what changed.

## Common issues

| Symptom | Cause and fix |
|---|---|
| `permission denied ... docker.sock` | user is not in `docker` group: run `usermod -aG docker`, log in again |
| `Imported target "PkgConfig::LUAJIT" includes non-existent path` | `build/<preset>` was configured outside container: remove directory and configure again |
| `ClientScript is not generated` | `_client/data` missing (or `X2_CLIENT_DATA` points elsewhere): build continues, but servers need it to run |

More on image and CI: [Build environment](tools/build-environment.md), [Continuous integration](tools/ci.md).
