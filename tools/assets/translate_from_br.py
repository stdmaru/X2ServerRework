#!/usr/bin/env python3
"""Translates the text of items, sets, skills and monsters that a client_data still has in English or Korean.

Usage:
  translate_from_br.py <client_data> --source-items <Item.lua> [--source-items ...]
                       [--source-sets <SetItem.lua> ...] [--source-skills <NewSkillTempletVer2.lua> ...]
                       [--items] [--sets] [--skills] [--monsters] [--dry-run] [--report <file>]

- Files are found by name in the client_data (tools/assets/client_data.py), never by path.
- Only text that is English or Korean (or Chinese) is replaced, by ID, with the first --source that has Portuguese text
  for that ID; text that is already Portuguese is kept, even where a source words it differently. The sources are
  BR 2017 first, then older Brazilian scripts (tools/assets/merge_items.py lists them).
- An item text with no source is then looked up among the texts this run translated: the same English text on another
  item (the "(Old)" copies of an item, shared descriptions) takes the same translation, and "(Old)" becomes "(Antigo)",
  as BR 2017 writes it.
- Skills: the name, the description and the per-level text, taken together by skill ID (BR 2017 is after the skill
  revamp, so its per-level numbers may differ from ours; the text is what is wanted, as for the skills that
  merge_skills.py adds).
- Monsters: names that are Korean placeholders ("not used") are translated by the table below.
- Line endings and the BOM of every file are kept. Running it again changes nothing.
See docs/tools/client-data.md.
"""
import argparse
import collections
import math
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import merge_skills as ms  # noqa: E402
from client_data import ClientData  # noqa: E402

HANGUL = re.compile("[ᄀ-ᇿ㄰-㆏가-힯]")
CJK = re.compile("[぀-ヿ㐀-䶿一-鿿]")
WORD = re.compile(r"[^\W\d_]+")
PT_ACCENT = re.compile("[áàâãéêíóôõúçÁÀÂÃÉÊÍÓÔÕÚÇ]")
PT_WORDS = re.compile(r"\b(de|do|da|dos|das|para|com|em|um|uma|que|ao|os|as|seu|sua|ou|não|por|no|na)\b")
EN_WORDS = set("the of and to a in is for with your you can be will this that are on by or an as it from at when after "
               "during use used item has have not all each other more than if only one two three increase increases "
               "decrease decreases damage attack skill level time per second seconds chance rate effect target enemy "
               "enemies player players monster monsters buff cannot may out into its their them while up down get "
               "gets".split())
ITEM_RE = re.compile(r"g_pItemManager:AddItemTemplet\s*\{(.*?)\n\}", re.S)
SET_RE = re.compile(r"g_pCX2SetItemManager:AddSetItemData_LUA\s*\{(.*?)\n\}", re.S)
STR_FIELD = r'%s\s*=\s*"((?:[^"\\]|\\.)*)"'
ITEM_TEXT = ("m_Name", "m_Description", "m_DescriptionInShop")
# placeholders the client shows as they are and that mean nothing: keep (BR 2017 has a text in some of them)
KEEP = {"Test Test Test"}
MONSTER_NAMES = {"사용하지 않음": "Não utilizado"}


def read_raw(path):
    return Path(path).read_bytes().decode("utf-8")


def write_raw(path, text):
    Path(path).write_bytes(text.encode("utf-8"))


def strict_pt(text):
    """A text that is plainly Portuguese (accents or common words), used to teach the detector."""
    return bool(text) and not HANGUL.search(text) and not CJK.search(text) and bool(PT_ACCENT.search(text) or PT_WORDS.search(text))


def strict_en(text):
    if not text or HANGUL.search(text) or CJK.search(text) or PT_ACCENT.search(text) or PT_WORDS.search(text):
        return False
    return any(w.lower() in EN_WORDS for w in WORD.findall(text))


class Lang:
    """Tells English from Portuguese by the words each language alone was seen with: a text is English when more of its
    words occur only in the English training text than only in the Portuguese one (words both have, or neither, say
    nothing, so the size of the two corpora does not matter). Trained on the sources' Portuguese and our own English."""

    def __init__(self):
        self.pt = set()
        self.en = set()

    def train(self, pt_texts, en_texts):
        for t in pt_texts:
            self.pt.update(w.lower() for w in WORD.findall(t))
        for t in en_texts:
            self.en.update(w.lower() for w in WORD.findall(t))
        self.only_en = self.en - self.pt
        self.only_pt = self.pt - self.en

    def kind(self, text):
        """empty, ko, cjk, other (no letters), pt or en."""
        if text is None or not text.strip():
            return "empty"
        if HANGUL.search(text):
            return "ko"
        if CJK.search(text):
            return "cjk"
        if not WORD.search(text):
            return "other"
        if PT_ACCENT.search(text):
            return "pt"
        words = [x.lower() for x in WORD.findall(text)]
        en = sum(w in self.only_en for w in words)
        pt = sum(w in self.only_pt for w in words) + len(PT_WORDS.findall(text))
        return "en" if en > pt else "pt"

    def foreign(self, text):
        return self.kind(text) in ("en", "ko", "cjk")


def str_field(block, key):
    m = re.search(STR_FIELD % key, block)
    return m.group(1) if m else None


def parse_texts(path, block_re, id_key, keys):
    text = read_raw(path)
    out = {}
    for m in block_re.finditer(text):
        i = re.search(r"%s\s*=\s*(\d+)" % id_key, m.group(1))
        if i:
            out[int(i.group(1))] = {k: str_field(m.group(1), k) for k in keys}
    return out


def replace_field(block, key, value):
    m = re.search(r'(%s\s*=\s*")((?:[^"\\]|\\.)*)(")' % key, block)
    return block[:m.start(2)] + value + block[m.end(2):]


class Report:
    def __init__(self):
        self.lines = []
        self.counts = collections.Counter()

    def add(self, what, key, source):
        self.counts[(what, source)] += 1

    def left(self, what, line):
        self.counts[(what, "left")] += 1
        self.lines.append(f"{what}\t{line}")


def translate_items(args, cd, report, lang):
    path = cd.find("Item.lua")
    ours = parse_texts(path, ITEM_RE, "m_ItemID", ITEM_TEXT)
    sources = [(Path(p).parent.name, parse_texts(p, ITEM_RE, "m_ItemID", ITEM_TEXT)) for p in args.source_items]

    # pass 1: a source's Portuguese text by ID; collect English -> Portuguese pairs for pass 2
    chosen = {}
    pairs = collections.defaultdict(collections.Counter)
    for i, d in ours.items():
        for k, v in d.items():
            if not v or v in KEEP or not lang.foreign(v):
                continue
            for name, s in sources:
                t = s.get(i, {}).get(k)
                if t and lang.kind(t) == "pt" and t != v:
                    chosen[(i, k)] = (t, name)
                    pairs[(k, v)][t] += 1
                    break

    def by_text(k, v):
        for key in ((k, v),) + (((k, v[len("(Old)"):].lstrip()),) if v.startswith("(Old)") else ()):
            if key in pairs:
                t = pairs[key].most_common(1)[0][0]
                if key[1] != v:
                    t = "(Antigo) " + t if not t.startswith("(Antigo)") else t
                return t
        return None

    text = read_raw(path)
    out = []
    pos = 0
    for m in ITEM_RE.finditer(text):
        body = m.group(1)
        i = re.search(r"m_ItemID\s*=\s*(\d+)", body)
        if not i:
            continue
        i = int(i.group(1))
        new = body
        for k in ITEM_TEXT:
            v = ours[i][k]
            if not v or v in KEEP or not lang.foreign(v):
                continue
            hit = chosen.get((i, k))
            if hit:
                new = replace_field(new, k, hit[0])
                report.add("items." + k, i, hit[1])
                continue
            t = by_text(k, v)
            if t:
                new = replace_field(new, k, t)
                report.add("items." + k, i, "same text")
            else:
                report.left("items." + k, f"{i}\t{v[:90]}")
        if new != body:
            out.append(text[pos:m.start(1)])
            out.append(new)
            pos = m.end(1)
    out.append(text[pos:])
    if not args.dry_run:
        write_raw(path, "".join(out))


def translate_sets(args, cd, report, lang):
    path = cd.find("SetItem.lua")
    ours = parse_texts(path, SET_RE, "m_SetID", ("m_SetName",))
    sources = [(Path(p).parent.name, parse_texts(p, SET_RE, "m_SetID", ("m_SetName",))) for p in args.source_sets]
    text = read_raw(path)
    out = []
    pos = 0
    for m in SET_RE.finditer(text):
        body = m.group(1)
        i = re.search(r"m_SetID\s*=\s*(\d+)", body)
        v = ours.get(int(i.group(1)), {}).get("m_SetName") if i else None
        if not v or not lang.foreign(v):
            continue
        t = next((s[int(i.group(1))]["m_SetName"] for _, s in sources
                  if int(i.group(1)) in s and lang.kind(s[int(i.group(1))]["m_SetName"]) == "pt"), None)
        if t and t != v:
            out.append(text[pos:m.start(1)])
            out.append(replace_field(body, "m_SetName", t))
            pos = m.end(1)
            report.add("sets.m_SetName", i.group(1), "source")
        else:
            report.left("sets.m_SetName", f"{i.group(1)}\t{v[:90]}")
    out.append(text[pos:])
    if not args.dry_run:
        write_raw(path, "".join(out))


def skill_texts(block):
    return {f: (ms.field(block, f) or (0, 0, None))[2] for f in ms.TEXT_FIELDS}


def list_strings(value):
    return re.findall(r'"((?:[^"\\]|\\.)*)"', value or "")


def translate_skills(args, cd, report, lang):
    path = cd.find("NewSkillTempletVer2.lua")
    raw = read_raw(path)
    crlf = "\r\n" in raw
    text = raw.replace("\r\n", "\n")
    chunks = ms.blocks(text.lstrip("﻿"))
    sources = [(Path(p).parent.name, {ms.skill_id(b): b for k, b in ms.blocks(ms.read(p)) if k == "templet"})
               for p in args.source_skills]
    result = []
    for kind, block in chunks:
        if kind != "templet":
            result.append(block)
            continue
        sid = ms.skill_id(block)
        mine = skill_texts(block)
        for name, s in sources:
            src = s.get(sid)
            if not src:
                continue
            theirs = skill_texts(src)
            for f in ms.TEXT_FIELDS:
                if not mine[f] or not theirs[f] or mine[f] == theirs[f]:
                    continue
                if not lang.foreign(" ".join(list_strings(mine[f])) if f == "m_EffectiveDesc" else mine[f].strip('"')):
                    continue
                if lang.kind(" ".join(list_strings(theirs[f])) if f == "m_EffectiveDesc" else theirs[f].strip('"')) != "pt":
                    continue
                if f == "m_EffectiveDesc" and len(list_strings(mine[f])) != len(list_strings(theirs[f])):
                    continue  # another number of levels: not the same skill any more
                a = ms.field(block, f)
                block = block[:a[0]] + theirs[f] + block[a[1]:]
                mine[f] = theirs[f]
                report.add("skills." + f, sid, name)
            break
        for f in ms.TEXT_FIELDS:
            v = mine[f]
            if v and lang.foreign(" ".join(list_strings(v)) if f == "m_EffectiveDesc" else v.strip('"')):
                report.left("skills." + f, f"{sid}\t{v[:90]!r}")
        result.append(block)
    out = "\n".join(result)
    if text.startswith("﻿") and not out.startswith("﻿"):
        out = "﻿" + out
    if crlf:
        out = out.replace("\n", "\r\n")
    if not args.dry_run:
        write_raw(path, out)


def translate_monsters(args, cd, report):
    path = cd.find("NPCTemplet.lua")
    text = read_raw(path)
    for ko, pt in MONSTER_NAMES.items():
        n = text.count('NAME = "%s"' % ko)
        if n:
            text = text.replace('NAME = "%s"' % ko, 'NAME = "%s"' % pt)
            report.counts[("monsters.NAME", "table")] += n
    for m in re.finditer(r'\b(?:NAME|DESC) = "([^"]*)"', text):
        if HANGUL.search(m.group(1)):
            report.left("monsters", m.group(1)[:90])
    if not args.dry_run:
        write_raw(path, text)


def build_lang(args, cd):
    """One detector for every table, trained on all the texts: the sources' Portuguese (and ours), our English."""
    pt, en = [], []

    def take(texts, ours):
        for t in texts:
            if strict_pt(t):
                pt.append(t)
            elif ours and strict_en(t):
                en.append(t)

    for p in args.source_items:
        take((v for d in parse_texts(p, ITEM_RE, "m_ItemID", ITEM_TEXT).values() for v in d.values() if v), False)
    for p in args.source_sets:
        take((d["m_SetName"] for d in parse_texts(p, SET_RE, "m_SetID", ("m_SetName",)).values() if d["m_SetName"]), False)
    for p in args.source_skills:
        for k, b in ms.blocks(ms.read(p)):
            if k == "templet":
                take((" ".join(list_strings(v)) if f == "m_EffectiveDesc" else v.strip('"')
                      for f, v in skill_texts(b).items() if v), False)
    take((v for d in parse_texts(cd.find("Item.lua"), ITEM_RE, "m_ItemID", ITEM_TEXT).values()
          for v in d.values() if v), True)
    take((d["m_SetName"] for d in parse_texts(cd.find("SetItem.lua"), SET_RE, "m_SetID", ("m_SetName",)).values()
          if d["m_SetName"]), True)
    for k, b in ms.blocks(ms.read(cd.find("NewSkillTempletVer2.lua"))):
        if k == "templet":
            take((" ".join(list_strings(v)) if f == "m_EffectiveDesc" else v.strip('"')
                  for f, v in skill_texts(b).items() if v), True)
    lang = Lang()
    lang.train(pt, en)
    return lang


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--source-items", action="append", default=[])
    ap.add_argument("--source-sets", action="append", default=[])
    ap.add_argument("--source-skills", action="append", default=[])
    ap.add_argument("--items", action="store_true")
    ap.add_argument("--sets", action="store_true")
    ap.add_argument("--skills", action="store_true")
    ap.add_argument("--monsters", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--report", help="write what is still foreign, one line per text, to this file")
    args = ap.parse_args()
    cd = ClientData(args.client_data)
    report = Report()
    lang = build_lang(args, cd)
    if args.items:
        translate_items(args, cd, report, lang)
    if args.sets:
        translate_sets(args, cd, report, lang)
    if args.skills:
        translate_skills(args, cd, report, lang)
    if args.monsters:
        translate_monsters(args, cd, report)
    for (what, src), n in sorted(report.counts.items()):
        print(f"{what:32} {src:24} {n}")
    if args.report:
        Path(args.report).write_text("\n".join(report.lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
