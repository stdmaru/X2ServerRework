#!/usr/bin/env python3
"""Copies the asset files a client_data lacks from other clients, following the files the meshes name.

Usage:
  copy_assets.py <client_data> (--scripts <file> ... | --names <file>) --dir <folder> [--dir ...]
                 [--kom <.kom or folder> ...] [--layout tools/assets/layout.txt] [--report <tsv>] [--dry-run]

- The wanted names are every file name (.dds, .tga, .x, .y, .xet...) quoted in --scripts (e.g. Item.lua), or the
  lines of --names, that no file of the client_data has (names are case-insensitive, as the client's).
- Each is taken from the first --dir that has it, in the order given, then from the first --kom archive that has it.
  A --dir holds an archive's extracted files; its name starts with the archive's (data148_kom -> data148.kom).
- The meshes (.x/.y, MSZIP-compressed or not) and .xet effects copied are scanned for the files they name, which are
  wanted too.
- A file goes where `x2_assets import` would put it (--layout, with the archive it came from). New files are read
  as loose files by debug builds; `pack` does not write them yet (docs/tools/client-data.md#packing).
See docs/tools/client-data.md (Bringing data from other clients).
"""
import argparse
import collections
import fnmatch
import os
import re
import struct
import sys
import zlib
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "kom"))
import kom  # noqa: E402

EXT = rb"\.(?:dds|tga|bmp|png|jpg|x|y|xet|xsk|ogg|wav)"
NAME = re.compile(rb"[A-Za-z0-9_\-.]{2,}" + EXT + rb"(?![A-Za-z0-9_])", re.I)
SCANNED = (".x", ".y", ".xet")


def mszip(b):
    """The data of a compressed DirectX file ('xof 0303bzip0032'): MSZIP blocks after the header and total size."""
    total = struct.unpack_from("<I", b, 16)[0]
    pos, out = 20, bytearray()
    while pos < len(b) and len(out) < total:
        csize = struct.unpack_from("<HH", b, pos)[1]
        block = b[pos + 4:pos + 4 + csize]
        pos += 4 + csize
        if block[:2] != b"CK":
            raise ValueError("not an MSZIP block")
        d = zlib.decompressobj(-15, zdict=bytes(out[-32768:])) if out else zlib.decompressobj(-15)
        out += d.decompress(block[2:])
    return bytes(out)


def names_in(data):
    """File names a mesh or effect names: ASCII in DirectX files, UTF-16LE in .xet."""
    if data[:4] == b"xof " and data[8:12] in (b"bzip", b"tzip"):
        data = data[:16] + mszip(data)
    found = {m.group(0).decode("latin1") for m in NAME.finditer(data)}
    for off in (0, 1):
        text = data[off:len(data) - (len(data) - off) % 2].decode("utf-16le", "ignore").encode("latin1", "ignore")
        found |= {m.group(0).decode("latin1") for m in NAME.finditer(text)}
    return found


def load_layout(path):
    rules = []
    for line in Path(path).read_text().splitlines():
        if line.strip() and not line.startswith("#"):
            p = line.split()
            rules.append((p[0], p[1].lower(), p[2].lower() if len(p) > 2 else None))
    return rules


def folder(rules, name, archive):
    for dest, pattern, arch in rules:
        if fnmatch.fnmatchcase(name.lower(), pattern) and (arch is None or fnmatch.fnmatchcase(archive, arch)):
            return dest.replace("{archive}", archive[:-4])
    return "_unsorted/" + archive[:-4]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--scripts", nargs="+", default=[])
    ap.add_argument("--names")
    ap.add_argument("--dir", action="append", default=[])
    ap.add_argument("--kom", action="append", default=[])
    ap.add_argument("--layout", default=str(Path(__file__).resolve().parent / "layout.txt"))
    ap.add_argument("--report")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cd = Path(args.client_data)
    have = set()
    for root, dirs, files in os.walk(cd):
        dirs[:] = [d for d in dirs if d != ".kom"] if Path(root) == cd else dirs
        have.update(f.lower() for f in files)
    if (cd / ".kom" / "blobs").is_dir():
        have.update(f.lower() for f in os.listdir(cd / ".kom" / "blobs"))

    wanted = []
    for s in args.scripts:
        wanted += sorted(names_in(Path(s).read_bytes()))
    if args.names:
        wanted += Path(args.names).read_text().split()

    dirs = []
    for d in args.dir:
        m = re.match(r"(data\w?\d+)", Path(d).name, re.I)
        if not m:
            sys.exit(f"error: {d}: the folder's name does not start with its archive's")
        dirs.append((Path(d), m.group(1).lower() + ".kom", {f.lower(): f for f in os.listdir(d)}))
    koms = []
    for k in args.kom:
        koms += sorted(Path(k).glob("*.kom")) if Path(k).is_dir() else [Path(k)]
    kom_entries = []
    for k in koms:
        kom_entries.append((k.name.lower(), {e.name.lower(): e for e in kom.read_kom(str(k))}))

    rules = load_layout(args.layout)
    queue = collections.deque(wanted)
    seen, copied, unavailable = set(), [], []
    while queue:
        name = queue.popleft()
        key = name.lower()
        if key in seen or key in have:
            continue
        seen.add(key)
        data = None
        for d, archive, files in dirs:
            if key in files:
                real, data, origin = files[key], (d / files[key]).read_bytes(), str(d)
                break
        else:
            for archive, entries in kom_entries:
                if key in entries:
                    real, data, origin = entries[key].name, kom.decode_entry(entries[key]), archive
                    break
        if data is None:
            unavailable.append(name)
            continue
        dest = cd / folder(rules, real, archive) / real
        copied.append((real, origin, dest.relative_to(cd)))
        if key.endswith(SCANNED):
            queue.extend(names_in(data))
        if not args.dry_run:
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(data)

    if args.report:
        with open(args.report, "w") as f:
            f.writelines(f"{n}\t{o}\t{d}\n" for n, o, d in copied)
            f.writelines(f"{n}\tunavailable\t\n" for n in sorted(unavailable))
    by_origin = collections.Counter(Path(o).name for _, o, _ in copied)
    print(f"{len(copied)} files {'to copy' if args.dry_run else 'copied'}" +
          "".join(f"{', ' if i else ': '}{n} from {o}" for i, (o, n) in enumerate(by_origin.most_common())))
    if unavailable:
        print(f"{len(unavailable)} in no source (some may be false matches in binary data): "
              f"{' '.join(sorted(unavailable)[:40])}")


if __name__ == "__main__":
    main()
