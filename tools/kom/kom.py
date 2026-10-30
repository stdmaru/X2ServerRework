#!/usr/bin/env python3
"""kom.py - Elsword .kom archive tool (list / unpack / pack), stdlib only.

Container ("KOG GC TEAM MASSFILE V.0.3.")
-----------------------------------------
  0   char[52]  magic, NUL padded
  52  u32       number of entries
  56  u32       compressed flag (1)
  60  u32       file time (sum of the entries' FileTime)
  64  u32       adler32 of the XML header
  68  u32       XML header size
  72  XML       <Files><File Name= Size= CompressedSize= Checksum= FileTime=
                Algorithm= />...</Files>
  ..  data      entry blobs, back to back, in XML order

Size is the decoded size, CompressedSize the stored blob size, Checksum the
adler32 of the stored blob.  V.0.1/V.0.2 archives (binary 72-byte entry
table, zlib only) can be listed and unpacked too.

Algorithm 0: blob = zlib(data).
Algorithm 2: blob = XORCRC(zlib(data)), "encrypt after compress", introduced
in 2013 (X2OPTIMIZE_ENCRYPT_AFTER_COMPRESS / ENFORCE_IMPORTANT_DATA_ENCRYPTION
in the client source).  XORCRC is a CRC-32 table driven stream cipher:
  key20  = five dwords {0xcc3d4ffb,0x593cdeaf,0xcf4235de,0xde34bd78,0x52b34a75}
           each XOR 0x38d2dfa4, little endian
  prefix = lower-cased entry name (UTF-16 code units, high byte only if
           non-zero) + the blob length (little endian, stopping at the first
           zero byte of the remaining value)
  crc    = 0xffffffff, run over prefix as crc = (crc>>8) ^ T[crc ^ b ^ key20[i%20]]
  key    = key20 rotated left by len(prefix) % 20
  per byte: comp = (crc & 0xff) ^ key[i % 20]
            s    = c ^ 0xff ^ comp ;  plain = (T[s] & 0xff) ^ comp
            crc  = (crc >> 8) ^ ((T[s] & 0xffffff00) | s)
  T is the standard reflected CRC-32 table (poly 0x04c11db7).
Algorithm 3: blob = XORCRC3(prefix[4] + z), z = zlib(data) without its adler32
trailer, used by the later (2016) data files such as data036_br.kom.  The
client inflates from byte 6 (raw deflate after the zlib header) and never reads
the prefix, whose meaning is unknown (not an adler32/crc32 of the data or of z);
pack writes adler32(z) there, like JunePack.  XORCRC3 is XORCRC with
  key20  = FD E1 08 EC 0D 09 02 DC DE 4C 8C 64 16 62 7D E4 97 16 FF DC
  and the prefix run through the decrypt update (comp/s/crc as below, output
  discarded) instead of the plain CRC update.
Reverse engineered by the Elsword-V5.0318.6_3 repository (KTDXCommonFunc.h
XORCRCDecryptAlgo3, tools/kpcrack/JuneExtract.cs and JunePack.cs); not in the
2014 client source.
Algorithm 1 (zlib + Blowfish, unused by current clients) is kept as an opaque
"name.SIZE.1.___" file, like the original komextract did.

Lua scripts
-----------
Since March 2013 the .lua entries are stripped LuaJIT 2.0.2 bytecode with the
KOG header "\\x1bKL\\x81" (stock: "\\x1bLJ\\x01"); the opcodes are unchanged.
Service builds of the client have the Lua parser removed, so these entries
must stay bytecode.  The later files (data036_br.kom) use version 0x82
("\\x1bKL\\x82"): the same LuaJIT 2.0 format with the opcodes reordered
(table from tools/kpcrack/KlTranslate.cs of the same repository); they are
mapped to 0x81 before decompiling, and back when an edited script is packed.
unpack decompiles them to readable Lua (UTF-8 with BOM,
as the client's loader requires).  The decompiler was validated on all 4553
scripts of the client: recompiling the output with the client's own LuaJIT
gives equivalent bytecode for every one of them.

pack --format keep (the default) writes each entry with the algorithm and
bytecode layout it was unpacked with; --format 2014-03 or 2014-08 converts every
encrypted entry (algorithm 2 <-> 3) and every Lua bytecode (0x81 <-> 0x82) to
that format.  Plain entries (algorithm 0) stay plain.

unpack writes <out>/.kom/manifest.json and keeps each decompiled script's
original bytecode in <out>/.kom/bytecode/.  pack then
  * reuses the original bytecode for scripts whose text is unchanged, so an
    unpack -> pack -> unpack round trip reproduces every file exactly;
  * recompiles edited scripts with the client's LuaJIT (luajit-kog/luajit
    next to this file, or --luajit / $KOM_LUAJIT) and stores them encrypted
    with Algorithm 2 again;
  * stores new files with Algorithm 0, and accepts the "name.SIZE.ALG.___"
    files written by the old komextract unchanged.
"""

import argparse
import hashlib
import json
import math
import os
import re
import struct
import subprocess
import sys
import tempfile
import zlib
from xml.dom.minidom import parseString
from xml.sax.saxutils import quoteattr

MAGIC_V3 = b"KOG GC TEAM MASSFILE V.0.3."
BOM = b"\xef\xbb\xbf"
META_DIR = ".kom"


# ================================================================ XORCRC

def _crc_table():
    def refl(v, n):
        r = 0
        for i in range(1, n + 1):
            if v & 1:
                r |= 1 << (n - i)
            v >>= 1
        return r
    tab = []
    for i in range(256):
        t = refl(i, 8) << 24
        for _ in range(8):
            t = ((t << 1) ^ (0x04C11DB7 if t & 0x80000000 else 0)) & 0xFFFFFFFF
        tab.append(refl(t, 32))
    return tab


CRC_T = _crc_table()
CRC_INV = {t & 0xff: i for i, t in enumerate(CRC_T)}   # low bytes are a permutation
XOR_KEYS = (0xcc3d4ffb, 0x593cdeaf, 0xcf4235de, 0xde34bd78, 0x52b34a75)
KEY20 = b"".join(struct.pack("<I", k ^ 0x38d2dfa4) for k in XOR_KEYS)
KEY20_ALGO3 = bytes.fromhex("FDE108EC0D0902DCDE4C8C6416627DE49716FFDC")


def _xorcrc_setup(name, size, algorithm=2):
    pre = bytearray()
    for ch in name.lower():
        v = ord(ch)
        pre.append(v & 0xff)
        if v >> 8:
            pre.append((v >> 8) & 0xff)
    s = size
    for _ in range(4):
        if s == 0:
            break
        pre.append(s & 0xff)
        s >>= 8
    crc = 0xFFFFFFFF
    if algorithm == 3:
        key20 = KEY20_ALGO3
        for i, b in enumerate(pre):
            comp = (crc & 0xff) ^ key20[i % 20]
            s = b ^ 0xff ^ comp
            crc = (crc >> 8) ^ ((CRC_T[s] & 0xFFFFFF00) | s)
    else:
        key20 = KEY20
        for i, b in enumerate(pre):
            crc = (crc >> 8) ^ CRC_T[(crc & 0xff) ^ b ^ key20[i % 20]]
    r = len(pre) % 20
    return crc, key20[r:] + key20[:r]


def xorcrc_decrypt(data, name, algorithm=2):
    crc, key = _xorcrc_setup(name, len(data), algorithm)
    out = bytearray(len(data))
    T = CRC_T
    for i, c in enumerate(data):
        comp = (crc & 0xff) ^ key[i % 20]
        s = c ^ 0xff ^ comp
        e = T[s]
        out[i] = (e & 0xff) ^ comp
        crc = (crc >> 8) ^ ((e & 0xFFFFFF00) | s)
    return bytes(out)


def xorcrc_encrypt(data, name, algorithm=2):
    crc, key = _xorcrc_setup(name, len(data), algorithm)
    out = bytearray(len(data))
    T, INV = CRC_T, CRC_INV
    for i, p in enumerate(data):
        comp = (crc & 0xff) ^ key[i % 20]
        s = INV[p ^ comp]
        out[i] = s ^ 0xff ^ comp
        crc = (crc >> 8) ^ ((T[s] & 0xFFFFFF00) | s)
    return bytes(out)


# ================================================================ container

class Entry(object):
    def __init__(self, name, size, csize, checksum, filetime, algorithm, blob):
        self.name, self.size, self.csize = name, size, csize
        self.checksum, self.filetime, self.algorithm = checksum, filetime, algorithm
        self.blob = blob


def read_kom(path):
    with open(path, "rb") as fp:
        d = fp.read()
    magic = d[:52].split(b"\0", 1)[0]
    if magic == MAGIC_V3:
        count, _compressed, _ftime, _adler, xsize = struct.unpack_from("<5I", d, 52)
        doc = parseString(d[72:72 + xsize].rstrip(b"\0"))
        entries, off = [], 72 + xsize
        for node in doc.getElementsByTagName("File"):
            csize = int(node.getAttribute("CompressedSize"))
            e = Entry(node.getAttribute("Name"), int(node.getAttribute("Size")), csize,
                      int(node.getAttribute("Checksum") or "0", 16),
                      int(node.getAttribute("FileTime") or "0", 16),
                      int(node.getAttribute("Algorithm") or "0"), d[off:off + csize])
            if len(e.blob) != csize:
                raise ValueError("%s: truncated entry %s" % (path, e.name))
            entries.append(e)
            off += csize
        if len(entries) != count:
            print("warning: header says %d entries, XML lists %d" % (count, len(entries)), file=sys.stderr)
        return entries
    if magic in (b"KOG GC TEAM MASSFILE V.0.1.", b"KOG GC TEAM MASSFILE V.0.2."):
        count = struct.unpack_from("<I", d, 52)[0]
        base = 60 + count * 72
        entries = []
        for i in range(count):
            p = 60 + i * 72
            name = d[p:p + 60].split(b"\0", 1)[0].decode("latin1")
            size, csize, off = struct.unpack_from("<3I", d, p + 60)
            blob = d[base + off:base + off + csize]
            entries.append(Entry(name, size, csize, zlib.adler32(blob), 0, 0, blob))
        return entries
    raise ValueError("%s: not a KOM archive" % path)


def write_kom(path, entries):
    xml = ['<?xml version="1.0"?><Files>']
    for e in entries:
        xml.append('<File Name=%s Size="%d" CompressedSize="%d" Checksum="%08x" FileTime="%08x" Algorithm="%d" />'
                   % (quoteattr(e.name), e.size, len(e.blob), e.checksum, e.filetime, e.algorithm))
    xml.append("</Files>")
    xml = "".join(xml).encode("utf-8")
    ftime = sum(e.filetime for e in entries) & 0xFFFFFFFF
    header = MAGIC_V3.ljust(52, b"\0") + struct.pack("<5I", len(entries), 1, ftime,
                                                       zlib.adler32(xml) & 0xFFFFFFFF, len(xml))
    tmp = path + ".tmp"
    with open(tmp, "wb") as fp:
        fp.write(header)
        fp.write(xml)
        for e in entries:
            fp.write(e.blob)
    os.replace(tmp, path)


def decode_entry(e):
    """Return the decoded data, or None for algorithms we keep opaque."""
    if e.algorithm == 0:
        return zlib.decompress(e.blob)
    if e.algorithm == 2:
        return zlib.decompress(xorcrc_decrypt(e.blob, e.name))
    if e.algorithm == 3:
        d = xorcrc_decrypt(e.blob, e.name, 3)
        inf = zlib.decompressobj(-zlib.MAX_WBITS)    # like the client: raw deflate from byte 6
        return inf.decompress(d[6:]) + inf.flush()
    return None


def encode_entry(name, data, algorithm, filetime=None):
    z = zlib.compress(data, 9)
    if algorithm == 2:
        blob = xorcrc_encrypt(z, name)
    elif algorithm == 3:
        z = z[:-4]
        blob = xorcrc_encrypt(struct.pack("<I", zlib.adler32(z) & 0xFFFFFFFF) + z, name, 3)
    else:
        blob = z
    ck = zlib.adler32(blob) & 0xFFFFFFFF
    return Entry(name, len(data), len(blob), ck, ck if filetime is None else filetime, algorithm, blob)


def is_bytecode(data):
    return data[:4] in (b"\x1bKL\x81", b"\x1bKL\x82", b"\x1bLJ\x01")


# 0x82 opcode -> 0x81 (stock LuaJIT 2.0) opcode; the other opcodes are unchanged
_OP82 = {}
for _a, _b, _n in ((0x0C, 0x10, 4), (0x10, 0x0C, 4), (0x14, 0x25, 6), (0x1A, 0x14, 17),
                   (0x2B, 0x3D, 8), (0x33, 0x2B, 6), (0x39, 0x31, 12)):
    for _i in range(_n):
        _OP82[_a + _i] = _b + _i
OP82_TO_81 = bytes(_OP82.get(i, i) for i in range(256))
OP81_TO_82 = bytes(sorted(range(256), key=lambda i: OP82_TO_81[i]))
assert all(OP82_TO_81[OP81_TO_82[i]] == i for i in range(256))


def bytecode_remap(data, table, version):
    """Rewrites the opcode byte of every instruction of a stripped KL dump."""
    out = bytearray(data)
    out[3] = version
    r = _Reader(bytes(out))
    r.p = 4
    if r.uleb() & 2 == 0:
        raise DecompileError("only stripped dumps can be remapped")
    while True:
        ln = r.uleb()
        if ln == 0:
            break
        end = r.p + ln
        r.p += 4
        r.uleb(); r.uleb()
        nbc = r.uleb()
        for i in range(nbc):
            out[r.p + 4 * i] = table[out[r.p + 4 * i]]
        r.p = end
    return bytes(out)


def bytecode_to_81(data):
    return bytecode_remap(data, OP82_TO_81, 0x81) if data[:4] == b"\x1bKL\x82" else data


def bytecode_to(data, version):
    """KL bytecode in the given layout (0x81 or 0x82); anything else is returned as is."""
    if data[:3] != b"\x1bKL" or data[3] == version:
        return data
    return bytecode_remap(bytecode_to_81(data), OP81_TO_82, 0x82) if version == 0x82 else bytecode_to_81(data)


# pack --format: algorithm of the encrypted entries and layout of the Lua bytecode
FORMATS = {
    "2014-03": (2, 0x81),   # our client's data036.kom
    "2014-08": (3, 0x82),   # data036.kom of August 2014
}


def sha256(b):
    return hashlib.sha256(b).hexdigest()


# ================================================================ LuaJIT compiler

COMPILE_LUA = BOM + b"""-- compile.lua src dst [src dst ...]: string.dump(loadfile(src), strip)
for i = 1, #arg, 2 do
  local f, err = loadfile(arg[i])
  if not f then io.stderr:write(tostring(err), "\\n") os.exit(1) end
  local o = assert(io.open(arg[i + 1], "wb"))
  o:write(string.dump(f, true))
  o:close()
end
"""


def find_luajit(explicit=None):
    for cand in (explicit, os.environ.get("KOM_LUAJIT"),
                 os.path.join(os.path.dirname(os.path.abspath(__file__)), "luajit-kog", "luajit")):
        if cand and os.path.isfile(cand) and os.access(cand, os.X_OK):
            return cand
    return None


def compile_lua(luajit, sources):
    """sources: {name: source bytes}.  Returns {name: bytecode}."""
    if not sources:
        return {}
    if luajit is None:
        raise SystemExit("error: %d edited script(s) need recompiling but the client LuaJIT was not "
                         "found (luajit-kog/luajit, --luajit or $KOM_LUAJIT): %s"
                         % (len(sources), ", ".join(sorted(sources))))
    out = {}
    with tempfile.TemporaryDirectory() as tmp:
        drv = os.path.join(tmp, "compile.lua")
        with open(drv, "wb") as fp:
            fp.write(COMPILE_LUA)
        args = []
        for i, (name, src) in enumerate(sorted(sources.items())):
            s = os.path.join(tmp, "%d.lua" % i)
            with open(s, "wb") as fp:
                fp.write(src if src.startswith(BOM) else BOM + src)
            args += [s, s + ".bc"]
        r = subprocess.run([luajit, drv] + args, capture_output=True, text=True)
        if r.returncode != 0:
            raise SystemExit("error: LuaJIT failed to compile an edited script:\n" + r.stderr.strip())
        for i, (name, _) in enumerate(sorted(sources.items())):
            with open(os.path.join(tmp, "%d.lua.bc" % i), "rb") as fp:
                bc = fp.read()
            if bc[:4] != b"\x1bKL\x81":
                raise SystemExit("error: %s does not produce KOG bytecode; is %s the client's LuaJIT?"
                                 % (luajit, name))
            out[name] = bc
    return out


# ================================================================ commands

def cmd_list(args):
    entries = read_kom(args.kom)
    for e in entries:
        print("%10d %10d  alg=%d  %08x  %s" % (e.size, e.csize, e.algorithm, e.checksum, e.name))
    print("%d entries" % len(entries))


def cmd_unpack(args):
    entries = read_kom(args.kom)
    out = args.out
    meta = os.path.join(out, META_DIR)
    bcdir = os.path.join(meta, "bytecode")
    os.makedirs(bcdir, exist_ok=True)
    manifest = {"source": os.path.basename(args.kom), "entries": []}
    stats = {"decompiled": 0, "bytecode": 0, "plain": 0, "raw": 0}
    for e in entries:
        if os.path.basename(e.name) != e.name or e.name in ("", ".", "..", META_DIR):
            raise SystemExit("error: refusing unsafe entry name %r" % e.name)
        data = decode_entry(e)
        rec = {"name": e.name, "algorithm": e.algorithm, "size": e.size, "filetime": e.filetime}
        if data is None:
            fname = "%s.%d.%d.___" % (e.name, e.size, e.algorithm)
            payload, rec["kind"] = e.blob, "raw"
        else:
            if len(data) != e.size:
                print("warning: %s: size %d, header says %d" % (e.name, len(data), e.size), file=sys.stderr)
            fname, payload, rec["kind"] = e.name, data, "plain"
            if e.name.lower().endswith(".lua") and is_bytecode(data):
                rec["kind"] = "bytecode"
                if not args.no_decompile:
                    try:
                        payload = BOM + decompile(bytecode_to_81(data)).encode("utf-8", "surrogateescape")
                        rec["kind"] = "decompiled"
                    except (DecompileError, RecursionError) as ex:
                        print("warning: %s: kept as bytecode (%s)" % (e.name, ex), file=sys.stderr)
                with open(os.path.join(bcdir, e.name), "wb") as fp:
                    fp.write(data)
        rec["file"] = fname
        rec["sha256"] = sha256(payload)
        with open(os.path.join(out, fname), "wb") as fp:
            fp.write(payload)
        manifest["entries"].append(rec)
        stats[rec["kind"]] += 1
    with open(os.path.join(meta, "manifest.json"), "w", encoding="utf-8") as fp:
        json.dump(manifest, fp, indent=1, ensure_ascii=False)
    print("%d entries: %d decompiled Lua, %d Lua kept as bytecode, %d plain, %d opaque"
          % (len(entries), stats["decompiled"], stats["bytecode"], stats["plain"], stats["raw"]))


def _parse_legacy(fname):
    # foo.ext.12345.2.___  (written by the original komextract)
    parts = fname[:-4].rsplit(".", 2)
    if len(parts) != 3 or not parts[1].isdigit() or not parts[2].isdigit():
        return None
    return parts[0], int(parts[1]), int(parts[2])


def cmd_pack(args):
    src = args.dir
    if not os.path.isdir(src):
        raise SystemExit("error: %s is not a directory" % src)
    meta = os.path.join(src, META_DIR)
    manifest = {"entries": []}
    if os.path.isfile(os.path.join(meta, "manifest.json")):
        with open(os.path.join(meta, "manifest.json"), encoding="utf-8") as fp:
            manifest = json.load(fp)
    by_file = {r["file"]: r for r in manifest["entries"]}
    present = sorted(f for f in os.listdir(src)
                     if f != META_DIR and os.path.isfile(os.path.join(src, f)))
    order = [r["file"] for r in manifest["entries"] if r["file"] in present]
    order += [f for f in present if f not in by_file]
    missing = [r["file"] for r in manifest["entries"] if r["file"] not in present]
    if missing:
        print("note: %d manifest entries no longer present, dropped: %s"
              % (len(missing), ", ".join(missing[:10])), file=sys.stderr)

    plan, recompile = [], {}
    for f in order:
        with open(os.path.join(src, f), "rb") as fp:
            data = fp.read()
        rec = by_file.get(f)
        if rec is None:
            legacy = _parse_legacy(f) if f.endswith(".___") else None
            if legacy:
                name, size, alg = legacy
                plan.append(("blob", Entry(name, size, len(data), zlib.adler32(data) & 0xFFFFFFFF,
                                           0, alg, data)))
            elif data:
                plan.append(("data", f, data, 0, None))
            continue
        name, alg, kind = rec["name"], rec["algorithm"], rec["kind"]
        same = sha256(data) == rec["sha256"]
        ftime = rec["filetime"] if same else None
        if kind == "raw":
            plan.append(("blob", Entry(name, rec.get("size", 0), len(data), zlib.adler32(data) & 0xFFFFFFFF,
                                       rec["filetime"], alg, data)))
            continue
        if kind == "decompiled" and same:
            with open(os.path.join(meta, "bytecode", name), "rb") as fp:
                data = fp.read()
        elif kind in ("decompiled", "bytecode") and not is_bytecode(data):
            recompile[name] = data
        plan.append(("data", name, data, alg, ftime))

    compiled = compile_lua(find_luajit(args.luajit), recompile)
    fmt = FORMATS.get(args.format)
    entries, edited = [], 0
    for p in plan:
        if p[0] == "blob":
            if fmt is not None and p[1].algorithm in (2, 3) and p[1].algorithm != fmt[0]:
                print("warning: %s is kept opaque with algorithm %d (it cannot be converted to %s)"
                      % (p[1].name, p[1].algorithm, args.format), file=sys.stderr)
            entries.append(p[1])
            continue
        _, name, data, alg, ftime = p
        if name in compiled:
            data = compiled[name]
            orig = os.path.join(meta, "bytecode", name)
            if fmt is None and os.path.isfile(orig):
                with open(orig, "rb") as fp:
                    if fp.read(4) == b"\x1bKL\x82":     # same opcode layout as the script it replaces
                        data = bytecode_remap(data, OP81_TO_82, 0x82)
        if fmt is not None:
            if alg in (2, 3):
                alg = fmt[0]
            data = bytecode_to(data, fmt[1])
        if ftime is None:
            edited += 1
        entries.append(encode_entry(name, data, alg, ftime))
    write_kom(args.out, entries)
    print("%s: %d entries (%d new or changed, %d Lua recompiled)"
          % (args.out, len(entries), edited, len(compiled)))


def cmd_decompile(args):
    with open(args.input, "rb") as fp:
        data = fp.read()
    text = BOM + decompile(bytecode_to_81(data)).encode("utf-8", "surrogateescape")
    if args.output:
        with open(args.output, "wb") as fp:
            fp.write(text)
    else:
        sys.stdout.buffer.write(text[3:])


def main():
    sys.setrecursionlimit(20000)
    ap = argparse.ArgumentParser(description="Elsword .kom archive tool")
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("list", help="list entries")
    p.add_argument("kom")
    p.set_defaults(func=cmd_list)
    p = sub.add_parser("unpack", help="extract (decrypting and decompiling Lua)")
    p.add_argument("kom")
    p.add_argument("out")
    p.add_argument("--no-decompile", action="store_true", help="write Lua bytecode as is")
    p.set_defaults(func=cmd_unpack)
    p = sub.add_parser("pack", help="build a .kom from a directory")
    p.add_argument("dir")
    p.add_argument("out")
    p.add_argument("--luajit", help="client LuaJIT used to recompile edited scripts")
    p.add_argument("--format", choices=["keep"] + sorted(FORMATS), default="keep",
                   help="keep: each entry as it was unpacked (default); 2014-03: algorithm 2 and Lua "
                        "bytecode 0x81 (our client's data036.kom); 2014-08: algorithm 3 and bytecode 0x82")
    p.set_defaults(func=cmd_pack)
    p = sub.add_parser("decompile", help="decompile one LuaJIT bytecode file")
    p.add_argument("input")
    p.add_argument("output", nargs="?")
    p.set_defaults(func=cmd_decompile)
    args = ap.parse_args()
    args.func(args)


# ================================================================ LuaJIT 2.0 decompiler
OPS = ("ISLT ISGE ISLE ISGT ISEQV ISNEV ISEQS ISNES ISEQN ISNEN ISEQP ISNEP "
       "ISTC ISFC IST ISF MOV NOT UNM LEN ADDVN SUBVN MULVN DIVVN MODVN "
       "ADDNV SUBNV MULNV DIVNV MODNV ADDVV SUBVV MULVV DIVVV MODVV POW CAT "
       "KSTR KCDATA KSHORT KNUM KPRI KNIL UGET USETV USETS USETN USETP UCLO "
       "FNEW TNEW TDUP GGET GSET TGETV TGETS TGETB TSETV TSETS TSETB TSETM "
       "CALLM CALL CALLMT CALLT ITERC ITERN VARG ISNEXT RETM RET RET0 RET1 "
       "FORI JFORI FORL IFORL JFORL ITERL IITERL JITERL LOOP ILOOP JLOOP JMP "
       "FUNCF IFUNCF JFUNCF FUNCV IFUNCV JFUNCV FUNCC FUNCCW").split()
OP = {n: i for i, n in enumerate(OPS)}
CMP_OPS = set(range(OP["ISLT"], OP["ISNEP"] + 1))
TEST_OPS = CMP_OPS | {OP["ISTC"], OP["ISFC"], OP["IST"], OP["ISF"]}
ARITH = {"ADD": "+", "SUB": "-", "MUL": "*", "DIV": "/", "MOD": "%"}

PROTO_CHILD, PROTO_VARARG, PROTO_HAS_RETURN = 0x01, 0x02, 0x20


class DecompileError(Exception):
    pass


class ForceLocal(Exception):
    def __init__(self, pc):
        Exception.__init__(self, pc)
        self.pc = pc


# ---------------------------------------------------------------- reader

class Proto(object):
    pass


class _Reader(object):
    def __init__(self, b):
        self.b, self.p = b, 0

    def byte(self):
        v = self.b[self.p]
        self.p += 1
        return v

    def uleb(self):
        v = sh = 0
        while True:
            c = self.byte()
            v |= (c & 0x7f) << sh
            sh += 7
            if c < 0x80:
                return v

    def uleb33(self):
        c = self.byte()
        v, sh = c >> 1, 6
        if c & 0x80:
            v &= 0x3f
            while True:
                c = self.byte()
                v |= (c & 0x7f) << sh
                sh += 7
                if c < 0x80:
                    break
        return v & 0xffffffff

    def mem(self, n):
        v = self.b[self.p:self.p + n]
        self.p += n
        return v


def _i32(v):
    return v - 0x100000000 if v & 0x80000000 else v


def _dbl(lo, hi):
    return struct.unpack("<d", struct.pack("<II", lo, hi))[0]


def _ktabk(r):
    tp = r.uleb()
    if tp >= 5:
        return r.mem(tp - 5)
    if tp == 3:
        return _i32(r.uleb())
    if tp == 4:
        lo = r.uleb()
        return _dbl(lo, r.uleb())
    return (None, False, True)[tp]


def parse_bytecode(data):
    r = _Reader(data)
    if r.mem(3) not in (b"\x1bKL", b"\x1bLJ"):
        raise DecompileError("not a LuaJIT bytecode dump")
    r.byte()
    flags = r.uleb()
    if flags & 1:
        raise DecompileError("big-endian dump not supported")
    if not flags & 2:
        r.mem(r.uleb())        # chunk name
    stack = []
    while True:
        ln = r.uleb()
        if ln == 0:
            break
        end = r.p + ln
        p = Proto()
        p.flags, p.nparams, p.framesize, nuv = r.byte(), r.byte(), r.byte(), r.byte()
        nkgc, nkn, nbc = r.uleb(), r.uleb(), r.uleb()
        if not flags & 2:
            if r.uleb():
                raise DecompileError("debug info not supported")
        p.bc = list(struct.unpack("<%dI" % nbc, r.mem(4 * nbc)))
        p.uv = list(struct.unpack("<%dH" % nuv, r.mem(2 * nuv)))
        kgc = []
        for _ in range(nkgc):
            tp = r.uleb()
            if tp >= 5:
                kgc.append(r.mem(tp - 5))
            elif tp == 1:
                na, nh = r.uleb(), r.uleb()
                arr = [_ktabk(r) for _ in range(na)]
                hsh = [(_ktabk(r), _ktabk(r)) for _ in range(nh)]
                kgc.append(("tab", arr, hsh))
            elif tp == 0:
                kgc.append(stack.pop())
            else:
                raise DecompileError("cdata constants not supported")
        p.kgc = kgc[::-1]
        kn = []
        for _ in range(nkn):
            isnum = r.b[r.p] & 1
            lo = r.uleb33()
            kn.append(_dbl(lo, r.uleb()) if isnum else float(_i32(lo)))
        p.kn = kn
        if r.p != end:
            raise DecompileError("bad prototype length")
        stack.append(p)
    if r.p != len(data) or len(stack) != 1:
        raise DecompileError("trailing data in dump")
    return stack[0]


# ---------------------------------------------------------------- AST

PREC_OR, PREC_AND, PREC_CMP, PREC_CAT, PREC_ADD, PREC_MUL, PREC_UN, PREC_POW = 1, 2, 3, 5, 6, 7, 8, 10
PREC_ATOM = 12
LUA_KEYWORDS = set("and break do else elseif end false for function goto if in "
                   "local nil not or repeat return then true until while".split())
_IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def is_ident(b):
    if not isinstance(b, bytes):
        return False
    try:
        s = b.decode("ascii")
    except UnicodeDecodeError:
        return False
    return bool(_IDENT.match(s)) and s not in LUA_KEYWORDS


def lua_str(b):
    out = ['"']
    i, n = 0, len(b)
    while i < n:
        c = b[i]
        if c >= 0x80:
            # keep valid UTF-8 sequences readable, escape everything else
            ln = 2 if 0xc2 <= c <= 0xdf else 3 if 0xe0 <= c <= 0xef else 4 if 0xf0 <= c <= 0xf4 else 0
            if ln and i + ln <= n:
                try:
                    b[i:i + ln].decode("utf-8")
                    out.append(b[i:i + ln].decode("utf-8"))
                    i += ln
                    continue
                except UnicodeDecodeError:
                    pass
            out.append("\\%d" % c)
        elif c == 0x22:
            out.append('\\"')
        elif c == 0x5c:
            out.append("\\\\")
        elif c == 0x0a:
            out.append("\\n")
        elif c == 0x0d:
            out.append("\\r")
        elif c == 0x09:
            out.append("\\t")
        elif c < 0x20 or c == 0x7f:
            nxt = b[i + 1] if i + 1 < n else 0
            out.append("\\%03d" % c if 0x30 <= nxt <= 0x39 else "\\%d" % c)
        else:
            out.append(chr(c))
        i += 1
    out.append('"')
    return "".join(out)


def lua_num(v):
    if v != v:
        return "(0/0)"
    if v == math.inf:
        return "(1/0)"
    if v == -math.inf:
        return "(-1/0)"
    if v == int(v) and abs(v) < 1e15:
        return str(int(v))
    r = repr(v)
    return r


class Expr(object):
    prec = PREC_ATOM
    multi = False            # call/vararg that may yield several values

    def r(self, prec=0):
        s = self.render()
        return "(%s)" % s if self.prec < prec else s


class Const(Expr):
    def __init__(self, v):
        self.v = v
        if isinstance(v, float) and (v < 0 or (v == 0 and math.copysign(1, v) < 0)):
            self.prec = PREC_UN

    def render(self):
        v = self.v
        if v is None:
            return "nil"
        if v is True:
            return "true"
        if v is False:
            return "false"
        if isinstance(v, bytes):
            return lua_str(v)
        return lua_num(v)


class Name(Expr):
    def __init__(self, name):
        self.name = name

    def render(self):
        return self.name


class LocalRef(Name):
    def __init__(self, var):
        self.var = var
        self.name = var.name

    def render(self):
        return self.var.name


class Global(Name):
    pass


class Vararg(Expr):
    def __init__(self, multi):
        self.multi = multi

    def render(self):
        return "..."


class Index(Expr):
    def __init__(self, obj, key):
        self.obj, self.key = obj, key

    def render(self):
        o = self.obj.r(PREC_ATOM)
        if isinstance(self.obj, (Const, FuncExpr, TableExpr)) or o.startswith("("):
            o = "(%s)" % self.obj.render() if not o.startswith("(") else o
        if isinstance(self.key, Const) and is_ident(self.key.v):
            return "%s.%s" % (o, self.key.v.decode())
        return "%s[%s]" % (o, self.key.render())


class MethodRef(Expr):
    def __init__(self, obj, name):
        self.obj, self.name = obj, name


class SelfArg(Expr):
    pass


class Call(Expr):
    def __init__(self, func, args, multi=False):
        self.func, self.args, self.multi = func, args, multi

    def render(self):
        if isinstance(self.func, MethodRef):
            o = self.func.obj.r(PREC_ATOM)
            if isinstance(self.func.obj, (Const, FuncExpr, TableExpr)) and not o.startswith("("):
                o = "(%s)" % o
            return "%s:%s(%s)" % (o, self.func.name.decode(), ", ".join(a.render() for a in self.args))
        f = self.func.r(PREC_ATOM)
        if isinstance(self.func, (FuncExpr, Const, TableExpr)) and not f.startswith("("):
            f = "(%s)" % f
        return "%s(%s)" % (f, ", ".join(a.render() for a in self.args))


class Paren(Expr):
    """Truncates a multi-valued expression to one value: (f())."""
    def __init__(self, e):
        self.e = e

    def render(self):
        return "(%s)" % self.e.render()


class BinOp(Expr):
    def __init__(self, op, a, b, prec, right=False):
        self.op, self.a, self.b, self.prec, self.right = op, a, b, prec, right

    def render(self):
        if self.right:
            return "%s %s %s" % (self.a.r(self.prec + 1), self.op, self.b.r(self.prec))
        return "%s %s %s" % (self.a.r(self.prec), self.op, self.b.r(self.prec + 1))


class UnOp(Expr):
    prec = PREC_UN

    def __init__(self, op, a):
        self.op, self.a = op, a

    def render(self):
        sep = " " if self.op == "not" else ""
        inner = self.a.r(PREC_UN)
        if self.op == "-" and inner.startswith("-"):
            inner = "(%s)" % inner
        return "%s%s%s" % (self.op, sep, inner)


class Concat(Expr):
    prec = PREC_CAT

    def __init__(self, parts):
        self.parts = parts

    def render(self):
        return " .. ".join(p.r(PREC_CAT + 1) for p in self.parts)


class Not(Expr):
    prec = PREC_UN

    def __init__(self, a):
        self.a = a

    def render(self):
        return "not " + self.a.r(PREC_UN)


class Cmp(Expr):
    """Comparison as emitted by the test instruction (op is the bytecode op)."""
    prec = PREC_CMP

    def __init__(self, op, a, b, swap=False):
        self.op, self.a, self.b, self.swap = op, a, b, swap

    def negate(self):
        return Cmp(self.op ^ 1, self.a, self.b, self.swap)

    def render(self):
        name = OPS[self.op]
        a, b = self.a, self.b
        if name in ("ISLT", "ISLE"):
            sym = "<" if name == "ISLT" else "<="
            if self.swap:
                return "%s %s %s" % (b.r(PREC_CMP + 1), ">" if sym == "<" else ">=", a.r(PREC_CMP + 1))
            return "%s %s %s" % (a.r(PREC_CMP + 1), sym, b.r(PREC_CMP + 1))
        if name in ("ISGE", "ISGT"):
            return "not (%s)" % self.negate().render()
        sym = "==" if name.startswith("ISEQ") else "~="
        return "%s %s %s" % (a.r(PREC_CMP + 1), sym, b.r(PREC_CMP + 1))

    @property
    def needs_not(self):
        return OPS[self.op] in ("ISGE", "ISGT")


class Logic(Expr):
    def __init__(self, op, a, b):
        self.op, self.a, self.b = op, a, b
        self.prec = PREC_AND if op == "and" else PREC_OR

    def render(self):
        return "%s %s %s" % (self.a.r(self.prec), self.op, self.b.r(self.prec + 1))


class Truth(Expr):
    """Truthiness test of a value (used inside condition trees)."""
    def __init__(self, e, neg=False):
        self.e, self.neg = e, neg


class FuncExpr(Expr):
    def __init__(self, params, vararg, body, decl_name=None):
        self.params, self.vararg, self.body = params, vararg, body

    def render(self, name=None, indent=""):
        ps = list(self.params) + (["..."] if self.vararg else [])
        head = "function%s(%s)" % (" " + name if name else "", ", ".join(ps))
        inner = render_block(self.body, indent + "\t")
        if not inner:
            return head + " end"
        return head + "\n" + inner + indent + "end"

    def r(self, prec=0):
        return self.render(indent=_CUR_INDENT[0])


class TableExpr(Expr):
    def __init__(self):
        self.arr = {}        # positional index (1..n) -> Expr
        self.hash = []       # template (constant) hash entries
        self.events = []     # dynamic fills in evaluation order: ("pos", i) / ("hash", k, v)
        self.tail = None     # multres expression appended positionally

    @staticmethod
    def _field(k, v):
        if isinstance(k, Const) and is_ident(k.v):
            return "%s = %s" % (k.v.decode(), v.render())
        return "[%s] = %s" % (k.render(), v.render())

    def render(self):
        n = max(self.arr) if self.arr else 0
        last = self.arr.get(n)
        # a single-valued call/vararg must not end up as the last positional
        # item, or it would expand to all its results
        single_tail = (self.tail is None and last is not None and isinstance(last, (Call, Vararg))
                       and not last.multi and (not self.events or self.events[-1] == ("pos", n)))
        dyn_keys = set(ev[1].render() for ev in self.events if ev[0] == "hash")
        late = [(k, v) for k, v in self.hash if k.render() in dyn_keys]
        early = [(k, v) for k, v in self.hash if k.render() not in dyn_keys]
        items = [] if single_tail and early else [self._field(k, v) for k, v in early]
        done = [0]

        def upto(i):
            while done[0] < i:
                done[0] += 1
                e = self.arr.get(done[0])
                items.append("nil" if e is None else e.render())
        for ev in self.events:
            if ev[0] == "pos":
                upto(ev[1])
            else:
                items.append(self._field(ev[1], ev[2]))
        upto(n)
        items.extend(self._field(k, v) for k, v in late)
        if single_tail:
            if early:
                items.extend(self._field(k, v) for k, v in early)
            else:
                items[-1] = "(%s)" % items[-1]
        if self.tail is not None:
            items.append(self.tail.render())
        if not items:
            return "{}"
        s = "{ " + ", ".join(items) + " }"
        if len(s) > 100 and _CUR_INDENT[0] is not None:
            ind = _CUR_INDENT[0]
            return "{\n" + "".join("%s\t%s,\n" % (ind, it.replace("\n", "\n\t")) for it in items) + ind + "}"
        return s


_CUR_INDENT = [""]


# ---------------------------------------------------------------- statements

class Var(object):
    def __init__(self, name):
        self.name = name


class Stmt(object):
    pass


class Local(Stmt):
    def __init__(self, vars_, values):
        self.vars, self.values = vars_, values


class LocalFunc(Stmt):
    def __init__(self, var, func):
        self.var, self.func = var, func


class Assign(Stmt):
    def __init__(self, targets, values):
        self.targets, self.values = targets, values


class CallStmt(Stmt):
    def __init__(self, call):
        self.call = call


class Return(Stmt):
    def __init__(self, values):
        self.values = values


class Break(Stmt):
    pass


class If(Stmt):
    def __init__(self, cond, then, orelse):
        self.cond, self.then, self.orelse = cond, then, orelse


class While(Stmt):
    def __init__(self, cond, body):
        self.cond, self.body = cond, body


class Repeat(Stmt):
    def __init__(self, body, cond):
        self.body, self.cond = body, cond


class NumFor(Stmt):
    def __init__(self, var, start, stop, step, body):
        self.var, self.start, self.stop, self.step, self.body = var, start, stop, step, body


class GenFor(Stmt):
    def __init__(self, vars_, exprs, body):
        self.vars, self.exprs, self.body = vars_, exprs, body


class Do(Stmt):
    def __init__(self, body):
        self.body = body


def _rl(exprs):
    return ", ".join(e.render() for e in exprs)


def render_block(stmts, ind):
    out = []
    for s in stmts:
        out.append(render_stmt(s, ind))
    return "".join(out)


def render_stmt(s, ind):
    _CUR_INDENT[0] = ind
    if isinstance(s, Local):
        names = ", ".join(v.name for v in s.vars)
        if not s.values:
            return "%slocal %s\n" % (ind, names)
        return "%slocal %s = %s\n" % (ind, names, _rl(s.values))
    if isinstance(s, LocalFunc):
        return "%slocal %s\n" % (ind, s.func.render(name=s.var.name, indent=ind))
    if isinstance(s, Assign):
        if (len(s.targets) == 1 and len(s.values) == 1 and isinstance(s.values[0], FuncExpr)
                and _func_name(s.targets[0])):
            return "%s%s\n" % (ind, s.values[0].render(name=_func_name(s.targets[0]), indent=ind))
        return "%s%s = %s\n" % (ind, _rl(s.targets), _rl(s.values))
    if isinstance(s, CallStmt):
        return "%s%s\n" % (ind, s.call.render())
    if isinstance(s, Return):
        if not s.values:
            return "%sreturn\n" % ind
        return "%sreturn %s\n" % (ind, _rl(s.values))
    if isinstance(s, Break):
        return "%sbreak\n" % ind
    if isinstance(s, If):
        out = "%sif %s then\n%s" % (ind, s.cond.render(), render_block(s.then, ind + "\t"))
        orelse = s.orelse
        while orelse and len(orelse) == 1 and isinstance(orelse[0], If):
            e = orelse[0]
            _CUR_INDENT[0] = ind
            out += "%selseif %s then\n%s" % (ind, e.cond.render(), render_block(e.then, ind + "\t"))
            orelse = e.orelse
        if orelse:
            out += "%selse\n%s" % (ind, render_block(orelse, ind + "\t"))
        return out + "%send\n" % ind
    if isinstance(s, While):
        return "%swhile %s do\n%s%send\n" % (ind, s.cond.render(), render_block(s.body, ind + "\t"), ind)
    if isinstance(s, Repeat):
        body = render_block(s.body, ind + "\t")
        _CUR_INDENT[0] = ind
        return "%srepeat\n%s%suntil %s\n" % (ind, body, ind, s.cond.render())
    if isinstance(s, NumFor):
        rng = "%s, %s" % (s.start.render(), s.stop.render())
        if s.step is not None:
            rng += ", " + s.step.render()
        return "%sfor %s = %s do\n%s%send\n" % (ind, s.var.name, rng, render_block(s.body, ind + "\t"), ind)
    if isinstance(s, GenFor):
        return "%sfor %s in %s do\n%s%send\n" % (ind, ", ".join(v.name for v in s.vars), _rl(s.exprs),
                                                 render_block(s.body, ind + "\t"), ind)
    if isinstance(s, Do):
        return "%sdo\n%s%send\n" % (ind, render_block(s.body, ind + "\t"), ind)
    raise DecompileError("cannot render %r" % s)


def _func_name(t):
    if isinstance(t, Global):
        return t.name
    if isinstance(t, Index) and isinstance(t.key, Const) and is_ident(t.key.v):
        base = _func_name(t.obj) if not isinstance(t.obj, LocalRef) else t.obj.render()
        if base:
            return "%s.%s" % (base, t.key.v.decode())
    if isinstance(t, LocalRef):
        return None
    return None


# ---------------------------------------------------------------- conditions

def negate(c):
    if isinstance(c, Cmp):
        return c.negate()
    if isinstance(c, Truth):
        return Truth(c.e, not c.neg)
    if isinstance(c, Logic):
        return Logic("or" if c.op == "and" else "and", negate(c.a), negate(c.b))
    raise DecompileError("cannot negate")


def cond_expr(c):
    """Convert a condition tree (Cmp/Truth/Logic) into a renderable expression."""
    if isinstance(c, Truth):
        return Not(c.e) if c.neg else c.e
    if isinstance(c, Logic):
        return Logic(c.op, cond_expr(c.a), cond_expr(c.b))
    return c


# ---------------------------------------------------------------- decompiler

class Pending(object):
    __slots__ = ("expr", "pc", "group", "start")

    def __init__(self, expr, pc, group=None, start=None):
        self.expr, self.pc, self.group = expr, pc, group
        self.start = pc if start is None else start


class FuncDecompiler(object):
    def __init__(self, proto, uvnames, depth=0):
        self.p = proto
        self.uvnames = uvnames
        self.depth = depth
        self.forced = set()
        self.strs = set(k.decode("latin1") for k in proto.kgc if isinstance(k, bytes))

    # -- helpers
    def ins(self, pc):
        i = self.p.bc[pc]
        return i & 0xff, (i >> 8) & 0xff, (i >> 16) & 0xff, i >> 24, i >> 16

    def jt(self, pc):
        return pc + 1 + (self.p.bc[pc] >> 16) - 0x8000

    def kstr(self, idx):
        v = self.p.kgc[idx]
        if not isinstance(v, bytes):
            raise DecompileError("expected string constant")
        return v

    def var(self, slot):
        v = self.slotvars.get(slot)
        if v is None:
            v = self.newvar(slot)
        return v

    def newvar(self, slot, prefix=None):
        if prefix is None:
            prefix = "p" if slot < self.p.nparams else "v"
        base = "%s%d" % (prefix, slot) if self.depth == 0 or prefix != "p" else "%s%d_%d" % (prefix, slot, self.depth)
        if self.depth and prefix == "v":
            base = "v%d_%d" % (slot, self.depth)
        name = base
        n = 1
        while name in self.strs or name in self.uvset:
            name = "%s_%d" % (base, n)
            n += 1
        v = Var(name)
        self.slotvars[slot] = v
        return v

    # -- entry
    def decompile(self):
        for _ in range(500):
            try:
                return self._decompile_once()
            except ForceLocal as e:
                if e.pc in self.forced:
                    raise DecompileError("local inference did not converge at pc %d" % e.pc)
                self.forced.add(e.pc)
        raise DecompileError("too many restarts")

    def _decompile_once(self):
        p = self.p
        self.uvset = set(self.uvnames)
        self.slotvars = {}
        self.params = [self.newvar(i).name for i in range(p.nparams)]
        self.pend = {}
        self.skip_gap = None
        self.decl_pos = {}
        self.cur_ends = frozenset()
        self.consumed = {}
        self.cstarts = []
        self.nact = p.nparams
        self.n = len(p.bc)
        # Drop the implicit trailing RET0
        end = self.n
        body = self.block(0, end, None)
        if (body and isinstance(body[-1], Return) and not body[-1].values
                and OPS[p.bc[-1] & 0xff] == "RET0" and not self._ret0_explicit()):
            body.pop()
        return FuncExpr(self.params, bool(p.flags & PROTO_VARARG), body)

    def _ret0_explicit(self):
        p = self.p
        if not p.flags & PROTO_HAS_RETURN:
            return False
        rets = [i for i in p.bc[:-1] if OPS[i & 0xff] in ("RET0", "RET1", "RET", "RETM", "CALLT", "CALLMT")]
        return not rets

    # -- pending temp management
    def top(self):
        t = self.nact
        while t in self.pend:
            t += 1
        return t

    def take(self, slot, pc):
        if slot < self.nact:
            return LocalRef(self.var(slot))
        pe = self.pend.pop(slot, None)
        if pe is None:
            if slot in self.consumed:
                raise ForceLocal(self.consumed[slot])
            raise DecompileError("read of undefined slot %d at pc %d" % (slot, pc))
        # Stack discipline: nothing above may remain pending
        for s in self.pend:
            if s > slot:
                self.pend[slot] = pe
                raise ForceLocal(pe.pc)
        if isinstance(pe.expr, (SelfArg, MethodRef)) or pe.group:
            self.consumed.pop(slot, None)
        else:
            self.consumed[slot] = pe.pc
        self.cstarts.append(pe.start)
        return pe.expr

    def take_many(self, slots, pc):
        """Read several operand slots; temps are consumed top-down."""
        vals = {}
        for s in sorted(set(slots), reverse=True):
            vals[s] = self.take(s, pc) if s not in vals else vals[s]
        return [vals[s] for s in slots]

    def define(self, slot, expr, pc, stmts):
        if slot < self.nact and not self.pend and self._temp_use(pc, slot):
            self.close_do(slot, stmts)
        if slot < self.nact:
            self.flush(stmts)
            stmts.append(Assign([LocalRef(self.var(slot))], [expr]))
            return
        if slot in self.pend:
            raise ForceLocal(self.pend[slot].pc)
        t = self.top()
        if slot > t and t in self.consumed and t not in self.pend and not self._late_fill(pc, slot, t) \
                and self.skip_gap != pc:
            raise ForceLocal(self.consumed[t])
        self.pend[slot] = Pending(expr, pc, start=min([pc] + self.cstarts))
        if pc in self.forced:
            self.promote(slot, stmts)

    def _temp_use(self, pc, slot):
        """Is the value written to 'slot' at pc used as a call frame base
        (so 'slot' is a temporary, not a local)?"""
        q = pc + 1
        while q < self.n and q < pc + 400:
            op, a, c, b, d = self.ins(q)
            nm = OPS[op]
            if nm in ("CALL", "CALLM", "CALLT", "CALLMT") and a == slot:
                return True
            if nm in ("CALL", "CALLM") and a < slot < a + 1 + (c if nm == "CALLM" else c - 1) + 1:
                return False
            if op in TEST_OPS or nm in ("JMP", "FORI", "FORL", "LOOP", "ITERL", "UCLO", "ISNEXT", "RET", "RET0",
                                        "RET1", "RETM", "GSET", "TSETS", "TSETB", "TSETV", "USETV", "TSETM"):
                return False
            if nm == "MOV" and d == slot:
                return False
            q += 1
        return False

    def close_do(self, slot, stmts):
        """Locals from 'slot' up went out of scope: wrap their statements in
        an explicit do ... end block."""
        pos = self.decl_pos.get(slot)
        if pos is None or pos[0] is not stmts:
            raise DecompileError("scope end of local %d not in current block" % slot)
        idx = pos[1]
        body = stmts[idx:]
        del stmts[idx:]
        stmts.append(Do(body))
        for s in list(self.decl_pos):
            if s >= slot:
                del self.decl_pos[s]
        self.nact = slot

    def _late_fill(self, pc, slot, t):
        """A relocatable expression may be emitted before a constant that
        gets the lower register (e.g. 'K > a.b' compiles a.b first)."""
        if slot != t + 1 or pc + 1 >= self.n:
            return False
        nop, na = self.ins(pc + 1)[:2]
        return OPS[nop] in ("KSHORT", "KNUM", "KSTR", "KPRI") and na == t

    def promote(self, upto, stmts):
        """Turn pending temps with slot <= upto into local declarations."""
        slots = sorted(s for s in self.pend if s <= upto)
        if not slots:
            return
        if slots[0] != self.nact:
            # Gap: the slots in between hold locals without a visible definition.
            for s in range(self.nact, slots[0]):
                self.decl_pos[s] = (stmts, len(stmts))
                stmts.append(Local([self.newvar(s)], []))
        i = 0
        while i < len(slots):
            s = slots[i]
            pe = self.pend.pop(s)
            grp = [s]
            vals = [pe.expr]
            if pe.group == "call":
                while i + 1 < len(slots) and self.pend[slots[i + 1]].group == ("part", s):
                    i += 1
                    grp.append(slots[i])
                    self.pend.pop(slots[i])
            elif isinstance(pe.expr, Const) and pe.expr.v is None:
                while (i + 1 < len(slots) and isinstance(self.pend[slots[i + 1]].expr, Const)
                       and self.pend[slots[i + 1]].expr.v is None and self.pend[slots[i + 1]].group != "call"):
                    i += 1
                    grp.append(slots[i])
                    self.pend.pop(slots[i])
                vals = []
            if isinstance(pe.expr, FuncExpr) and getattr(pe.expr, "selfref", False):
                v = self.newvar(s)
                pe.expr.fix_self(v)
                self.decl_pos[s] = (stmts, len(stmts))
                stmts.append(LocalFunc(v, pe.expr))
            else:
                if pe.group == "part":
                    raise DecompileError("orphan multi-result part")
                vs = [self.newvar(x) for x in grp]
                for x in grp:
                    self.decl_pos[x] = (stmts, len(stmts))
                stmts.append(Local(vs, vals))
            self.nact = grp[-1] + 1
            i += 1

    def flush(self, stmts):
        """Statement boundary: every remaining pending temp is a local."""
        if self.pend:
            self.promote(max(self.pend), stmts)

    # -- blocks
    def block(self, start, end, loop, ends=frozenset()):
        """Decompile [start, end). loop = (break_target,) or None.
        ends: jump targets that are equivalent to 'end' (LuaJIT merges the
        pending jumps of a trailing if into the following JMP)."""
        saved = self.cur_ends
        ends = set(ends) | {end}
        # a JMP right at the block end makes its target equivalent to the end
        if end < self.n and OPS[self.ins(end)[0]] == "JMP":
            ends.add(self.jt(end))
        self.cur_ends = frozenset(ends)
        stmts = []
        pc = start
        while pc < end:
            pc = self.step(pc, end, stmts, loop)
        if self.pend:
            self.flush(stmts)
        self.cur_ends = saved
        return stmts

    def subblock(self, start, end, loop, ends=frozenset()):
        saved_nact, saved_vars = self.nact, dict(self.slotvars)
        if self.pend:
            raise DecompileError("pending temps across block boundary")
        body = self.block(start, end, loop, ends)
        self.nact = saved_nact
        # restore outer names (inner declarations go out of scope)
        for s in list(self.slotvars):
            if s in saved_vars:
                self.slotvars[s] = saved_vars[s]
        return body

    def step(self, pc, end, stmts, loop):
        op, a, c, b, d = self.ins(pc)
        name = OPS[op]
        if op in TEST_OPS:
            return self.do_cond(pc, end, stmts, loop)
        if name == "FORI":
            return self.do_numfor(pc, end, stmts, loop)
        if name == "LOOP":
            return self.do_loop(pc, end, stmts, loop)
        if name in ("JMP", "ISNEXT", "UCLO"):
            tgt = self.jt(pc)
            if name == "UCLO" and tgt == pc + 1:
                self.flush(stmts)
                return pc + 1
            if name in ("JMP", "ISNEXT") and tgt < self.n and OPS[self.p.bc[tgt] & 0xff] in ("ITERC", "ITERN") \
                    and tgt + 1 < self.n and OPS[self.p.bc[tgt + 1] & 0xff] == "ITERL" and self.jt(tgt + 1) == pc + 1:
                return self.do_genfor(pc, end, stmts, loop)
            if loop is not None and tgt == loop[0]:
                self.flush(stmts)
                stmts.append(Break())
                return pc + 1
            if name == "UCLO" and loop is not None and tgt == loop[0]:
                self.flush(stmts)
                stmts.append(Break())
                return pc + 1
            if tgt == pc + 1:
                self.flush(stmts)
                return pc + 1
            # Dead jump after a return (escape jump of an if whose body returned)
            raise DecompileError("unstructured jump at pc %d" % pc)
        return self.simple(pc, stmts)

    # -- straight-line instructions
    def simple(self, pc, stmts):
        self.cstarts = []
        r = self._simple(pc, stmts)
        return pc + 1 if r is None else r

    def _simple(self, pc, stmts):
        op, a, c, b, d = self.ins(pc)
        name = OPS[op]
        p = self.p
        if name == "MOV":
            nxt = self.ins(pc + 1) if pc + 1 < self.n else None
            if nxt and OPS[nxt[0]] == "TGETS" and nxt[1] + 1 == a and nxt[3] == d:
                return  # method call setup; handled by TGETS
            if nxt and OPS[nxt[0]] == "KSTR" and pc + 2 < self.n:
                n2 = self.ins(pc + 2)
                if OPS[n2[0]] == "TGETV" and n2[1] + 1 == a and n2[3] == d and n2[2] == a + 1:
                    self.skip_gap = pc + 1
                    return
            if a in self.pend and a < d:
                # MOV into an unconsumed pending slot: that slot is a local
                raise ForceLocal(self.pend[a].pc)
            if d >= self.nact and d in self.pend:
                pe = self.pend[d]
                # x = f() with a local x: call result is copied by MOV
                if not (a < self.nact and isinstance(pe.expr, (Call, Vararg)) and not pe.group):
                    raise ForceLocal(pe.pc)
            self.define(a, self.take(d, pc), pc, stmts)
        elif name == "KSTR":
            self.define(a, Const(self.kstr(d)), pc, stmts)
        elif name == "KSHORT":
            v = d - 0x10000 if d & 0x8000 else d
            self.define(a, Const(float(v)), pc, stmts)
        elif name == "KNUM":
            self.define(a, Const(p.kn[d]), pc, stmts)
        elif name == "KPRI":
            self.define(a, Const((None, False, True)[d]), pc, stmts)
        elif name == "KNIL":
            for s in range(a, d + 1):
                self.define(s, Const(None), pc, stmts)
        elif name == "UGET":
            self.define(a, Name(self.uvnames[d]), pc, stmts)
        elif name == "GGET":
            self.define(a, Global(self.gname(d)), pc, stmts)
        elif name == "FNEW":
            self.define(a, self.closure(d, a), pc, stmts)
        elif name == "TNEW":
            t = TableExpr()
            t.hint = d
            self.define(a, t, pc, stmts)
        elif name == "TDUP":
            self.define(a, self.template(p.kgc[d]), pc, stmts)
        elif name in ("TGETS", "TGETB", "TGETV"):
            prev = self.ins(pc - 1) if pc > 0 else None
            method = False
            if name == "TGETS" and prev and OPS[prev[0]] == "MOV" and prev[1] == a + 1 and prev[4] == b:
                method = True
            if name == "TGETV" and pc >= 2 and OPS[prev[0]] == "KSTR" and prev[1] == a + 2:
                p2 = self.ins(pc - 2)
                if OPS[p2[0]] == "MOV" and p2[1] == a + 1 and p2[4] == b and c == a + 2:
                    self.pend.pop(c, None)
                    obj = self.take(b, pc)
                    self.cstarts.append(pc - 2)
                    self.define(a, MethodRef(obj, self.kstr(prev[4])), pc, stmts)
                    self.pend[a + 1] = Pending(SelfArg(), pc)
                    return
            if method:
                if a < self.nact and not self.pend:
                    self.close_do(a, stmts)
                obj = self.take(b, pc)
                self.cstarts.append(pc - 1)
                self.define(a, MethodRef(obj, self.kstr(c)), pc, stmts)
                self.pend[a + 1] = Pending(SelfArg(), pc)
                return
            if name == "TGETV":
                obj, key = self.take_many([b, c], pc)
            else:
                key = Const(self.kstr(c)) if name == "TGETS" else Const(float(c))
                obj = self.take(b, pc)
            self.define(a, Index(obj, key), pc, stmts)
        elif name in ("UNM", "NOT", "LEN"):
            v = self.take(d, pc)
            if name == "NOT":
                e = Not(v)
            else:
                e = UnOp("-" if name == "UNM" else "#", v)
            self.define(a, e, pc, stmts)
        elif name[:3] in ARITH and name[3:] in ("VN", "NV", "VV"):
            sym = ARITH[name[:3]]
            prec = PREC_ADD if sym in "+-" else PREC_MUL
            kind = name[3:]
            if kind == "VV":
                x, y = self.take_many([b, c], pc)
            elif kind == "VN":
                x = self.take(b, pc)
                y = Const(p.kn[c])
            else:
                y = self.take(b, pc)
                x = Const(p.kn[c])
            self.define(a, BinOp(sym, x, y, prec), pc, stmts)
        elif name == "POW":
            x, y = self.take_many([b, c], pc)
            self.define(a, BinOp("^", x, y, PREC_POW, right=True), pc, stmts)
        elif name == "CAT":
            parts = [None] * (c - b + 1)
            for s in range(c, b - 1, -1):
                parts[s - b] = self.take(s, pc)
            flat = []
            for x in parts:
                flat.append(x)
            self.define(a, Concat(flat), pc, stmts)
        elif name == "VARG":
            if b == 0:
                self.define(a, Vararg(True), pc, stmts)
            elif b == 2:
                self.define(a, Vararg(False), pc, stmts)
            else:
                for s in range(a, a + b - 1):
                    if s == a:
                        self.define(s, Vararg(True), pc, stmts)
                        self.pend[s].group = "call"
                    else:
                        self.define(s, Const(None), pc, stmts)
                        self.pend[s].group = ("part", a)
        elif name in ("GSET", "TSETS", "TSETB", "TSETV", "USETV", "USETS", "USETN", "USETP"):
            return self.store(pc, stmts)
        elif name == "TSETM":
            tbl = self.pend.get(a - 1)
            if tbl is None or not isinstance(tbl.expr, TableExpr):
                raise DecompileError("TSETM outside constructor")
            val = self.take(a, pc)
            tbl.expr.tail = val
        elif name in ("CALL", "CALLM"):
            self.call(pc, stmts)
        elif name in ("CALLT", "CALLMT"):
            call = self.call_expr(pc, a, d - 1, name == "CALLMT", stmts)
            self.flush(stmts)
            stmts.append(Return([call]))
        elif name in ("RET0", "RET1", "RET", "RETM"):
            vals = []
            if name == "RET1":
                pe = self.pend.get(a)
                if pe is not None and isinstance(pe.expr, Call) and not isinstance(pe.expr, Paren):
                    raise ForceLocal(pe.pc)
                vals = [self.take(a, pc)]
            elif name == "RET":
                vals = [None] * (d - 1)
                for s in range(a + d - 2, a - 1, -1):
                    vals[s - a] = self.take(s, pc)
            elif name == "RETM":
                vals = [None] * (d + 1)
                vals[d] = self.take_multi(a + d, pc)
                for s in range(a + d - 1, a - 1, -1):
                    vals[s - a] = self.take(s, pc)
            self.flush(stmts)
            stmts.append(Return(vals))
        else:
            raise DecompileError("unhandled opcode %s at pc %d" % (name, pc))

    def gname(self, d):
        name = self.kstr(d)
        if not is_ident(name):
            raise DecompileError("non-identifier global name")
        return name.decode("ascii")

    def take_multi(self, slot, pc):
        pe = self.pend.get(slot)
        if pe is None or not getattr(pe.expr, "multi", False):
            raise DecompileError("expected multi-result at slot %d pc %d" % (slot, pc))
        return self.take(slot, pc)

    def call_expr(self, pc, a, nargs, multres, stmts):
        args = [None] * nargs
        tail = None
        if multres:
            tail = self.take_multi(a + 1 + nargs, pc)
        for s in range(a + nargs, a, -1):
            args[s - a - 1] = self.take(s, pc)
        func = self.take(a, pc)
        if isinstance(func, MethodRef):
            if not args or not isinstance(args[0], SelfArg):
                raise DecompileError("broken method call")
            args = args[1:]
        if tail is not None:
            args.append(tail)
        for x in args:
            if isinstance(x, SelfArg):
                raise DecompileError("stray self")
        return Call(func, args)

    def call(self, pc, stmts):
        op, a, c, b, d = self.ins(pc)
        name = OPS[op]
        nargs = c - 1 if name == "CALL" else c
        call = self.call_expr(pc, a, nargs, name == "CALLM", stmts)
        if b == 1:
            self.flush(stmts)
            stmts.append(CallStmt(call))
        elif b == 0:
            call.multi = True
            self.define(a, call, pc, stmts)
        elif b == 2:
            self.define(a, call, pc, stmts)
        else:
            call.multi = True
            self.define(a, call, pc, stmts)
            self.pend[a].group = "call"
            for s in range(a + 1, a + b - 1):
                self.define(s, Const(None), pc, stmts)
                self.pend[s].group = ("part", a)

    def template(self, k):
        _, arr, hsh = k
        t = TableExpr()
        t.hint = None
        if arr and arr[0] is not None:
            t.hash.append((Const(0.0), self._kconst(arr[0])))
        for i, v in enumerate(arr[1:], 1):
            if v is not None:
                t.arr[i] = self._kconst(v)
        for kk, v in hsh:
            t.hash.append((self._kconst(kk), self._kconst(v)))
        return t

    def _kconst(self, v):
        if isinstance(v, int) and not isinstance(v, bool):
            v = float(v)
        return Const(v)

    def store(self, pc, stmts):
        op, a, c, b, d = self.ins(pc)
        name = OPS[op]
        # Table constructor fill
        if name in ("TSETS", "TSETB", "TSETV"):
            tp = self.pend.get(b)
            if tp is not None and isinstance(tp.expr, TableExpr) and (a > b or a < self.nact):
                if name == "TSETV":
                    if c == b:
                        raise DecompileError("odd TSETV")
                    val, key = self.take_many([a, c], pc)
                else:
                    val = self.take(a, pc)
                    key = Const(self.kstr(c)) if name == "TSETS" else Const(float(c))
                t = tp.expr
                if not self._fill_fits(t, name, c, key):
                    raise ForceLocal(tp.pc)
                idx = c if name == "TSETB" else None
                if name == "TSETV" and isinstance(key, Const) and isinstance(key.v, float) \
                        and key.v == int(key.v) and key.v > 255 and int(key.v) == (max(t.arr) if t.arr else 0) + 1:
                    idx = int(key.v)
                if idx is not None and idx >= 1 and idx not in t.arr and t.tail is None:
                    t.arr[idx] = val
                    t.events.append(("pos", idx))
                else:
                    t.events.append(("hash", key, val))
                return pc + 1
        targets, values = [], []
        while True:
            op, a, c, b, d = self.ins(pc)
            name = OPS[op]
            if name == "GSET":
                val = self.take(a, pc)
                tgt = Global(self.gname(d))
            elif name in ("USETV", "USETS", "USETN", "USETP"):
                if name == "USETV":
                    val = self.take(d, pc)
                elif name == "USETS":
                    val = Const(self.kstr(d))
                elif name == "USETN":
                    val = Const(self.p.kn[d])
                else:
                    val = Const((None, False, True)[d])
                tgt = Name(self.uvnames[a])
            else:
                if name == "TSETV":
                    val, obj, key = self.take_many([a, b, c], pc)
                else:
                    val, obj = self.take_many([a, b], pc)
                    key = Const(self.kstr(c)) if name == "TSETS" else Const(float(c))
                tgt = Index(obj, key)
            targets.append(tgt)
            values.append(val)
            # multi-assignment continues while further stores consume the
            # next pending value
            if not self.pend:
                break
            if pc + 1 >= self.n:
                break
            nop, na, nc, nb, nd = self.ins(pc + 1)
            nname = OPS[nop]
            topslot = max(self.pend)
            if nname in ("GSET", "TSETS", "TSETB", "TSETV") and na == topslot and not isinstance(
                    self.pend[topslot].expr, TableExpr):
                pc += 1
                continue
            if nname == "USETV" and nd == topslot:
                pc += 1
                continue
            break
        self.flush(stmts)
        stmts.append(Assign(targets[::-1], values[::-1]))
        return pc + 1

    @staticmethod
    def _fill_fits(t, name, c, key):
        """Check a constructor fill against the TNEW size hint."""
        hint = getattr(t, "hint", None)
        if hint is None:
            return True
        narr, hbits = hint & 0x7ff, hint >> 11
        positional = name == "TSETB" or (isinstance(key, Const) and isinstance(key.v, float)
                                         and key.v == int(key.v) and 0 <= key.v and name == "TSETV"
                                         and key.v > 255)
        if positional:
            return narr > 0
        nh = sum(1 for ev in t.events if ev[0] == "hash") + 1
        need = 0 if nh == 0 else 1 if nh == 1 else 1 + (nh - 1).bit_length()
        return need <= hbits

    def closure(self, d, slot):
        child = self.p.kgc[d]
        names = []
        selfref = False
        for u in child.uv:
            if u & 0x8000:
                s = u & 0xff
                if s == slot and s not in self.pend and s >= self.nact:
                    selfref = True
                    names.append(None)
                else:
                    names.append(self.var(s).name if s < self.nact else self._pending_local_name(s))
            else:
                names.append(self.uvnames[u])
        fd = FuncDecompiler(child, [n or "__self__" for n in names], self.depth + 1)
        fe = fd.decompile()
        if selfref:
            fe.selfref = True
            idxs = [i for i, n in enumerate(names) if n is None]
            fe.self_uv = idxs
            fe.uvnames = fd.uvnames

            def fix_self(var, fe=fe, fd=fd, child=child, names=names):
                nm = [var.name if n is None else n for n in names]
                fd2 = FuncDecompiler(child, nm, self.depth + 1)
                fe2 = fd2.decompile()
                fe.body, fe.params = fe2.body, fe2.params
            fe.fix_self = fix_self
        return fe

    def _pending_local_name(self, s):
        pe = self.pend.get(s)
        if pe is not None:
            raise ForceLocal(pe.pc)
        raise DecompileError("upvalue refers to unknown slot %d" % s)

    # -- control flow
    def do_cond(self, pc, end, stmts, loop):
        self.cstarts = []
        snap = (dict(self.pend), self.nact, dict(self.slotvars))
        cond = self.test_cond(pc)
        start = min([pc] + self.cstarts)
        if OPS[self.ins(pc + 1)[0]] != "JMP":
            raise DecompileError("test without jump at pc %d" % pc)
        # comparison used as a value:  test; JMP Lt; KPRI r false; JMP L; Lt: KPRI r true; L:
        T = pc + 2
        if T + 2 < self.n and self.jt(pc + 1) == T + 2:
            i0, i1, i2 = self.ins(T), self.ins(T + 1), self.ins(T + 2)
            if (OPS[i0[0]] == "KPRI" and i0[4] == 1 and OPS[i1[0]] == "JMP" and self.jt(T + 1) == T + 3
                    and OPS[i2[0]] == "KPRI" and i2[4] == 2 and i2[1] == i0[1]):
                self.cstarts = [start]
                self.define(i0[1], cond_expr(cond), T + 2, stmts)
                return T + 3
        self.flush(stmts)
        first = [cond, self.jt(pc + 1), start, pc + 2, snap]
        items, after = self.parse_chain(pc + 2, end, stmts, [first])
        # items: list of [cond, target, startpc]; 'after' = pc after last JMP
        # Choose the longest valid prefix
        for k in range(len(items), 0, -1):
            res = self.reduce_chain(items[:k])
            if res is not None:
                cond, tgt = res
                T = items[k - 1][3]
                self._rewind_chain(items, k)
                return self.build_if_or_loop(start, cond, tgt, T, end, stmts, loop)
        raise DecompileError("irreducible condition at pc %d" % pc)

    def _rewind_chain(self, items, k):
        if k < len(items):
            pend, nact, sv = items[k][4]
            self.pend, self.nact, self.slotvars = dict(pend), nact, dict(sv)

    def parse_chain(self, pc, end, stmts, items=None):
        items = items or []
        while pc < end:
            st = pc
            # operand computation: simple instructions until a test
            saved = (dict(self.pend), self.nact)
            snap = (dict(self.pend), self.nact, dict(self.slotvars))
            q = pc
            ok = True
            while q < end and self.ins(q)[0] not in TEST_OPS:
                nm = OPS[self.ins(q)[0]]
                if nm in ("JMP", "FORI", "LOOP", "UCLO", "ISNEXT", "CALLT", "CALLMT", "RET0", "RET1", "RET", "RETM",
                          "GSET", "TSETS", "TSETB", "TSETV", "USETV", "USETS", "USETN", "USETP", "TSETM"):
                    ok = False
                    break
                if nm in ("CALL", "CALLM") and self.ins(q)[3] == 1:
                    ok = False
                    break
                q += 1
            if not ok or q >= end or (items and q + 1 >= end):
                break
            if OPS[self.ins(q + 1)[0]] != "JMP":
                raise DecompileError("test without jump at pc %d" % q)
            if items:
                # tentative: operand code must be pure expression code; any jump
                # target into the middle of it ends the chain
                pass
            sub = []
            try:
                for x in range(pc, q):
                    self.simple(x, sub)
                self.cstarts = []
                cond = self.test_cond(q)
            except ForceLocal:
                raise
            except DecompileError:
                if not items:
                    raise
                self.pend, self.nact = saved
                break
            if sub or self.pend:
                if not items:
                    raise DecompileError("statements inside condition at pc %d" % pc)
                self.pend, self.nact = saved
                break
            items.append([cond, self.jt(q + 1), st, q + 2, snap])
            pc = q + 2
        if not items:
            raise DecompileError("empty condition at pc %d" % pc)
        return items, pc

    def test_cond(self, q):
        cond = self._test_cond(q)
        # The JMP after a test records the free register: anything below it
        # that we consumed as a temporary was really a local variable.
        if q + 1 < self.n and OPS[self.ins(q + 1)[0]] == "JMP":
            ja = self.ins(q + 1)[1]
            t = self.top()
            name = OPS[self.ins(q)[0]]
            allow = t + 1 if name in ("IST", "ISF") and self.ins(q)[4] == t else t
            if ja > allow and t in self.consumed and t not in self.pend:
                raise ForceLocal(self.consumed[t])
        return cond

    def _test_cond(self, q):
        op, a, c, b, d = self.ins(q)
        name = OPS[op]
        p = self.p
        if name in ("IST", "ISF"):
            e = self.take(d, q)
            return Truth(e, name == "ISF")
        if name in ("ISTC", "ISFC"):
            raise DecompileError("value-context and/or not supported")
        if name in ("ISLT", "ISGE", "ISLE", "ISGT", "ISEQV", "ISNEV"):
            da = self.pend.get(a)
            dd = self.pend.get(d)
            x, y = self.take_many([a, d], q)
            swap = False
            if da is not None and dd is not None and a > d:
                swap = True
            elif da is not None and dd is None and name in ("ISLT", "ISGE", "ISLE", "ISGT"):
                pass
            return Cmp(op, x, y, swap)
        if name in ("ISEQS", "ISNES"):
            k = Const(self.kstr(d))
        elif name in ("ISEQN", "ISNEN"):
            k = Const(p.kn[d])
        else:
            k = Const((None, False, True)[d])
        x = self.take(a, q)
        return Cmp(op, x, k)

    def reduce_chain(self, items):
        it = [list(x[:4]) for x in items]   # cond, target, start, next
        while len(it) > 1:
            done = False
            for i in range(len(it) - 2, -1, -1):
                c1, t1, s1, n1 = it[i]
                c2, t2, s2, n2 = it[i + 1]
                label_used = any(j != i and j != i + 1 and it[j][1] == s2 for j in range(len(it)))
                if label_used:
                    continue
                if t1 == t2:
                    it[i:i + 2] = [[Logic("or", c1, c2), t2, s1, n2]]
                    done = True
                    break
                if t1 == n2:
                    it[i:i + 2] = [[Logic("and", negate(c1), c2), t2, s1, n2]]
                    done = True
                    break
            if not done:
                return None
        c, t, s, n = it[0]
        return c, t

    def build_if_or_loop(self, start, jcond, F, T, end, stmts, loop):
        # jcond: "jump to F if jcond", fallthrough T
        ends = self.cur_ends

        def norm(x):
            return end if x in ends else x
        # while loop
        if F > T and T < end and OPS[self.ins(T)[0]] == "LOOP" and self.jt(T) == F \
                and OPS[self.ins(F - 1)[0]] == "JMP" and self.jt(F - 1) == start and F <= end:
            body = self.subblock(T + 1, F - 1, (F,), {F - 1, start})
            stmts.append(While(cond_expr(negate(jcond)), body))
            return F
        Fn = norm(F)
        if loop is not None and F == loop[0] and Fn != end and end - 1 > T \
                and OPS[self.ins(end - 1)[0]] == "JMP" and self.jt(end - 1) == F:
            # the jump was merged into a trailing 'break' of this block
            then = self.subblock(T, end - 1, loop, {end - 1, F})
            stmts.append(If(cond_expr(negate(jcond)), then, []))
            return end - 1
        if loop is not None and F == loop[0] and Fn != end:
            # 'if c then ... else break end': the false jump was merged into
            # the break that forms the else block
            for P in range(end - 1, T, -1):
                if OPS[self.ins(P)[0]] == "JMP" and self.jt(P) == F and OPS[self.ins(P - 1)[0]] == "JMP" \
                        and norm(self.jt(P - 1)) in (P + 1, end) and P - 1 >= T:
                    E = P + 1
                    then = self.subblock(T, P - 1, loop, {P - 1, self.jt(P - 1)} | (ends if E == end else frozenset()))
                    stmts.append(If(cond_expr(negate(jcond)), then, [Break()]))
                    return E
        if loop is not None and F == loop[0] and Fn != end:
            # condition jumps straight out of the loop
            stmts.append(If(cond_expr(jcond), [Break()], []))
            return T
        if Fn < T or Fn > end:
            raise DecompileError("condition jumps outside block at pc %d" % start)
        outer = ends if Fn == end else frozenset()
        if Fn - 1 >= T and OPS[self.ins(Fn - 1)[0]] == "JMP" and Fn != end or \
                (Fn == end and Fn - 1 >= T and OPS[self.ins(Fn - 1)[0]] == "JMP" and F == end):
            E = self.jt(Fn - 1)
            En = norm(E)
            is_break = loop is not None and E == loop[0] and En != end
            if not is_break and Fn < En <= end:
                then = self.subblock(T, Fn - 1, loop, {Fn - 1, E} | (ends if En == end else frozenset()))
                orelse = self.subblock(Fn, En, loop, ends if En == end else frozenset())
                stmts.append(If(cond_expr(negate(jcond)), then, orelse))
                return En
        then = self.subblock(T, Fn, loop, outer)
        stmts.append(If(cond_expr(negate(jcond)), then, []))
        return Fn

    def do_loop(self, pc, end, stmts, loop):
        self.flush(stmts)
        exit_ = self.jt(pc)
        if OPS[self.ins(exit_ - 1)[0]] == "JMP" and self.jt(exit_ - 1) == pc \
                and self.ins(exit_ - 2)[0] not in TEST_OPS:
            body = self.subblock(pc + 1, exit_ - 1, (exit_,), {exit_ - 1, pc})
            stmts.append(While(Const(True), body))
            return exit_
        # repeat ... until cond: find the trailing condition chain jumping back to pc
        cstart = self.find_until(pc, exit_)
        saved_nact, saved_vars = self.nact, dict(self.slotvars)
        body = self.block(pc + 1, cstart, (exit_,), {cstart})
        items, after = self.parse_chain(cstart, exit_, body)
        if after != exit_:
            raise DecompileError("bad repeat condition at pc %d" % pc)
        res = self.reduce_chain(items)
        if res is None or res[1] != pc:
            raise DecompileError("bad repeat jump at pc %d" % pc)
        cond = cond_expr(negate(res[0]))
        self.nact = saved_nact
        for s in list(self.slotvars):
            if s in saved_vars:
                self.slotvars[s] = saved_vars[s]
        stmts.append(Repeat(body, cond))
        return exit_

    def find_until(self, pc, exit_):
        # the last JMP before exit jumps back to pc; walk back over the chain
        q = exit_ - 1
        if OPS[self.ins(q)[0]] != "JMP" or self.jt(q) != pc:
            raise DecompileError("repeat without back jump at pc %d" % pc)
        # find earliest test whose chain ends at exit: scan backwards for a run
        # of test+jmp groups with pure expression code in between
        best = q - 1
        k = q - 1
        while k > pc:
            if self.ins(k)[0] in TEST_OPS and OPS[self.ins(k + 1)[0]] == "JMP":
                best = k
                # move to the start of operand code
                j = k - 1
                ja = self.ins(k + 1)[1]
                while j > pc and OPS[self.ins(j)[0]] not in ("JMP",) and self.ins(j)[0] not in TEST_OPS \
                        and not self._is_stmt(j) and self.ins(j)[1] >= ja:
                    j -= 1
                if j > pc and OPS[self.ins(j)[0]] == "JMP" and self.ins(j - 1)[0] in TEST_OPS \
                        and pc < self.jt(j) <= exit_ and (self.jt(j) == pc or self.jt(j) >= j + 1):
                    k = j - 1
                    best = j + 1
                    continue
                return j + 1
            break
        return best

    def _is_stmt(self, j):
        nm = OPS[self.ins(j)[0]]
        if nm in ("GSET", "TSETS", "TSETB", "TSETV", "USETV", "USETS", "USETN", "USETP", "RET0", "RET1",
                  "RET", "RETM", "CALLT", "CALLMT", "FORL", "ITERL", "LOOP", "FORI", "UCLO"):
            return True
        if nm in ("CALL", "CALLM") and self.ins(j)[3] == 1:
            return True
        return False

    def do_numfor(self, pc, end, stmts, loop):
        op, a, c, b, d = self.ins(pc)
        exit_ = self.jt(pc)
        if OPS[self.ins(exit_ - 1)[0]] != "FORL" or self.jt(exit_ - 1) != pc + 1:
            raise DecompileError("bad FORI at pc %d" % pc)
        step = self.take(a + 2, pc)
        stop = self.take(a + 1, pc)
        start = self.take(a, pc)
        self.flush(stmts)
        if a != self.nact:
            raise DecompileError("for base mismatch at pc %d" % pc)
        saved = self.nact
        self.nact = a + 3
        var = self.newvar(a + 3, "i")
        self.nact = a + 4
        body = self.subblock(pc + 1, exit_ - 1, (exit_,), {exit_ - 1})
        self.nact = saved
        if isinstance(step, Const) and step.v == 1.0 and self._step_default(pc, a):
            step = None
        stmts.append(NumFor(var, start, stop, step, body))
        return exit_

    def _step_default(self, pc, a):
        # default step is emitted as KSHORT base+2, 1 right before FORI
        op, aa, c, b, d = self.ins(pc - 1)
        return OPS[op] == "KSHORT" and aa == a + 2 and d == 1

    def do_genfor(self, pc, end, stmts, loop):
        op, a, c, b, d = self.ins(pc)
        iterc = self.jt(pc)
        exit_ = iterc + 2
        iop, ia, ic, ib, idd = self.ins(iterc)
        nvars = ib - 1
        e3 = self.take(a - 1, pc)
        e2 = self.take(a - 2, pc)
        e1 = self.take(a - 3, pc)
        exprs = [e1, e2, e3]
        # collapse an adjusted multi-result call: pairs(t) -> call with 3 results
        if isinstance(e1, Call) and e1.multi and isinstance(e2, Const) and e2.v is None \
                and isinstance(e3, Const) and e3.v is None:
            exprs = [e1]
        elif isinstance(e2, Call) and e2.multi and isinstance(e3, Const) and e3.v is None:
            exprs = [e1, e2]
        else:
            # trailing nils from adjustment
            while len(exprs) > 1 and isinstance(exprs[-1], Const) and exprs[-1].v is None \
                    and getattr(exprs[-1], "_adjust", True):
                exprs.pop()
        self.flush(stmts)
        if a - 3 != self.nact:
            raise DecompileError("for-in base mismatch at pc %d" % pc)
        saved = self.nact
        self.nact = a
        vars_ = [self.newvar(a + i, "k" if i == 0 else "v") for i in range(nvars)]
        self.nact = a + nvars
        body = self.subblock(pc + 1, iterc, (exit_,), {iterc})
        self.nact = saved
        stmts.append(GenFor(vars_, exprs, body))
        return exit_


def decompile(data):
    proto = parse_bytecode(data)
    fe = FuncDecompiler(proto, []).decompile()
    _CUR_INDENT[0] = ""
    return render_block(fe.body, "")


if __name__ == "__main__":
    main()
