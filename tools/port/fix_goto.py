#!/usr/bin/env python3
"""Hoists the declarations a goto jumps over (GCC: "jump to label" / "crosses initialization of").

MSVC let `goto end_proc` (the DO_QUERY macro of the DB threads) skip the initialization of later locals;
standard C++ does not. Each crossed declaration moves to the top of its function body:

- `T name = <literal>;`        the whole line moves (initializing earlier changes nothing)
- `T name = <expression>;`     `T name{};` moves up and `name = <expression>;` stays where it was
                               (a top-level `const` is dropped then: the value is computed later)

Only one-line declarations at the label's level are handled; the rest (references, multi-line declarations,
several names in one statement) is listed for the manual pass. Run with the log of a GCC build:

    cmake --build <gcc dir> --target <target> -- -k 0 > build.log 2>&1
    python3 tools/port/fix_goto.py build.log [--dry-run]
"""
import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

NOTE = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):\d+: note:\s+crosses initialization of '(?P<decl>[^']+)'")
LITERAL = re.compile(r"^(?:-?\d+(?:\.\d*)?[fFuUlL]*|true|false|NULL|nullptr|0x[0-9A-Fa-f]+|L?\"[^\"]*\"|'.')$")


def function_start(lines, index):
    """Index of the `{` that opens the function containing lines[index] (a `{` alone at column 0)."""
    for i in range(index, -1, -1):
        if lines[i].rstrip("\r\n") == "{":
            return i
    return None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    crossed = defaultdict(set)  # file -> {(line, decl)}
    for text in Path(args.log).read_text(encoding="utf-8", errors="replace").splitlines():
        m = NOTE.match(text)
        if m and not m["file"].endswith((".hpp", ".h")):
            crossed[m["file"]].add((int(m["line"]), m["decl"]))

    manual, changed = [], 0
    for file, decls in sorted(crossed.items()):
        path = Path(file)
        lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
        hoist = defaultdict(list)   # function `{` index -> declarations to insert
        edits = {}                  # line index -> replacement
        for lineno, decl in sorted(decls):
            i = lineno - 1
            name = decl.split()[-1].lstrip("*&")
            m = re.match(r"^(?P<indent>\s*)(?P<type>[^=;]*?\b" + re.escape(name) + r")\s*=\s*(?P<init>[^;]+);(?P<rest>.*)$",
                         lines[i].rstrip("\r\n"))
            start = function_start(lines, i)
            if not m or start is None or "&" in m["type"] or "," in m["type"]:
                manual.append(f"{file}:{lineno}: {decl}")
                continue
            if LITERAL.match(m["init"].strip()):
                hoist[start].append(lines[i].strip())
                edits[i] = None
            else:
                tdecl = m["type"].strip()
                if "*" not in tdecl:  # a top-level const only; in `const T* p` it belongs to the pointee
                    tdecl = re.sub(r"^const\s+", "", tdecl)
                hoist[start].append(f"{tdecl}{{}};")
                edits[i] = f"{m['indent']}{name} = {m['init'].strip()};{m['rest']}\n"
        if not hoist:
            continue
        out = []
        for i, text in enumerate(lines):
            if i in edits:
                if edits[i] is not None:
                    out.append(edits[i])
                continue
            out.append(text)
            if i in hoist:
                out.append("\t// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)\n")
                out.extend(f"\t{d}\n" for d in hoist[i])
        changed += sum(len(v) for v in hoist.values())
        if not args.dry_run:
            path.write_text("".join(out), encoding="utf-8")
        print(f"{file}: {sum(len(v) for v in hoist.values())} declaration(s) hoisted")

    print(f"{changed} declaration(s) hoisted", file=sys.stderr)
    for m in manual:
        print(f"manual: {m}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
