#!/usr/bin/env python3
"""Fills our general.ess, by ID, with another client's strings where ours has none.

Usage:
  fill_strings.py --ess <our general.ess> --strings <their General.ess> [--strings ...] [--dry-run]

String IDs are stable between clients (BR 2017 keeps 2014's, 2026-11-16), so an ID means the same string in both.
For each ID, in --strings order, the first source with a real text that is not Korean wins:
- an ID whose text in ours is a placeholder (empty, or its own name `STR_ID_<n>`) takes that text;
- an ID whose text in ours is Korean takes it too (PT-BR whenever a source has it).
Every other string of ours is kept: our file is the superset.
"""
import argparse
import re
from pathlib import Path

from merge_quests import ess_line, read_ess

KOREAN = re.compile("[가-힣]")


def placeholder(i, s):
    return s in ("", f"STR_ID_{i}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--ess", required=True)
    ap.add_argument("--strings", action="append", required=True)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    ours = read_ess(args.ess)
    sources = [read_ess(p) for p in args.strings]
    filled, translated = [], []
    for i, s in ours.items():
        empty, korean = placeholder(i, s), bool(KOREAN.search(s))
        if not (empty or korean):
            continue
        for src in sources:
            t = src.get(i, "")
            if not placeholder(i, t) and not KOREAN.search(t):
                ours[i] = t
                (filled if empty else translated).append(i)
                break

    left = sum(placeholder(i, s) for i, s in ours.items())
    print(f"{len(filled)} placeholders filled, {len(translated)} Korean strings translated, {left} placeholders left")
    if not args.dry_run:
        Path(args.ess).write_bytes(b"\xff\xfe" + b"".join(ess_line(i, ours[i]) for i in sorted(ours)))
        print(f"wrote {args.ess}")


if __name__ == "__main__":
    main()
