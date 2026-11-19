"""Removes an #ifdef X (and its #endif) nested directly inside an #ifdef X of ours."""
import re, sys
sys.path.insert(0, __file__.rsplit("/", 1)[0])
from srcedit import Src
OURS = {"X2_ADD_2ND_PATH", "X2_ADD_3RD_PATH", "X2_ARA_3RD_PATH", "X2_ELESIS_3RD_PATH"}
for p in sys.argv[1:]:
    f = Src(p)
    lines = f.s.split("\n")
    out, stack, n = [], [], 0
    for l in lines:
        s = l.strip()
        m = re.match(r"#\s*(ifdef|ifndef|if)\b\s*(\w*)", s)
        if m:
            name = m.group(2) if m.group(1) == "ifdef" else None
            if name in OURS and name in [x for x, _ in stack]:
                stack.append((name, True)); n += 1; continue
            stack.append((name, False))
        elif re.match(r"#\s*(else|elif)\b", s):
            if stack and stack[-1][1]:
                raise SystemExit(f"{p}: #else in a dropped block")
        elif re.match(r"#\s*endif\b", s):
            _, drop = stack.pop()
            if drop:
                continue
        out.append(l)
    f.s = "\n".join(out); f.save()
    print(p, n, "removed")
