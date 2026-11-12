[Home](../README.md) › [Tools](README.md) › Mechanical porting

# Mechanical porting (`tools/port`)

Scripts for a mechanical pass over a module **after** it has been imported (see [Importer](importer.md)).
Unlike the importer, these edit `src/` in place: run, review the diff, and commit. Created on 2026-10-29 for
`server/common-build` ([Server: Common](../porting/server-common.md)).

| Script | What it does | Input |
|---|---|---|
| `mechanical.py` | Win32/ATL/D3DX/Winsock includes → platform, Boost → `std`/`x2::`, `BOOST_FOREACH` → range-for, invalid `##` pasting | module folder |
| `fix_typename.py` | Inserts `typename` required by GCC (`need 'typename' before ...`) at reported column | build log |
| `gen_packet_tests.py` | Generates `src/server/Common/tests/test_packets_*.cpp` (round-trip all serialized types) and checks `PUT`/`GET` symmetry. `--check` only checks (used by ctest) | packet `.cpp` files |
| `fix_warnings.py` | `-Wswitch` → `default: break;`; `-Wunused-parameter` → `/*name*/`; `-Wreorder` → declaration-order initializer list | build log |
| `fix_unused.py` | `-Wunused-variable`/`-Wunused-but-set-variable`: deletes the local (keeping a side-effecting right-hand side) or marks it `[[maybe_unused]]`; `--maybe-unused` only marks (for locals one configuration does not read, e.g. without `X2_DEV_TOOLS`) | build log |
| `clang_fixits.py` | Recompiles the reported files with `compile_commands.json` and applies Clang's fix-its for the chosen warnings (warning and first note) | build log |
| `fix_sign_compare.py` | `-Wsign-compare`: `find(...) == -1` → `npos`; `for (int i = 0; i < x.size(); ...)` → `static_cast<int>(x.size())`; otherwise casts the signed operand | build log |

Typical use:

```bash
cmake --build --preset linux-debug --target x2_server_common -- -k 0 > build.log 2>&1
python3 tools/port/fix_typename.py build.log
python3 tools/port/fix_warnings.py build.log --only switch,unused-parameter,reorder
```

## Precautions

- **Run passes one at a time and rebuild between them.** Line numbers come from the log, and a pass that inserts lines
  (`switch`) shifts every later warning. Run `switch` last.
- `clang_fixits.py` needs the caret output: do not pass `-fno-caret-diagnostics`, which also drops the fix-its.
- **GCC columns are display columns** (tab = 8 columns, East Asian wide character = 2), not byte offsets; Clang's are bytes. Scripts convert them (`display_to_index`).
- For `-Wswitch`, GCC points to the parenthesis, not the word `switch`: script searches for `switch` on the same line.
  An earlier version searched forward and edited the next `switch`.
- For `-Wreorder`, order comes from declarations **at the class's own scope**, using the line reported by GCC.
  Only initializers move; commas and comments stay in place. Execution is unchanged because members were always initialized in declaration order.
- Each location is fixed once, even if several `.cpp` files report it. A fix can expose another issue: rebuild.
- **Use a clean build log** (`--clean-first`) to see every warning; incremental build shows warnings only from recompiled files.
