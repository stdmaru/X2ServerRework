"""Simulates the preprocessor over the configuration headers (ServerDefine.h, KTDX.h...) to find which
feature macros a build really defines. Closed world: anything not defined is undefined.

Only what configuration headers need is supported: #if/#ifdef/#ifndef/#elif/#else/#endif, #define/#undef
and quoted #include. Everything else is ignored.
"""

import re
from dataclasses import dataclass, field
from pathlib import Path

from . import condres, includes, textio

_IDENT_RE = re.compile(r"[A-Za-z_]\w*")
_DEFINED_RE = re.compile(r"\bdefined\s*(?:\(\s*([A-Za-z_]\w*)\s*\)|([A-Za-z_]\w*))")
_COMMENTED_DEFINE_RE = re.compile(r"^\s*//+\s*#\s*define\s+([A-Za-z_]\w*)(?!\s*\()", re.M)


@dataclass
class SimResult:
    defined: dict[str, str] = field(default_factory=dict)  # final macro table (name -> value)
    files: list[Path] = field(default_factory=list)          # files visited, in order
    mentioned: set[str] = field(default_factory=set)        # names in any #define/#undef, active or not
    function_like: set[str] = field(default_factory=set)
    unresolved_includes: list[tuple[Path, str]] = field(default_factory=list)
    skipped_includes: list[tuple[Path, Path]] = field(default_factory=list)  # not followed (see `follow`)
    unsupported: list[tuple[Path, int, str]] = field(default_factory=list)


def _substitute(expr: str, table: dict[str, str]) -> str:
    """defined(X) -> 0/1, then identifiers -> numeric value or 0 (C semantics for undefined names)."""
    expr = _DEFINED_RE.sub(lambda m: "1" if (m.group(1) or m.group(2)) in table else "0", expr)

    def ident(m: re.Match) -> str:
        val = table.get(m.group(0), "").strip()
        return val if re.fullmatch(r"\(?-?\d+[uUlL]*\)?|0[xX][0-9a-fA-F]+", val) else "0"

    return _IDENT_RE.sub(ident, expr)


def evaluate_closed(expr: str, table: dict[str, str]) -> int | None:
    val, _ = condres.evaluate(_substitute(expr, table), condres.Flags())
    return val if isinstance(val, int) else None


def simulate(entry: Path | list[Path], initial: dict[str, str], include_dirs: list[Path], read=None,
             follow=None) -> SimResult:
    """Runs `entry` (or several entries in order, sharing one macro table, as when a translation unit includes
    them one after another). `read(path) -> bytes` lets callers verify integrity of what is read (defaults to
    Path.read_bytes). `follow(path) -> bool` limits which included files are entered (default: all)."""
    read = read or (lambda p: p.read_bytes())
    follow = follow or (lambda p: True)
    res = SimResult(defined=dict(initial))
    index = includes.CaseIndex()
    for e in entry if isinstance(entry, list) else [entry]:
        _run_file(e, res, include_dirs, index, read, follow, depth=0)
    return res


def scan_mentions(path: Path, read=None) -> tuple[set[str], set[str]]:
    """Names in #define/#undef (also commented out) and function-like macros of a config header."""
    read = read or (lambda p: p.read_bytes())
    text = textio.normalize(textio.decode(read(path)).text)
    names, funcs = set(_COMMENTED_DEFINE_RE.findall(text)), set()
    for _, line in condres._logical_lines(text):
        m = condres._DIRECTIVE_RE.match(line)
        if not m or m.group(3) not in ("define", "undef"):
            continue
        code, _ = condres._strip_comments(m.group(4))
        mm = re.match(r"\s*([A-Za-z_]\w*)(\()?", code)
        if mm:
            (funcs if mm.group(2) else names).add(mm.group(1))
    return names - funcs, funcs


def live_lines(text: str, table: dict[str, str]) -> list[str] | None:
    """Fully preprocesses `text` (conditionals and local #define/#undef, no includes) under `table` and
    returns the surviving non-directive lines. None if a condition cannot be evaluated."""
    table = dict(table)
    out: list[str] = []
    stack: list[list[bool]] = []
    in_comment = False

    def live() -> bool:
        return not stack or stack[-1][2]

    for _, line in condres._logical_lines(text):
        m = None if in_comment else condres._DIRECTIVE_RE.match(line)
        in_comment = condres._scan_comment_state(line, in_comment)
        if not m:
            if live():
                out.append(line)
            continue
        kw = m.group(3)
        code, _ = condres._strip_comments(m.group(4))
        code = code.replace("\\\n", " ").strip()
        if kw in ("if", "ifdef", "ifndef"):
            if not live():
                stack.append([False, True, False])
                continue
            if kw == "if":
                v = evaluate_closed(code, table)
                if v is None:
                    return None
            else:
                name = code.split()[0] if code else ""
                v = int((name in table) == (kw == "ifdef"))
            stack.append([True, bool(v), bool(v)])
        elif kw in ("elif", "else"):
            if not stack:
                return None
            g = stack[-1]
            if not g[0] or g[1]:
                g[2] = False
                continue
            v = 1 if kw == "else" else evaluate_closed(code, table)
            if v is None:
                return None
            g[1] = g[2] = bool(v)
        elif kw == "endif":
            if not stack:
                return None
            stack.pop()
        elif not live():
            continue
        elif kw == "define":
            mm = re.match(r"([A-Za-z_]\w*)(\([^)]*\))?\s*(.*)", code, re.S)
            if mm:
                table[mm.group(1)] = "" if mm.group(2) else mm.group(3).strip()
                out.append(f"#define {mm.group(1)}")  # keep defines visible to the comparison
        elif kw == "undef":
            if code:
                table.pop(code.split()[0], None)
                out.append(f"#undef {code.split()[0]}")
        else:
            out.append(line.strip())  # include, pragma, error...
    return out


def _run_file(path: Path, res: SimResult, include_dirs, index, read, follow, depth: int):
    if depth > 32:
        raise RuntimeError(f"include depth exceeded at {path}")
    res.files.append(path)
    names, funcs = scan_mentions(path, read)
    res.mentioned |= names
    res.function_like |= funcs

    text = textio.normalize(textio.decode(read(path)).text)
    stack: list[list[bool]] = []  # [parent_live, taken, live]
    in_comment = False

    def live() -> bool:
        return not stack or stack[-1][2]

    for lineno, line in condres._logical_lines(text):
        m = None if in_comment else condres._DIRECTIVE_RE.match(line)
        in_comment = condres._scan_comment_state(line, in_comment)
        if not m:
            continue
        kw, tail = m.group(3), m.group(4)
        code, _ = condres._strip_comments(tail)
        code = code.replace("\\\n", " ").strip()

        if kw in ("if", "ifdef", "ifndef"):
            parent = live()
            if not parent:
                stack.append([False, True, False])
                continue
            if kw == "if":
                v = evaluate_closed(code, res.defined)
                if v is None:
                    res.unsupported.append((path, lineno, line.strip()))
                    v = 0
            else:
                name = code.split()[0] if code else ""
                v = int((name in res.defined) == (kw == "ifdef"))
            stack.append([True, bool(v), bool(v)])
        elif kw in ("elif", "else"):
            if not stack:
                continue
            g = stack[-1]
            if not g[0] or g[1]:
                g[2] = False
                continue
            if kw == "else":
                v = 1
            else:
                v = evaluate_closed(code, res.defined)
                if v is None:
                    res.unsupported.append((path, lineno, line.strip()))
                    v = 0
            g[1] = g[2] = bool(v)
        elif kw == "endif":
            if stack:
                stack.pop()
        elif not live():
            continue
        elif kw == "define":
            mm = re.match(r"([A-Za-z_]\w*)(\([^)]*\))?\s*(.*)", code, re.S)
            if mm:
                res.defined[mm.group(1)] = "" if mm.group(2) else mm.group(3).strip()
        elif kw == "undef":
            if code:
                res.defined.pop(code.split()[0], None)
        elif kw == "include":
            mm = re.match(r'"([^"]+)"', code)
            if not mm:
                continue
            hit = includes.resolve_include(index, path, '"', mm.group(1), include_dirs)
            if hit is None:
                res.unresolved_includes.append((path, mm.group(1)))
            elif not follow(hit):
                res.skipped_includes.append((path, hit))
            else:
                _run_file(hit, res, include_dirs, index, read, follow, depth + 1)
