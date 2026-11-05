#!/usr/bin/env python3
"""Differential check of the #if resolver on a whole source tree.

For every file, the original and the resolved text are fully preprocessed under the real build
configuration (from the profile simulation) and must keep exactly the same code lines. Run for the service
and the development configuration, each with two assignments of the macros the resolver does not know.

Usage: tools/import/check_equivalence.py server|client [--limit N]
"""

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from x2import import condres, flagsim, pipeline, profile, textio  # noqa: E402

# Macros that are flag-like for the comparison but managed by the resolver itself
DEV = condres.DEV_SWITCH


def strip(lines: list[str], flags: condres.Flags) -> list[str]:
    """Drops what the resolver removes on purpose (#define/#undef of feature flags, #pragma comment(lib))
    and blank lines (normalization trims the end of files)."""
    out = []
    for x in lines:
        if not x.strip():
            continue
        if (x.startswith("#define ") or x.startswith("#undef ")) and flags.is_flag(x.split()[1]):
            continue
        if x.startswith("#") and "comment" in x and condres._PRAGMA_LIB_RE.search(x):
            continue
        out.append(x)
    return out


def configs(pr: profile.Profile, flags: condres.Flags, entries=None):
    read = lambda p: p.read_bytes()  # noqa: E731
    svc = pr.simulate(dev=False, read=read, entries=entries).defined
    dev = pr.simulate(dev=True, read=read, entries=entries).defined

    def table(sim: dict[str, str], dev_on: bool) -> dict[str, str]:
        t = dict(sim)
        # hand-written overrides (server.toml): true/false states win over the simulation
        for n in flags.true:
            t.setdefault(n, "")
        for n in flags.false:
            t.pop(n, None)
        for n in flags.dev:
            if dev_on:
                t.setdefault(n, "")
            else:
                t.pop(n, None)
        for n in flags.dev_not:
            if dev_on:
                t.pop(n, None)
            else:
                t.setdefault(n, "")
        t[DEV] = "1" if dev_on else "0"
        return t

    return {"service": table(svc, False), "dev": table(dev, True)}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("profile")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--show", help="print the diff of this file (path relative to the profile root)")
    args = ap.parse_args()

    pr = profile.load(args.profile)
    flag_names = ["base", f"{args.profile}.generated", args.profile]
    flags = pipeline.load_flags([n for n in flag_names if (pipeline.TOOL / "flags" / f"{n}.toml").exists()])
    root = pr.root
    files = pr.files()
    if args.limit:
        files = files[: args.limit]

    # each file is checked against the configuration chain its original project really read
    tables_of = {}
    default = configs(pr, flags)
    for layer in pr.layers:
        t = configs(pr, flags, layer.entries)
        for p in pr.files(layer.code_globs):
            tables_of[p] = (layer.name, t)
    per_layer: dict[str, list[int]] = {}
    unknown_sets = {"unknown-undefined": {}, "unknown-defined": None}

    checked = differ = skipped = unbalanced = 0
    problems: list[str] = []
    for p in files:
        rel = p.relative_to(root).as_posix()
        text = textio.normalize(textio.decode(p.read_bytes()).text)
        res = condres.resolve(text, flags)
        if res.unbalanced:
            unbalanced += 1
            problems.append(f"unbalanced #if: {rel}")
            continue
        resolved = textio.normalize(res.text)
        lname, tables = tables_of.get(p, ("default", default))
        ok = True
        for cfg, table in tables.items():
            for uname, extra in unknown_sets.items():
                t = dict(table)
                if extra is None:  # every identifier tested in this file and not yet known -> defined
                    for ident in set(flagsim._IDENT_RE.findall(text)) - set(t):
                        if flags.state(ident) is None and ident != DEV:
                            t[ident] = ""
                a = flagsim.live_lines(text, t)
                b = flagsim.live_lines(resolved, t)
                if a is None or b is None:
                    skipped += 1
                    ok = False
                    problems.append(f"not evaluable ({cfg}, {uname}): {rel}")
                    break
                if strip(a, flags) != strip(b, flags):
                    if args.show and rel == args.show:
                        import difflib
                        print("\n".join(difflib.unified_diff(strip(a, flags), strip(b, flags), "original", "resolved", lineterm="", n=1)))
                    differ += 1
                    ok = False
                    problems.append(f"DIFFERENT ({cfg}, {uname}): {rel}")
                    break
            if not ok:
                break
        checked += ok
        c = per_layer.setdefault(lname, [0, 0])
        c[0] += 1
        c[1] += ok
    if pr.layers:
        print("per layer: " + ", ".join(f"{n} {ok}/{tot}" for n, (tot, ok) in sorted(per_layer.items())))
    print(f"files: {len(files)}, equivalent: {checked}, different: {differ}, not evaluable: {skipped}, "
          f"unbalanced: {unbalanced}")
    for line in problems[:60]:
        print("  " + line)
    return 0 if not differ else 1


if __name__ == "__main__":
    sys.exit(main())
