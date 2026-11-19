"""Keeps only the new paths' part of a 3-way merge: gate.py <X2Lib file> [--write]
Reads ours (src/client/X2Lib/<file>) and the merge (scratchpad/m3/<file>). Each change the merge makes to ours is
kept, inside #ifdef <path switch>, when it names something of the new paths; otherwise ours is kept. Conflicts whose
V5 side names the paths are left in for a manual pass (reported); the others resolve to ours."""
import difflib, re, sys
from pathlib import Path
REPO = Path("/home/maru/dev/X2ServerRework")
M3 = Path(__file__).resolve().parent.parent / "m3"
SW = ["X2_ELESIS_3RD_PATH", "X2_ARA_3RD_PATH", "X2_ADD_2ND_PATH", "X2_ADD_3RD_PATH"]

def load_ids():
    ids = {s: set() for s in SW}
    for p in ["src/client/X2Lib/X2SkillTree.h", "src/client/X2Lib/X2Unit.h", "src/client/X2Lib/X2ComboTree.h"]:
        s = (REPO / p).read_text("cp949", "replace")
        for m in re.finditer(r"#ifdef (X2_\w+_PATH)\n(.*?)#endif", s, re.S):
            ids[m.group(1)] |= set(re.findall(r"\b([A-Z][A-Z0-9_]{3,})\s*[=,:]", m.group(2)))
    return ids
IDS = load_ids()
CAMEL = {}
for _s, _ids in IDS.items():
    names = set()
    for _n in _ids:
        m = re.match(r"SI_[A-Z]+_(?:SDK|SCA|ALS|AA|AAT|AMM|ATT|ADE|COMMON)_([A-Z0-9_]+)$", _n)
        if m and len(m.group(1)) >= 7 and "_" not in m.group(1):
            names.add(m.group(1).capitalize())
        if m and "_" in m.group(1):
            names.add("".join(w.capitalize() for w in m.group(1).split("_")))
            names.add(m.group(1))
    CAMEL[_s] = names
# names of states, methods and members: class abbreviations as words or name parts
PAT = {
    "X2_ELESIS_3RD_PATH": r"(?<![A-Za-z0-9])(SDK|SCA)(?=_|[A-Z][a-z])|DARK_KNIGHT|DarkKnight|CRIMSON_AVENGER|ESI_ThirdPath|ShadowLink|SHADOW_LINK|WayOfSwordPoint|EJBuster|X2_ELESIS_3RD_PATH|BLOOD_REVENGE|BLOOD_RUNAWAY|JUDGEMENT_BLEEDING|ASSAULT_STRIKE|INJECTION_STIGMA|SHADOW_EDGE|SHADE_THE_ILLUSION|BRUTAL_CUTTER|PUNISH|MADNESS_RUNAWAY|Motion_EL_DK|Motion_EL_CA",
    "X2_ARA_3RD_PATH": r"(?<![A-Za-z0-9])(ALS|AA)(?=_|[A-Z][a-z])|LITTLE_SPECTER|ARA_ASURA|ASI_ThirdPath|FPBeads|FPCancel|Necromancy|NECROMANCY|Thunder|MOONLIGHT_RHAPSODY|REVERSE_STIGMA|X2_ARA_3RD_PATH|Motion_Ara_LS|Motion_Ara_AA",
    "X2_ADD_2ND_PATH": r"ThirdPath|IsAddSkillKeyHeld|Panzer|(?<![A-Za-z0-9])(AAT|AMM)(?=_|[A-Z][a-z])|ARC_TRACER|MASTERMIND|APOCALYPSE|Apocalypse|X2_ADD_2ND_PATH|Motion_ADD_AT|Motion_ADD_MM",
    "X2_ADD_3RD_PATH": r"ThirdPath|IsAddSkillKeyHeld|StigmaMark|StigmaTarget|MaxStrike|UltimateFury|Overlimit|(?<![A-Za-z0-9])(ATT|ADE)(?=_|[A-Z][a-z])|TIME_TRACER|DIABOLIC_ESPER|CREST_OF_TIME|Crest|PARANOIA|ForceNova|FORCE_NOVA|X2_ADD_3RD_PATH|Motion_ADD_TT|Motion_ADD_DE",
}

def paths_of(lines):
    text = "\n".join(re.sub(r"//.*$", "", l) for l in lines)
    words = set(re.findall(r"\b[A-Z][A-Z0-9_]{3,}\b", text))
    out = [s for s in SW if words & IDS[s] or re.search(PAT[s], text) or any(c in text for c in CAMEL[s])]
    return out

def one_block(body):
    """True when body is a single #ifdef X2_...#endif block (V5's own guard, already translated)."""
    lines = [re.sub(r"\s+", " ", l).strip() for l in body]
    lines = [l for l in lines if l]
    if not lines or not lines[0].startswith(("#ifdef X2_", "#if defined(X2_")) or lines[-1] != "#endif":
        return False
    depth = 0
    for n, l in enumerate(lines):
        if re.match(r"#\s*if", l): depth += 1
        elif l.startswith("#endif"):
            depth -= 1
            if depth == 0 and n != len(lines) - 1: return False
    return True

def guard(paths):
    if len(paths) == 1:
        return f"#ifdef {paths[0]}"
    return "#if " + " || ".join(f"defined({p})" for p in paths)

CONFLICTS = []

def resolve_conflicts(lines):
    out, manual, i = [], 0, 0
    while i < len(lines):
        if not lines[i].startswith("<<<<<<< "):
            out.append(lines[i]); i += 1; continue
        a = i + 1
        b = next(n for n in range(a, len(lines)) if lines[n].startswith("||||||| "))
        c = next(n for n in range(b, len(lines)) if lines[n] == "=======")
        d = next(n for n in range(c, len(lines)) if lines[n].startswith(">>>>>>> "))
        ours, base, theirs = lines[a:b], lines[b + 1:c], lines[c + 1:d]
        if paths_of([l for l in theirs if l not in set(base)]):
            manual += 1
            out += ours + [f"//@@CONFLICT {manual}"]
            CONFLICTS.append((manual, ours, base, theirs))
        else:
            out += ours
        i = d + 1
    return out, manual

def main():
    name = sys.argv[1]
    p = REPO / "src/client/X2Lib" / name
    raw = p.read_bytes()
    enc = "utf-8"
    try:
        text = raw.decode("utf-8")
    except UnicodeDecodeError:
        text, enc = raw.decode("cp949"), "cp949"
    crlf, bom = "\r\n" in text, text.startswith("﻿")
    ours = text.replace("\r\n", "\n").lstrip("﻿").split("\n")
    merged, manual = resolve_conflicts((M3 / name).read_text().split("\n"))
    norm = lambda l: re.sub(r"\s+", " ", l).strip()
    sm = difflib.SequenceMatcher(None, [norm(l) for l in ours], [norm(l) for l in merged], autojunk=False)
    out, kept, dropped = [], 0, []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            out += ours[i1:i2]; continue
        new = merged[j1:j2]
        marks = [l for l in new if l.startswith("//@@CONFLICT")]
        new = [l for l in new if not l.startswith("//@@CONFLICT")]
        if marks and not any(norm(l) for l in new):
            out += ours[i1:i2] + marks; continue
        if marks:
            ours_tail = marks
        else:
            ours_tail = []
        if tag == "delete" or not any(norm(l) for l in new):
            out += ours[i1:i2] + ours_tail; continue
        paths = paths_of(new)
        if not paths:
            out += ours[i1:i2] + ours_tail
            dropped.append((i1 + 1, new))
            continue
        kept += 1
        # V5's own guards inside the hunk are already ours; don't wrap a hunk that is itself one guarded block
        body = [l for l in new]
        selfguarded = one_block(body)
        if tag == "insert":
            out += body if selfguarded else [guard(paths)] + body + ["#endif"]
        else:
            if selfguarded and not any(norm(l) for l in ours[i1:i2]):
                out += body
            else:
                out += [guard(paths)] + body + ["#else"] + ours[i1:i2] + ["#endif"]
        out += ours_tail
    out_tail = None
    result = "\n".join(out)
    with open(M3 / f"{name}.conflicts", "w") as f:
        for n, o, b, t in CONFLICTS:
            f.write(f"=== CONFLICT {n}\n--- ours\n" + "\n".join(o) + "\n--- base\n" + "\n".join(b) + "\n--- theirs\n" + "\n".join(t) + "\n")
    print(f"{name}: kept {kept} changes, dropped {len(dropped)}, manual conflicts {manual}")
    with open(M3 / f"{name}.dropped", "w") as f:
        for n, new in dropped:
            f.write(f"@@ ours line {n}\n" + "\n".join("+" + l for l in new) + "\n")
    if "--write" in sys.argv:
        if crlf: result = result.replace("\n", "\r\n")
        if bom: result = "﻿" + result
        p.write_bytes(result.encode(enc, "replace"))

main()
