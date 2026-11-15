#!/usr/bin/env python3
"""Brings skills and skill tree entries from other clients' NewSkillTempletVer2.lua into ours.

Usage:
  merge_skills.py <our NewSkillTempletVer2.lua> --skills 9000-9014,9200 --classes UC_ADD_NASOD_RULER,...
                  --source <file> [--source <file> ...] --text <file> [--tree <file>] [--dry-run]

- Skill templets (AddSkillTemplet_LUA) with the --skills IDs are taken from the first --source that has them.
- Their text (m_wstrName, m_wstrMainDesc, m_EffectiveDesc) is taken from --text by ID where the source's is empty
  or Korean.
- Skill tree entries (AddSkillTreeTemplet_LUA) of the --classes are taken from the first of --tree (if given) and
  the sources that has entries for that class.
- Entries already in our file with the same skill ID or class are replaced, so a second run changes nothing.
The file is rewritten in place (UTF-8 with BOM, LF), keeping every other line as it was.
See docs/project/add-character.md and docs/tools/client-data.md (Content changes).
"""
import argparse
import re
import sys
from pathlib import Path

TEMPLET = "g_pSkillTree:AddSkillTemplet_LUA("
TREE = "g_pSkillTree:AddSkillTreeTemplet_LUA("
HANGUL = re.compile("[ᄀ-ᇿ㄰-㆏가-힣]")
TEXT_FIELDS = ("m_wstrName", "m_wstrMainDesc", "m_EffectiveDesc")


def read(path):
    return Path(path).read_text(encoding="utf-8-sig").replace("\r\n", "\n")


def blocks(text):
    """Splits a script into (kind, text) chunks: top-level calls 'templet'/'tree' and everything else 'other'."""
    out = []
    lines = text.split("\n")
    i = 0
    while i < len(lines):
        line = lines[i]
        kind = "templet" if line.startswith(TEMPLET) else "tree" if line.startswith(TREE) else None
        if kind is None:
            out.append(("other", line))
            i += 1
            continue
        j = i
        while not lines[j].startswith("})"):
            j += 1
        out.append((kind, "\n".join(lines[i:j + 1])))
        i = j + 1
    return out


def skill_id(block):
    m = re.search(r"^\tm_eID = (\d+),?$", block, re.M)
    return int(m.group(1)) if m else None


def tree_class(block):
    m = re.search(r'^\tm_eUnitClass = UNIT_CLASS(?:\.(\w+)|\["(\w+)"\]),?$', block, re.M)
    return (m.group(1) or m.group(2)) if m else None


def tree_normalize(block):
    """Our tree entries: UNIT_CLASS.UC_X and a comma after every field."""
    block = re.sub(r'UNIT_CLASS\["(\w+)"\]', r"UNIT_CLASS.\1", block)
    return re.sub(r"^(\t\w+ = [^\n]*[^,\n])$", r"\1,", block, flags=re.M)


def field(block, name):
    """The value text of a top-level field (a string, or a { ... } list), with its span."""
    m = re.search(r"^\t%s = " % name, block, re.M)
    if not m:
        return None
    start = m.end()
    if block[start] == "{":
        end = block.index("\n\t}", start) + 3
    else:
        end = block.index("\n", start)
    value = block[start:end]
    if value.endswith(","):
        value = value[:-1]
    return start, start + len(value), value


def needs_text(value):
    return value in ('""', "{}", "{\n\t}") or bool(HANGUL.search(value))


def with_text(block, text_block):
    for name in TEXT_FIELDS:
        ours = field(block, name)
        theirs = field(text_block, name) if text_block else None
        if ours and theirs and needs_text(ours[2]) and not needs_text(theirs[2]):
            block = block[:ours[0]] + theirs[2] + block[ours[1]:]
    return block


def id_list(spec):
    ids = []
    for part in spec.split(","):
        a, _, b = part.partition("-")
        ids += range(int(a), int(b or a) + 1)
    return ids


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("target")
    ap.add_argument("--skills", required=True)
    ap.add_argument("--classes", default="")
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--text")
    ap.add_argument("--tree", action="append", default=[])
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    ids = id_list(args.skills)
    classes = [c for c in args.classes.split(",") if c]
    sources = [(p, blocks(read(p))) for p in args.source]
    trees = [(p, blocks(read(p))) for p in args.tree] + sources
    text = {skill_id(b): b for k, b in blocks(read(args.text)) if k == "templet"} if args.text else {}

    new_templets = []
    for sid in ids:
        for path, bl in sources:
            found = next((b for k, b in bl if k == "templet" and skill_id(b) == sid), None)
            if found:
                block = with_text(found, text.get(sid))
                korean = [n for n in TEXT_FIELDS if field(block, n) and HANGUL.search(field(block, n)[2])]
                print(f"skill {sid}: {Path(path).parent.name}/{Path(path).name}" + (f" (still Korean: {', '.join(korean)})" if korean else ""))
                new_templets.append(block)
                break
        else:
            sys.exit(f"error: skill {sid} is in no source")

    new_tree = []
    for cls in classes:
        for path, bl in trees:
            found = [tree_normalize(b) for k, b in bl if k == "tree" and tree_class(b) == cls]
            if found:
                missing = sorted({int(re.search(r"m_iSkillID = (\d+)", b).group(1)) for b in found} - set(ids))
                print(f"tree {cls}: {len(found)} entries from {Path(path).parent.name}/{Path(path).name}")
                if missing:
                    print(f"  note: skills not in --skills (must already be ours): {missing}")
                new_tree += found
                break
        else:
            sys.exit(f"error: class {cls} has no tree entries in any source")

    ours = blocks(read(args.target))
    known = {skill_id(b) for k, b in ours if k == "templet"}
    for sid in {int(re.search(r"m_iSkillID = (\d+)", b).group(1)) for b in new_tree} - set(ids) - known:
        sys.exit(f"error: the tree uses skill {sid}, which is neither ours nor in --skills")
    kept = [(k, b) for k, b in ours if not (k == "templet" and skill_id(b) in ids)
            and not (k == "tree" and tree_class(b) in classes)]
    last_templet = max(i for i, (k, _) in enumerate(kept) if k == "templet")
    last_tree = max(i for i, (k, _) in enumerate(kept) if k == "tree")
    out = []
    for i, (k, b) in enumerate(kept):
        out.append(b)
        if i == last_templet:
            out += new_templets
        if i == last_tree:
            out += new_tree
    result = "\n".join(out)
    if result == "\n".join(b for _, b in ours):
        print("no change")
    elif not args.dry_run:
        Path(args.target).write_text(result, encoding="utf-8-sig", newline="\n")
        print(f"wrote {args.target}: {len(new_templets)} skills, {len(new_tree)} tree entries")


if __name__ == "__main__":
    main()
