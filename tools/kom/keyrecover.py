#!/usr/bin/env python3
"""keyrecover.py - recover the XORCRC key of a .kom from its own entries, stdlib only.

When a client's .kom does not decode with any key kom.py knows (an entry fails with "does not decode with any known
Algorithm 3 key"), this tool finds a key for it from the encrypted entries alone, without the client's executable.

Why it works
------------
XORCRC (kom.py) is linear over GF(2): the CRC-32 table T satisfies T[a ^ b] = T[a] ^ T[b], and every step of the
cipher (comp = crc ^ key, s = c ^ 0xff ^ comp, plain = T[s] ^ comp, crc = (crc >> 8) ^ (T[s] | s), and the setup over
the name/size prefix) is an XOR of such terms.  For a known ciphertext every plaintext bit is therefore an affine
function of the 160 key bits, and each plaintext bit we can predict is one linear equation.  Gaussian elimination
over enough equations gives the key.

Plaintext we can predict (2026-11-15):
  * the zlib header of every entry: 0x78 (32 KB window) and, when only one of 9c/da/01/5e fits, its second byte, at
    byte 0 (Algorithm 2) or 4 (Algorithm 3, after the 4-byte prefix);
  * empty Lua scripts (18 bytes of bytecode): their exact zlib stream, for both bytecode layouts and every level;
  * --reference: entries of the same name and size in an archive we can already decode (another client or
    region), recompressed at every zlib level;
  * --known NAME:OFFSET:HEX for anything else.
For a given ciphertext byte only half of the plaintext values are possible under the cipher, whatever the key, so
most wrong guesses are rejected before they reach the solver: of an entry's candidates (zlib levels, bytecode
layouts, header bytes) only those that fit the ciphertext are kept, and only the bytes they all agree on are used.
An entry whose equations contradict the others is dropped.

The 160 key bits are not all determined: on our client's data036.kom (2026-11-15) the equations reach rank 144, and
every key of the remaining 16-dimensional class decodes every entry identically.  The tool sets the free bits to
zero, so the key it prints may differ from the client's while being equivalent to it.  It checks the key by
decoding every encrypted entry and prints the line to add to kom.ALGO3_KEYS.

If the solver reports contradictions for every entry, the cipher itself changed (another setup or CRC update,
not just another key): try --setup with the other variant, or extend sym_decrypt.

Usage
-----
  keyrecover.py recover data036.kom                  # or a folder of komextract "name.SIZE.ALG.___" blobs
  keyrecover.py recover data036_xx.kom --reference ../our/data036.kom --known Config.lua:4:789c
  keyrecover.py selftest                             # recovers random keys from synthetic archives
"""

import argparse
import os
import random
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kom  # noqa: E402

T = kom.CRC_T
NKEY = 160
ONE = 1 << NKEY                       # the constant term of an affine form
EMPTY_CHUNK_81 = bytes.fromhex("1b4b4c81020b020001000000014700010000")   # stripped empty script, KL 0x81


# ------------------------------------------------------------------ symbolic XORCRC

def _const(v):
    return [ONE if (v >> j) & 1 else 0 for j in range(8)]


def _xor(a, b):
    return [x ^ y for x, y in zip(a, b)]


_TBITS = [[k for k in range(32) if (T[1 << j] >> k) & 1] for j in range(8)]


def _table(byte):
    """T applied to a symbolic byte: 32 affine forms."""
    out = [0] * 32
    for j in range(8):
        if byte[j]:
            for k in _TBITS[j]:
                out[k] ^= byte[j]
    return out


def _key(i):
    return [1 << (8 * (i % 20) + j) for j in range(8)]


def _step(crc, c, keybyte):
    comp = _xor(crc[:8], keybyte)
    s = _xor(_const(c ^ 0xff), comp)
    t = _table(s)
    return _xor(t[:8], comp), _xor(crc[8:] + [0] * 8, s + t[8:])


def sym_decrypt(blob, name, setup, nbytes):
    """Affine forms (8 per byte) of the first nbytes plaintext bytes of an entry, as functions of the key bits."""
    pre = kom.xorcrc_prefix(name, len(blob))
    crc = [ONE] * 32
    for i, b in enumerate(pre):
        if setup == 3:
            _, crc = _step(crc, b, _key(i))
        else:
            crc = _xor(crc[8:] + [0] * 8, _table(_xor(_xor(crc[:8], _const(b)), _key(i))))
    r = len(pre)
    out = []
    for i in range(min(nbytes, len(blob))):
        p, crc = _step(crc, blob[i], _key(r + i))
        out.append(p)
    return out


# ------------------------------------------------------------------ GF(2) elimination

class Solver(object):
    def __init__(self):
        self.rows = {}

    def copy(self):
        s = Solver()
        s.rows = dict(self.rows)
        return s

    def add(self, row):
        """Adds one equation (affine form == 0); False when it contradicts the previous ones."""
        while True:
            v = row & (ONE - 1)
            if not v:
                return not row
            p = v.bit_length() - 1
            if p not in self.rows:
                self.rows[p] = row
                return True
            row ^= self.rows[p]

    def key(self):
        x = {}
        for p in sorted(self.rows):
            r = self.rows[p]
            val = 1 if r & ONE else 0
            m = r & (ONE - 1) & ~(1 << p)
            while m:
                q = m.bit_length() - 1
                val ^= x.get(q, 0)
                m &= ~(1 << q)
            x[p] = val
        return bytes(sum(x.get(8 * b + j, 0) << j for j in range(8)) for b in range(20))


# ------------------------------------------------------------------ plaintext guesses

def feasible(c, p):
    """Whether plaintext byte p can come from ciphertext byte c under XORCRC, for some key/state."""
    return any((T[c ^ 0xff ^ x] & 0xff) ^ x == p for x in range(256))


_FEAS = {}


def fits(blob, offset, plain):
    if offset + len(plain) > len(blob):
        return False
    for i, p in enumerate(plain):
        k = (blob[offset + i], p)
        if k not in _FEAS:
            _FEAS[k] = feasible(*k)
        if not _FEAS[k]:
            return False
    return True


def zstreams(data, algorithm):
    """Every distinct zlib stream of data (levels 0-9), as stored by that algorithm (Algorithm 3: no adler32)."""
    out = []
    for level in range(10):
        z = zlib.compress(data, level)
        if algorithm == 3:
            z = z[:-4]
        if z not in out:
            out.append(z)
    return out


def agreed(blob, offset, candidates):
    """Runs (offset, bytes) where every candidate that fits the ciphertext has the same byte: if the true
    plaintext is among the candidates, those bytes are right."""
    fit = [c for c in candidates if fits(blob, offset, c)]
    if not fit:
        return []
    n = min(len(c) for c in fit)
    runs, start = [], None
    for i in range(n + 1):
        same = i < n and all(c[i] == fit[0][i] for c in fit)
        if same and start is None:
            start = i
        elif not same and start is not None:
            runs.append((offset + start, fit[0][start:i]))
            start = None
    return runs


def guesses(e, algorithm, reference, known, limit):
    """(offset, plaintext) guesses for one encrypted entry, the strongest first."""
    off = 4 if algorithm == 3 else 0
    out = []
    for name, o, plain in known:
        if name.lower() == e.name.lower():
            out.append((o, plain))
    data = None
    ref = reference.get(e.name.lower())
    if ref is not None and len(ref) == e.size:
        data = [ref]
    elif e.size == len(EMPTY_CHUNK_81) and e.name.lower().endswith(".lua"):
        data = [EMPTY_CHUNK_81, kom.bytecode_to(EMPTY_CHUNK_81, 0x82)]
    if data:
        cands = [z[:limit] for d in data for z in zstreams(d, algorithm)]
        out += agreed(e.blob, off, cands)
    if not out:
        out += agreed(e.blob, off, [b"\x78\x9c", b"\x78\xda", b"\x78\x01", b"\x78\x5e"])
    return out


# ------------------------------------------------------------------ recovery

def load_entries(path):
    return kom.read_blob_dir(path) if os.path.isdir(path) else kom.read_kom(path)


def load_reference(paths):
    ref = {}
    for path in paths:
        for e in load_entries(path):
            try:
                data = kom.decode_entry(e)
            except zlib.error:
                continue
            if data is not None:
                ref.setdefault(e.name.lower(), data)
    return ref


def recover(entries, algorithm, setup, reference=None, known=(), limit=64, verbose=True):
    """Returns (key20, solver rank, entries used, entries dropped) or None when nothing fits."""
    reference = reference or {}
    work = []
    for e in entries:
        if e.algorithm != algorithm:
            continue
        g = guesses(e, algorithm, reference, known, limit)
        work.append((sum(len(p) for _, p in g), e, g))
    work.sort(key=lambda w: -w[0])
    solver, used, dropped = Solver(), 0, []
    for _, e, g in work:
        trial = solver.copy()
        ok = True
        n = max(o + len(p) for o, p in g)
        sym = sym_decrypt(e.blob, e.name, setup, n)
        for o, plain in g:
            for i, p in enumerate(plain):
                for j in range(8):
                    if not trial.add(sym[o + i][j] ^ (ONE if (p >> j) & 1 else 0)):
                        ok = False
                        break
                if not ok:
                    break
            if not ok:
                break
        if ok:
            solver, used = trial, used + 1
        else:
            dropped.append(e.name)
    if not used:
        return None
    if verbose:
        print("%d entries used, %d dropped as contradictory, rank %d of %d"
              % (used, len(dropped), len(solver.rows), NKEY))
    return solver.key(), len(solver.rows), used, dropped


def verify(entries, algorithm, key20):
    ok, bad = 0, []
    for e in entries:
        if e.algorithm != algorithm:
            continue
        try:
            d = kom.xorcrc_decrypt(e.blob, e.name, algorithm, key20)
            if algorithm == 3:
                inf = zlib.decompressobj(-zlib.MAX_WBITS)
                d = inf.decompress(d[6:]) + inf.flush()
            else:
                d = zlib.decompress(d)
            if len(d) != e.size:
                raise zlib.error("size")
            ok += 1
        except zlib.error:
            bad.append(e.name)
    return ok, bad


def cmd_recover(args):
    entries = load_entries(args.input)
    algs = sorted({e.algorithm for e in entries if e.algorithm in (2, 3)})
    if not algs:
        raise SystemExit("error: %s has no XORCRC entries (Algorithm 2 or 3)" % args.input)
    algorithm = args.algorithm or algs[-1]
    known = []
    for k in args.known:
        name, off, hx = k.rsplit(":", 2)
        known.append((name, int(off), bytes.fromhex(hx)))
    reference = load_reference(args.reference)
    setups = [args.setup] if args.setup else [algorithm, 5 - algorithm]
    for setup in setups:
        print("Algorithm %d, setup %d:" % (algorithm, setup))
        res = recover(entries, algorithm, setup, reference, known, args.limit)
        if res is None:
            print("  no entry fits")
            continue
        key20 = res[0]
        if setup != algorithm:
            print("  note: this setup is not what kom.py implements for Algorithm %d" % algorithm)
        ok, bad = verify(entries, algorithm, key20) if setup == algorithm else (0, ["(not verified)"])
        print("  key %s: %d entries decode, %d do not%s"
              % (key20.hex(), ok, len(bad), (" (" + ", ".join(bad[:5]) + ")") if bad else ""))
        if ok and not bad:
            print('\nAdd to kom.ALGO3_KEYS:\n    "<client>": bytes.fromhex("%s"),' % key20.hex()
                  if algorithm == 3 else "\nAlgorithm 2 key20: %s" % key20.hex())
            return
    raise SystemExit("error: no key decodes every entry")


# ------------------------------------------------------------------ self test

def synthetic(algorithm, key20, rnd, count=60):
    """A synthetic archive: entries with real-looking names, empty scripts and random text."""
    entries = []
    for i in range(count):
        name = "Synthetic_%s_%d.lua" % ("x" * rnd.randrange(12), i)
        if i % 4 == 0:
            data = kom.bytecode_to(EMPTY_CHUNK_81, 0x82 if algorithm == 3 else 0x81)
        else:
            data = bytes(rnd.randrange(32, 127) for _ in range(rnd.randrange(50, 3000)))
        z = zlib.compress(data, 6)
        if algorithm == 3:
            z = struct.pack("<I", rnd.getrandbits(32)) + z[:-4]
        blob = kom.xorcrc_encrypt(z, name, algorithm, key20)
        entries.append(kom.Entry(name, len(data), len(blob), 0, 0, algorithm, blob))
    return entries


def cmd_selftest(args):
    rnd = random.Random(args.seed)
    for algorithm in (2, 3):
        key20 = bytes(rnd.randrange(256) for _ in range(20))
        entries = synthetic(algorithm, key20, rnd)
        found = recover(entries, algorithm, algorithm, verbose=False)
        ok, bad = verify(entries, algorithm, found[0])
        print("Algorithm %d: rank %d, %d/%d entries decode with the recovered key%s"
              % (algorithm, found[1], ok, ok + len(bad), "" if found[0] != key20 else " (identical key)"))
        if bad:
            raise SystemExit("selftest failed")


def main():
    ap = argparse.ArgumentParser(description="recover the XORCRC key of a .kom from its encrypted entries")
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("recover", help="find a key for a .kom or a komextract blob folder")
    p.add_argument("input")
    p.add_argument("--algorithm", type=int, choices=(2, 3), help="entries to attack (default: 3 if present)")
    p.add_argument("--setup", type=int, choices=(2, 3), help="prefix setup variant (default: try both)")
    p.add_argument("--reference", action="append", default=[],
                   help="decodable .kom or blob folder with some of the same files (repeatable)")
    p.add_argument("--known", action="append", default=[], metavar="NAME:OFFSET:HEX",
                   help="known plaintext of an entry's stored stream (repeatable)")
    p.add_argument("--limit", type=int, default=64, help="known bytes used per entry (default 64)")
    p.set_defaults(func=cmd_recover)
    p = sub.add_parser("selftest", help="recover random keys from synthetic archives")
    p.add_argument("--seed", type=int, default=2026)
    p.set_defaults(func=cmd_selftest)
    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
