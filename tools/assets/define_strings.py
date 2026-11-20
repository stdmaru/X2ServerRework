#!/usr/bin/env python3
"""Defines the STR_IDs that given files use and copies their strings into our general.ess.

Usage:
  define_strings.py <client_data> --refs <file> [--refs ...] [--code-refs <file> ...] --ess <our general.ess>
                    --strings <their General.ess> [--strings ...] [--dry-run]

Every STR_ID_<n> of the --refs files (what was just brought from other clients: unit templets, buffs, quests) that
StringID_def.lua lacks is defined (STR_ID_<n> = <n>, appended), and every one whose string in --ess is a placeholder
(empty or its own name, which the client shows as is) takes the first --strings' text that is real; ours is never
changed otherwise. --code-refs (C++, which has the IDs from its own header) only need the strings, not the Lua
definitions. String IDs are the same in ours, BR 2017's and V5's (docs/tools/client-data.md). A second run changes
nothing.
"""
import argparse
import re
from pathlib import Path

from client_data import ClientData
from merge_quests import ess_line, read_ess


def placeholder(n, s):
    return s in ("", f"STR_ID_{n}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--refs", action="append", required=True)
    ap.add_argument("--code-refs", action="append", default=[])
    ap.add_argument("--ess", required=True)
    ap.add_argument("--strings", action="append", required=True)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    def ids(paths):
        out = set()
        for r in paths:
            out |= set(map(int, re.findall(r"\bSTR_ID_(\d+)\b", Path(r).read_text(encoding="utf-8", errors="replace"))))
        return out
    lua_used = ids(args.refs)
    used = lua_used | ids(args.code_refs)

    defs_path = ClientData(args.client_data).find("StringID_def.lua")
    defs_raw = defs_path.read_bytes()
    defined = set(map(int, re.findall(r"^STR_ID_(\d+) = ", defs_raw.decode("utf-8-sig"), re.M)))
    new_defs = sorted(lua_used - defined)
    ess = read_ess(args.ess)
    theirs = [read_ess(s) for s in args.strings]
    fill, unknown = {}, []
    for n in sorted(used):
        if not placeholder(n, ess.get(n, "")):
            continue
        text = next((t[n] for t in theirs if not placeholder(n, t.get(n, ""))), None)
        if text is None:
            unknown.append(n)
        else:
            fill[n] = text
    print(f"{len(used)} STR_IDs used: {len(new_defs)} to define, {len(fill)} strings to copy, {len(unknown)} in no "
          f"--strings" + (f": {', '.join(f'STR_ID_{n}' for n in unknown)}" if unknown else ""))
    if args.dry_run:
        return
    if new_defs:
        nl = "\r\n" if b"\r\n" in defs_raw else "\n"
        body = defs_raw.decode("utf-8-sig").rstrip("\r\n") + nl + nl.join(f"STR_ID_{n} = {n}" for n in new_defs) + nl
        defs_path.write_bytes((b"\xef\xbb\xbf" if defs_raw.startswith(b"\xef\xbb\xbf") else b"") + body.encode())
    if fill:
        table = dict(ess)
        table.update(fill)
        Path(args.ess).write_bytes(b"\xff\xfe" + b"".join(ess_line(i, table[i]) for i in sorted(table)))
    print(f"wrote {defs_path} and {args.ess}")


if __name__ == "__main__":
    main()
