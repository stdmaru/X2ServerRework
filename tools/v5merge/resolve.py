"""resolve.py <src file> <n> <text file|->: replaces the line '//@@CONFLICT n' with the text (empty drops it)."""
import sys
from pathlib import Path
p = Path(sys.argv[1]); n = sys.argv[2]
text = sys.stdin.read() if sys.argv[3] == "-" else Path(sys.argv[3]).read_text()
raw = p.read_bytes()
try:
    s, enc = raw.decode("utf-8"), "utf-8"
except UnicodeDecodeError:
    s, enc = raw.decode("cp949"), "cp949"
nl = "\r\n" if "\r\n" in s else "\n"
lines = s.split(nl)
idx = [i for i, l in enumerate(lines) if l.strip() == f"//@@CONFLICT {n}"]
assert len(idx) == 1, idx
new = text.rstrip("\n").split("\n") if text.strip() else []
lines[idx[0]:idx[0] + 1] = new
p.write_bytes(nl.join(lines).encode(enc))
print(f"{p.name}: conflict {n} -> {len(new)} lines")
