#!/usr/bin/env python3
"""List the places where the game's code lays out the UI (T52), for the widescreen migration.

T52 takes the layout out of X2Lib so the UI editor can place everything: a fixed spot becomes content (the dialog's
script, a named placement, a placement with a target), and a position that must stay computed is computed in canvas
coordinates. This tool finds the sites still to convert and guesses their kind:

  A  a dialog placed or moved to fixed values (SetPos / Move with literals)
  B  a dialog placed at fixed values of another dialog's element (only known by review: reported as A)
  C  a position derived at run time (the mouse, another element's position, a variable), kept in code; flagged
     `screen` when it mixes in 1024/768/512/384 literals, which do not follow the canvas
  D  a control moved inside its dialog (SetOffsetPos, control SetPos, picture points and sizes) with literals;
     the same calls with variables are reported as C (positions) or `data` (sizes, which stay code)
  E  a UI particle created at a layout point
  F  an anchor set by code
  G  a dialog or control built without a script

Usage:
  scan_placement.py [roots ...] [-o placement.tsv]      (default root: src/client/X2Lib)

Writes one row per site (file, line, function, kind, call, receiver, script, arguments, flags) and a summary on
stderr. Converted sites (ApplyPlacement and the like) are not matched, so the list shrinks as the migration
proceeds. Guesses are a starting point for review, not a classification.
"""
from __future__ import annotations

import argparse
import re
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DEFAULT_ROOTS = [REPO / "src/client/X2Lib"]

DIALOG_CALLS = ("SetPos", "Move")
CONTROL_POS_CALLS = ("SetOffsetPos", "SetPos")
PICTURE_CALLS = ("SetPoint", "SetSize", "SetSizeX", "SetSizeY")
SCREEN_LITERALS = re.compile(r"(?<![\w.])(1024|768|512|384)(?:\.0*)?f?(?![\w.])")
NUMBER = r"-?\s*\d+(?:\.\d*)?f?"
LITERAL_ARGS = re.compile(
    rf"^\s*(?:D3DXVECTOR[23]\s*\(\s*{NUMBER}(?:\s*,\s*{NUMBER}){{1,2}}\s*\)|{NUMBER})"
    rf"(?:\s*,\s*(?:D3DXVECTOR[23]\s*\(\s*{NUMBER}(?:\s*,\s*{NUMBER}){{1,2}}\s*\)|{NUMBER}|D3DXCOLOR\s*\([^()]*\)|true|false|\w+))*\s*$"
)
SENTINEL = re.compile(r"-\s*999(?:\.0*)?f?\b")  # the message boxes' "default spot" value
MOUSE = re.compile(r"Mouse|Cursor", re.IGNORECASE)

RECEIVER = r"([A-Za-z_]\w*(?:\s*\[[^\]]*\])?(?:\s*\(\s*\))?)"
CALL = re.compile(RECEIVER + r"\s*->\s*(SetPos|Move|SetOffsetPos|SetPoint|SetSize|SetSizeX|SetSizeY|SetAnchor)\s*\(")
PARTICLE = re.compile(r"GetUI(?:Major|Minor)Particle\s*\(\s*\)\s*->\s*(CreateSequence(?:Handle)?)\s*\(")
WINDOW_POS = re.compile(r"\bGetWindowPos\s*\(")
NEW_DIALOG = re.compile(r"(?:([A-Za-z_]\w*)(?:\s*\[[^\]]*\])?\s*=\s*)?new\s+CKTDGUIDialog\s*\(")
NEW_CONTROL = re.compile(
    r"(?:([A-Za-z_]\w*)\s*=\s*)?new\s+CKTDGUI(Static|Button|RadioButton|CheckBox|ListBox|IMEEditBox|EditBox|ComboBox|"
    r"ScrollBar|NavigationBox|WheelBox|Slider)\b"
)
# A definition `Type Class::Name(` (some of the original's are indented); not a statement such as `return X::Y(`.
FUNCTION = re.compile(r"^\s*(?!return\b|if\b|else\b|while\b|for\b|switch\b|case\b|delete\b|new\b)"
                      r"(?:[A-Za-z_][\w:<>,\*&\s~]*?\b)?([A-Za-z_]\w*::~?[A-Za-z_]\w*)\s*\(")

DIALOG_NAME = re.compile(r"DLG|Dlg|dlg|Dialog")
# A variable's declared type, as `Type* name`, `Type name =` or a parameter `Type* name)`.
DECLARATION = re.compile(r"\b([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*[\*&]?\s*(?:const\s+)?\*?\s*([A-Za-z_]\w*)\s*(?=[=;,)\[])")
UI_TYPE = re.compile(r"^(?:CKTDGUI\w*(?:::\w+)*|CPictureData|CKTDGUIControl::\w+|UIPointData|CString)$")
NOT_TYPES = {"return", "delete", "new", "else", "case", "goto", "sizeof", "const", "struct", "class", "typedef"}
UI_NAME = re.compile(r"DLG|Dlg|dlg|Dialog|Static|Button|Picture|Pic[A-Z_]|^pPic|Control|Radio|Check|List|Edit|Combo|"
                     r"Slot|Text|String|Gage|Gauge|Bar_|Bar$|Tab|Window", re.IGNORECASE)
PICTURE_NAME = re.compile(r"Picture|Pic\b|pPic")


@dataclass
class Site:
    file: str
    line: int
    function: str
    kind: str
    call: str
    receiver: str
    script: str
    args: str
    flags: str


def strip_comments(text: str) -> str:
    """Blank out comments and string contents' comment markers, keeping line numbers and string literals."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            i = n if j < 0 else j
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join("\n" if ch == "\n" else " " for ch in text[i:j]))
            i = j
        elif c == '"' or (c == "L" and i + 1 < n and text[i + 1] == '"' and (i == 0 or not text[i - 1].isalnum())):
            j = i + (2 if c == "L" else 1)
            while j < n and text[j] != '"':
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        elif c == "'":
            j = i + 1
            while j < n and text[j] != "'":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        else:
            out.append(c)
            i += 1
    return "".join(out)


def drop_disabled(text: str) -> str:
    """Blank out `#if 0` blocks (the original's disabled code), keeping line numbers."""
    lines = text.split("\n")
    depth = 0  # nesting inside a disabled block
    for k, line in enumerate(lines):
        s = line.strip()
        if depth:
            if re.match(r"#\s*if", s):
                depth += 1
            elif re.match(r"#\s*endif", s):
                depth -= 1
            elif depth == 1 and re.match(r"#\s*(else|elif)", s):
                depth = 0
            lines[k] = ""
        elif re.match(r"#\s*if\s+0\b", s):
            depth = 1
            lines[k] = ""
    return "\n".join(lines)


def call_args(text: str, open_paren: int) -> str:
    """The text between the parenthesis at open_paren and its match, on one line."""
    depth, j = 0, open_paren
    while j < len(text):
        if text[j] == "(":
            depth += 1
        elif text[j] == ")":
            depth -= 1
            if depth == 0:
                break
        elif text[j] == ";" and depth <= 1:
            break
        j += 1
    return re.sub(r"\s+", " ", text[open_paren + 1:j]).strip()


def receiver_key(receiver: str) -> str:
    return re.match(r"[A-Za-z_]\w*", receiver).group(0)


def scripts_by_variable(sources: dict[Path, str]) -> tuple[dict[Path, dict[str, set[str]]], dict[str, set[str]]]:
    """Dialog variables and the scripts assigned to them, per file and across all files (members are often made in
    one file and placed in another)."""
    per_file: dict[Path, dict[str, set[str]]] = {}
    everywhere: dict[str, set[str]] = defaultdict(set)
    for path, text in sources.items():
        local: dict[str, set[str]] = defaultdict(set)
        for m in NEW_DIALOG.finditer(text):
            if not m.group(1):
                continue
            args = call_args(text, m.end() - 1)
            s = re.search(r'L"([^"]+\.lua)"', args, re.IGNORECASE)
            if s:
                local[m.group(1)].add(s.group(1))
                everywhere[m.group(1)].add(s.group(1))
        per_file[path] = local
    return per_file, everywhere


def is_literal(args: str) -> bool:
    return bool(LITERAL_ARGS.match(args))


def classify(call: str, receiver: str, args: str) -> tuple[str, list[str]]:
    flags = []
    literal = is_literal(args)
    if SCREEN_LITERALS.search(args) and not literal:
        flags.append("screen")
    if MOUSE.search(args):
        flags.append("mouse")
    key = receiver_key(receiver)
    is_dialog = bool(DIALOG_NAME.search(key)) and not key.startswith(("pStatic", "pButton", "pControl"))
    if call == "SetAnchor":
        return "F", flags
    if call in PICTURE_CALLS or (call == "SetPos" and PICTURE_NAME.search(key)):
        if literal:
            return "D", flags
        return ("data" if call.startswith("SetSize") else "C"), flags
    if is_dialog and call in DIALOG_CALLS:
        return ("A" if literal else "C"), flags
    if call in CONTROL_POS_CALLS:
        return ("D" if literal else "C"), flags
    return "C", flags


def split_args(args: str) -> list[str]:
    """Top-level comma-separated arguments."""
    parts, depth, cur = [], 0, []
    for ch in args:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    if cur or parts:
        parts.append("".join(cur).strip())
    return parts


@dataclass
class Function:
    name: str      # Class::Name
    start: int     # 0-based line of its definition
    params: list[str]


def enclosing_functions(text: str) -> tuple[list[str], list[Function | None]]:
    """The function each line belongs to (definitions start at column 0, as in the original), with its parameters."""
    lines = text.split("\n")
    names: list[str] = []
    owners: list[Function | None] = []
    current: Function | None = None
    for k, line in enumerate(lines):
        m = FUNCTION.match(line)
        if m and not line.rstrip().endswith(";"):
            signature = "\n".join(lines[k:k + 8])
            open_paren = signature.index("(", m.start(1))
            params = []
            for p in split_args(call_args(signature, open_paren)):
                p = p.split("=")[0].strip()
                ident = re.findall(r"[A-Za-z_]\w*", p)
                params.append(ident[-1] if ident else "")
            current = Function(m.group(1), k, params)
        names.append(current.name if current else "")
        owners.append(current)
    return names, owners


def literal_vector(value: str) -> bool:
    return bool(re.fullmatch(rf"\s*D3DXVECTOR[23]\s*\(\s*{NUMBER}(?:\s*,\s*{NUMBER}){{1,2}}\s*\)\s*", value))


def assignments(var: str, text_lines: list[str], begin: int, end: int) -> list[str]:
    """What a local vector is set to between two lines (its declaration and plain assignments); an adjustment
    (`v -= ...`, `v.x = ...`) is returned as is, so the caller sees it is not a plain literal."""
    found = []
    v = re.escape(var)
    block = "\n".join(text_lines[begin:end])
    for m in re.finditer(rf"(?:\bD3DXVECTOR[23]\s+{v}\s*(\([^;]*\))|\b{v}\s*=\s*([^;=][^;]*)|\b{v}\s*(\.[xyz]\s*=|[-+*/]=)\s*([^;]*));", block):
        if m.group(1):
            found.append("D3DXVECTOR2" + m.group(1))
        elif m.group(2):
            found.append(re.sub(r"\s+", " ", m.group(2)).strip())
        else:
            found.append(var + m.group(3) + " " + re.sub(r"\s+", " ", m.group(4)).strip())
    return found


def declared_types(texts: list[str]) -> dict[str, set[str]]:
    """Every type each variable name is declared with, across the sources and headers."""
    types: dict[str, set[str]] = defaultdict(set)
    for text in texts:
        for m in DECLARATION.finditer(text):
            if m.group(1) not in NOT_TYPES:
                types[m.group(2)].add(m.group(1))
    return types


def is_ui(receiver: str, types: dict[str, set[str]]) -> bool:
    """Whether a call's receiver is UI (a dialog, a control or a picture), by its declared type or else its name.
    Meshes, particles, units and effects also have SetPos and Move."""
    key = receiver_key(receiver)
    known = types.get(key, set())
    if any(UI_TYPE.match(t) for t in known):
        return True
    if any(t.startswith(("CKTDG", "CX2", "CKTDX")) for t in known):
        return False
    return bool(UI_NAME.search(key))


def scan(roots: list[Path]) -> list[Site]:
    sources: dict[Path, str] = {}
    headers: list[str] = []
    header_sources: dict[Path, str] = {}
    for root in roots:
        files = [root] if root.is_file() else sorted(root.rglob("*.cpp"))
        for path in files:
            raw = path.read_bytes().decode("utf-8", errors="replace")
            sources[path] = drop_disabled(strip_comments(raw))
        if root.is_dir():
            for h in sorted(root.rglob("*.h")):
                header_sources[h] = drop_disabled(strip_comments(h.read_bytes().decode("utf-8", errors="replace")))
            headers += list(header_sources.values())
    per_file, everywhere = scripts_by_variable(sources)
    types = declared_types(list(sources.values()) + headers)

    sites: list[Site] = []
    wrappers: list[tuple[Function, int, str, str, str]] = []
    for path, text in sources.items():
        try:
            rel = str(path.relative_to(REPO))
        except ValueError:
            rel = str(path)
        functions, owners = enclosing_functions(text)
        text_lines = text.split("\n")

        def line_of(pos: int) -> int:
            return text.count("\n", 0, pos) + 1

        def script_of(receiver: str) -> str:
            key = receiver_key(receiver)
            names = per_file[path].get(key) or everywhere.get(key) or set()
            return ",".join(sorted(names))

        def add(pos: int, kind: str, call: str, receiver: str, script: str, args: str, flags: list[str]):
            ln = line_of(pos)
            sites.append(Site(rel, ln, functions[ln - 1], kind, call, receiver, script, args, ",".join(flags)))

        for m in CALL.finditer(text):
            receiver, call = re.sub(r"\s+", "", m.group(1)), m.group(2)
            if not is_ui(receiver, types):
                continue
            args = call_args(text, m.end() - 1)
            kind, flags = classify(call, receiver, args)
            script = script_of(receiver)
            first = split_args(args)[0] if args else ""
            if kind == "C" and re.fullmatch(r"[A-Za-z_]\w*", first):
                ln = line_of(m.start()) - 1
                owner = owners[ln]
                values = assignments(first, text_lines, owner.start if owner else max(0, ln - 80), ln + 1)
                if values and all(literal_vector(v) for v in values):
                    kind = "A" if call in DIALOG_CALLS and DIALOG_NAME.search(receiver_key(receiver)) else "D"
                    flags.append("via:" + first)
                    args += " = " + " | ".join(sorted(set(values)))
                elif owner and first in owner.params and all(v.startswith(first + ".") or v.startswith(first + " ") or
                                                             v.startswith(first + "-") or v.startswith(first + "+")
                                                             for v in values):
                    flags.append("param")
                    wrappers.append((owner, owner.params.index(first), call, receiver, script))
            add(m.start(), kind, call, receiver, script, args, flags)
        for m in PARTICLE.finditer(text):
            args = call_args(text, m.end() - 1)
            name = re.search(r'L"([^"]+)"', args)
            flags = ["screen"] if SCREEN_LITERALS.search(args) else []
            add(m.start(), "E", m.group(1), "", name.group(1) if name else "", args, flags)
        for m in WINDOW_POS.finditer(text):
            if text[max(0, m.start() - 2):m.start()] == "::" and functions[line_of(m.start()) - 1].endswith("GetWindowPos"):
                continue
            add(m.start(), "C", "GetWindowPos", "", "", call_args(text, m.end() - 1), [])
        for m in NEW_DIALOG.finditer(text):
            args = call_args(text, m.end() - 1)
            parts = [p.strip() for p in args.split(",")]
            if len(parts) >= 2 and parts[1] in ("NULL", "nullptr", 'L""'):
                add(m.start(), "G", "new CKTDGUIDialog", m.group(1) or "", "", args, [])
        for m in NEW_CONTROL.finditer(text):
            add(m.start(), "G", "new CKTDGUI" + m.group(2), m.group(1) or "", "", "", [])

    sites += through_wrappers({**sources, **header_sources}, wrappers)
    sites.sort(key=lambda s: (s.file, s.line))
    return sites


# Methods too common to follow by name: a call `SetPos(` is not necessarily the wrapper's.
GENERIC = {"SetPos", "SetPosition", "Move", "SetOffsetPos", "SetSize", "SetPoint", "SetShow", "SetString", "Create",
           "Init", "Open", "Update", "OnFrameMove", "SetCenter"}
MAX_CALLS = 60  # a wrapper defined in several classes and called more often than this is ambiguous by name


def inline_owner(text_lines: list[str], ln: int, param: str) -> Function | None:
    """For a call in a header's inline method (not at column 0): the nearest signature above it declaring param."""
    for k in range(ln, max(-1, ln - 15), -1):
        m = re.search(r"\b([A-Za-z_]\w*)\s*\(", text_lines[k])
        if not m or m.group(1) in ("if", "for", "while", "switch", "return"):
            continue
        signature = "\n".join(text_lines[k:ln + 1])
        args = call_args(signature, signature.index("(", m.start(1)))
        params = []
        for a in split_args(args):
            ident = re.findall(r"[A-Za-z_]\w*", a.split("=")[0])
            params.append(ident[-1] if ident else "")
        if param in params and re.search(rf"\w[\w:<>]*\s*[\*&]?\s*\b{re.escape(param)}\s*(?:[,=)]|$)", args):
            return Function(m.group(1), k, params)
    return None


def through_wrappers(sources: dict[Path, str], wrappers: list[tuple[Function, int, str, str, str]]) -> list[Site]:
    """Fixed spots passed to a function that places a dialog with its parameter (the tutorial pointer:
    CreateMouseClickUI(stage, D3DXVECTOR2(805, 215)) -> MouseClickUI(..., vPos, ...) -> SetPos(vPos)), followed
    through functions that pass their own parameter on, up to a few levels."""
    found: list[Site] = []
    seen: set[tuple[str, int]] = set()
    queue = list(wrappers)
    while queue:
        owner, index, call, receiver, script = queue.pop()
        short = owner.name.split("::")[-1]
        if (owner.name, index) in seen or short in GENERIC:
            continue
        seen.add((owner.name, index))
        pattern = re.compile(rf"\b{re.escape(short)}\s*\(")
        calls = sum(len(pattern.findall(t)) for t in sources.values())
        definitions = sum(len(re.findall(rf"::{re.escape(short)}\s*\(", t)) for t in sources.values())
        if calls > MAX_CALLS and definitions > 1:
            print(f"  not followed (called {calls} times by name): {owner.name}", file=sys.stderr)
            continue
        for path, text in sources.items():
            functions, owners = enclosing_functions(text)
            text_lines = text.split("\n")
            rel = str(path.relative_to(REPO)) if path.is_relative_to(REPO) else str(path)
            for m in pattern.finditer(text):
                ln = text.count("\n", 0, m.start())
                caller = owners[ln]
                if caller and caller.start == ln and caller.name.split("::")[-1] == short:
                    continue  # the definition itself
                args = split_args(call_args(text, m.end() - 1))
                if index >= len(args):
                    continue
                value = args[index]
                if re.fullmatch(r"(?:const\s+)?[\w:<>]+\s*[\*&]?\s+[\*&]?\w+(?:\s*=.*)?", value):
                    continue  # a declaration, not a call
                flags = ["through:" + short]
                if literal_vector(value):
                    shown = value
                elif re.fullmatch(r"[A-Za-z_]\w*", value):
                    if path.suffix == ".h":
                        caller = inline_owner(text_lines, ln, value) or caller
                    values = assignments(value, text_lines, caller.start if caller else max(0, ln - 80), ln + 1)
                    if caller and value in caller.params and not values:
                        queue.append((caller, caller.params.index(value), call, receiver, script))
                        continue
                    if not values or not all(literal_vector(v) for v in values):
                        found.append(Site(rel, ln + 1, functions[ln], "C", short, receiver, script,
                                          value + (" = " + " | ".join(values) if values else ""), ",".join(flags)))
                        continue
                    shown = value + " = " + " | ".join(sorted(set(values)))
                else:
                    found.append(Site(rel, ln + 1, functions[ln], "C", short, receiver, script, value, ",".join(flags)))
                    continue
                if SENTINEL.search(shown):
                    flags.append("default")  # (-999, -999): the function's own default spot, not a fixed one
                    found.append(Site(rel, ln + 1, functions[ln], "C", short, receiver, script, shown, ",".join(flags)))
                    continue
                found.append(Site(rel, ln + 1, functions[ln], "A", short, receiver, script, shown, ",".join(flags)))
    return found


def write_tsv(sites: list[Site], out) -> None:
    out.write("# file\tline\tfunction\tkind\tcall\treceiver\tscript\targs\tflags\n")
    for s in sites:
        out.write("\t".join(str(v) for v in (s.file, s.line, s.function, s.kind, s.call, s.receiver, s.script,
                                             s.args.replace("\t", " "), s.flags)) + "\n")


def summary(sites: list[Site]) -> str:
    kinds = Counter(s.kind for s in sites)
    lines = [f"{len(sites)} sites in {len({s.file for s in sites})} files"]
    lines.append("  by kind: " + ", ".join(f"{k} {kinds[k]}" for k in sorted(kinds)))
    screen = sum(1 for s in sites if "screen" in s.flags.split(","))
    lines.append(f"  computed with 1024/768/512/384 literals (screen): {screen}")
    spots: dict[str, set[str]] = defaultdict(set)
    for s in sites:
        if s.kind == "A" and s.script:
            for script in s.script.split(","):
                spots[script].add(s.args)
    multi = sorted(((len(v), k) for k, v in spots.items() if len(v) > 1), reverse=True)
    if multi:
        lines.append("  dialogs placed at several fixed spots (named placements): "
                     + ", ".join(f"{k} {n}" for n, k in multi))
    through: dict[str, Counter] = defaultdict(Counter)
    for s in sites:
        for f in s.flags.split(","):
            if f.startswith("through:"):
                through[f[8:]][s.kind] += 1
    if through:
        lines.append("  placed through a function (fixed spots at its callers): "
                     + ", ".join(f"{w} {sum(c.values())} ({c['A']} fixed)" for w, c in
                                 sorted(through.items(), key=lambda i: -sum(i[1].values()))))
    files = Counter(Path(s.file).name for s in sites)
    lines.append("  most sites: " + ", ".join(f"{f} {n}" for f, n in files.most_common(12)))
    return "\n".join(lines)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("roots", nargs="*", type=Path, help="source folders or files (default: src/client/X2Lib)")
    parser.add_argument("-o", "--output", type=Path, help="TSV to write (default: stdout)")
    args = parser.parse_args(argv)
    sites = scan([r.resolve() for r in args.roots] or DEFAULT_ROOTS)
    if args.output:
        with args.output.open("w", encoding="utf-8", newline="\n") as f:
            write_tsv(sites, f)
    else:
        write_tsv(sites, sys.stdout)
    print(summary(sites), file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
