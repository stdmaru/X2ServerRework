[Home](../README.md) › Tools

# Project tools

| Page | Contents |
|---|---|
| [Importer](importer.md) | `tools/import`: imports and normalizes modules from `_reference` into `src/` (encoding, flags, includes) |
| [Mechanical porting](mechanical-porting.md) | `tools/port`: mechanical pass and build-log-guided fixes for errors/warnings (`typename`, `switch`, `reorder`...) |
| [Development database](development-database.md) | `docker/`: SQL Server 2022 in Docker with databases from `_database/`, connection strings, and `KODBC` tests against it |
| [Database patches](database-patches.md) | Additive SQL repairs for missing database objects; patches are reviewed and applied manually |
| [Server package](runtime.md) | `runtime/` and `x2_run_*` targets: package under `dist/`, `ClientScript` generated from the client, `tools/kom` |
| [Build environment](build-environment.md) | Dev Container, `docker/build.sh`, and `docker/build/Dockerfile`: shared Ubuntu image for local and CI builds/tests (T24) |
| [Continuous integration](ci.md) | `.github/workflows/ci.yml`: Linux x86_64 and ARM64 build/test jobs, an x86_64 GCC check, and Windows clang-cl |
| [client_data](client-data.md) | `src/tools/AssetTool` (`x2_assets`): imports the client's `.kom` into an organized folder + manifest and packs it back byte for byte (T35); layout rules in `tools/assets/layout.txt` |
| [ServerManager](servermanager.md) | `src/tools/ServerManager`: SDL3 + ImGui tool that starts and monitors the servers, locally and over SSH (T30); protocol client, orchestration, SSH and window |
| [Flags and verification](flags.md) | `gen_flags.py` (which flags original build defines), `check_equivalence.py` (proves resolver leaves code unchanged) and `compare_flags.py` (server x client in shared code) |
