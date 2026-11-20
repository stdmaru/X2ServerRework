#!/usr/bin/env python3
"""Translates the text of items, sets, skills and monsters that a client_data still has in English or Korean.

Usage:
  translate_from_br.py <client_data> --source <folder of scripts> [--source ...] [--strings <General.ess> ...]
                       [--items] [--sets] [--skills] [--records] [--ess <our general.ess>] [--monsters]
                       [--dry-run] [--report <file>]

- Files are found by name in the client_data (tools/assets/client_data.py), never by path.
- Sources are folders of decompiled scripts, in priority order (user, 2026-11-20: BR 2014-08 first, BR 2017 as the
  fallback); each file is found in them by name (Item.lua, SetItem.lua, NewSkillTempletVer2.lua, FieldQuest.lua,
  FieldSubQuest.lua, TitleTable.lua).
- Text that is English, Korean or Chinese is replaced, by ID, with the first source that has Portuguese for that ID. Text
  that is already Portuguese is kept, even where a source words it differently, except text that is a lower-priority
  source's own (an earlier run took it from BR 2017, and a better source has since been given).
- --records: quest titles and texts (FieldQuest.lua), sub-quest descriptions (FieldSubQuest.lua) and titles
  (TitleTable.lua). --ess with --strings (General.ess files, same rules): the strings scenes, NPCs and UI use by ID.
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


def pick(lang, value, texts):
    """The Portuguese to put where ours is `value`, from `texts` (the sources' text for this field, in priority order,
    None where a source has none): the first Portuguese one, if ours is foreign or is a lower source's text."""
    for n, t in enumerate(texts):
        if t and lang.kind(t) == "pt":
            if t != value and (lang.foreign(value) or value in texts[n + 1:]):
                return t, n
            return None, n
    return None, None


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


def src_files(args, name):
    """(folder name, path) of the --source folders that have this file, in priority order."""
    return [(Path(d).name, Path(d) / name) for d in args.source if (Path(d) / name).is_file()]


def translate_items(args, cd, report, lang):
    path = cd.find("Item.lua")
    ours = parse_texts(path, ITEM_RE, "m_ItemID", ITEM_TEXT)
    sources = [(n, parse_texts(f, ITEM_RE, "m_ItemID", ITEM_TEXT)) for n, f in src_files(args, "Item.lua")]

    # pass 1: a source's Portuguese text by ID; collect English -> Portuguese pairs for pass 2
    chosen = {}
    pairs = collections.defaultdict(collections.Counter)
    for i, d in ours.items():
        for k, v in d.items():
            if not v or v in KEEP:
                continue
            t, n = pick(lang, v, [s.get(i, {}).get(k) for _, s in sources])
            if t:
                chosen[(i, k)] = (t, sources[n][0])
                pairs[(k, v)][t] += 1

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
            if not v or v in KEEP:
                continue
            hit = chosen.get((i, k))
            if hit:
                new = replace_field(new, k, hit[0])
                report.add("items." + k, i, hit[1])
            elif lang.foreign(v):
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


def translate_records(args, cd, report, lang, filename, call, id_key, keys):
    """A table of `call({ ... })` records with an ID and text fields (quests, sub-quests, titles)."""
    path = cd.find(filename)
    block_re = re.compile(call + r"\(\{(.*?)\n\}\)", re.S)
    ours = parse_texts(path, block_re, id_key, keys)
    sources = [(n, parse_texts(f, block_re, id_key, keys)) for n, f in src_files(args, filename)]
    text = read_raw(path)
    out = []
    pos = 0
    for m in block_re.finditer(text):
        body = m.group(1)
        i = re.search(r"%s\s*=\s*(\d+)" % id_key, body)
        if not i:
            continue
        i = int(i.group(1))
        new = body
        for k in keys:
            v = ours[i][k]
            if not v or not WORD.search(v):
                continue
            t, n = pick(lang, v, [s.get(i, {}).get(k) for _, s in sources])
            if t:
                new = replace_field(new, k, t)
                report.add(f"{filename}.{k}", i, sources[n][0])
            elif lang.foreign(v):
                report.left(f"{filename}.{k}", f"{i}\t{v[:90]}")
        if new != body:
            out.append(text[pos:m.start(1)])
            out.append(new)
            pos = m.end(1)
    out.append(text[pos:])
    if not args.dry_run:
        write_raw(path, "".join(out))


RECORDS = (
    ("FieldQuest.lua", r"g_pQuestManager:AddQuestTemplet_LUA", "m_iID",
     ("m_wstrTitle", "m_wstrMainText", "m_wstrThanksText", "m_wstrDissClearText", "m_wstrEpisodeGroupTitle")),
    ("FieldSubQuest.lua", r"g_pQuestManager:AddSubQuestTemplet_LUA", "m_iID", ("m_wstrDescription",)),
    ("TitleTable.lua", r"TitleManager:AddTitleInfo_LUA", "m_iTitleID", ("m_TitleName", "m_Description")),
)


def translate_sets(args, cd, report, lang):
    path = cd.find("SetItem.lua")
    keys = ("m_SetName",)
    ours = parse_texts(path, SET_RE, "m_SetID", keys)
    sources = [(n, parse_texts(f, SET_RE, "m_SetID", keys)) for n, f in src_files(args, "SetItem.lua")]
    text = read_raw(path)
    out = []
    pos = 0
    for m in SET_RE.finditer(text):
        body = m.group(1)
        i = re.search(r"m_SetID\s*=\s*(\d+)", body)
        if not i:
            continue
        i = int(i.group(1))
        v = ours[i]["m_SetName"]
        if not v:
            continue
        t, n = pick(lang, v, [s.get(i, {}).get("m_SetName") for _, s in sources])
        if t:
            out.append(text[pos:m.start(1)])
            out.append(replace_field(body, "m_SetName", t))
            pos = m.end(1)
            report.add("sets.m_SetName", i, sources[n][0])
        elif lang.foreign(v):
            report.left("sets.m_SetName", f"{i}\t{v[:90]}")
    out.append(text[pos:])
    if not args.dry_run:
        write_raw(path, "".join(out))


def skill_texts(block):
    return {f: (ms.field(block, f) or (0, 0, None))[2] for f in ms.TEXT_FIELDS}


def list_strings(value):
    return re.findall(r'"((?:[^"\\]|\\.)*)"', value or "")


def plain(f, v):
    return " ".join(list_strings(v)) if f == "m_EffectiveDesc" else (v or "").strip('"')


def translate_skills(args, cd, report, lang):
    path = cd.find("NewSkillTempletVer2.lua")
    raw = read_raw(path)
    crlf = "\r\n" in raw
    text = raw.replace("\r\n", "\n")
    chunks = ms.blocks(text.lstrip("\ufeff"))
    sources = [(n, {ms.skill_id(b): b for k, b in ms.blocks(ms.read(f)) if k == "templet"})
               for n, f in src_files(args, "NewSkillTempletVer2.lua")]
    result = []
    for kind, block in chunks:
        if kind != "templet":
            result.append(block)
            continue
        sid = ms.skill_id(block)
        mine = skill_texts(block)
        theirs = [skill_texts(s[sid]) if sid in s else {} for _, s in sources]
        for f in ms.TEXT_FIELDS:
            v = mine[f]
            if not v:
                continue
            texts = [t.get(f) for t in theirs]
            if f == "m_EffectiveDesc":  # another number of levels: not the same skill any more
                texts = [t if t and len(list_strings(t)) == len(list_strings(v)) else None for t in texts]
            ptn = [t if t and lang.kind(plain(f, t)) == "pt" else None for t in texts]
            n = next((k for k, t in enumerate(ptn) if t), None)
            if n is not None and ptn[n] != v and (lang.foreign(plain(f, v)) or v in texts[n + 1:]):
                a = ms.field(block, f)
                block = block[:a[0]] + ptn[n] + block[a[1]:]
                report.add("skills." + f, sid, sources[n][0])
            elif n is None and lang.foreign(plain(f, v)):
                report.left("skills." + f, f"{sid}\t{v[:90]!r}")
        result.append(block)
    out = "\n".join(result)
    if text.startswith("\ufeff") and not out.startswith("\ufeff"):
        out = "\ufeff" + out
    if crlf:
        out = out.replace("\n", "\r\n")
    if not args.dry_run:
        write_raw(path, out)


def translate_ess(args, report, lang):
    from merge_quests import ess_line, read_ess
    ours = read_ess(args.ess)
    sources = [(Path(p).parent.name, read_ess(p)) for p in args.strings]
    changed = 0
    for i, v in ours.items():
        if not v or v == f"STR_ID_{i}":
            continue
        t, n = pick(lang, v, [s.get(i) for _, s in sources])
        if t:
            ours[i] = t
            changed += 1
            report.add("general.ess", i, sources[n][0])
        elif lang.foreign(v):
            report.left("general.ess", f"{i}\t{v[:90]}")
    if not args.dry_run and changed:
        Path(args.ess).write_bytes(b"\xff\xfe" + b"".join(ess_line(i, ours[i]) for i in sorted(ours)))


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
    from merge_quests import read_ess
    pt, en = [], []

    def take(texts, ours):
        for t in texts:
            if strict_pt(t):
                pt.append(t)
            elif ours and strict_en(t):
                en.append(t)

    def scan(item_files, set_files, skill_files, record_files, ours):
        for f in item_files:
            take((v for d in parse_texts(f, ITEM_RE, "m_ItemID", ITEM_TEXT).values() for v in d.values() if v), ours)
        for f in set_files:
            take((d["m_SetName"] for d in parse_texts(f, SET_RE, "m_SetID", ("m_SetName",)).values()
                  if d["m_SetName"]), ours)
        for f in skill_files:
            for k, b in ms.blocks(ms.read(f)):
                if k == "templet":
                    take((plain(fl, v) for fl, v in skill_texts(b).items() if v), ours)
        for (name, call, id_key, keys), fs in zip(RECORDS, record_files):
            for f in fs:
                rx = re.compile(call + r"\(\{(.*?)\n\}\)", re.S)
                take((v for d in parse_texts(f, rx, id_key, keys).values() for v in d.values() if v), ours)

    def files(name):
        return [f for _, f in src_files(args, name)]

    scan(files("Item.lua"), files("SetItem.lua"), files("NewSkillTempletVer2.lua"),
         [files(r[0]) for r in RECORDS], False)
    scan([cd.find("Item.lua")], [cd.find("SetItem.lua")], [cd.find("NewSkillTempletVer2.lua")],
         [[cd.find(r[0])] for r in RECORDS], True)
    strings = [read_ess(p) for p in args.strings] + ([read_ess(args.ess)] if args.ess else [])
    for n, t in enumerate(strings):
        take((v for v in t.values() if v), n == len(strings) - 1 and bool(args.ess))
    lang = Lang()
    lang.train(pt, en)
    return lang


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--source", action="append", default=[], help="folder of decompiled scripts, in priority order")
    ap.add_argument("--strings", action="append", default=[], help="a General.ess, in priority order")
    ap.add_argument("--ess", help="our general.ess (in the client's folder, outside client_data)")
    ap.add_argument("--items", action="store_true")
    ap.add_argument("--sets", action="store_true")
    ap.add_argument("--skills", action="store_true")
    ap.add_argument("--records", action="store_true")
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
    if args.records:
        for r in RECORDS:
            translate_records(args, cd, report, lang, *r)
    if args.ess:
        translate_ess(args, report, lang)
    if args.monsters:
        translate_monsters(args, cd, report)
    for (what, src), n in sorted(report.counts.items()):
        print(f"{what:34} {src:24} {n}")
    if args.report:
        Path(args.report).write_text("\n".join(report.lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
