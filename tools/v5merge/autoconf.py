"""autoconf.py <X2Lib file>: resolves the //@@CONFLICT markers whose V5 side only inserts lines into base, when ours
matches base line by line (comments aside). Each inserted chunk (split at blank lines) is kept, guarded, when it
names the paths. The others are reported."""
import difflib, re, sys
sys.path.insert(0, __file__.rsplit("/", 1)[0])
import importlib.util
spec = importlib.util.spec_from_file_location("gate", __file__.rsplit("/", 1)[0] + "/gate.py")
from pathlib import Path
src = open(spec.origin).read().split("\ndef main():")[0]
g = {"__file__": spec.origin}
exec(src, g)
paths_of, guard = g["paths_of"], g["guard"]
REPO = Path("/home/maru/dev/X2ServerRework"); M3 = Path(spec.origin).resolve().parent.parent / "m3"
code = lambda l: "" if re.match(r"\s*(//)?\s*#pragma\s+(region|endregion)", l) else re.sub(r"\s+", " ", re.sub(r"//.*$|/\*.*?\*/", "", l)).strip()

def chunks(lines):
    """Splits at blank lines, but only where the braces opened so far are closed."""
    out, cur, depth = [], [], 0
    for l in lines:
        if not l.strip() and depth <= 0:
            if cur: out.append(cur); cur = []
            depth = 0
        else:
            cur.append(l)
            c = code(l)
            depth += c.count("{") - c.count("}")
    if cur: out.append(cur)
    return out

def added(base, theirs):
    """The lines theirs adds to base, in chunks, guarded; parity chunks dropped."""
    sm = difflib.SequenceMatcher(None, [code(l) for l in base], [code(l) for l in theirs], autojunk=False)
    out = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag in ("insert", "replace"):
            for c in chunks([l for l in theirs[j1:j2] if not re.match(r"\s*//\}\}", l)]):
                if all(code(l) == "" for l in c):
                    continue
                pp = paths_of(c)
                if pp:
                    out += [""] + ([guard(pp)] + c + ["#endif"] if not (code(c[0]).startswith("#if") and code(c[-1]) == "#endif") else c)
                else:
                    DROPPED.append(c)
    return out
DROPPED = []

def resolve(ours, base, theirs):
    if len(ours) != len(base) or any(code(a) != code(b) for a, b in zip(ours, base)):
        return ours + added(base, theirs)
    sm = difflib.SequenceMatcher(None, [code(l) for l in base], [code(l) for l in theirs], autojunk=False)
    out = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            out += ours[i1:i2]; continue
        if tag == "delete":
            out += ours[i1:i2]; continue   # keep ours
        if tag == "replace" and any(code(l) for l in base[i1:i2]):
            return None
        out += ours[i1:i2]
        for c in chunks([l for l in theirs[j1:j2] if not re.match(r"\s*//\}\}", l)]):
            if all(code(l) == "" for l in c):
                continue
            p = paths_of(c)
            if p:
                if code(c[0]).startswith("#if") and code(c[-1]) == "#endif":
                    out += c
                else:
                    out += [guard(p)] + c + ["#endif"]
    return out

def main():
    name = sys.argv[1]
    p = REPO / "src/client/X2Lib" / name
    raw = p.read_bytes()
    try: text, enc = raw.decode("utf-8"), "utf-8"
    except UnicodeDecodeError: text, enc = raw.decode("cp949"), "cp949"
    nl = "\r\n" if "\r\n" in text else "\n"
    lines = text.split(nl)
    t = (M3 / f"{name}.conflicts").read_text()
    left = []
    for blk in t.split("=== CONFLICT ")[1:]:
        n = blk.split("\n")[0]
        o = blk.split("--- ours\n")[1].split("--- base\n")[0].rstrip("\n").split("\n")
        b = blk.split("--- base\n")[1].split("--- theirs\n")[0].rstrip("\n").split("\n")
        th = blk.split("--- theirs\n")[1].rstrip("\n").split("\n")
        idx = [i for i, l in enumerate(lines) if l.strip() == f"//@@CONFLICT {n}"]
        if not idx: continue
        r = resolve(o, b, th)
        if r is None:
            left.append(n); continue
        i = idx[0]
        if any(code(x) for x in o): assert [code(x) for x in lines[i - len(o):i]] == [code(x) for x in o], n
        lines[i - len(o):i + 1] = r
    p.write_bytes(nl.join(lines).encode(enc))
    print(name, "left:", left, "dropped chunks:", len(DROPPED))
    for c in DROPPED:
        print("  - dropped:", " / ".join(x.strip()[:60] for x in c[:3]))

main()
