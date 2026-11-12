#!/usr/bin/env python3
"""Applies Clang's fix-it hints for selected warnings to the files of a target, using the compile database.

Each translation unit is checked with -fsyntax-only, only the chosen warnings on, and
-fdiagnostics-parseable-fixits. Fix-its on the warning itself and on its first note are applied (for
-Wlogical-op-parentheses and -Wparentheses the first note is "place parentheses around ..."). Each source
range is edited once, however many translation units reported it. Run the build again afterwards.

Usage (in the build image): tools/port/clang_fixits.py build/linux-debug --filter src/client/X2Lib/
           --warnings logical-op-parentheses,parentheses [--root src/client] [-j 10] [--dry-run]
"""

import argparse
import json
import re
import shlex
import subprocess
import sys
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

DIAG = re.compile(r"^(?P<file>[^:\s]+):\d+:\d+: (?P<kind>warning|note|error): .*?(?:\[-W(?P<flag>[\w-]+)\])?$")
FIXIT = re.compile(r'^fix-it:"(?P<file>[^"]+)":\{(?P<l1>\d+):(?P<c1>\d+)-(?P<l2>\d+):(?P<c2>\d+)\}:"(?P<text>.*)"$')


def unescape(s: str) -> str:
    return re.sub(r"\\(.)", lambda m: {"n": "\n", "t": "\t"}.get(m.group(1), m.group(1)), s)


def run(entry: dict, warnings: list[str], extra: tuple[str, ...] = ()) -> str:
    args = entry["arguments"] if "arguments" in entry else shlex.split(entry["command"])
    out = []
    skip = False
    for a in args[1:]:
        if skip:
            skip = False
            continue
        if a in ("-o", "-MF", "-MT", "-MQ"):
            skip = True
            continue
        if a in ("-c", "-MD", "-MMD") or a.startswith("-W"):
            continue
        out.append(a)
    cmd = [args[0], *out, "-fsyntax-only", "-Wno-everything", *[f"-W{w}" for w in warnings],
           "-fdiagnostics-parseable-fixits", "-ferror-limit=0", *extra]
    return subprocess.run(cmd, cwd=entry["directory"], capture_output=True, text=True, errors="replace").stderr


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("build_dir")
    ap.add_argument("--filter", required=True, help="substring of the translation units to check")
    ap.add_argument("--warnings", required=True)
    ap.add_argument("--root", default="src", help="only edit files under this directory")
    ap.add_argument("-j", type=int, default=10)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    warnings = args.warnings.split(",")
    root = Path(args.root).resolve()
    entries = [e for e in json.loads((Path(args.build_dir) / "compile_commands.json").read_text())
               if args.filter in e["file"]]
    with ThreadPoolExecutor(args.j) as pool:
        logs = list(pool.map(lambda e: run(e, warnings), entries))

    edits: dict[Path, set] = defaultdict(set)
    for log in logs:
        state = None  # None, "warning" (take fix-its), "note" (first note: take), "done"
        for raw in log.splitlines():
            d = DIAG.match(raw)
            if d:
                if d["kind"] == "warning":
                    state = "warning" if d["flag"] in warnings else "done"
                elif d["kind"] == "note":
                    state = "note" if state == "warning" else "done"
                else:
                    state = "done"
                continue
            f = FIXIT.match(raw)
            if f and state in ("warning", "note"):
                path = Path(f["file"]).resolve()
                if root in path.parents:
                    edits[path].add((int(f["l1"]), int(f["c1"]), int(f["l2"]), int(f["c2"]), unescape(f["text"])))

    total = 0
    for path, items in sorted(edits.items()):
        data = path.read_bytes()
        offsets = [0] + [i + 1 for i, b in enumerate(data) if b == 0x0A]
        spans = sorted(((offsets[l1 - 1] + c1 - 1, offsets[l2 - 1] + c2 - 1, t) for l1, c1, l2, c2, t in items),
                       reverse=True)
        for a, b, t in spans:
            data = data[:a] + t.encode() + data[b:]
        if not args.dry_run:
            path.write_bytes(data)
        print(f"{path}: {len(spans)}")
        total += len(spans)
    print(f"{total} edits", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
