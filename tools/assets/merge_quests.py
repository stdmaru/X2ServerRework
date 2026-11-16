#!/usr/bin/env python3
"""Brings quests, with their sub-quests, scenes and strings, from other clients' scripts into ours.

Usage:
  merge_quests.py <client_data> --quests 35310-35313,35470 --source <dir> [--source <dir> ...]
                  [--fallback <dir> ...] --text <dir> --ess <our general.ess> --strings <their General.ess> [--dry-run]

A <dir> holds a client's FieldQuest.lua, FieldSubQuest.lua, EventScene.lua and EventSceneObject.lua (data036).
- Each quest is taken from the first --source, then the first --fallback, that has it; the sub-quests it names
  (m_vecSubQuest) and the scenes (m_wstrStartScene, ...) that ours lacks come the same way, then from --text, and the
  scene objects those scenes name that ours lacks too.
- Text (titles, quest text, sub-quest descriptions) of a --source is kept unless it is Korean; a --fallback's (e.g. an
  English client) and Korean text are replaced by --text's, by ID, when that one has it.
- Unit classes our Enum.lua lacks are dropped from class lists (a quest that changes to one is an error).
- STR_ID_<n> the scenes use are defined in StringID_def.lua, and their strings are copied from --strings into
  --ess where ours lacks them (an ID our table has with its own name as text counts as missing).
- References to items, dungeons and NPCs that the client_data lacks are reported.
Every run can be repeated (a second run changes nothing). See docs/tools/client-data.md (Bringing data from other
clients).
"""
import argparse
import re
import struct
import sys
from pathlib import Path

from client_data import ClientData

HANGUL = re.compile("[ᄀ-ᇿ㄰-㆏가-힣]")
QUEST, SUB = "AddQuestTemplet_LUA", "AddSubQuestTemplet_LUA"
SCENE, OBJECT = "AddEventScene_LUA", "AddEventSceneObject_LUA"
QUEST_TEXT = ("m_wstrTitle", "m_wstrMainText", "m_wstrThanksText", "m_wstrDissClearText")
SUB_TEXT = ("m_wstrDescription",)


def read(path):
    return Path(path).read_text(encoding="utf-8-sig", errors="replace").replace("\r\n", "\n")


def blocks(text, call, key):
    """{key value: block} of the top-level calls g_p...:<call>({ ... }) of a script."""
    out = {}
    for m in re.finditer(r"^g_p\w+:%s\(\{\n.*?^\}\);?[ \t]*$" % call, text, re.M | re.S):
        k = re.search(r"^\t%s = (?:\"([^\"]*)\"|(\d+))" % key, m.group(0), re.M)
        if k:
            out[k.group(1) if k.group(1) is not None else int(k.group(2))] = m.group(0)
    return out


def string_field(block, name):
    m = re.search(r'^(\t%s = )("(?:[^"\\]|\\.)*")' % name, block, re.M)
    return m


def with_text(block, text_block, fields, replace_all):
    for name in fields:
        ours = string_field(block, name)
        theirs = string_field(text_block, name) if text_block else None
        if not ours or not theirs or HANGUL.search(theirs.group(2)) or theirs.group(2) == '""':
            continue
        if replace_all or HANGUL.search(ours.group(2)) or ours.group(2) == '""':
            block = block[:ours.start(2)] + theirs.group(2) + block[ours.end(2):]
    return block


def id_list(spec):
    ids = []
    for part in spec.split(","):
        a, _, b = part.partition("-")
        ids += range(int(a), int(b or a) + 1)
    return ids


def read_ess(path):
    b = Path(path).read_bytes()
    units = struct.unpack("<%dH" % (len(b) // 2), b[:len(b) // 2 * 2])
    if units and units[0] == 0xFEFF:  # the BOM is not XORed
        units = units[1:]
    text = "".join(chr(u if u in (10, 13) else u ^ 16) for u in units)
    table = {}
    for line in text.split("\r\n"):
        i, tab, v = line.partition("\t")
        if tab and i.isdigit():
            table[int(i)] = v
    return table


def ess_line(i, s):
    """One General.ess line: UTF-16LE, every code unit but the CRLF XORed with 16."""
    return b"".join(struct.pack("<H", ord(c) ^ 16) for c in f"{i}\t{s}") + b"\r\x00\n\x00"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--quests", required=True)
    ap.add_argument("--source", action="append", default=[])
    ap.add_argument("--fallback", action="append", default=[])
    ap.add_argument("--text", required=True)
    ap.add_argument("--ess", required=True)
    ap.add_argument("--strings", required=True)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cd = ClientData(args.client_data)
    names = {"quest": "FieldQuest.lua", "sub": "FieldSubQuest.lua", "scene": "EventScene.lua",
             "object": "EventSceneObject.lua"}
    files = {k: cd.find(n) for k, n in names.items()}
    calls = {"quest": (QUEST, "m_iID"), "sub": (SUB, "m_iID"), "scene": (SCENE, "EVENT_SCENE_ID"),
             "object": (OBJECT, "SCENE_OBJECT_NAME")}

    def load(d):
        out = {}
        for k, n in names.items():
            p = Path(d) / n
            out[k] = blocks(read(p), *calls[k]) if p.is_file() else {}
        return out

    ours = {k: blocks(read(p), *calls[k]) for k, p in files.items()}
    sources = [(d, load(d), False) for d in args.source] + [(d, load(d), True) for d in args.fallback]
    text = load(args.text)
    enum = read(cd.find("Enum.lua"))
    classes = set(re.findall(r"^\s*(UC_\w+)\s*=", enum, re.M))

    def take(kind, key, extra=()):
        for d, s, fallback in list(sources) + list(extra):
            if key in s[kind]:
                return s[kind][key], fallback, d
        return None, None, None

    added = {k: [] for k in files}
    log = []
    for qid in id_list(args.quests):
        if qid in ours["quest"]:
            continue
        b, fallback, d = take("quest", qid)
        if b is None:
            sys.exit(f"error: quest {qid} is in no source")
        b = with_text(b, text["quest"].get(qid), QUEST_TEXT, fallback)
        for c in re.findall(r"UNIT_CLASS\.(\w+)", b):
            if c not in classes:
                if re.search(r"m_eChangeUnitClass = UNIT_CLASS\.%s\b" % c, b):
                    sys.exit(f"error: quest {qid} changes the class to {c}, which Enum.lua lacks")
                b = re.sub(r"\n\t*UNIT_CLASS\.%s,?(?=\n)" % c, "", b)
                b = re.sub(r"UNIT_CLASS\.%s,? ?" % c, "", b)
        added["quest"].append(b)
        log.append(f"quest {qid}: {Path(d).name}{' (text from --text)' if fallback else ''}")
        m = re.search(r"m_vecSubQuest = \{(.*?)\}", b, re.S)
        for sid in map(int, re.findall(r"\d+", m.group(1) if m else "")):
            if sid in ours["sub"] or any(re.search(r"^\tm_iID = %d,?$" % sid, x, re.M) for x in added["sub"]):
                continue
            sb, sfallback, sd = take("sub", sid, [(args.text, text, False)])
            if sb is None:
                sys.exit(f"error: sub-quest {sid} of quest {qid} is in no source")
            added["sub"].append(with_text(sb, text["sub"].get(sid), SUB_TEXT, sfallback))
        for scene in re.findall(r'Scene = "([^"]+)"', b):
            if scene in ours["scene"] or any(f'EVENT_SCENE_ID = "{scene}"' in x for x in added["scene"]):
                continue
            sb, _, _ = take("scene", scene, [(args.text, text, False)])
            if sb is None:
                print(f"warning: scene {scene} of quest {qid} is in no source")
                continue
            added["scene"].append(sb)

    for sb in list(added["scene"]):
        for obj in set(re.findall(r'SCENE_OBJECT_NAME = "([^"]+)"', sb)):
            # PLAYER is the player's own character (CX2EventScene), not a scene object
            if obj == "PLAYER" or obj in ours["object"] or any(f'SCENE_OBJECT_NAME = "{obj}"' in x for x in added["object"]):
                continue
            ob, _, _ = take("object", obj, [(args.text, text, False)])
            if ob is None:
                print(f"warning: scene object {obj} is in no source")
                continue
            added["object"].append(ob)

    # Strings the new scenes use.
    def_path = cd.find("StringID_def.lua")
    defs = read(def_path)
    defined = set(map(int, re.findall(r"^STR_ID_(\d+) = ", defs, re.M)))
    used = sorted({int(n) for b in added["scene"] for n in re.findall(r"\bSTR_ID_(\d+)\b", b)})
    ess, theirs = read_ess(args.ess), read_ess(args.strings)
    new_defs = [n for n in used if n not in defined]
    new_strings = [n for n in used if ess.get(n, f"STR_ID_{n}") in ("", f"STR_ID_{n}")]
    lost = [n for n in new_strings if n not in theirs]
    if lost:
        print(f"warning: {len(lost)} strings in neither table: {lost[:20]}")
    new_strings = [n for n in new_strings if n in theirs]

    # References to report.
    item_ids = set(map(int, re.findall(r"m_ItemID = (\d+)", read(cd.find("Item.lua")))))
    dungeon_enum = read(cd.find("DungeonEnum.lua"))
    dungeons = set(map(int, re.findall(r"^\s*DI_\w+\s*=\s*(\d+)", dungeon_enum, re.M)))
    dungeon_names = set(re.findall(r"^\s*(DI_\w+)\s*=", dungeon_enum, re.M))
    npc_names = set(re.findall(r"^\s*(NUI_\w+)\s*=", enum, re.M))
    blob = "\n".join(added["quest"] + added["sub"])
    items = {int(i) for i in re.findall(r"m_i(?:Collection)?ItemID = (\d+)", blob)} - {0}
    for b in added["quest"]:
        for f in ("HIDE_QUEST_ITEM",):
            m = re.search(r"%s = \{(.*?)\}" % f, b, re.S)
            items |= set(map(int, re.findall(r"\d+", m.group(1)))) if m else set()
    missing_items = sorted(items - item_ids)
    dids = {int(x) for m in re.findall(r"m_eDungeonID = \{([^}]*)\}", blob) for x in re.findall(r"\d+", m)}
    dids |= {int(x) for x in re.findall(r"m_eDungeonID = (\d+)", blob)}
    missing_dungeons = sorted(dids - {0} - dungeons)
    missing_names = sorted({n for n in re.findall(r"DUNGEON_ID\.(\w+)", blob) if n not in dungeon_names} |
                           {n for n in re.findall(r"NPC_UNIT_ID\.(\w+)", blob) if n not in npc_names})

    for line in log:
        print(line)
    print(f"{len(added['quest'])} quests, {len(added['sub'])} sub-quests, {len(added['scene'])} scenes, "
          f"{len(added['object'])} scene objects, {len(new_defs)} STR_ID definitions, {len(new_strings)} strings")
    if missing_items:
        print(f"note: items the client_data lacks: {missing_items}")
    if missing_dungeons or missing_names:
        print(f"note: dungeons or names Enum.lua lacks: {missing_dungeons + missing_names}")
    if args.dry_run or not any(added.values()) and not new_defs and not new_strings:
        if not any(added.values()):
            print("no change")
        return

    for k, path in files.items():
        if added[k]:
            raw = path.read_bytes()
            crlf = b"\r\n" in raw[:4096]
            body = raw.decode("utf-8-sig").rstrip("\r\n")
            new = "\n".join(added[k])
            body += ("\r\n" if crlf else "\n") + (new.replace("\n", "\r\n") if crlf else new) + ("\r\n" if crlf else "\n")
            path.write_bytes(b"\xef\xbb\xbf" + body.encode("utf-8"))
            print(f"wrote {path}")
    if new_defs:
        raw = def_path.read_bytes()
        crlf = b"\r\n" in raw[:4096]
        nl = "\r\n" if crlf else "\n"
        body = raw.decode("utf-8-sig").rstrip("\r\n") + nl + nl.join(f"STR_ID_{n} = {n}" for n in new_defs) + nl
        def_path.write_bytes(b"\xef\xbb\xbf" + body.encode("utf-8"))
        print(f"wrote {def_path}")
    if new_strings:
        table = dict(ess)
        table.update({n: theirs[n] for n in new_strings})
        out = b"\xff\xfe" + b"".join(ess_line(i, table[i]) for i in sorted(table))
        Path(args.ess).write_bytes(out)
        print(f"wrote {args.ess}")


if __name__ == "__main__":
    main()
