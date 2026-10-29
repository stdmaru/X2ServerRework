"""Import of one module: selection -> decode -> resolve flags -> fix includes -> write + report."""

import difflib
import fnmatch
import functools
import hashlib
import json
import re
import subprocess
import tomllib
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path

from . import condres, includes, textio, vcxproj

REPO = Path(__file__).resolve().parents[3]
TOOL = REPO / "tools" / "import"
REFERENCE = REPO / "_reference" / "X2Server"

SOURCE_SUFFIXES = {".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx", ".inl", ".ipp", ".nsmap"}  # .nsmap: gSOAP

# Standard headers the original sometimes spells with capitals (<Queue>); Windows headers are left alone,
# they are replaced by x2/platform/win32.h during the manual port.
STD_HEADERS = {
    "algorithm", "array", "atomic", "bitset", "cassert", "cctype", "cerrno", "cfloat", "climits", "cmath",
    "cstdarg", "cstddef", "cstdint", "cstdio", "cstdlib", "cstring", "ctime", "cwchar", "deque", "exception",
    "fstream", "functional", "iomanip", "ios", "iosfwd", "iostream", "istream", "iterator", "limits", "list",
    "locale", "map", "memory", "new", "numeric", "ostream", "queue", "set", "sstream", "stack", "stdexcept",
    "streambuf", "string", "tuple", "typeinfo", "utility", "valarray", "vector",
    "assert.h", "ctype.h", "errno.h", "float.h", "limits.h", "math.h", "stdarg.h", "stddef.h", "stdio.h",
    "stdlib.h", "string.h", "time.h", "wchar.h",
}


class ImportError_(Exception):
    pass


@dataclass
class FileRecord:
    src: Path
    dest: Path
    reason: str                          # "vcxproj", "header", "extra", "include"
    encoding: str = ""
    had_crlf: bool = False
    result: condres.Result | None = None
    include_fixes: dict[str, str] = field(default_factory=dict)
    text: str = ""


@dataclass
class Module:
    name: str
    source_root: Path
    alt_root: Path | None
    include_dirs: list[Path]
    projects: list[tuple[Path, str]]
    extra: list[str]
    exclude: list[str]
    maps: list[tuple[str, str]]
    flags: condres.Flags
    seed_filter: list[str] = field(default_factory=list)    # only these vcxproj files are seeds
    include_scope: list[str] = field(default_factory=list)  # includes are followed only into these paths
    extern_dirs: list[Path] = field(default_factory=list)   # already ported modules: spelling fixes only


STATES = ("true", "false", "dev", "dev_not")


def load_flags(names: list[str]) -> condres.Flags:
    """Loads flag files in order; a later file overrides earlier ones (base -> generated -> hand-written).
    Inside one file a name may appear in a single state only."""
    state: dict[str, str] = {}
    patterns: list[re.Pattern] = []
    for name in names:
        data = tomllib.loads((TOOL / "flags" / f"{name}.toml").read_text())
        here: dict[str, str] = {}
        for section in data.values():
            if not isinstance(section, dict):
                continue
            for st in STATES:
                for flag in section.get(st, []):
                    if here.get(flag, st) != st:
                        raise ImportError_(f"flags/{name}.toml: {flag} is both {here[flag]} and {st}")
                    here[flag] = st
            patterns += [re.compile(p) for p in section.get("false_patterns", [])]
        state.update(here)
    flags = condres.Flags(false_patterns=patterns)
    for flag, st in state.items():
        getattr(flags, st).add(flag)
    return flags


def load_module(name: str) -> Module:
    data = tomllib.loads((TOOL / "modules" / f"{name}.toml").read_text())
    # a top-level key written after a [[table]] silently belongs to that table in TOML
    top = {"name", "frozen", "source_root", "alt_root", "include_dirs", "flags", "extra", "exclude",
           "seed_filter", "include_scope", "extern_dirs", "projects", "map"}
    if set(data) - top:
        raise ImportError_(f"unexpected top-level keys: {sorted(set(data) - top)}")
    allowed = {"projects": {"path", "config"}, "map": {"from", "to"}}
    for table, keys in allowed.items():
        for entry in data.get(table, []):
            if set(entry) - keys:
                raise ImportError_(f"unexpected keys in [[{table}]]: {sorted(set(entry) - keys)}")
    root = (REFERENCE / data["source_root"]).resolve()
    alt = (REFERENCE / data["alt_root"]).resolve() if data.get("alt_root") else None
    return Module(
        name=data["name"],
        source_root=root,
        alt_root=alt,
        include_dirs=[root / d for d in data.get("include_dirs", [])],
        projects=[(root / p["path"], p["config"]) for p in data.get("projects", [])],
        extra=data.get("extra", []),
        exclude=data.get("exclude", []),
        maps=[(m["from"].strip("/"), m["to"].strip("/")) for m in data["map"]],
        flags=load_flags(data.get("flags", [])),
        seed_filter=data.get("seed_filter", []),
        include_scope=data.get("include_scope", []),
        extern_dirs=[REPO / d for d in data.get("extern_dirs", [])],
    )


def _rel(mod: Module, path: Path) -> str | None:
    try:
        return path.resolve().relative_to(mod.source_root).as_posix()
    except ValueError:
        return None


def _matches(rel: str, patterns: list[str]) -> bool:
    return any(fnmatch.fnmatch(rel, pat) or fnmatch.fnmatch(rel.lower(), pat.lower()) for pat in patterns)


def _excluded(mod: Module, rel: str) -> bool:
    return _matches(rel, mod.exclude)


def _dest(mod: Module, rel: str) -> Path:
    best = None
    for src, dst in mod.maps:
        if src == "" or rel == src or rel.startswith(src + "/"):
            if best is None or len(src) > len(best[0]):
                best = (src, dst)
    if best is None:
        raise ImportError_(f"no [[map]] rule for {rel}")
    tail = rel[len(best[0]):].lstrip("/") if best[0] else rel
    return REPO / best[1] / tail


def run(name: str, dry_run: bool = False, force: bool = False) -> Path:
    data = tomllib.loads((TOOL / "modules" / f"{name}.toml").read_text())
    if data.get("frozen") and not dry_run:
        raise ImportError_(f"module '{name}' is frozen: it was ported by hand in src/, re-importing would "
                           "destroy that work (only --dry-run is allowed)")
    mod = load_module(name)
    index = includes.CaseIndex()

    records: dict[Path, FileRecord] = {}
    queue: list[tuple[Path, str]] = []
    excluded_refs: Counter[str] = Counter()
    unresolved: Counter[str] = Counter()
    outside: Counter[str] = Counter()
    missing_seeds: list[str] = []
    other_modules: Counter[str] = Counter()
    extern_refs: Counter[str] = Counter()
    queued: set[Path] = set()

    def enqueue(path: Path, reason: str):
        rel = _rel(mod, path)
        if rel is None:
            outside[str(path)] += 1
            return
        if reason in ("vcxproj", "header") and mod.seed_filter and not _matches(rel, mod.seed_filter):
            return
        if _excluded(mod, rel):
            excluded_refs[rel] += 1
            return
        if path.suffix.lower() not in SOURCE_SUFFIXES:
            return
        if mod.include_scope and not _matches(rel, mod.include_scope):
            other_modules[rel] += 1
            return
        if path not in queued:
            queued.add(path)
            queue.append((path, reason))

    for proj, config in mod.projects:
        try:
            sources, headers = vcxproj.project_files(proj, config)
        except ValueError as e:
            raise ImportError_(str(e)) from e
        for p in sources:
            real = index.lookup(Path("/"), str(p))
            if real is None:
                rel = _rel(mod, p)
                in_filter = not mod.seed_filter or (rel is not None and _matches(rel, mod.seed_filter))
                if in_filter and (rel is None or not _excluded(mod, rel)):
                    missing_seeds.append(rel or str(p))
            else:
                enqueue(real, "vcxproj")
        for p in headers:
            real = index.lookup(Path("/"), str(p))
            if real is not None:
                enqueue(real, "header")
    for rel in mod.extra:
        real = index.lookup(mod.source_root, rel)
        if real is None:
            missing_seeds.append(rel)
        else:
            enqueue(real, "extra")

    while queue:
        path, reason = queue.pop(0)
        rel = _rel(mod, path)
        rec = FileRecord(src=path, dest=_dest(mod, rel), reason=reason)
        records[path] = rec

        raw = path.read_bytes()
        _check_integrity(path, raw)
        dec = textio.decode(raw)
        rec.encoding, rec.had_crlf = dec.encoding, dec.had_crlf
        text = textio.normalize(dec.text)
        rec.result = condres.resolve(text, mod.flags)
        text = rec.result.text

        for delim, spelled in includes.find_includes(text):
            hit = includes.resolve_include(index, path, delim, spelled, mod.include_dirs)
            if hit is None:
                ext = includes.resolve_include(index, Path("/nonexistent"), "<", spelled, mod.extern_dirs)
                if ext is not None:
                    good = includes.correct_spelling(spelled, ext)
                    if good != spelled:
                        rec.include_fixes[spelled] = good
                    extern_refs[good] += 1
                    continue
                if spelled.lower() in STD_HEADERS and spelled != spelled.lower():
                    rec.include_fixes[spelled] = spelled.lower()
                    continue
                unresolved[spelled] += 1
                continue
            good = includes.correct_spelling(spelled, hit)
            if good != spelled:
                rec.include_fixes[spelled] = good
            enqueue(hit, "include")
        rec.text = textio.normalize(includes.rewrite_includes(text, rec.include_fixes))

    _check_dest_includes(records, mod, index)
    report_dir = TOOL / "reports" / mod.name
    manifest_path = TOOL / "manifests" / f"{mod.name}.json"
    if not dry_run:
        _write(records, manifest_path, force)
    _report(mod, records, report_dir, excluded_refs, unresolved, outside, missing_seeds, other_modules, extern_refs)
    return report_dir / "REPORT.md"


def _check_dest_includes(records: dict[Path, FileRecord], mod: Module, index: includes.CaseIndex):
    """The destination layout must keep every include working (includes are not rewritten yet)."""
    dest_dirs = []
    for d in mod.include_dirs:  # include dirs of other modules (servers) have no [[map]] here
        try:
            dest_dirs.append(_dest(mod, _rel(mod, d)))
        except ImportError_:
            pass
    for rec in records.values():
        for delim, spelled in includes.find_includes(rec.text):
            hit = includes.resolve_include(index, rec.src, delim, spelled, mod.include_dirs)
            if hit is None or hit not in records:
                continue
            want = records[hit].dest.resolve()
            bases = ([rec.dest.parent] if delim == '"' else []) + dest_dirs
            if not any((b / spelled).resolve() == want for b in bases):
                rec.result.unsupported.append((0, f"include {spelled!r} breaks in destination layout"))


@functools.cache
def _reference_index() -> dict[str, str]:
    """Blob hash of every file tracked by the _reference git repository (path relative to its root)."""
    out = subprocess.run(["git", "-C", str(REFERENCE.resolve()), "ls-files", "-s", "-z"],
                         capture_output=True, check=True).stdout
    index = {}
    for rec in out.split(b"\0"):
        if rec:
            meta, path = rec.split(b"\t", 1)
            index[path.decode("utf-8", "surrogateescape")] = meta.split()[1].decode()
    return index


def _check_integrity(path: Path, data: bytes):
    """The original disk copy had silently corrupted files: refuse anything that differs from git."""
    rel = path.resolve().relative_to(REFERENCE.resolve()).as_posix()
    expected = _reference_index().get(rel)
    if expected is None:
        raise ImportError_(f"{rel}: not tracked by the _reference git repository")
    for blob in (data, data.replace(b"\r\n", b"\n")):
        if hashlib.sha1(b"blob %d\0" % len(blob) + blob).hexdigest() == expected:
            return
    raise ImportError_(f"{rel}: content differs from git (corrupted or modified copy)")


def _sha(text: str) -> str:
    return hashlib.sha256(text.encode()).hexdigest()


def _write(records: dict[Path, FileRecord], manifest_path: Path, force: bool):
    old = json.loads(manifest_path.read_text()) if manifest_path.exists() else {}
    edited = []
    for rel, sha in old.items():
        p = REPO / rel
        if p.exists() and _sha(p.read_text()) != sha:
            edited.append(rel)
    if edited and not force:
        raise ImportError_("destination files were edited after import (use --force to overwrite):\n  "
                           + "\n  ".join(edited))
    new = {}
    for rec in sorted(records.values(), key=lambda r: r.dest):
        rec.dest.parent.mkdir(parents=True, exist_ok=True)
        rec.dest.write_text(rec.text)
        new[rec.dest.relative_to(REPO).as_posix()] = _sha(rec.text)
    for rel in set(old) - set(new):  # files no longer imported
        p = REPO / rel
        if p.exists():
            p.unlink()
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(new, indent=2, sort_keys=True) + "\n")


def _alt_diff(mod: Module, rec: FileRecord) -> list[str] | None:
    if mod.alt_root is None:
        return None
    rel = _rel(mod, rec.src)
    alt = includes.CaseIndex().lookup(mod.alt_root, rel)
    if alt is None:
        return ["(missing in alternative source)"]
    text = condres.resolve(textio.normalize(textio.decode(alt.read_bytes()).text), mod.flags).text
    text = textio.normalize(includes.rewrite_includes(text, rec.include_fixes))
    if text == rec.text:
        return None
    # compare ignoring whitespace: one side was reformatted, which hides the real changes
    squash = lambda s: [ln for ln in (re.sub(r"\s+", "", x) for x in s.splitlines()) if ln]  # noqa: E731
    a, b = squash(rec.text), squash(text)
    if a == b:
        return ["formatting only"]
    return list(difflib.unified_diff(a, b, f"{mod.source_root.name}/{rel}", f"{mod.alt_root.name}/{rel}",
                                     lineterm="", n=2))


def _report(mod, records, report_dir: Path, excluded_refs, unresolved, outside, missing_seeds, other_modules,
            extern_refs):
    report_dir.mkdir(parents=True, exist_ok=True)
    diff_dir = report_dir / "alt-diff"
    for old in diff_dir.glob("*.diff") if diff_dir.exists() else []:
        old.unlink()

    recs = sorted(records.values(), key=lambda r: r.dest)
    enc = Counter(r.encoding for r in recs)
    total_lines = sum(r.text.count("\n") for r in recs)
    removed = sum(r.result.lines_removed for r in recs)
    groups = sum(r.result.groups_resolved for r in recs)
    libs = Counter(lib.lower() for r in recs for lib in r.result.pragma_libs)

    L = [f"# Import report: `{mod.name}`", "",
         "Generated by `tools/import/import.py`. Do not edit by hand.", "",
         f"- source: `_reference/X2Server/{mod.source_root.name}`"
         + (f" (compared with `{mod.alt_root.name}`)" if mod.alt_root else ""),
         f"- files imported: **{len(recs)}**, lines written: {total_lines}",
         f"- `#if` groups resolved: {groups}, lines removed: {removed}",
         f"- encodings: " + ", ".join(f"{k} {v}" for k, v in sorted(enc.items())),
         f"- files with CRLF converted: {sum(r.had_crlf for r in recs)}", ""]

    L += ["## Files", "", "| destination | source | reason | enc | removed | notes |", "|---|---|---|---|---:|---|"]
    for r in recs:
        notes = []
        if r.include_fixes:
            notes.append(f"{len(r.include_fixes)} include(s) fixed")
        if r.result.dev_blocks:
            notes.append(f"{len(r.result.dev_blocks)} dev block(s)")
        if r.result.unbalanced:
            notes.append("**unbalanced #if**")
        L.append(f"| `{r.dest.relative_to(REPO).as_posix()}` | `{_rel(mod, r.src)}` | {r.reason} | "
                 f"{r.encoding} | {r.result.lines_removed} | {', '.join(notes)} |")
    L.append("")

    dev = [(r, b) for r in recs for b in r.result.dev_blocks]
    L += ["## Blocks rewritten to `X2_DEV_TOOLS` (triage)", ""]
    L += [f"- `{r.dest.relative_to(REPO).as_posix()}:{b.line}` `{b.original}` -> `{b.emitted}`" for r, b in dev] or ["(none)"]
    L.append("")

    fixes = [(r, a, b) for r in recs for a, b in r.include_fixes.items()]
    L += ["## Include spelling fixed (case / backslash)", ""]
    L += [f"- `{r.dest.relative_to(REPO).as_posix()}`: `{a}` -> `{b}`" for r, a, b in fixes] or ["(none)"]
    L.append("")

    warn = [f"- `{r.dest.relative_to(REPO).as_posix()}:{n}` {t}" for r in recs for n, t in r.result.unsupported]
    warn += [f"- `{r.dest.relative_to(REPO).as_posix()}` has unbalanced `#if`/`#endif`" for r in recs
             if r.result.unbalanced]
    warn += [f"- seed listed but missing: `{s}`" for s in missing_seeds]
    L += ["## Warnings", ""] + (warn or ["(none)"]) + [""]

    L += ["## Removed `#define`/`#undef` of feature flags", ""]
    defs = Counter(d for r in recs for d in r.result.defines_removed)
    L += [", ".join(f"`{d}`" for d in sorted(defs)) or "(none)", ""]

    L += ["## `#pragma comment(lib)` removed (link deps to re-create in CMake)", ""]
    L += [", ".join(f"`{k}` ×{v}" for k, v in sorted(libs.items())) or "(none)", ""]

    L += ["## References to excluded files, by exclude pattern (become external dependencies)", ""]
    by_pat: Counter[str] = Counter()
    for rel, n in excluded_refs.items():
        pat = next(p for p in mod.exclude if fnmatch.fnmatch(rel, p) or fnmatch.fnmatch(rel.lower(), p.lower()))
        by_pat[pat] += n
    L += [f"- `{k}` ×{v}" for k, v in sorted(by_pat.items())] or ["(none)"]
    L.append("")
    L += ["## Includes not found in the module (system / third-party / other modules)", ""]
    L += [", ".join(f"`{k}`" for k in sorted(unresolved)) or "(none)", ""]
    if extern_refs:
        L += ["## Includes of already ported modules (`extern_dirs`)", ""]
        L += [", ".join(f"`{k}` ×{v}" for k, v in sorted(extern_refs.items())), ""]
    if other_modules:
        L += ["## Includes of files outside `include_scope` (belong to other modules)", ""]
        L += [f"- `{k}` ×{v}" for k, v in sorted(other_modules.items())] + [""]
    if outside:
        L += ["## References outside the module root", ""] + [f"- `{k}`" for k in sorted(outside)] + [""]

    if mod.alt_root:
        L += [f"## Differences against `{mod.alt_root.name}`", ""]
        any_diff = False
        for r in recs:
            d = _alt_diff(mod, r)
            if d is None:
                continue
            any_diff = True
            rel = _rel(mod, r.src)
            if len(d) == 1:
                L.append(f"- `{rel}`: {d[0]}")
                continue
            diff_dir.mkdir(parents=True, exist_ok=True)
            fname = rel.replace("/", "__") + ".diff"
            (diff_dir / fname).write_text("\n".join(d) + "\n")
            plus = sum(1 for x in d if x.startswith("+") and not x.startswith("+++"))
            minus = sum(1 for x in d if x.startswith("-") and not x.startswith("---"))
            L.append(f"- `{rel}`: +{plus} -{minus} ([diff](alt-diff/{fname}))")
        if not any_diff:
            L.append("(identical)")
        L.append("")

    (report_dir / "REPORT.md").write_text("\n".join(L))
