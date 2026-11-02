[Home](../README.md) › Tools

# Project tools

| Page | Contents |
|---|---|
| [Importer](importer.md) | `tools/import`: imports and normalizes modules from `_reference` into `src/` (encoding, flags, includes) |
| [Mechanical porting](mechanical-porting.md) | `tools/port`: mechanical pass and build-log-guided fixes for errors/warnings (`typename`, `switch`, `reorder`...) |
| [Development database](development-database.md) | `docker/`: SQL Server 2022 in Docker with databases from `_database/`, connection strings, and `KODBC` tests against it |
| [Run directories](runtime.md) | `runtime/` and `x2_run_*` targets: ready-to-run server folders, `ClientScript` generated from the client, `tools/kom` |
| [Build environment](build-environment.md) | `docker/build.sh` and `docker/build/Dockerfile`: Ubuntu image for all local and CI builds/tests (T24) |
| [Continuous integration](ci.md) | `.github/workflows/ci.yml`: warning-free x86_64 builds and tests on main pushes with Clang, GCC, and clang-cl (Windows); ARM64 Linux CI is planned |
| [Flags and verification](flags.md) | `gen_flags.py` (which flags original build defines) and `check_equivalence.py` (proves resolver leaves code unchanged) |
