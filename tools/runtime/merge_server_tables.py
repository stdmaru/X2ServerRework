#!/usr/bin/env python3
"""Brings drops, cubes and crafting entries our ServerResource (and client) lack from another server's scripts.

Usage:
  merge_server_tables.py --ours runtime/ServerResource --source <V5 ServerResource> \\
      --items <client_data>/items/Item.lua --enum <client_data>/data/Enum.lua \\
      [--unit-types UT_ADD,UT_ELESIS,UT_ARA] [--cubes 90000130,90000131] \\
      [--client-manufacture <client_data>/items/ManufactureItem.lua --manufacture-source <ManufactureItem.lua> ...] \\
      [--dry-run]

Ours is kept as it is; entries are added by ID, only where ours has none (see docs/project/add-character.md):
- RandomItemTable.lua (cubes): in a cube both files have, the class entries (m_cUnitClass -> m_iItemGroupID) ours
  lacks for a class our Enum.lua has (UC_NONE never: the server matches the opener's class, so a cube without the
  class cannot be opened by it); the whole templets of --cubes; the item groups those name that ours lacks.
- DropTable.lua (monster drops): in a monster drop both files have (same function, dungeon or field and monster), the
  DropItemList entries ours lacks whose item is of one of --unit-types.
- DropData.lua: AddNotifyDropItemInfo of items of --unit-types.
- ManufactureResultTable.lua (crafting results): the recipes (AddManufactureResultTemplet) whose result group gives
  an item of --unit-types, and those groups. With --client-manufacture, the client's ManufactureItem.lua gets the same
  recipes (AddManufactureTemplet: materials and cost), their AddManufactureResultGroup lines and the
  AddManufactureVillageData that shows them, each from the first --manufacture-source that has it.
Every item an added entry names must be in --items; an entry naming one that is not is left out and listed.
"""
import argparse
import collections
import re
import sys
from pathlib import Path

ENC = dict(encoding="utf-8", errors="surrogateescape")


def read(path):
    raw = Path(path).read_bytes()
    bom = raw.startswith(b"\xef\xbb\xbf")
    text = raw[3 if bom else 0:].decode(**ENC)
    return text, bom, ("\r\n" if "\r\n" in text else "\n")


def write(path, text, bom):
    Path(path).write_bytes((b"\xef\xbb\xbf" if bom else b"") + text.encode(**ENC))


def append(text, nl, lines):
    return text.rstrip("\r\n") + nl + nl + nl.join(lines) + nl


def code(line):
    return line.split("--", 1)[0]


def load_items(path):
    text = Path(path).read_text(**ENC)
    items = {}
    for m in re.finditer(r"m_ItemID\s*=\s*(\d+),(.*?)(?=m_ItemID\s*=|\Z)", text, re.S):
        ut = re.search(r'm_UnitType\s*=\s*UNIT_TYPE\["(\w+)"\]', m[2])
        items[int(m[1])] = ut[1] if ut else "UT_NONE"
    return items


# ---------------------------------------------------------------------------------------------------------- cubes

TEMPLET = re.compile(r"^g_pRandomItemManager:AddRandomItemTemplet\s*\{.*?^\}", re.S | re.M)
SUB = re.compile(r'\{\s*m_cUnitClass\s*=\s*UNIT_CLASS\["(\w+)"\]\s*,\s*m_iItemGroupID\s*=\s*(\d+)\s*,\s*\}')
RGROUP = re.compile(r"^g_pRandomItemManager:AddRandomItemGroup\(\s*(\d+)\s*,\s*(\d+)\s*,[^)]*\).*$", re.M)


def cube_templets(text):
    out = {}
    for m in TEMPLET.finditer(text):
        iid = int(re.search(r"m_ItemID\s*=\s*(\d+)", m[0])[1])
        out[iid] = (m, [(c, int(g)) for c, g in SUB.findall(m[0])])
    return out


def item_groups(regex, text):
    groups = collections.defaultdict(list)
    for m in regex.finditer(text):
        groups[int(m[1])].append((int(m[2]), code(m[0]).rstrip()))
    return groups


def merge_cubes(ours_dir, src_dir, items, classes, cubes, report, dry):
    path = ours_dir / "RandomItemTable.lua"
    text, bom, nl = read(path)
    src, _, _ = read(src_dir / "RandomItemTable.lua")
    ot, st = cube_templets(text), cube_templets(src)
    og, sg = item_groups(RGROUP, text), item_groups(RGROUP, src)

    def groups_ok(gids, what):
        for g in gids:
            if g in og:
                continue
            if g not in sg:
                report.append(f"cubes: {what} names group {g}, in neither file; left out")
                return False
            bad = [i for i, _ in sg[g] if i not in items]
            if bad:
                report.append(f"cubes: {what}: group {g} names items our client lacks {bad[:5]}; left out")
                return False
        return True

    inserts, new_templets, new_groups, added = [], [], [], collections.Counter()
    for iid, (m, subs) in st.items():
        if iid not in ot:
            continue
        have = {c for c, _ in ot[iid][1]}
        extra = [(c, g) for c, g in subs if c != "UC_NONE" and c in classes and c not in have]
        extra = [(c, g) for c, g in extra if groups_ok([g], f"cube {iid} {c}")]
        if extra:
            inserts.append((ot[iid][0], extra))
            new_groups += [g for _, g in extra]
            added.update(c for c, _ in extra)
    for iid in cubes:
        if iid in ot or iid not in st:
            report.append(f"cubes: --cubes {iid} is {'already ours' if iid in ot else 'not in the source'}")
            continue
        if iid not in items:
            report.append(f"cubes: --cubes {iid} is not in our client; left out")
            continue
        subs = [(c, g) for c, g in st[iid][1] if c == "UC_NONE" or c in classes]
        if not groups_ok([g for _, g in subs], f"cube {iid}"):
            continue
        body = st[iid][0][0]
        for c, g in st[iid][1]:
            if (c, g) not in subs:  # a class our Enum.lua lacks
                body = re.sub(r'\s*\{\s*m_cUnitClass\s*=\s*UNIT_CLASS\["%s"\]\s*,\s*m_iItemGroupID\s*=\s*%d\s*,\s*\}\s*,'
                              % (c, g), "", body)
        new_templets.append(body.replace("\r\n", "\n").replace("\n", nl))
        new_groups += [g for _, g in subs]

    # insert class entries before each templet's closing brace (from the end, so offsets stay valid)
    for m, extra in sorted(inserts, key=lambda x: -x[0].start()):
        block = m[0]
        entries = "".join(f"{nl}       {nl}       {{{nl}              m_cUnitClass = UNIT_CLASS[\"{c}\"],{nl}"
                          f"              m_iItemGroupID = {g},{nl}       }}," for c, g in extra)
        cut = block.rfind("},") + 2  # after the last class entry
        block = block[:cut] + entries + block[cut:]
        text = text[:m.start()] + block + text[m.end():]
    lines = []
    if new_templets:
        lines += ["-- Cubes from V5 (merge_server_tables.py, 2026-11-15)"] + new_templets + [""]
    groups = sorted({g for g in new_groups if g not in og})
    if groups:
        lines.append("-- Their item groups and those of the class entries added to our cubes")
        for g in groups:
            lines += [line for _, line in sg[g]]
    if lines:
        text = append(text, nl, lines)
    print(f"RandomItemTable: {sum(added.values())} class entries in {len(inserts)} cubes ({dict(added)}), "
          f"{len(new_templets)} cubes, {len(groups)} groups ({sum(len(sg[g]) for g in groups)} rows)")
    if not dry:
        write(path, text, bom)


# ---------------------------------------------------------------------------------------------------------- drops

DROPINFO = re.compile(r"^DropTable:(Add\w+DropInfo)\s*\{.*?^\}", re.S | re.M)
DROPLIST = re.compile(r"(DropItemList\s*=\s*\{)(.*?)(\}\s*,)[ \t\r]*$", re.M)  # one line: { { id, rate },... },
PAIR = re.compile(r"\{\s*(\d+)\s*,\s*([\d.]+)\s*\}")


def drop_infos(text):
    out = {}
    for m in DROPINFO.finditer(text):
        key = (m[1],) + tuple(int(x) for x in re.findall(r"(?:DungeonID|BattleFieldID|MonsterID|NpcID)\s*=\s*(\d+)",
                                                         m[0]))
        lst = DROPLIST.search(m[0])
        out[key] = (m, PAIR.findall(lst[2]) if lst else None)
    return out


def merge_drops(ours_dir, src_dir, items, unit_types, report, dry):
    path = ours_dir / "DropTable.lua"
    text, bom, nl = read(path)
    src, _, _ = read(src_dir / "DropTable.lua")
    oi, si = drop_infos(text), drop_infos(src)
    edits, added = [], collections.Counter()
    for key, (_, pairs) in si.items():
        if key not in oi or not pairs or oi[key][1] is None:
            if key not in oi and pairs and any(items.get(int(i)) in unit_types for i, _ in pairs):
                report.append(f"drops: {key} (with items of {sorted(unit_types)}) is not ours; left out")
            continue
        have = {int(i) for i, _ in oi[key][1]}
        extra = [(i, r) for i, r in pairs if int(i) not in have and items.get(int(i)) in unit_types]
        if extra:
            edits.append((oi[key][0], extra))
            added.update(items[int(i)] for i, _ in extra)
    for m, extra in sorted(edits, key=lambda x: -x[0].start()):
        block = m[0]
        lst = DROPLIST.search(block)
        body = lst[2].rstrip()
        if body and not body.endswith(","):
            body += ","
        body += "".join(f"{{ {i}, {r} }}," for i, r in extra)
        block = block[:lst.start(2)] + body + block[lst.end(2):]
        text = text[:m.start()] + block + text[m.end():]
    print(f"DropTable: {sum(added.values())} items in {len(edits)} monster drops ({dict(added)})")
    if not dry:
        write(path, text, bom)

    path = ours_dir / "DropData.lua"
    text, bom, nl = read(path)
    src, _, _ = read(src_dir / "DropData.lua")
    notify = re.compile(r"^DropTable:AddNotifyDropItemInfo\(\s*(\d+)\s*\)", re.M)
    have = {int(i) for i in notify.findall(text)}
    new = [int(i) for i in notify.findall(src) if int(i) not in have and items.get(int(i)) in unit_types]
    if new:
        text = append(text, nl, ["-- From V5 (merge_server_tables.py, 2026-11-15)"]
                      + [f"DropTable:AddNotifyDropItemInfo( {i} )" for i in new])
    print(f"DropData: {len(new)} notified drops")
    if not dry:
        write(path, text, bom)


# ---------------------------------------------------------------------------------------------------- manufacture

MRESULT = re.compile(r"^g_pManufactureItemManager:AddManufactureResultTemplet\s*\{.*?^\}", re.S | re.M)
MGROUP = re.compile(r"^g_pManufactureItemManager:AddManufactureResultGroupWithRate\(\s*(\d+)\s*,\s*(\d+)\s*,[^)]*\).*$",
                    re.M)
CTEMPLET = re.compile(r"^g_pManufactureItemManager:AddManufactureTemplet\(\{.*?^\}\)", re.S | re.M)
CGROUP = re.compile(r"^g_pManufactureItemManager:AddManufactureResultGroup\(\s*(\d+)\s*,\s*(\d+)\s*\).*$", re.M)
CVILLAGE = re.compile(r"^g_pManufactureItemManager:AddManufactureVillageData\(\s*(\d+)\s*,\s*(\d+)\s*,.*?\}\)",
                      re.S | re.M)


def manufacture_ids(regex, text):
    out = {}
    for m in regex.finditer(text):
        out[int(re.search(r"m_ManufactureID\s*=\s*(\d+)", m[0])[1])] = (
            int(re.search(r"m_ResultGroupID\s*=\s*(\d+)", m[0])[1]), m[0])
    return out


def merge_manufacture(ours_dir, src_dir, items, unit_types, client, client_sources, report, dry):
    path = ours_dir / "ManufactureResultTable.lua"
    text, bom, nl = read(path)
    src, _, _ = read(src_dir / "ManufactureResultTable.lua")
    ot, st = manufacture_ids(MRESULT, text), manufacture_ids(MRESULT, src)
    og, sg = item_groups(MGROUP, text), item_groups(MGROUP, src)
    picked = []
    for mid, (gid, _) in st.items():
        if mid in ot or not any(items.get(i) in unit_types for i, _ in sg.get(gid, [])):
            continue
        bad = [i for i, _ in sg[gid] if i not in items]
        if bad:
            report.append(f"manufacture: recipe {mid} gives items our client lacks {bad[:5]}; left out")
            continue
        picked.append(mid)
    lines = []
    if picked:
        lines.append("-- Recipes from V5 (merge_server_tables.py, 2026-11-15)")
        lines += [st[mid][1].replace("\r\n", "\n").replace("\n", nl) for mid in picked]
        groups = sorted({st[mid][0] for mid in picked if st[mid][0] not in og})
        for g in groups:
            lines += [line for _, line in sg[g]]
        text = append(text, nl, lines)
    print(f"ManufactureResultTable: {len(picked)} recipes")
    if not dry:
        write(path, text, bom)
    if not client or not picked:
        return

    text, bom, nl = read(client)
    ct = manufacture_ids(CTEMPLET, text)
    cg = item_groups(CGROUP, text)
    cv = {(int(a), int(b)) for a, b in CVILLAGE.findall(text)}
    sources = [(p, read(p)[0]) for p in client_sources]
    parsed = [(p, manufacture_ids(CTEMPLET, s), item_groups(CGROUP, s),
               [(int(m[1]), int(m[2]), m[0]) for m in CVILLAGE.finditer(s)]) for p, s in sources]
    templets, groups, villages = [], [], []
    for mid in picked:
        if mid in ct:
            continue
        for p, t, g, v in parsed:
            if mid in t:
                body = t[mid][1]
                mats = [int(x) for x in re.findall(r"m_MaterialID\s*=\s*(\d+)", body)]
                bad = [x for x in mats if x not in items]
                if bad:
                    report.append(f"manufacture: client recipe {mid} ({Path(p).parent.name}) needs items our "
                                  f"client lacks {bad[:5]}")
                templets.append(body.replace("\r\n", "\n").replace("\n", nl))
                gid = t[mid][0]
                if gid not in cg:
                    groups += [line for _, line in g.get(gid, [])]
                break
        else:
            report.append(f"manufacture: client recipe {mid} is in no --manufacture-source")
        for p, t, g, v in parsed:
            rows = [body for a, b, body in v if a == mid and (a, b) not in cv]
            if rows:
                villages += [r.replace("\r\n", "\n").replace("\n", nl) for r in rows]
                break
        else:
            if not any(a == mid for a, _ in cv):
                report.append(f"manufacture: no village shows recipe {mid} in any source")
    lines = ["-- Recipes from merge_server_tables.py (2026-11-15)"] + templets + groups + villages
    text = append(text, nl, lines)
    print(f"client ManufactureItem: {len(templets)} recipes, {len(groups)} result rows, {len(villages)} village rows")
    if not dry:
        write(client, text, bom)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--ours", type=Path, required=True)
    ap.add_argument("--source", type=Path, required=True)
    ap.add_argument("--items", type=Path, required=True)
    ap.add_argument("--enum", type=Path, required=True)
    ap.add_argument("--unit-types", default="UT_ADD")
    ap.add_argument("--cubes", default="")
    ap.add_argument("--client-manufacture", type=Path)
    ap.add_argument("--manufacture-source", type=Path, action="append", default=[])
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()
    items = load_items(a.items)
    classes = set(re.findall(r"\b(UC_\w+)\s*=", a.enum.read_text(**ENC)))
    unit_types = set(a.unit_types.split(","))
    cubes = [int(x) for x in a.cubes.split(",") if x]
    report = []
    merge_cubes(a.ours, a.source, items, classes, cubes, report, a.dry_run)
    merge_drops(a.ours, a.source, items, unit_types, report, a.dry_run)
    merge_manufacture(a.ours, a.source, items, unit_types, a.client_manufacture, a.manufacture_source, report,
                      a.dry_run)
    for line in report:
        print("note:", line)


if __name__ == "__main__":
    sys.exit(main())
