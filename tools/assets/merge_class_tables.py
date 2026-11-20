#!/usr/bin/env python3
"""Brings the class entries our client lacks from other clients' class tables into ours.

Usage:
  merge_class_tables.py <client_data> --classes UC_ADD_ARC_TRACER,... --source <data036 folder> [--source ...]
                        [--dry-run]

For each of UnitTemplet.lua (one AddUnitTemplet per class, keyed by `unitClass`), CharacterStatTable.lua (one row per
class, keyed by its first value) and CorrelateUnitClassTemplet.lua (one group per class: the class, then the classes
it comes from, keyed by the first), the entries of --classes that ours lacks are taken, as they are, from the first
--source folder that has them, and inserted after our last entry. Classes our Enum.lua lacks are refused. Nothing of
ours is changed, so a second run changes nothing. See docs/tools/client-data.md (Bringing data from other clients).
"""
import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from client_data import ClientData  # noqa: E402

CLASS = r"UNIT_CLASS\.(\w+)"


def read(path):
    return Path(path).read_text(encoding="utf-8-sig", errors="replace").replace("\r\n", "\n")


def unit_templets(text):
    """[(class, start, end)] of the AddUnitTemplet calls."""
    out = []
    for m in re.finditer(r"^g_pUnitManager:AddUnitTemplet\(\{\n.*?^\}\)\n?", text, re.M | re.S):
        c = re.search(r"^\tunitClass = " + CLASS, m.group(0), re.M)
        out.append((c.group(1) if c else None, m.start(), m.end()))
    return out


def rows(text):
    """[(class, start, end)] of the entries inside the file's one call: `\t{ UNIT_CLASS.X, ... },` on one line, or
    `\t{` ... `\t},` over several (CorrelateUnitClassTemplet). Keyed by the entry's first UNIT_CLASS."""
    out = []
    for m in re.finditer(r"^\t\{.*?\},?\n|^\t\{\n.*?^\t\},?\n", text, re.M | re.S):
        c = re.search(CLASS, m.group(0))
        out.append((c.group(1) if c else None, m.start(), m.end()))
    return out


FILES = {"UnitTemplet.lua": unit_templets, "CharacterStatTable.lua": rows, "CorrelateUnitClassTemplet.lua": rows}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--classes", required=True)
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cd = ClientData(args.client_data)
    classes = [c.strip() for c in args.classes.split(",") if c.strip()]
    enum = read(cd.find("Enum.lua"))
    unknown = [c for c in classes if not re.search(r"^\t%s = \d+," % c, enum, re.M)]
    if unknown:
        raise SystemExit(f"error: Enum.lua has no {', '.join(unknown)}")

    for name, split in FILES.items():
        path = cd.find(name)
        text = read(path)
        ours = split(text)
        have = {c for c, _, _ in ours}
        added = []
        for cls in classes:
            if cls in have:
                continue
            for folder in args.source:
                src = Path(folder) / name
                if not src.is_file():
                    continue
                their = read(src)
                hit = [(s, e) for c, s, e in split(their) if c == cls]
                if hit:
                    s, e = hit[0]
                    block = their[s:e]
                    if not block.endswith("\n"):
                        block += "\n"
                    if split is rows and not block.rstrip("\n").endswith(","):
                        block = block.rstrip("\n") + ",\n"
                    added.append((cls, block, src))
                    break
            else:
                print(f"{name}: {cls} in no source")
        for cls, _, src in added:
            print(f"{name}: {cls} <- {src}")
        if args.dry_run or not added:
            continue
        end = ours[-1][2]
        if text[end - 1] != "\n":
            text = text[:end] + "\n" + text[end:]
            end += 1
        text = text[:end] + "".join(b for _, b, _ in added) + text[end:]
        Path(path).write_text("﻿" + text, encoding="utf-8")


if __name__ == "__main__":
    main()
