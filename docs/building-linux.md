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

The **original client** (`_client/`) and **database backups** (`_database/`) are not part of the repository. You do not need them
to build, but you do need them to run the servers ([Run the servers](running.md)). Without `_client/`, the build warns that it
skipped `ClientScript`.

## 2a. Docker (recommended)

Host requirements: Docker with the Compose plugin, and Git.

| Distribution | Command |
|---|---|
| Ubuntu / Debian | `sudo apt install docker.io docker-compose-v2 git` |
| Arch | `sudo pacman -S docker docker-compose git` |
| Fedora | `sudo dnf install moby-engine docker-compose git` |

Your user account must be able to run Docker without `sudo`:

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

- On the first run, `docker/build.sh` builds the image from `docker/build/Dockerfile`. Later runs are nearly instant thanks to Docker's
  build cache. Without arguments, the script opens a shell inside the image.
- The repository is mounted at the same path as on the host, so `build/` and `compile_commands.json` are available to your editor.
- The container runs as your user. ccache is stored in `~/.cache/x2-build`.
- `X2_*` variables from your shell and `docker/.env` are passed into the container ([Run the servers](running.md#2-local-settings)).

## 2b. Native (without Docker)

Packages:

| Distribution | Command |
|---|---|
| Ubuntu 26.04 | `sudo apt install build-essential clang cmake ninja-build pkg-config git python3 unixodbc-dev tdsodbc` |
| Arch | `sudo pacman -S base-devel clang cmake ninja pkgconf git python unixodbc freetds` |

These packages are installed in the image. Native builds require CMake 3.28 or later and a Clang compiler with C++20 support.
ccache is optional. CMake uses installed packages when available; otherwise, it downloads pinned versions of zlib, LuaJIT, Asio,
and doctest. Only upstream LuaJIT 2.1 is supported. Ubuntu's LuaJIT package is an OpenResty fork and is ignored; in that case,
CMake builds LuaJIT from the pinned commit.

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
```

**Note:** remove `build/<preset>` before switching between native and container builds. The cache stores library paths that may exist
in only one environment, and Ninja does not rebuild just because the compiler has changed.

## 3. Output

- Executables in `build/<preset>/src/server/<Server>/`.
- **Ready-to-run folders** in `build/<preset>/run/<Server>/`: executable, filled configs, and scripts.
  Normal build assembles these (`X2_RUN_FOLDERS`). See [Run the servers](running.md).
- A no-op build takes less than a second; subsequent builds rebuild only what has changed.

## Common issues

| Symptom | Cause and fix |
|---|---|
| `permission denied ... docker.sock` | user is not in `docker` group: run `usermod -aG docker`, log in again |
| `Imported target "PkgConfig::LUAJIT" includes non-existent path` | `build/<preset>` was configured outside container: remove directory and configure again |
| `ClientScript is not generated` | `_client/data` missing (or `X2_CLIENT_DATA` points elsewhere): build continues, but servers need it to run |

More on image and CI: [Build environment](tools/build-environment.md), [Continuous integration](tools/ci.md).
