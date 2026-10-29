"""Partial preprocessor: resolves #if/#ifdef/#elif groups for a known set of macros.

Like unifdef/coan, but with one extra rule required by the project (docs/analise/debug-e-dev.md):
macros in the "dev" set are not resolved but rewritten to the X2_DEV_TOOLS switch.

Each macro is one of:
  true      - defined in the target build
  false     - not defined in the target build
  dev       - defined only in development builds  -> X2_DEV_TOOLS
  dev_not   - defined only in non-development builds -> !X2_DEV_TOOLS
  (absent)  - unknown: conditions using it are kept as they are (_WIN32, _MSC_VER, guards...)
"""

import re
from dataclasses import dataclass, field

DEV_SWITCH = "X2_DEV_TOOLS"

_DIRECTIVE_RE = re.compile(r"^(\s*)#(\s*)(\w+)(.*)$", re.S)
_TOKEN_RE = re.compile(
    r"\s*(?:(?P<num>0[xX][0-9a-fA-F]+|\d+)[uUlL]*|(?P<id>[A-Za-z_]\w*)"
    r"|(?P<op>&&|\|\||==|!=|<=|>=|<<|>>|[!()<>+\-*/%&|^~?:,]))")


@dataclass
class Flags:
    true: set[str] = field(default_factory=set)
    false: set[str] = field(default_factory=set)
    false_patterns: list[re.Pattern] = field(default_factory=list)
    dev: set[str] = field(default_factory=set)
    dev_not: set[str] = field(default_factory=set)

    def state(self, name: str) -> str | None:
        if name in self.dev:
            return "dev"
        if name in self.dev_not:
            return "dev_not"
        if name in self.true:
            return "true"
        if name in self.false or any(p.match(name) for p in self.false_patterns):
            return "false"
        return None

    def is_flag(self, name: str) -> bool:
        return self.state(name) is not None


# --- expression evaluation -------------------------------------------------------------------

@dataclass
class Res:
    """Residual (unresolved) expression."""
    text: str
    prec: int          # precedence of the outermost operator, 100 = atom
    dev: bool = False  # depends on X2_DEV_TOOLS


Val = int | Res

# binary operators: precedence (higher binds tighter)
_BIN = {"||": 1, "&&": 2, "|": 3, "^": 4, "&": 5, "==": 6, "!=": 6, "<": 7, ">": 7, "<=": 7, ">=": 7,
        "<<": 8, ">>": 8, "+": 9, "-": 9, "*": 10, "/": 10, "%": 10}


class _Unsupported(Exception):
    pass


class _Parser:
    def __init__(self, text: str, flags: Flags):
        self.toks = self._tokenize(text)
        self.i = 0
        self.flags = flags
        self.touched = False  # some known macro was resolved

    @staticmethod
    def _tokenize(text: str) -> list[str]:
        toks, pos = [], 0
        text = text.strip()
        while pos < len(text):
            m = _TOKEN_RE.match(text, pos)
            if not m or m.end() == pos:
                raise _Unsupported(text)
            toks.append(m.group("num") or m.group("id") or m.group("op"))
            pos = m.end()
        return toks

    def peek(self) -> str | None:
        return self.toks[self.i] if self.i < len(self.toks) else None

    def take(self, expected: str | None = None) -> str:
        tok = self.peek()
        if tok is None or (expected is not None and tok != expected):
            raise _Unsupported(" ".join(self.toks))
        self.i += 1
        return tok

    def parse(self) -> Val:
        val = self.binary(1)
        if self.peek() is not None:  # ternary, comma, ... -> not supported
            raise _Unsupported(" ".join(self.toks))
        return val

    def binary(self, min_prec: int) -> Val:
        left = self.unary()
        while (op := self.peek()) in _BIN and _BIN[op] >= min_prec:
            self.take()
            prec = _BIN[op]
            if op in ("&&", "||"):
                right = self.binary(prec + 1)
                left = _logic(op, left, right)
            else:
                right = self.binary(prec + 1)
                left = _arith(op, prec, left, right)
        return left

    def unary(self) -> Val:
        tok = self.peek()
        if tok == "!":
            self.take()
            return _not(self.unary())
        if tok in ("-", "+", "~"):
            self.take()
            v = self.unary()
            if isinstance(v, int):
                return {"-": -v, "+": v, "~": ~v}[tok]
            return Res(tok + _wrap(v, 100), 100, v.dev)
        return self.primary()

    def primary(self) -> Val:
        tok = self.take()
        if tok == "(":
            v = self.binary(1)
            self.take(")")
            return v if isinstance(v, int) else Res("(" + v.text + ")", 100, v.dev)
        if tok == "defined":
            paren = self.peek() == "("
            if paren:
                self.take("(")
            name = self.take()
            if paren:
                self.take(")")
            return self.symbol(name, f"defined({name})", as_defined=True)
        if tok[0].isdigit():
            return int(tok, 0)
        if re.match(r"[A-Za-z_]", tok):
            if self.peek() == "(":  # function-like macro call: keep as is
                depth, parts = 0, [tok]
                while True:
                    t = self.take()
                    parts.append(t)
                    depth += t == "("
                    depth -= t == ")"
                    if depth == 0:
                        break
                return Res("".join(parts), 100)
            return self.symbol(tok, tok, as_defined=False)
        raise _Unsupported(" ".join(self.toks))

    def symbol(self, name: str, text: str, as_defined: bool) -> Val:
        st = self.flags.state(name)
        if st is None or name == DEV_SWITCH:
            return Res(text, 100)
        if st == "dev":
            self.touched = True
            return Res(DEV_SWITCH, 100, True)
        if st == "dev_not":
            self.touched = True
            return Res("!" + DEV_SWITCH, 100, True)
        if st == "false":
            self.touched = True
            return 0
        # true: `defined(X)` is 1; a bare `X` would need its value, which feature flags do not have
        if as_defined:
            self.touched = True
            return 1
        return Res(text, 100)


def _wrap(v: Val, prec: int) -> str:
    if isinstance(v, int):
        return str(v)
    return v.text if v.prec >= prec else "(" + v.text + ")"


def _not(v: Val) -> Val:
    if isinstance(v, int):
        return int(not v)
    if v.text == DEV_SWITCH:
        return Res("!" + DEV_SWITCH, 100, True)
    if v.text == "!" + DEV_SWITCH:
        return Res(DEV_SWITCH, 100, True)
    return Res("!" + _wrap(v, 100), 100, v.dev)


def _logic(op: str, a: Val, b: Val) -> Val:
    absorbing, neutral = (0, 1) if op == "&&" else (1, 0)
    for x, y in ((a, b), (b, a)):
        if isinstance(x, int):
            if bool(x) == bool(absorbing):
                return absorbing
            return int(bool(y)) if isinstance(y, int) else y
    prec = _BIN[op]
    return Res(f"{_wrap(a, prec)} {op} {_wrap(b, prec + 1)}", prec, a.dev or b.dev)


def _arith(op: str, prec: int, a: Val, b: Val) -> Val:
    if isinstance(a, int) and isinstance(b, int):
        if op in ("/", "%") and b == 0:
            raise _Unsupported(op)
        return int(eval(f"{a} {op} {b}".replace("/", "//")))  # noqa: S307 - ints only
    return Res(f"{_wrap(a, prec)} {op} {_wrap(b, prec + 1)}", prec,
               getattr(a, "dev", False) or getattr(b, "dev", False))


def evaluate(expr: str, flags: Flags) -> tuple[Val, bool]:
    """Returns (value or residual, whether any known macro took part)."""
    p = _Parser(expr, flags)
    try:
        v = p.parse()
    except _Unsupported:
        return Res(expr.strip(), 0), False
    return v, p.touched


# --- line handling -----------------------------------------------------------------------------

def _strip_comments(s: str) -> tuple[str, str]:
    """Splits a directive tail into (code, trailing comment) without touching string literals."""
    out, i, comment = [], 0, ""
    while i < len(s):
        c = s[i]
        if c in "\"'":
            j = i + 1
            while j < len(s) and s[j] != c:
                j += 2 if s[j] == "\\" else 1
            out.append(s[i:j + 1])
            i = j + 1
        elif s.startswith("//", i):
            comment = s[i:]
            break
        elif s.startswith("/*", i):
            j = s.find("*/", i + 2)
            if j < 0:
                comment = s[i:]
                break
            comment += s[i:j + 2]
            out.append(" ")
            i = j + 2
        else:
            out.append(c)
            i += 1
    return "".join(out), comment.strip()


def _scan_comment_state(line: str, in_comment: bool) -> bool:
    """Returns whether a /* */ comment is still open at the end of `line`."""
    i = 0
    while i < len(line):
        if in_comment:
            j = line.find("*/", i)
            if j < 0:
                return True
            in_comment, i = False, j + 2
            continue
        c = line[i]
        if c in "\"'":
            j = i + 1
            while j < len(line) and line[j] != c:
                j += 2 if line[j] == "\\" else 1
            i = j + 1
        elif line.startswith("//", i):
            return False
        elif line.startswith("/*", i):
            in_comment, i = True, i + 2
        else:
            i += 1
    return in_comment


@dataclass
class DevBlock:
    line: int          # 1-based line in the original file
    original: str      # original directive text
    emitted: str       # rewritten directive


@dataclass
class Result:
    text: str
    groups_resolved: int = 0
    lines_removed: int = 0
    defines_removed: list[str] = field(default_factory=list)
    pragma_libs: list[str] = field(default_factory=list)
    dev_blocks: list[DevBlock] = field(default_factory=list)
    unsupported: list[tuple[int, str]] = field(default_factory=list)
    unbalanced: bool = False


@dataclass
class _Group:
    parent_live: bool
    emitted_if: bool = False    # an #if line was written for this group
    taken: bool = False         # a branch known to be true was already seen
    live: bool = False          # current branch is being emitted
    resolved_any: bool = False


def _logical_lines(text: str) -> list[tuple[int, str]]:
    """Joins backslash-continued lines. Returns (first line number, text)."""
    out, buf, start = [], [], 0
    for n, line in enumerate(text.split("\n"), 1):
        if not buf:
            start = n
        if line.endswith("\\"):
            buf.append(line)
            continue
        buf.append(line)
        out.append((start, "\n".join(buf)))
        buf = []
    if buf:
        out.append((start, "\n".join(buf)))
    return out


def _fmt(indent: str, hspace: str, kw: str, expr: str, comment: str) -> str:
    line = f"{indent}#{hspace}{kw}"
    if expr:
        line += " " + expr
    if comment:
        line += " " + comment
    return line


_PRAGMA_LIB_RE = re.compile(r'\(\s*lib\s*,\s*"([^"]+)"')


def resolve(text: str, flags: Flags) -> Result:
    res = Result(text="")
    out: list[str] = []
    stack: list[_Group] = []
    in_comment = False

    def live() -> bool:
        return not stack or stack[-1].live

    for lineno, line in _logical_lines(text):
        m = None if in_comment else _DIRECTIVE_RE.match(line)
        in_comment = _scan_comment_state(line, in_comment)
        if not m:
            if live():
                out.append(line)
            else:
                res.lines_removed += line.count("\n") + 1
            continue

        indent, hspace, kw, tail = m.groups()
        code, comment = _strip_comments(tail)
        code = code.replace("\\\n", " ")

        if kw in ("if", "ifdef", "ifndef"):
            g = _Group(parent_live=live())
            stack.append(g)
            if not g.parent_live:
                res.lines_removed += 1
                continue
            if kw == "if":
                val, touched = evaluate(code, flags)
            else:
                name = code.strip()
                val, touched = evaluate(("!" if kw == "ifndef" else "") + f"defined({name})", flags)
            _open_branch(g, kw, val, touched, line, indent, hspace, comment, lineno, out, res)
        elif kw in ("elif", "else"):
            if not stack:
                res.unbalanced = True
                out.append(line)
                continue
            g = stack[-1]
            if not g.parent_live:
                res.lines_removed += 1
                continue
            if g.taken:
                g.live = False
                res.lines_removed += 1
                continue
            if kw == "else":
                val, touched = 1, False
            else:
                val, touched = evaluate(code, flags)
            _next_branch(g, kw, val, touched, line, indent, hspace, comment, lineno, out, res)
        elif kw == "endif":
            if not stack:
                res.unbalanced = True
                out.append(line)
                continue
            g = stack.pop()
            if g.parent_live and g.emitted_if:
                # MSVC accepts `#endif FOO`; keep the name as a comment instead
                extra = code.strip()
                c = comment or (f"// {extra}" if extra else "")
                out.append(_fmt(indent, hspace, "endif", "", c))
            else:
                res.lines_removed += 1
            if g.parent_live and g.resolved_any:
                res.groups_resolved += 1
        else:
            if not live():
                res.lines_removed += line.count("\n") + 1
                continue
            if kw in ("define", "undef"):
                parts = code.split(None, 1)
                name = parts[0] if parts else ""
                bare = "(" not in name and len(parts) == 1
                if name and bare and flags.is_flag(name):
                    res.defines_removed.append(name)
                    res.lines_removed += 1
                    continue
            if kw == "pragma":
                pm = _PRAGMA_LIB_RE.search(code)
                if code.strip().startswith("comment") and pm:
                    res.pragma_libs.append(pm.group(1))
                    res.lines_removed += 1
                    continue
            out.append(line)

    if stack:
        res.unbalanced = True
    res.text = "\n".join(out)
    return res


def _open_branch(g, kw, val, touched, line, indent, hspace, comment, lineno, out, res):
    if isinstance(val, Res) and not touched:
        # nothing known involved: keep the original directive untouched
        g.emitted_if, g.live = True, True
        out.append(line)
        if val.prec == 0:
            res.unsupported.append((lineno, line.strip()))
        return
    g.resolved_any = True
    if isinstance(val, int):
        g.live = bool(val)
        g.taken = bool(val)
        res.lines_removed += 1
        return
    g.emitted_if, g.live = True, True
    new = _fmt(indent, hspace, "if", val.text, comment)
    out.append(new)
    if val.dev:
        res.dev_blocks.append(DevBlock(lineno, line.strip(), new.strip()))


def _next_branch(g, kw, val, touched, line, indent, hspace, comment, lineno, out, res):
    if isinstance(val, Res) and not touched and g.emitted_if and not g.resolved_any:
        g.live = True
        out.append(line)
        if val.prec == 0:
            res.unsupported.append((lineno, line.strip()))
        return
    g.resolved_any = g.resolved_any or touched
    if isinstance(val, int):
        if not val:
            g.live = False
            res.lines_removed += 1
            return
        g.taken, g.live = True, True
        if g.emitted_if:
            out.append(_fmt(indent, hspace, "else", "", comment))
        else:
            res.lines_removed += 1
        return
    g.live = True
    if g.emitted_if:
        new = _fmt(indent, hspace, "elif", val.text, comment)
    else:
        g.emitted_if = True
        new = _fmt(indent, hspace, "if", val.text, comment)
    out.append(new)
    if val.dev:
        res.dev_blocks.append(DevBlock(lineno, line.strip(), new.strip()))
