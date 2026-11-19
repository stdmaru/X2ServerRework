"""3-way merge of V5's changes into ours: merge3.py <X2Lib file name> [--write]
base = V4 resolved with the client flags, theirs = V5 resolved likewise, ours = src/client/X2Lib/<file>.
The result (with conflict markers) goes to scratchpad/m3/<file>; --write puts it into ours."""
import re, subprocess, sys
from pathlib import Path
sys.path.insert(0, "/home/maru/dev/X2ServerRework/tools/import")
from x2import import pipeline, condres
REPO = Path("/home/maru/dev/X2ServerRework")
V5 = "/media/games/Elsword-V5.0318.6_3"
OUT = Path(__file__).resolve().parent.parent / "m3"
FLAGS = {"SERV_ADD_ARC_TRACER": "X2_ADD_2ND_PATH", "SERV_ADD_MASTERMIND": "X2_ADD_2ND_PATH",
         "SERV_ARA_LITTLE_SPECTER": "X2_ARA_3RD_PATH", "SERV_ARA_ASURA": "X2_ARA_3RD_PATH",
         "SERV_ELESIS_DARK_KNIGHT": "X2_ELESIS_3RD_PATH", "SERV_ELESIS_CRIMSON_AVENGER": "X2_ELESIS_3RD_PATH",
         "SERV_ADD_TIME_TRACER": "X2_ADD_3RD_PATH", "SERV_ADD_DIABOLIC_ESPER": "X2_ADD_3RD_PATH"}

def show(rev, name):
    raw = subprocess.run(["git", "-C", V5, "show", f"{rev}:JP_TW_EU_CN_US/Trunk/X2Lib/{name}"], capture_output=True, check=True).stdout
    try:
        return raw.decode("utf-8")
    except UnicodeDecodeError:
        return raw.decode("cp949", "replace")

def clean(text, flags):
    text = text.replace("\r\n", "\n").lstrip("﻿")
    r = condres.resolve(text, flags)
    t = r.text
    for k, v in FLAGS.items():
        t = re.sub(r"\b%s\b" % k, v, t)
    t = re.sub(r"[ \t]*//\{\{ 2026-[0-9-]+ MARCH2015gc[^\n]*", "", t)
    t = re.sub(r"^(#endif)\s*//.*$", r"\1", t, flags=re.M)
    t = re.sub(r"^(\s*#\s*(?:ifdef|ifndef)\s+\w+)\s*//.*$", r"\1", t, flags=re.M)
    def single(m):
        names = set(re.findall(r"defined\s*\(\s*(\w+)\s*\)", m.group(0)))
        return f"#ifdef {names.pop()}" if len(names) == 1 else m.group(0)
    t = re.sub(r"^#if\s+defined.*$", single, t, flags=re.M)
    return t

def main():
    name = sys.argv[1]
    flags = pipeline.load_flags(["base", "client.generated"])
    ours_p = REPO / "src/client/X2Lib" / name
    raw = ours_p.read_bytes()
    crlf = b"\r\n" in raw
    try:
        ours = raw.decode("utf-8"); enc = "utf-8"
    except UnicodeDecodeError:
        ours = raw.decode("cp949"); enc = "cp949"
    bom = ours.startswith("﻿")
    ours = ours.replace("\r\n", "\n").lstrip("﻿")
    if "--v4-base" in sys.argv:
        base = clean(show("ae28cbe8", name), flags)
    else:   # our original: ours is it plus our edits
        raw_ref = (REPO / "_reference/X2Server-github/X2Lib" / name).read_bytes()
        try: ref = raw_ref.decode("utf-8")
        except UnicodeDecodeError: ref = raw_ref.decode("cp949", "replace")
        base = clean(ref, flags)
    theirs = clean(show("379dc65d", name), flags)
    OUT.mkdir(exist_ok=True)
    for n, t in (("ours", ours), ("base", base), ("theirs", theirs)):
        (OUT / f"{name}.{n}").write_text(t)
    p = subprocess.run(["git", "merge-file", "-p", "--diff3", "--diff-algorithm=histogram", str(OUT / f"{name}.ours"), str(OUT / f"{name}.base"), str(OUT / f"{name}.theirs")], capture_output=True)
    merged = p.stdout.decode()
    (OUT / name).write_text(merged)
    print(name, "conflicts:", p.returncode, "lines", ours.count("\n"), "->", merged.count("\n"))
    if "--write" in sys.argv:
        out = merged.replace("\n", "\r\n") if crlf else merged
        if bom: out = "﻿" + out
        ours_p.write_bytes(out.encode(enc, "replace"))

main()
