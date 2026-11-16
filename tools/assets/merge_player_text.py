#!/usr/bin/env python3
"""Adds to our EventScene.lua the per-character lines (PLAYER_TEXT<n>) a newer client has and ours lacks.

Usage:
  merge_player_text.py <client_data> --source <EventScene.lua> [--source ...] --ess <our general.ess>
                       --strings <their General.ess> [--max-index 8] [--dry-run]

A scene's TEXT_PLAY picks PLAYER_TEXT<unit type - 1> for the player's character (CX2EventScene::GetPlayerText) and
falls back to TEXT, empty in most of them, when the index is missing: Add (UT_ADD = 9) needs PLAYER_TEXT8, which our
2014-03 scenes lack. For each TEXT_PLAY of ours, keyed by scene ID, sequence and text play, the PLAYER_TEXT<n> lines up
to --max-index that ours lacks are taken from the first --source that has them; our lines are never changed. The STR_IDs
they bring are defined in StringID_def.lua and their strings copied from --strings into --ess where ours
lacks them.
"""
import argparse
import re
from pathlib import Path

from client_data import ClientData
from merge_quests import ess_line, read_ess

SCENE = re.compile(r'^\s*EVENT_SCENE_ID = "([^"]+)"')
OPEN = re.compile(r"^(\s*)(EVENT_SEQUENCE\d+|TEXT_PLAY\d+) = \{\s*$")
PLAYER = re.compile(r"^(\s*)PLAYER_TEXT(\d+) = (STR_ID_\d+),\s*$")


def text_plays(lines):
    """{(scene, sequence, text play): {index: (line number, indent, value)}} and each text play's last line."""
    plays, scene, seq, play, last = {}, None, None, None, {}
    for n, line in enumerate(lines):
        if m := SCENE.match(line):
            scene, seq, play = m.group(1), None, None
        elif m := OPEN.match(line):
            if m.group(2).startswith("EVENT_SEQUENCE"):
                seq, play = m.group(2), None
            else:
                play = m.group(2)
        elif play and (m := PLAYER.match(line)):
            key = (scene, seq, play)
            plays.setdefault(key, {})[int(m.group(2))] = (n, m.group(1), m.group(3))
            last[key] = n
    return plays, last


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--ess", required=True)
    ap.add_argument("--strings", required=True)
    ap.add_argument("--max-index", type=int, default=8)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cd = ClientData(args.client_data)
    path = cd.find("EventScene.lua")
    raw = path.read_bytes()
    nl = "\r\n" if b"\r\n" in raw else "\n"
    lines = raw.decode("utf-8-sig").split(nl)
    ours, last = text_plays(lines)
    sources = [text_plays(Path(p).read_bytes().decode("utf-8-sig", "replace").splitlines())[0] for p in args.source]

    inserts, missing, used = {}, [], set()
    for key, idx in ours.items():
        for i in range(max(idx) + 1, args.max_index + 1):
            src = next((s[key][i][2] for s in sources if i in s.get(key, {})), None)
            if src is None:
                missing.append((key, i))
                break
            indent = idx[max(idx)][1]
            inserts.setdefault(last[key], []).append(f"{indent}PLAYER_TEXT{i} = {src},")
            used.add(int(src[7:]))

    defs_path = cd.find("StringID_def.lua")
    defs_raw = defs_path.read_bytes()
    defined = set(map(int, re.findall(r"^STR_ID_(\d+) = ", defs_raw.decode("utf-8-sig"), re.M)))
    new_defs = sorted(used - defined)
    ess, theirs = read_ess(args.ess), read_ess(args.strings)
    new_strings = [n for n in sorted(used) if ess.get(n, f"STR_ID_{n}") in ("", f"STR_ID_{n}")]
    unknown = [n for n in new_strings if theirs.get(n, f"STR_ID_{n}") in ("", f"STR_ID_{n}")]

    print(f"{sum(map(len, inserts.values()))} lines in {len(inserts)} text plays, {len(new_defs)} STR_ID definitions, "
          f"{len(new_strings) - len(unknown)} strings; {len(missing)} text plays in no source")
    for (scene, seq, play), i in missing:
        print(f"  no PLAYER_TEXT{i}: {scene} {seq} {play}")
    for n in unknown:
        print(f"  STR_ID_{n}: no text in --strings")
    if args.dry_run:
        return

    out = []
    for n, line in enumerate(lines):
        out.append(line)
        out += inserts.get(n, [])
    path.write_bytes((b"\xef\xbb\xbf" if raw.startswith(b"\xef\xbb\xbf") else b"") + nl.join(out).encode())
    if new_defs:
        dnl = "\r\n" if b"\r\n" in defs_raw else "\n"
        body = defs_raw.decode("utf-8-sig").rstrip("\r\n") + dnl + dnl.join(f"STR_ID_{n} = {n}" for n in new_defs) + dnl
        defs_path.write_bytes((b"\xef\xbb\xbf" if defs_raw.startswith(b"\xef\xbb\xbf") else b"") + body.encode())
    table = dict(ess)
    for n in new_strings:
        if n not in unknown:
            table[n] = theirs[n]
    Path(args.ess).write_bytes(b"\xff\xfe" + b"".join(ess_line(i, table[i]) for i in sorted(table)))
    print(f"wrote {path}, {defs_path}, {args.ess}")


if __name__ == "__main__":
    main()
