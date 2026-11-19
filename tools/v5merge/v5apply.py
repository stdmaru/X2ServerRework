"""Applies V5's class-path insertions to our sources: v5apply.py <hunk file> <our file> [--dry-run]

Only additions are applied: each run of '+' lines whose content is not among the hunk's '-' lines is inserted
after the line of ours that matches the nearest distinctive line before it. V5's flags become our switches.
Unknown SERV_ flags in a run are reported and the run is skipped.
"""
import re
import sys

from srcedit import Src

FLAGS = {
    "SERV_ADD_ARC_TRACER": "X2_ADD_2ND_PATH", "SERV_ADD_MASTERMIND": "X2_ADD_2ND_PATH",
    "SERV_ARA_LITTLE_SPECTER": "X2_ARA_3RD_PATH", "SERV_ARA_ASURA": "X2_ARA_3RD_PATH",
    "SERV_ELESIS_DARK_KNIGHT": "X2_ELESIS_3RD_PATH", "SERV_ELESIS_CRIMSON_AVENGER": "X2_ELESIS_3RD_PATH",
    "SERV_ADD_TIME_TRACER": "X2_ADD_3RD_PATH", "SERV_ADD_DIABOLIC_ESPER": "X2_ADD_3RD_PATH",
}
import tomllib
_T, _F = set(), set()
for _n in ("client.generated.toml", "server.generated.toml"):
    _d = tomllib.load(open("/home/maru/dev/X2ServerRework/tools/import/flags/" + _n, "rb"))
    for _sec in _d.values():
        _T |= set(_sec.get("true", []))
        _F |= set(_sec.get("false", []))
OURS = set(FLAGS.values())


def resolve(lines):
    """Resolves the importer's flags in a run (true: keep the code, false: drop it); None on an unknown flag."""
    out, stack = [], []  # stack of (kind, keep) where kind 'ours' keeps the directive
    def keeping():
        return all(k for _, k in stack)
    for line in lines:
        m = re.match(r"^\s*#\s*(ifdef|ifndef|if|else|elif|endif)\b(.*)$", line)
        if not m:
            if keeping():
                out.append(line)
            continue
        d, rest = m.group(1), m.group(2)
        names = set(re.findall(r"\b[A-Z][A-Z0-9_]{3,}\b", rest)) - {"defined"}
        if d in ("ifdef", "ifndef", "if"):
            if names and names <= OURS:
                stack.append(("ours", True))
                if keeping():
                    out.append(line)
            elif names and names <= (_T | _F):
                val = all(n in _T for n in names) if d != "ifndef" else not any(n in _T for n in names)
                if "||" in rest:
                    val = any(n in _T for n in names)
                stack.append(("flag", val))
            else:
                return None, names - OURS - _T - _F
        elif d == "else":
            if not stack:
                return None, {"#else"}
            kind, val = stack.pop()
            if kind == "ours":
                stack.append(("ours", True))
                if keeping():
                    out.append(line)
            else:
                stack.append(("flag", not val))
        elif d == "endif":
            if not stack:
                continue  # closes something before the run
            kind, _ = stack.pop()
            if kind == "ours" and keeping():
                out.append(line)
        else:
            return None, {"#elif"}
    while stack:  # unclosed: keep balance
        kind, _ = stack.pop()
        if kind == "ours":
            out.append("#endif")
    return out, set()


TRIVIAL = re.compile(r"^(\{|\}|\};|break;|\} break;|default:|return true;|return false;|)$")


def norm(line):
    line = re.sub(r"//.*$", "", line)
    return re.sub(r"\s+", " ", line).strip()


def is_pp(line):
    return norm(line).startswith("#")


def hunks(path):
    out, cur = [], None
    for l in open(path, encoding="utf-8"):
        l = l.rstrip("\n")
        if l.startswith("@@"):
            cur = [("@", l.split("@@")[2].strip() if l.count("@@") >= 2 else "")]
            out.append(cur)
        elif cur is not None and l and l[0] in " +-":
            cur.append((l[0], l[1:]))
        elif cur is not None and l == "":
            cur.append((" ", ""))
    return out


def translate(lines):
    text = "\n".join(lines)
    for k, v in FLAGS.items():
        text = re.sub(r"\b%s\b" % k, v, text)
    text = re.sub(r"^\s*//\{\{.*MARCH2015gc.*$\n?", "", text, flags=re.M)
    text = re.sub(r"^\s*//\}\}.*$\n?", "", text, flags=re.M)
    text = re.sub(r"(#endif)\s*//.*$", r"\1", text, flags=re.M)
    text = re.sub(r"^(#ifdef \w+)\s*//.*$", r"\1", text, flags=re.M)
    # "#if defined(A) || defined(A)" -> "#ifdef A"
    def single(m):
        names = set(re.findall(r"defined\((\w+)\)", m.group(0)))
        return f"#ifdef {names.pop()}" if len(names) == 1 else m.group(0)
    text = re.sub(r"^#if defined\(.*$", single, text, flags=re.M)
    # merge "#endif\n#ifdef X ... " when the previous block was #ifdef X
    prev = None
    out = []
    for line in text.split("\n"):
        m = re.match(r"^#ifdef (\w+)\s*$", line)
        if m and out and out[-1].strip() == "#endif" and prev == m.group(1):
            out.pop()
            continue
        if m:
            prev = m.group(1)
        out.append(line)
    return out


def apply(hunk_file, our_file, dry=False):
    f = Src(our_file)
    ours = f.s.split("\n")
    nours = [norm(l) for l in ours]
    inserts = []  # (line index to insert before, lines)
    problems = []
    for h in hunks(hunk_file):
        func = h[0][1] if h and h[0][0] == "@" else ""
        h = [x for x in h if x[0] != "@"]
        fm = re.search(r"([\w:~]+)\s*\(", func)
        fstart = 0
        if fm:
            fi = [n for n, l in enumerate(ours) if fm.group(1) + "(" in l.replace(" (", "(") and not l.rstrip().endswith(";")]
            fstart = fi[0] if fi else 0
        removed = {norm(t) for k, t in h if k == "-"}
        seq = [(k, t) for k, t in h if k in " +"]
        import difflib
        oldside = [norm(t) for k, t in h if k in " -"]
        newside = [norm(t) for k, t in seq]
        added = [False] * len(seq)
        for tag, a1, a2, b1, b2 in difflib.SequenceMatcher(None, oldside, newside, autojunk=False).get_opcodes():
            if tag in ("insert", "replace"):
                for b in range(b1, b2):
                    added[b] = True
        new = [added[n] and k == "+" and norm(t) != "" and not is_pp(t) and not TRIVIAL.match(norm(t)) for n, (k, t) in enumerate(seq)]
        joins = [k == "+" and (norm(t) == "" or is_pp(t) or TRIVIAL.match(norm(t)) is not None) for k, t in seq]
        # blank '+' lines inside a run count as part of it
        i = 0
        while i < len(seq):
            if not new[i]:
                i += 1
                continue
            # start the run at the joinable '+' lines right before it (an #ifdef)
            while i > 0 and joins[i - 1]:
                i -= 1
            j = i
            while j < len(seq) and (new[j] or joins[j]):
                j += 1
            while j > i and joins[j - 1] and norm(seq[j - 1][1]) != "#endif":
                body = "\n".join(norm(t) for _, t in seq[i:j])
                if "}" in norm(seq[j - 1][1]) and body.count("{") >= body.count("}"):
                    break
                j -= 1
            run = [t for _, t in seq[i:j]]
            # anchor: nearest distinctive, non-preprocessor line before the run
            k = i - 1
            tail = []
            while k >= 0 and (is_pp(seq[k][1]) or TRIVIAL.match(norm(seq[k][1]))):
                if not is_pp(seq[k][1]) and norm(seq[k][1]):
                    tail.insert(0, norm(seq[k][1]))
                k -= 1
            before_mode = False
            if k < 0:
                # anchor after the run: insert before the first distinctive line that follows it
                k = j
                while k < len(seq) and (is_pp(seq[k][1]) or TRIVIAL.match(norm(seq[k][1]))):
                    k += 1
                if k >= len(seq):
                    problems.append(("no anchor", run[:3]))
                    i = j
                    continue
                before_mode = True
                tail = []
            anchor = norm(seq[k][1])
            run = [l for l in "\n".join(translate(run)).split("\n")]
            run, unknown = resolve(run)
            if run is None:
                problems.append((f"unknown flags {sorted(unknown)} after {anchor!r}", run[:4]))
                i = j
                continue
            # the anchor's line in ours: the one after the previous insertion point, else the first
            idxs = [n for n, l in enumerate(nours) if l == anchor and n >= fstart]
            if not idxs:
                problems.append((f"anchor not in ours: {anchor!r}", run[:3]))
                i = j
                continue
            if len(idxs) > 1 and fstart:
                n = idxs[0]
            elif len(idxs) > 1:
                ctx = [norm(t) for _, t in seq[max(0, k - 6):k] if norm(t) and not is_pp(t)]
                def score(n):
                    window = set(nours[max(0, n - 40):n])
                    return sum(c in window for c in ctx)
                best = max(idxs, key=score)
                if sorted(idxs, key=score)[-2:] and len(idxs) > 1 and score(best) == score(sorted(idxs, key=score)[-2]):
                    problems.append((f"ambiguous anchor {anchor!r} ({len(idxs)} matches)", run[:3]))
                    i = j
                    continue
                n = best
            if len(idxs) == 1:
                n = idxs[0]
            if before_mode:
                # step back over the blank/brace/comment lines that precede it in ours
                while n > 0 and (nours[n - 1] == "" or ours[n - 1].strip().startswith("//")):
                    n -= 1
                inserts.append((n, run + [""]))
                i = j
                continue
            n += 1
            for t in tail:  # walk over the trivial lines that follow the anchor in V5
                while n < len(ours) and nours[n] == "":
                    n += 1
                if n < len(ours) and nours[n] == t:
                    n += 1
            inserts.append((n, run))
            i = j
    for n, lines in sorted(inserts, key=lambda x: -x[0]):
        ours[n:n] = lines
    if not dry:
        f.s = "\n".join(ours)
        f.save()
    return inserts, problems


if __name__ == "__main__":
    ins, probs = apply(sys.argv[1], sys.argv[2], "--dry-run" in sys.argv)
    print(f"{sys.argv[2]}: {len(ins)} insertions")
    for p, run in probs:
        print("  !", p, "|", " / ".join(x.strip() for x in run))
