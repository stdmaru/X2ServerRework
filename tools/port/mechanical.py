#!/usr/bin/env python3
"""Mechanical porting pass over an imported module (run once, right after the import; then commit).

- Windows/ATL/DirectX/Boost includes -> platform layer or standard headers
- Boost names that have a 1:1 standard (or x2::) equivalent -> that equivalent
Everything that needs judgement (variate_generator, tokenizer, noncopyable, BOOST_FOREACH...) is left for
the manual pass and listed at the end.

Usage: tools/port/mechanical.py src/server/Common [--dry-run]
"""

import argparse
import re
import sys
from pathlib import Path

SUFFIXES = {".c", ".cpp", ".h", ".hpp", ".inl"}

PLATFORM = '"x2/platform/win32.h"'

# include (case-insensitive path) -> replacement include, or None to drop it
INCLUDES = {
    "windows.h": PLATFORM, "atltime.h": PLATFORM, "atlstr.h": PLATFORM, "tchar.h": PLATFORM,
    "wtypes.h": PLATFORM, "objbase.h": PLATFORM, "mmsystem.h": PLATFORM, "process.h": PLATFORM,
    "d3dx9math.h": '"x2/platform/d3dx_math.h"', "d3dx9.h": '"x2/platform/d3dx_math.h"',
    "serializer/zlib/zlib.h": "<zlib.h>",
    "winsock2.h": '"x2/platform/net.h"', "winsock.h": '"x2/platform/net.h"', "ws2tcpip.h": '"x2/platform/net.h"',
    "boost/shared_ptr.hpp": "<memory>", "boost/weak_ptr.hpp": "<memory>", "boost/scoped_ptr.hpp": "<memory>",
    "boost/shared_array.hpp": "<memory>", "boost/enable_shared_from_this.hpp": "<memory>",
    "boost/smart_ptr.hpp": "<memory>", "boost/make_shared.hpp": "<memory>",
    "boost/bind.hpp": "<functional>", "boost/bind/bind.hpp": "<functional>", "boost/ref.hpp": "<functional>",
    "boost/function.hpp": "<functional>",
    "boost/random.hpp": "<random>",
    "boost/format.hpp": '"x2/platform/format.h"',
    "boost/timer.hpp": '"x2/platform/timer.h"',
    "boost/noncopyable.hpp": '"x2/platform/noncopyable.h"', "boost/utility.hpp": None, "boost/static_assert.hpp": None,
    "boost/foreach.hpp": None, "boost/test/utils/foreach.hpp": None,
}

# regex -> replacement (applied in order)
NAMES = [
    (r"\bboost::shared_ptr\b", "std::shared_ptr"),
    (r"\bboost::weak_ptr\b", "std::weak_ptr"),
    (r"\bboost::scoped_ptr\b", "std::unique_ptr"),
    (r"\bboost::static_pointer_cast\b", "std::static_pointer_cast"),
    (r"\bboost::dynamic_pointer_cast\b", "std::dynamic_pointer_cast"),
    (r"\bboost::enable_shared_from_this\b", "std::enable_shared_from_this"),
    (r"\bboost::make_shared\b", "std::make_shared"),
    (r"\bboost::bind\b", "std::bind"),
    (r"\bboost::placeholders::", "std::placeholders::"),
    (r"\bboost::ref\b", "std::ref"),
    (r"\bboost::cref\b", "std::cref"),
    (r"\bboost::function\b", "std::function"),
    (r"\bboost::mt19937\b", "std::mt19937"),
    (r"\bboost::minstd_rand\b", "std::minstd_rand"),
    (r"\bboost::uniform_int<", "std::uniform_int_distribution<"),
    (r"\bboost::uniform_real<\s*>", "std::uniform_real_distribution<>"),
    (r"\bboost::uniform_real<", "std::uniform_real_distribution<"),
    (r"\bboost::wformat\b", "x2::wformat"),
    (r"\bboost::format\b", "x2::format"),
    (r"\bboost::str\(", "x2::str("),
    (r"\bboost::timer\b", "x2::Timer"),
    (r"\bboost::noncopyable\b", "x2::NonCopyable"),
    (r"\bBOOST_STATIC_ASSERT\(", "static_assert("),
    (r"\busing namespace boost::placeholders;", "using namespace std::placeholders;"),
    # MSVC-only token pasting: `name##()`, `name##::` (pasting with punctuation) and L#x (wide stringizing)
    (r"##\s*(?=\(|::|\)|,|;|\[)", ""),
    (r"(?<=::)\s*##\s*", ""),
    (r"\bL#(\w+)", r"L ## #\1"),
    # BOOST_TEST_FOREACH( type, var, container ) / BOOST_FOREACH( decl, container ) -> range-for
    (r"\bBOOST_TEST_FOREACH\(\s*(.+?)\s*,\s*(\w+)\s*,\s*(.+?)\s*\)\s*$", r"for( \1 \2 : \3 )"),
    (r"\bBOOST_FOREACH\(\s*(.+?\w)\s*,\s*(.+?)\s*\)\s*$", r"for( \1 : \2 )"),
]

INCLUDE_RE = re.compile(r'^(\s*#\s*include\s*)([<"])([^>"]+)([>"])(.*)$')


def process(text: str) -> tuple[str, int]:
    out, changes, seen = [], 0, set()
    for line in text.split("\n"):
        m = INCLUDE_RE.match(line)
        if m:
            key = m.group(3).replace("\\", "/").lower()
            if key in INCLUDES:
                repl = INCLUDES[key]
                changes += 1
                if repl is None or repl in seen:
                    continue
                seen.add(repl)
                line = f"{m.group(1)}{repl}{m.group(5)}"
            else:
                seen.add(f"{m.group(2)}{m.group(3)}{m.group(4)}")
        for pat, rep in NAMES:
            line, n = re.subn(pat, rep, line)
            if n:
                changes += n
                if rep.endswith("static_assert("):  # BOOST_STATIC_ASSERT(x) -> static_assert(x, "")
                    line = re.sub(r"static_assert\(([^;]*)\)\s*;", r'static_assert(\1, "");', line)
        out.append(line)
    return "\n".join(out), changes


LEFTOVER = re.compile(r"\bboost::\w+|BOOST_\w+|#\s*include\s*[<\"]boost/[^>\"]+")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("dir")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    total = files = 0
    leftovers: dict[str, list[str]] = {}
    for p in sorted(Path(args.dir).rglob("*")):
        if p.suffix.lower() not in SUFFIXES or not p.is_file():
            continue
        text = p.read_text()
        new, n = process(text)
        if n:
            files += 1
            total += n
            if not args.dry_run:
                p.write_text(new)
        for i, line in enumerate(new.split("\n"), 1):
            if LEFTOVER.search(line) and not line.lstrip().startswith("//"):
                leftovers.setdefault(str(p), []).append(f"{i}: {line.strip()[:120]}")
    print(f"{total} change(s) in {files} file(s)")
    if leftovers:
        print("left for the manual pass:")
        for f, lines in leftovers.items():
            for line in lines:
                print(f"  {f}:{line}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
