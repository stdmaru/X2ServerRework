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

Typical use:

```bash
cmake --build --preset linux-debug --target x2_server_common -- -k 0 > build.log 2>&1
python3 tools/port/fix_typename.py build.log
python3 tools/port/fix_warnings.py build.log --only switch,unused-parameter,reorder
```

## Precautions

- **GCC columns are display columns** (tab = 8 columns), not byte offsets. Scripts convert them (`display_to_index`).
- For `-Wswitch`, GCC points to the parenthesis, not the word `switch`: script searches for `switch` on the same line.
  An earlier version searched forward and edited the next `switch`.
- For `-Wreorder`, order comes from declarations **at the class's own scope**, using the line reported by GCC.
  Only initializers move; commas and comments stay in place. Execution is unchanged because members were always initialized in declaration order.
- Each location is fixed once, even if several `.cpp` files report it. A fix can expose another issue: rebuild.
- **Use a clean build log** (`--clean-first`) to see every warning; incremental build shows warnings only from recompiled files.
