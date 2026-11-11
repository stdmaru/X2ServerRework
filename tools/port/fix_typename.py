#!/usr/bin/env python3
"""Inserts the `typename` that GCC asks for, guided by a build log.

MSVC accepted dependent type names without `typename`; GCC reports each one as
    file:line:col: error: need 'typename' before 'X::Y' because 'X' is a dependent scope
with `col` at the start of the qualified name (a display column: tabs expand to 8). The keyword is inserted right there.
Repeated reports (one per translation unit) are applied once; run the build again afterwards,
since one fix can uncover the next.

Usage: tools/port/fix_typename.py build.log [--root src] [--dry-run]
"""

import argparse
import unicodedata
import re
import sys
from collections import defaultdict
from pathlib import Path

PATTERN = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):(?P<col>\d+): error: need [‘'`]typename[’'`] before [‘'`](?P<name>[^’'`]+)")


def display_to_index(text: str, col: int, tabstop: int = 8) -> int:
    """GCC's default column unit is the display column (1-based, tabs to the next multiple of 8, wide characters
    such as Hangul take 2)."""
    width = 0
    for i, ch in enumerate(text):
        if width >= col - 1:
            return i
        if ch == "\t":
            width = (width // tabstop + 1) * tabstop
        else:
            width += 2 if unicodedata.east_asian_width(ch) in "WF" else 1
    return len(text)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--root", default="src", help="only edit files under this directory")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    root = Path(args.root).resolve()
    sites: dict[Path, set[tuple[int, int, str]]] = defaultdict(set)
    for line in Path(args.log).read_text(errors="replace").splitlines():
        m = PATTERN.match(line)
        if not m:
            continue
        path = Path(m["file"]).resolve()
        if root not in path.parents:
            continue
        sites[path].add((int(m["line"]), int(m["col"]), m["name"]))

    total = 0
    for path, found in sorted(sites.items()):
        lines = path.read_text(encoding="utf-8").split("\n")
        # right to left inside a line, so earlier columns stay valid
        for lineno, col, name in sorted(found, key=lambda s: (s[0], -s[1])):
            text = lines[lineno - 1]
            i = display_to_index(text, col)
            if text[:i].rstrip().endswith("typename"):
                continue
            first = name.split("<")[0].split("::")[0]
            if not text[i:].lstrip().startswith(first):
                print(f"skip {path}:{lineno}:{col}: '{name}' not found at the column", file=sys.stderr)
                continue
            lines[lineno - 1] = text[:i] + "typename " + text[i:]
            total += 1
            print(f"{path}:{lineno}: {lines[lineno - 1].strip()}")
        if not args.dry_run:
            path.write_text("\n".join(lines), encoding="utf-8")
    print(f"{total} typename inserted in {len(sites)} files", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
