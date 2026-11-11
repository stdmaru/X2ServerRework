#!/usr/bin/env python3
"""Fixes mechanical GCC warnings of imported code, guided by a build log.

  -Wswitch            adds `default: break;` to the switch (MSVC does not warn about unhandled enum values;
                      the original relied on falling out of the switch, which is what `default: break;` does)
  -Wunused-parameter  comments out the parameter name: `int iValue` -> `int /*iValue*/` (not inside a macro: listed)
  -Wreorder           sorts the constructor's mem-initializer list in member declaration order, base classes first
                      (only the text changes: members were always initialized in declaration order)

Each site is fixed once, however many translation units reported it. Columns are GCC display columns
(tabs expand to 8). Run the build again afterwards.

Usage: tools/port/fix_warnings.py build.log [--root src] [--only switch,unused-parameter,reorder] [--dry-run]
"""

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

from fix_typename import display_to_index

PATTERN = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):(?P<col>\d+): warning: (?P<msg>.*)\[-W(?P<flag>[\w-]+)=?\]$")
QUOTED = re.compile(r"[‘'`]([^’'`]+)[’'`]")


def strip_code(text: str) -> str:
    """Blanks out comments and string/char literals (same length), so braces can be counted."""
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out[i:j] = " " * (j - i)
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out[i:j] = [ch if ch == "\n" else " " for ch in text[i:j]]
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out[i + 1:j] = " " * (j - i - 1)
            i = j + 1
        else:
            i += 1
    return "".join(out)


def line_offsets(text: str) -> list[int]:
    offsets = [0]
    for i, c in enumerate(text):
        if c == "\n":
            offsets.append(i + 1)
    return offsets


def fix_switch(text: str, sites: set[tuple[int, int]]) -> tuple[str, int]:
    code = strip_code(text)
    offsets = line_offsets(text)
    edits = []
    for line, col in sites:
        line_text = text[offsets[line - 1]:].split("\n", 1)[0]
        # GCC points inside the statement (at the parenthesis): look for the keyword on that line
        pos = code.rfind("switch", offsets[line - 1], offsets[line - 1] + display_to_index(line_text, col) + 1)
        if pos < 0:
            print(f"skip line {line}: no switch at column {col}", file=sys.stderr)
            continue
        open_brace = code.find("{", pos)
        depth, i = 0, open_brace
        while i < len(code):
            if code[i] == "{":
                depth += 1
            elif code[i] == "}":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        body = code[open_brace + 1:i]
        # a `default:` at the switch's own level (nested switches have their own braces)
        level, has_default = 0, False
        for m in re.finditer(r"[{}]|\bdefault\s*:", body):
            tok = m.group(0)
            if tok == "{":
                level += 1
            elif tok == "}":
                level -= 1
            elif level == 0:
                has_default = True
        if has_default:
            continue
        line_start = text.rfind("\n", 0, i) + 1
        closing_indent = re.match(r"[ \t]*", text[line_start:]).group(0)
        if text[line_start:i].strip():  # `}` shares the line with code: put the default before it
            edits.append((i, f" default: break; "))
        else:
            edits.append((line_start, f"{closing_indent}default:\n{closing_indent}\tbreak;\n"))
    for pos, ins in sorted(edits, reverse=True):
        text = text[:pos] + ins + text[pos:]
    return text, len(edits)


def fix_unused_parameter(text: str, sites: set[tuple[int, int, str]]) -> tuple[str, int]:
    lines = text.split("\n")
    count = 0
    for line, col, name in sorted(sites, key=lambda s: (s[0], -s[1])):
        src = lines[line - 1]
        i = display_to_index(src, col)
        if not src.startswith(name, i) or src[:i].rstrip().endswith("/*"):
            print(f"skip line {line}: '{name}' not at column {col}", file=sys.stderr)
            continue
        # A macro (IMPL_ON_FUNC...): the name is used by other expansions, so it can't be commented out there.
        if src.lstrip().startswith("#define") or (line >= 2 and lines[line - 2].rstrip().endswith("\\")):
            print(f"skip line {line}: '{name}' is in a macro (use [[maybe_unused]] by hand)", file=sys.stderr)
            continue
        lines[line - 1] = src[:i] + "/*" + name + "*/" + src[i + len(name):]
        count += 1
    return "\n".join(lines), count


def class_range(code: str, offsets: list[int], decl_line: int) -> tuple[int, int]:
    """Offsets of the body of the class that declares the member at decl_line."""
    pos, depth = offsets[decl_line - 1], 0
    while pos > 0:
        pos -= 1
        if code[pos] == "}":
            depth += 1
        elif code[pos] == "{":
            if depth == 0:
                break
            depth -= 1
    end, depth = pos, 0
    while end < len(code):
        if code[end] == "{":
            depth += 1
        elif code[end] == "}":
            depth -= 1
            if depth == 0:
                break
        end += 1
    return pos, end


def fix_reorder(path: Path, text: str, sites: set[tuple[int, Path, int]], class_texts: dict) -> tuple[str, int]:
    """sites: (constructor line, file and line of a member declaration of its class)."""
    code = strip_code(text)
    offsets = line_offsets(text)
    regions = {}
    for line, decl_path, decl_line in sites:
        body = code.find("{", offsets[line - 1])
        # the ':' that starts the list: the last `) :` before the body
        colon = None
        for m in re.finditer(r"\)\s*(?:const\s*)?:(?!:)", code[:body]):
            colon = m.end()
        if colon is not None:
            regions[(colon, body)] = (decl_path, decl_line)
    edits = []
    for (colon, body), (decl_path, decl_line) in regions.items():
        if re.search(r"^\s*#", code[colon:body], re.M):
            print(f"skip {path}:{colon}: preprocessor lines in the list (sort by hand)", file=sys.stderr)
            continue
        if decl_path not in class_texts:
            ctext = text if decl_path == path else decl_path.read_text(encoding="utf-8")
            ccode = strip_code(ctext)
            class_texts[decl_path] = (ccode, line_offsets(ctext))
        ccode, coffsets = class_texts[decl_path]
        a, b = class_range(ccode, coffsets, decl_line)
        # the class's own level only: blank out member function bodies and nested types
        members, depth = [], 0
        for ch in ccode[a + 1:b]:
            if ch == "{":
                depth += 1
            members.append(ch if depth == 0 or ch == "\n" else " ")
            if ch == "}":
                depth -= 1
        members = "".join(members)

        # the initializers ("name( ... )" / "name{ ... }") at the top level of the list, in order
        seg = code[colon:body]
        cores, i = [], 0
        while i < len(seg):
            m = re.compile(r"[A-Za-z_]\w*\s*[({]").match(seg, i)
            if not m:
                i += 1
                continue
            depth, k = 0, m.end() - 1
            while k < len(seg):
                depth += seg[k] in "({"
                depth -= seg[k] in ")}"
                if depth == 0:
                    break
                k += 1
            name = re.match(r"\w+", seg[m.start():]).group(0)
            cores.append((m.start(), k + 1, name))
            i = k + 1
        # the class head ("class X : public Base, ..."): base classes are initialized before every member
        head = ccode[max(ccode.rfind(";", 0, a), ccode.rfind("}", 0, a)) + 1:a]
        keys = []
        for index, (_, _, name) in enumerate(cores):
            d = re.search(r"[\s*&>]" + re.escape(name) + r"\s*(;|\[|=|:(?!:)|,)", members)
            if not d and re.search(r"\b" + re.escape(name) + r"\b", head):
                keys.append(index - len(cores) - len(members))
                continue
            if not d:
                print(f"skip {path}:{colon}: member '{name}' not found in its class", file=sys.stderr)
                break
            keys.append(d.start())
        else:
            texts = [text[colon + x:colon + y] for x, y, _ in cores]
            ordered = [t for _, t in sorted(zip(keys, texts), key=lambda kt: kt[0])]
            if ordered != texts:
                new_seg, last = [], 0
                for (x, y, _), t in zip(cores, ordered):
                    new_seg.append(text[colon + last:colon + x])
                    new_seg.append(t)
                    last = y
                new_seg.append(text[colon + last:body])
                edits.append((colon, body, "".join(new_seg)))
    for a, b, new in sorted(edits, reverse=True):
        text = text[:a] + new + text[b:]
    return text, len(edits)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--root", default="src", help="only edit files under this directory")
    ap.add_argument("--only", default="switch,unused-parameter,reorder")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    only = set(args.only.split(","))
    root = Path(args.root).resolve()
    switches: dict[Path, set] = defaultdict(set)
    params: dict[Path, set] = defaultdict(set)
    reorders: dict[Path, set] = defaultdict(set)
    here = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):\d+: warning:\s+when initialized here \[-Wreorder\]")
    after = re.compile(r"^(?P<file>[^:\s]+):(?P<line>\d+):\d+: warning: .* will be initialized after \[-Wreorder\]")
    member_decl = None
    for raw in Path(args.log).read_text(errors="replace").splitlines():
        a = after.match(raw)
        if a:
            member_decl = (Path(a["file"]).resolve(), int(a["line"]))
            continue
        h = here.match(raw)
        if h and "reorder" in only and member_decl:
            path = Path(h["file"]).resolve()
            if root in path.parents:
                reorders[path].add((int(h["line"]),) + member_decl)
            continue
        m = PATTERN.match(raw)
        if not m:
            continue
        path = Path(m["file"]).resolve()
        if root not in path.parents:
            continue
        site = (int(m["line"]), int(m["col"]))
        if m["flag"] == "switch" and "switch" in only:
            switches[path].add(site)
        elif m["flag"] == "unused-parameter" and "unused-parameter" in only:
            q = QUOTED.search(m["msg"])
            if q:
                params[path].add(site + (q.group(1),))

    total = 0
    for path in sorted(set(switches) | set(params) | set(reorders)):
        text = path.read_text(encoding="utf-8")
        n = 0
        if path in switches:
            text, k = fix_switch(text, switches[path])
            n += k
        if path in params:
            text, k = fix_unused_parameter(text, params[path])
            n += k
        if path in reorders:
            text, k = fix_reorder(path, text, reorders[path], {})
            n += k
        if n and not args.dry_run:
            path.write_text(text, encoding="utf-8")
        print(f"{path}: {n}")
        total += n
    print(f"{total} fixes", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
