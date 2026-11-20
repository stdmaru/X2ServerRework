#!/usr/bin/env python3
"""Brings the named records other clients have and ours lacks, those that given files name, into our data scripts.

Usage:
  merge_records.py <client_data> --refs <file> [--refs ...] --source <folder> [--source ...] [--files A.lua,...]
                   [--dry-run]

Records, by file (the files are found in the client_data by name):
  EffectSet.lua               g_pEffectSet:AddEffectSetTemplet_LUA({ ... }), keyed by its NAME
  DamageEffect.lua            top-level tables NAME = { ... } (also read from a source's DamageEffect_Character.lua)
  BattleFieldBuffTemplet.lua  g_pBuffTempletManager:AddBuffTemplet_LUA({ ... }), keyed by BUFF_TEMPLET_ID
  BattleFieldBuffFactor.lua   g_pBuffTempletManager:AddBuffFactor_LUA({ ... }), keyed by BUFF_FACTOR_ID
  ComboTree.lua               top-level tables (one per class: ADD_ARC_TRACER = { ... })
  GameMajorParticle*.txt ...  `eventsequence "NAME"` of each particle system (GameMajor, GameMinor, UIMajor) and
  GameMajorXMeshPlayer.txt .. `TEMPLET "NAME"` of each mesh player, looked up in all our files of that system (those
                              X2StateStartUp.cpp loads) and in all the source's; added to the system's last file
                              (GameMajorParticle23.txt), before its closing brace, bytes and CRLF kept

Wanted names: every identifier and quoted string of the --refs files (the states, skills and code of what is being
added: diff them out first rather than passing whole files, or every gap of ours is filled). A record of ours is
never changed; a wanted record ours lacks is taken, as it is, from the first --source folder whose file of the same
name has it, and appended. The names of the records taken are wanted too, across all the files, so what an effect
set names (a damage effect, which names a buff factor, ...) comes along. Each file is checked with LuaJIT when
written (luajit on PATH). A second run changes nothing. See docs/tools/client-data.md (Bringing data from other
clients).
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from client_data import ClientData  # noqa: E402


def call(prefix, key):
    start = re.compile(r"^%s\(\{" % re.escape(prefix))
    key = re.compile(key, re.M)

    def split(lines):
        out, i = [], 0
        while i < len(lines):
            if not start.match(lines[i]):
                i += 1
                continue
            j = i
            while not lines[j].startswith("})"):
                j += 1
            m = key.search("\n".join(lines[i:j + 1]))
            out.append((m.group(1) if m else None, i, j))
            i = j + 1
        return out
    return split


def top_tables(lines):
    out, i = [], 0
    while i < len(lines):
        m = re.match(r"^(\w+) = \{", lines[i])
        if not m:
            i += 1
            continue
        j = i
        if not lines[i].rstrip().endswith("}"):
            j = i + 1
            while not re.match(r"^\}\s*$", lines[j]):
                j += 1
        out.append((m.group(1), i, j))
        i = j + 1
    return out


KINDS = {
    "EffectSet.lua": call("g_pEffectSet:AddEffectSetTemplet_LUA", r'^\tNAME = "([^"]+)"|^\{ NAME = "([^"]+)"'),
    "DamageEffect.lua": top_tables,
    "BattleFieldBuffTemplet.lua": call("g_pBuffTempletManager:AddBuffTemplet_LUA",
                                       r"^\tBUFF_TEMPLET_ID = BUFF_TEMPLET_ID\.(\w+)"),
    "BattleFieldBuffFactor.lua": call("g_pBuffTempletManager:AddBuffFactor_LUA",
                                      r"^\tBUFF_FACTOR_ID = BUFF_FACTOR_ID\.(\w+)"),
    "ComboTree.lua": top_tables,
}


# Other names a source may keep the same records under (BR 2017 split DamageEffect.lua by owner).
SOURCE_NAMES = {"DamageEffect.lua": ["DamageEffect.lua", "DamageEffect_Character.lua"]}


def read(path):
    return Path(path).read_text(encoding="utf-8-sig", errors="replace").replace("\r\n", "\n")


def names_in(text):
    return set(re.findall(r"[A-Za-z_]\w*", text)) | set(re.findall(r'"([^"\n]+)"', text))


class Records:
    def __init__(self, path, split, raw=False):
        self.path = path
        self.raw = raw
        if raw:  # particle scripts: bytes kept as they are (CRLF, any encoding)
            self.lines = Path(path).read_bytes().decode("latin-1").split("\r\n")
        else:
            self.lines = read(path).split("\n")
        self.records = {}
        for name, i, j in split(self.lines):
            if name:
                self.records.setdefault(name, (i, j))

    def block(self, name):
        i, j = self.records[name]
        return self.lines[i:j + 1]


def sequences(keyword):
    """Particle and mesh player scripts: `\t<keyword> "NAME"` ... `\t}` inside the file's one block."""
    start = re.compile(r'^\t%s "([^"]+)"' % keyword, re.I)

    def split(lines):
        out, i = [], 0
        while i < len(lines):
            m = start.match(lines[i])
            if not m:
                i += 1
                continue
            j = i + 1
            while not re.match(r"^\t\}\s*$", lines[j]):
                j += 1
            out.append((m.group(1), i, j))
            i = j + 1
        return out
    return split


# Particle systems: (our files, by name, in load order; the last is where records are added), the sources' file
# name pattern, and the record keyword. Our files are those X2StateStartUp.cpp loads.
SYSTEMS = {
    "GameMajorParticle": (["GameMajorParticle.txt"] + [f"GameMajorParticle{n}.txt" for n in range(2, 24)]
                          + ["GameMajorParticle_INT.txt"], r"GameMajorParticle(\d*|_INT)\.txt", "eventsequence"),
    "GameMinorParticle": (["GameMinorParticle_INT.txt", "GameMinorParticle.txt"], r"GameMinorParticle(_INT)?\.txt",
                          "eventsequence"),
    "UIMajorParticle": (["UIMajorParticle_INT.txt", "UIMajorParticle.txt"], r"UIMajorParticle(_INT)?\.txt",
                        "eventsequence"),
    "GameMajorXMeshPlayer": (["GameMajorXMeshPlayer_INT.txt", "GameMajorXMeshPlayer.txt"],
                             r"GameMajorXMeshPlayer(_INT)?\.txt", "TEMPLET"),
    "GameMinorXMeshPlayer": (["GameMinorXMeshPlayer.txt"], r"GameMinorXMeshPlayer\.txt", "TEMPLET"),
    "UIMajorXMeshPlayer": (["UIMajorXMeshPlayer_INT.txt", "UIMajorXMeshPlayer.txt"],
                           r"UIMajorXMeshPlayer(_INT)?\.txt", "TEMPLET"),
}


def numbered(path):
    m = re.search(r"(\d+)\.txt$", path.name)
    return (int(m.group(1)) if m else (0 if "_INT" not in path.name else 10 ** 6), path.name)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--refs", action="append", required=True)
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--files", default=",".join(list(KINDS) + list(SYSTEMS)))
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cd = ClientData(args.client_data)
    groups = [f for f in args.files.split(",") if f]
    ours, target, sources = {}, {}, {}
    for g in groups:
        if g in KINDS:
            ours[g] = [Records(cd.find(g), KINDS[g])]
            target[g] = ours[g][0]
            sources[g] = [Records(Path(s) / n, KINDS[g]) for s in args.source for n in SOURCE_NAMES.get(g, [g])
                          if (Path(s) / n).is_file()]
        else:
            names, pattern, keyword = SYSTEMS[g]
            split = sequences(keyword)
            ours[g] = [Records(cd.find(n), split, raw=True) for n in names]
            target[g] = ours[g][-1] if not names[-1].endswith("_INT.txt") else ours[g][-2]
            sources[g] = [Records(p, split, raw=True) for s in args.source
                          for p in sorted((q for q in Path(s).iterdir() if re.fullmatch(pattern, q.name, re.I)),
                                          key=numbered)]

    wanted = set()
    for r in args.refs:
        wanted |= names_in(read(r))
    added = {g: [] for g in groups}
    todo = set(wanted)
    while todo:
        more = set()
        for g in groups:
            have = set().union(*(o.records for o in ours[g]))
            for name in sorted(todo):
                if name in have or any(n == name for n, _, _ in added[g]):
                    continue
                for src in sources[g]:
                    if name in src.records:
                        block = src.block(name)
                        added[g].append((name, block, src.path))
                        more |= names_in("\n".join(block)) - wanted
                        break
        wanted |= more
        todo = more

    for g in groups:
        for name, _, src in added[g]:
            print(f"{g}: {name} <- {src}")
        print(f"{g}: {len(added[g])} records to add" + (f" (into {target[g].path.name})" if added[g] else ""))
        if args.dry_run or not added[g]:
            continue
        out = target[g]
        lines = out.lines
        if out.raw:
            end = max(i for i, line in enumerate(lines) if line.strip() == "}")
            for _, block, _ in reversed(added[g]):
                lines[end:end] = block
            Path(out.path).write_bytes("\r\n".join(lines).encode("latin-1"))
            continue
        while lines and lines[-1] == "":
            lines.pop()
        for _, block, _ in added[g]:
            lines.extend(block)
        Path(out.path).write_text("\ufeff" + "\n".join(lines) + "\n", encoding="utf-8")
        check = subprocess.run(["luajit", "-bl", str(out.path)], capture_output=True, text=True)
        if check.returncode:
            raise SystemExit(f"error: {out.path} does not load: {check.stderr.strip()}")


if __name__ == "__main__":
    main()
