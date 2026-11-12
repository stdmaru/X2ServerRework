#!/usr/bin/env python3
"""Fixes -Wsign-compare in imported code without changing what the comparison computes.

Clang reports the operand ranges (-fdiagnostics-print-source-range-info). For each comparison:

  s.find(x) == -1           -> s.find(x) == std::wstring::npos (std::string for narrow strings)
  for (int i = 0; i < v.size(); ...)
                            -> i < static_cast<int>( v.size() )   (i starts at 0: same result)
  anything else             -> the signed operand is cast to the unsigned type, which is what the original
                               comparison did implicitly (a negative index still fails `i < v.size()`)

Run in the build image: tools/port/fix_sign_compare.py build/linux-debug --filter src/client/X2Lib/
       [--root src/client] [-j 10] [--dry-run]
"""

import argparse
import json
import re
import sys
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from clang_fixits import run

WARN = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):(?P<col>\d+):(?P<ranges>(?:\{\d+:\d+-\d+:\d+\})+): warning: "
                  r"comparison of integers of different signs: '(?P<lt>[^']+)'(?: \(aka '(?P<la>[^']+)'\))? and "
                  r"'(?P<rt>[^']+)'(?: \(aka '(?P<ra>[^']+)'\))? \[-Wsign-compare\]$")
RANGE = re.compile(r"\{(\d+):(\d+)-(\d+):(\d+)\}")
FIND = re.compile(r"\.\s*(?:r?find|find_(?:first|last)_(?:not_)?of)\s*\(")


def unsigned(t: str, aka: str | None) -> bool:
    return "unsigned" in (aka or t) or "size_t" in t


def unsigned_name(t: str, aka: str | None) -> str:
    t = re.sub(r"\bconst\s+", "", t).strip()
    if t in ("DWORD", "UINT", "WORD", "BYTE", "ULONG", "size_t", "unsigned int", "unsigned long", "unsigned short",
             "unsigned char"):
        return "size_t" if t == "unsigned long" else t
    a = re.sub(r"\bconst\s+", "", aka or t).strip()
    return {"unsigned long": "size_t", "unsigned int": "unsigned int", "unsigned short": "unsigned short",
            "unsigned char": "unsigned char"}.get(a, "size_t")


def signed_name(t: str, aka: str | None) -> str:
    t = re.sub(r"\bconst\s+", "", aka or t).strip()
    return t if t in ("int", "long", "short", "char", "long long", "signed char") else "int"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("build_dir")
    ap.add_argument("--filter", required=True)
    ap.add_argument("--root", default="src")
    ap.add_argument("-j", type=int, default=10)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    root = Path(args.root).resolve()
    entries = [e for e in json.loads((Path(args.build_dir) / "compile_commands.json").read_text())
               if args.filter in e["file"]]
    with ThreadPoolExecutor(args.j) as pool:
        logs = list(pool.map(lambda e: run(e, ["sign-compare"], ("-fdiagnostics-print-source-range-info",)),
                             entries))
    sites: dict[Path, set] = defaultdict(set)
    for log in logs:
        for raw in log.splitlines():
            m = WARN.match(raw)
            if not m:
                continue
            path = Path(m["file"]).resolve()
            rs = [tuple(map(int, r)) for r in RANGE.findall(m["ranges"])]
            if root in path.parents and len(rs) == 2:
                sites[path].add((rs[0], rs[1], m["lt"], m["la"], m["rt"], m["ra"]))

    total = 0
    for path, items in sorted(sites.items()):
        data = path.read_bytes()
        offsets = [0] + [i + 1 for i, b in enumerate(data) if b == 0x0A]

        def span(r):
            return offsets[r[0] - 1] + r[1] - 1, offsets[r[2] - 1] + r[3] - 1

        edits = []
        for lr, rr, lt, la, rt, ra in items:
            (la_, lb), (ra_, rb) = span(lr), span(rr)
            lhs, rhs = data[la_:lb].decode(errors="replace"), data[ra_:rb].decode(errors="replace")
            op = data[lb:ra_].decode(errors="replace").strip()
            l_unsigned = unsigned(lt, la)
            u_text, s_text = (lhs, rhs) if l_unsigned else (rhs, lhs)
            (ua, ub), (sa, sb) = ((la_, lb), (ra_, rb)) if l_unsigned else ((ra_, rb), (la_, lb))
            u_t, u_a, s_t, s_a = (lt, la, rt, ra) if l_unsigned else (rt, ra, lt, la)
            line_start = data.rfind(b"\n", 0, la_) + 1
            line = data[line_start:data.find(b"\n", la_)].decode(errors="replace")
            if op in ("==", "!=") and s_text.strip() == "-1" and FIND.search(u_text):
                npos = "std::wstring::npos" if ("L\"" in line or "wstr" in u_text.lower()) else "std::string::npos"
                edits.append((sa, sb, npos))
            elif (op in ("<", "<=") and not l_unsigned and re.fullmatch(r"\w+", s_text)
                  and re.search(r"\bfor\s*\(\s*(?:int|long)\s+" + s_text + r"\s*=\s*0\s*;", line)):
                edits.append((ua, ub, f"static_cast<{signed_name(s_t, s_a)}>( {u_text} )"))
            else:
                edits.append((sa, sb, f"static_cast<{unsigned_name(u_t, u_a)}>( {s_text} )"))
        # one edit per range; skip overlapping ones (nested comparisons): the next build reports them again
        edits = sorted(set(edits), reverse=True)
        applied, last = [], len(data) + 1
        for a, b, t in edits:
            if b <= last:
                data = data[:a] + t.encode() + data[b:]
                last = a
                applied.append(t)
        if not args.dry_run:
            path.write_bytes(data)
        print(f"{path}: {len(applied)}")
        total += len(applied)
    print(f"{total} fixes", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
