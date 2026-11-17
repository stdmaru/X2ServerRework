#!/usr/bin/env python3
"""Adds to our PetTemplet.lua and RidingPetTemplet.lua the pets (by ID) a newer client has and ours lacks.

Usage:
  merge_pets.py <client_data> [--pets 100-102,116] [--riding-pets 11] --source <data036 folder> [--source ...]
                --ess <our general.ess> --strings <their General.ess> [--dry-run]

- A pet is an AddPetTemplet (PET_ID = PET_UNIT_ID.<name>) and a riding pet an AddRidingPetTemplet
  (RIDING_PET_ID = RIDING_PET_UNIT_ID.<name>); IDs are turned into names with our Enum.lua (PET_UNIT_ID and
  RIDING_PET_UNIT_ID), so a pet our enum does not name cannot be added. Each templet comes, as it is, from the first
  --source folder (holding PetTemplet.lua / RidingPetTemplet.lua) that has it, and is appended to ours.
- The STR_IDs the added templets use are defined in StringID_def.lua and their strings copied from --strings into
  --ess where ours lacks them.
- The scripts, textures and meshes the templets name are reported when the client_data has none of that name
  (bring them with copy_assets.py).
"""
import argparse
import re
from pathlib import Path

from client_data import ClientData
from merge_quests import ess_line, id_list, read_ess

KINDS = (  # (file, call, enum table, ID field)
    ("PetTemplet.lua", "g_pPetManager:AddPetTemplet", "PET_UNIT_ID", "PET_ID"),
    ("RidingPetTemplet.lua", "g_pRidingPetManager:AddRidingPetTemplet", "RIDING_PET_UNIT_ID", "RIDING_PET_ID"),
)
FILE_NAME = re.compile(r'"([\w\-. ]+\.(?:lua|tga|dds|x|y|xet))"', re.I)


def read(path):
    raw = Path(path).read_bytes()
    return raw, raw.decode("utf-8-sig", "surrogateescape"), ("\r\n" if b"\r\n" in raw else "\n")


def enum_table(enum_text, table):
    m = re.search(r"^" + table + r"\s*=\s*\{(.*?)\}", enum_text, re.S | re.M)
    return {int(v): k for k, v in re.findall(r"(\w+)\s*=\s*(-?\d+)", m[1])} if m else {}


def templets(text, call, field, table):
    out = {}
    for m in re.finditer(r"^" + re.escape(call) + r"\(\{.*?^\}\)", text, re.S | re.M):
        k = re.search(field + r"\s*=\s*" + table + r'(?:\.(\w+)|\["(\w+)"\])', m[0])
        if k:
            out[k[1] or k[2]] = m[0]
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--pets", default="")
    ap.add_argument("--riding-pets", default="")
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--ess", required=True)
    ap.add_argument("--strings", required=True)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cd = ClientData(args.client_data)
    enum_text = read(cd.find("Enum.lua"))[1]
    used, writes = set(), []
    for (file, call, table, field), spec in zip(KINDS, (args.pets, args.riding_pets)):
        wanted = id_list(spec) if spec else []
        if not wanted:
            continue
        names = enum_table(enum_text, table)
        path = cd.find(file)
        raw, text, nl = read(path)
        ours = templets(text, call, field, table)
        sources = [(p, templets(read(Path(p) / file)[1], call, field, table)) for p in args.source
                   if (Path(p) / file).is_file()]
        added = []
        for i in wanted:
            name = names.get(i)
            if name is None:
                print(f"  {table} {i}: not in our Enum.lua; left out")
            elif name in ours:
                print(f"  {name} ({i}): ours already")
            else:
                src = next(((p, t[name]) for p, t in sources if name in t), None)
                if src is None:
                    print(f"  {name} ({i}): in no --source")
                    continue
                print(f"  {name} ({i}): from {src[0]}")
                added.append(src[1].replace("\r\n", "\n").replace("\n", nl))
                used |= set(map(int, re.findall(r"\bSTR_ID_(\d+)\b", src[1])))
                for f in sorted(set(FILE_NAME.findall(src[1]))):
                    try:
                        cd.find(f)
                    except SystemExit:
                        print(f"    {f}: not in the client_data")
        print(f"{file}: {len(added)} templets")
        if added:
            body = text.rstrip("\r\n") + nl + nl + nl.join(added) + nl
            writes.append((path, (b"\xef\xbb\xbf" if raw.startswith(b"\xef\xbb\xbf") else b"")
                           + body.encode("utf-8", "surrogateescape")))

    defs_path = cd.find("StringID_def.lua")
    defs_raw, defs_text, dnl = read(defs_path)
    new_defs = sorted(used - set(map(int, re.findall(r"^STR_ID_(\d+) = ", defs_text, re.M))))
    ess, theirs = read_ess(args.ess), read_ess(args.strings)
    new_strings = [n for n in sorted(used) if ess.get(n, f"STR_ID_{n}") in ("", f"STR_ID_{n}")]
    unknown = [n for n in new_strings if theirs.get(n, f"STR_ID_{n}") in ("", f"STR_ID_{n}")]
    print(f"{len(new_defs)} STR_ID definitions, {len(new_strings) - len(unknown)} strings")
    for n in unknown:
        print(f"  STR_ID_{n}: no text in --strings")
    if args.dry_run:
        return
    for path, data in writes:
        path.write_bytes(data)
    if new_defs:
        body = defs_text.rstrip("\r\n") + dnl + dnl.join(f"STR_ID_{n} = {n}" for n in new_defs) + dnl
        defs_path.write_bytes((b"\xef\xbb\xbf" if defs_raw.startswith(b"\xef\xbb\xbf") else b"") + body.encode())
    if len(new_strings) > len(unknown):
        table = dict(ess)
        for n in new_strings:
            if n not in unknown:
                table[n] = theirs[n]
        Path(args.ess).write_bytes(b"\xff\xfe" + b"".join(ess_line(i, table[i]) for i in sorted(table)))
    print("wrote", ", ".join(str(p) for p, _ in writes + ([(defs_path, 0)] if new_defs else [])
                             + ([(args.ess, 0)] if len(new_strings) > len(unknown) else [])))


if __name__ == "__main__":
    main()
