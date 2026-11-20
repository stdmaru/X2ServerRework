#!/usr/bin/env python3
"""Brings the state tables a character's code asks for from other clients' unit scripts into ours.

Usage:
  merge_unit_script.py <client_data> --script Add_NasodRuler.lua --code <X2GUAdd.cpp> [--code ...]
                       --source <script> [--source <script> ...] [--parent INIT_COMMON_RANDOM_STATE ...]
                       [--alias EL_=ELESIS_] [--dry-run]

- Wanted names: every quoted string of the --code files that looks like a table name (upper case, digits and
  underscores, with at least one underscore, mixed case allowed after the first underscore: ANSI_AMM_COMBO_XXDownX).
- A top-level table (`NAME = {` at column 0) that ours lacks is taken, as it is, from the first --source that has
  it, and appended to our script. The quoted strings of the tables taken are wanted too (a class's START_STATE_LIST
  names its start states), so the closure is brought.
- A table nested one level down in a --parent (`\tNAME = {` inside INIT_COMMON_RANDOM_STATE, say: its per-class
  tables) that ours lacks and is wanted is inserted before the parent's closing brace. Only the --parent tables:
  the nested tables of the states we have (BR 2017's revamp added some) are left as they are. --alias OURS=THEIRS
  looks a wanted name up in the sources with a prefix of theirs (our Elesis tables are EL_*, BR 2017's ELESIS_*).
- Wanted names that no source has are listed (some are not tables at all: strings of the code).
Nothing of ours is changed or replaced, so a second run changes nothing. The script is rewritten in place (UTF-8
with BOM, LF). See docs/tools/client-data.md (Bringing data from other clients).
"""
import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from client_data import ClientData  # noqa: E402

NAME = re.compile(r'"([A-Z][A-Z0-9]*_[A-Za-z0-9_]+)"')
TOP = re.compile(r"^(\w+) = \{")
NESTED = re.compile(r"^\t(\w+) = \{")


def read(path):
    return Path(path).read_text(encoding="utf-8-sig", errors="replace").replace("\r\n", "\n")


def tables(lines, start_re, indent):
    """{name: (first line, last line)} of the tables opened by start_re, closed by a line `<indent>}`."""
    out = {}
    i = 0
    while i < len(lines):
        m = start_re.match(lines[i])
        if not m:
            i += 1
            continue
        j = i
        if not lines[i].rstrip().rstrip(",").endswith("}"):
            j = i + 1
            while j < len(lines) and not re.match(r"^%s\},?\s*$" % indent, lines[j]):
                j += 1
        out.setdefault(m.group(1), (i, j))
        i = j + 1
    return out


class Script:
    def __init__(self, path):
        self.path = path
        self.lines = read(path).split("\n")
        self.top = tables(self.lines, TOP, "")

    def block(self, name):
        i, j = self.top[name]
        return self.lines[i:j + 1]

    def nested(self, parent):
        i, j = self.top[parent]
        inner = tables(self.lines[i + 1:j], NESTED, "\t")
        return {k: (a + i + 1, b + i + 1) for k, (a, b) in inner.items()}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--script", required=True)
    ap.add_argument("--code", action="append", required=True)
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--alias", action="append", default=[], help="OURS=THEIRS name prefix")
    ap.add_argument("--parent", action="append", default=[], help="a top-level table whose nested tables are merged")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    ours_path = ClientData(args.client_data).find(args.script)
    ours = Script(ours_path)
    sources = [Script(s) for s in args.source]
    aliases = [a.split("=", 1) for a in args.alias]

    def names_in(text):
        return set(NAME.findall(text))

    wanted = set()
    for code in args.code:
        wanted |= names_in(Path(code).read_text(encoding="utf-8", errors="replace"))

    def their_names(name):
        yield name
        for o, t in aliases:
            if name.startswith(o):
                yield t + name[len(o):]

    # Top-level tables, with the closure of the names they quote.
    added, origin = [], {}
    todo = sorted(wanted)
    seen = set()
    while todo:
        name = todo.pop()
        if name in seen or name in ours.top:
            continue
        seen.add(name)
        for src in sources:
            if name in src.top:
                block = src.block(name)
                added.append((name, block))
                origin[name] = src.path
                more = names_in("\n".join(block)) - seen
                wanted |= more
                todo.extend(sorted(more))
                break

    # Nested tables of the top-level tables both have.
    inserts = {}  # our closing line -> [lines]
    nested_added = []
    for parent in args.parent:
        if parent not in ours.top:
            continue
        ours_nested = ours.nested(parent)
        for src in sources:
            if parent not in src.top:
                continue
            theirs = src.nested(parent)
            for name in sorted(wanted):
                if name in ours_nested:
                    continue
                for their in their_names(name):
                    if their in theirs:
                        a, b = theirs[their]
                        block = src.lines[a:b + 1]
                        block[0] = block[0].replace(their, name, 1)
                        if not block[-1].rstrip().endswith(","):
                            block[-1] = block[-1].rstrip() + ","
                        inserts.setdefault(ours.top[parent][1], []).extend(block)
                        nested_added.append((f"{parent}.{name}", src.path))
                        ours_nested[name] = None
                        break

    for name, path in nested_added:
        print(f"nested {name}  <- {path}")
    for name, _ in added:
        print(f"table  {name}  <- {origin[name]}")
    print(f"{len(added)} tables and {len(nested_added)} nested tables to add to {ours_path}")
    referenced = set(re.findall(r"^[A-Z]{2,5}_\w+$", "\n".join(sorted(wanted)), re.M))
    known = set(ours.top) | {n for n, _ in added}
    missing = sorted(n for n in referenced if n not in known and re.match(r"^[A-Z]+SI_", n))
    if missing:
        print(f"state-like names in no script: {', '.join(missing)}")
    if args.dry_run or not (added or nested_added):
        return

    out = []
    for i, line in enumerate(ours.lines):
        if i in inserts:
            out.extend(inserts[i])
        out.append(line)
    while out and out[-1] == "":
        out.pop()
    for _, block in added:
        out.extend(block)
    Path(ours_path).write_text("﻿" + "\n".join(out) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
