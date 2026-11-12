#!/usr/bin/env python3
"""Removes unused local variables of imported code, guided by a Clang or GCC build log.

  -Wunused-variable          deletes the declaration when its initializer has no side effects (literals, names,
                             member access, casts, Get/Is/Find/Has calls); otherwise marks it [[maybe_unused]]
  -Wunused-but-set-variable  deletes the declaration and the statements that only assign it: `x = f();` keeps
                             `f();`, `x = 0;` and `x++;` go. When the variable appears anywhere else, or an
                             assignment is the body of an unbraced if/else/loop, it is marked [[maybe_unused]]

Only whole-line statements with a single declarator are edited; anything else is listed and left alone.
The build validates the result. Usage: tools/port/fix_unused.py build.log [--root src/client] [--maybe-unused]
  --maybe-unused   only add [[maybe_unused]] (for variables a configuration does not use, e.g. without X2_DEV_TOOLS)
"""

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

from fix_warnings import strip_code

PATTERN = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):(?P<col>\d+): warning: (?P<msg>.*)"
                     r"\[-W(?P<flag>unused-variable|unused-but-set-variable)\]$")
QUOTED = re.compile(r"[‘'`]([^’'`]+)[’'`]")
CALL = re.compile(r"(?:\b(?:Get|Is|Find|Has|Can)\w*|\b(?:static|reinterpret|const)_cast\s*<[^<>]*>|\bsizeof)\s*\(")
SIDE_EFFECTS = re.compile(r"\bnew\b|\+\+|--|(?<![=!<>])=(?!=)|\b(?:delete|throw)\b")
QUALIFIERS = r"(?:(?:static|const|constexpr|volatile|unsigned|signed|struct|class|enum)\s+)*"


def trivial(expr: str) -> bool:
    """No side effects: literals, names, operators, member access and getter-like calls only."""
    code = strip_code(expr)
    if SIDE_EFFECTS.search(code):
        return False
    rest = CALL.sub("", code)
    # any other call (name followed by '(') may have side effects
    return re.search(r"[\w>\]]\s*\(", rest) is None


def declaration(line: str, name: str):
    """(prefix, init or None) when the line is one statement declaring only `name`."""
    code = strip_code(line)
    m = re.match(r"^(?P<indent>\s*)(?P<type>" + QUALIFIERS + r"[\w:<>,\s*&]+?)[\s*&]+" + re.escape(name)
                 + r"\s*(?P<array>(?:\[[^\]]*\])*)\s*(?:(?P<eq>=)\s*(?P<init>.*?)|(?P<paren>\(.*\)|\{.*\}))?\s*;\s*$",
                 code)
    if not m or "," in re.sub(r"<[^<>]*>", "", m["type"]):
        return None
    if m["eq"]:
        init = line[m.start("init"):m.end("init")]
    elif m["paren"]:
        init = line[m.start("paren") + 1:m.end("paren") - 1]
    else:
        init = ""
    return m, init


def block_end(code: str, start: int) -> int:
    """Offset of the '}' that closes the block containing `start`."""
    depth = 0
    for i in range(start, len(code)):
        if code[i] == "{":
            depth += 1
        elif code[i] == "}":
            if depth == 0:
                return i
            depth -= 1
    return len(code)


def unbraced_body(lines: list[str], index: int) -> bool:
    """The line is the body of an if/else/for/while without braces."""
    for k in range(index - 1, -1, -1):
        prev = strip_code(lines[k]).strip()
        if not prev:
            continue
        return not prev.endswith((";", "{", "}", ":")) and not prev.startswith("#")
    return False


def mark_maybe_unused(lines: list[str], index: int) -> bool:
    m = re.match(r"^(\s*)(?!\[\[maybe_unused\]\])", lines[index])
    if not m or "[[maybe_unused]]" in lines[index]:
        return False
    lines[index] = m.group(1) + "[[maybe_unused]] " + lines[index][m.end():]
    return True


def fix_file(path: Path, sites: set[tuple[int, str, str]], only_mark: bool) -> int:
    text = path.read_text(encoding="utf-8")
    lines = text.split("\n")
    delete: set[int] = set()
    replace: dict[int, str] = {}
    count = 0
    for line, name, flag in sorted(sites):
        i = line - 1
        decl = declaration(lines[i], name)
        if decl is None:
            print(f"skip {path}:{line}: '{name}' is not a single declaration", file=sys.stderr)
            continue
        m, init = decl
        if only_mark:
            count += mark_maybe_unused(lines, i)
            continue
        if flag == "unused-variable":
            if trivial(init) and not unbraced_body(lines, i):
                delete.add(i)
                count += 1
            else:
                count += mark_maybe_unused(lines, i)
            continue
        # set but not used: every other occurrence must be a whole-line assignment
        offsets = [0]
        for ln in lines:
            offsets.append(offsets[-1] + len(ln) + 1)
        code = strip_code("\n".join(lines))
        end = block_end(code, offsets[i + 1])
        edits: dict[int, str | None] = {}
        ok = trivial(init) and not unbraced_body(lines, i)
        word = re.compile(r"(?<![\w.>:])" + re.escape(name) + r"\b")
        for j in range(i + 1, len(lines)):
            if offsets[j] >= end:
                break
            cl = strip_code(lines[j])
            if not word.search(cl):
                continue
            a = re.match(r"^(\s*)" + re.escape(name) + r"\s*(?:[-+*/|&^]|<<|>>)?=(?!=)\s*(.*?)\s*;\s*$", cl)
            inc = re.match(r"^\s*(?:\+\+|--)?\s*" + re.escape(name) + r"\s*(?:\+\+|--)?\s*;\s*$", cl)
            if len(word.findall(cl)) != 1 or not (a or inc) or unbraced_body(lines, j):
                ok = False
                break
            if inc:
                edits[j] = None
            else:
                rhs = lines[j][a.start(2):a.end(2)]
                edits[j] = None if trivial(rhs) else a.group(1) + rhs + ";"
        if not ok:
            count += mark_maybe_unused(lines, i)
            continue
        delete.add(i)
        for j, new in edits.items():
            if new is None:
                delete.add(j)
            else:
                replace[j] = new
        count += 1
    for j, new in replace.items():
        lines[j] = new
    lines = [ln for k, ln in enumerate(lines) if k not in delete]
    path.write_text("\n".join(lines), encoding="utf-8")
    return count


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--root", default="src", help="only edit files under this directory")
    ap.add_argument("--maybe-unused", action="store_true")
    args = ap.parse_args()
    root = Path(args.root).resolve()
    sites: dict[Path, set] = defaultdict(set)
    for raw in Path(args.log).read_text(errors="replace").splitlines():
        m = PATTERN.match(raw)
        if not m:
            continue
        path = Path(m["file"]).resolve()
        q = QUOTED.search(m["msg"])
        if root in path.parents and q:
            sites[path].add((int(m["line"]), q.group(1), m["flag"]))
    total = 0
    for path in sorted(sites):
        n = fix_file(path, sites[path], args.maybe_unused)
        print(f"{path}: {n}")
        total += n
    print(f"{total} fixes", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
