"""#include discovery and case-insensitive resolution (the original was written on Windows)."""

import os
import re
from pathlib import Path

_INCLUDE_RE = re.compile(r'^(\s*#\s*include\s*)([<"])([^>"]+)([>"])', re.M)


class CaseIndex:
    """Maps lowercase absolute paths to real paths, built lazily per directory."""

    def __init__(self):
        self._dirs: dict[Path, dict[str, str]] = {}

    def _listing(self, d: Path) -> dict[str, str]:
        if d not in self._dirs:
            try:
                self._dirs[d] = {n.lower(): n for n in os.listdir(d)}
            except OSError:
                self._dirs[d] = {}
        return self._dirs[d]

    def lookup(self, base: Path, rel: str) -> Path | None:
        """Resolves `rel` under `base` ignoring case. Returns the real path or None."""
        cur = base
        for part in rel.replace("\\", "/").split("/"):
            if part in ("", "."):
                continue
            if part == "..":
                cur = cur.parent
                continue
            real = self._listing(cur).get(part.lower())
            if real is None:
                return None
            cur = cur / real
        return cur if cur.is_file() else None


def find_includes(text: str) -> list[tuple[str, str]]:
    """Returns [(delimiter, path)] for every #include in `text`."""
    return [(m.group(2), m.group(3)) for m in _INCLUDE_RE.finditer(text)]


def resolve_include(index: CaseIndex, including: Path, delim: str, spelled: str,
                    include_dirs: list[Path]) -> Path | None:
    bases = ([including.parent] if delim == '"' else []) + include_dirs
    for base in bases:
        hit = index.lookup(base, spelled)
        if hit is not None:
            return hit
    return None


def correct_spelling(spelled: str, base_hit: Path) -> str:
    """Rewrites `spelled` with the real case of the resolved file and '/' separators."""
    parts = [p for p in spelled.replace("\\", "/").split("/")]
    real: list[str] = []
    cur = base_hit
    for p in reversed(parts):
        if p in ("..", ".", ""):
            real.append(p)
        else:
            real.append(cur.name)
            cur = cur.parent
    return "/".join(reversed(real))


def rewrite_includes(text: str, fix: dict[str, str]) -> str:
    """Replaces spelled include paths using `fix` (old spelling -> new spelling)."""
    if not fix:
        return text

    def sub(m: re.Match) -> str:
        new = fix.get(m.group(3))
        return m.group(0) if new is None else f"{m.group(1)}{m.group(2)}{new}{m.group(4)}"

    return _INCLUDE_RE.sub(sub, text)
