#!/usr/bin/env python3
"""List the animation sets of client motion files (.x).

Reads both formats the client loads (CKTDXDeviceXSkinMesh::Load):
- KSM (KOG skinned mesh, 'KSM ' magic): the 'KAC ' animation controller block at
  KSMHeader::m_dwOffset_AnimationController (KTDXAnimationController::ReadFromMemory);
- DirectX .x ('xof '): text, binary, and MSZIP-compressed binary ('bzip').

Usage:
  ksm_anims.py <file.x>...                  print each file's set names
  ksm_anims.py --diff <ours.x> <other.x>    sets only one of the two has
"""
import argparse
import re
import struct
import sys
import zlib


def _ksm_names(d):
    ac = struct.unpack_from('<I', d, 8)[0]
    if ac == 0:
        return []
    a = d[ac:]
    if a[:4] != b'KAC ':
        raise ValueError('no KAC block')
    out = []
    for i in range(struct.unpack_from('<I', a, 8)[0]):
        off = struct.unpack_from('<I', a, 12 + 4 * i)[0]
        name_off = struct.unpack_from('<I', a, off)[0]
        if name_off == 0:
            out.append('')
            continue
        n = struct.unpack_from('<I', a, name_off)[0]
        out.append(a[name_off + 4:name_off + 4 + n].decode('latin1'))
    return out


def _mszip(body):
    pos, out = 4, b''   # skip the uncompressed size
    while pos < len(body):
        _, csz = struct.unpack_from('<HH', body, pos)
        blk = body[pos + 4:pos + 4 + csz]
        pos += 4 + csz
        if blk[:2] != b'CK':
            raise ValueError('bad MSZIP block')
        # each block may refer back to the previous 32 KB
        dc = zlib.decompressobj(-15, zdict=out[-32768:]) if out else zlib.decompressobj(-15)
        out += dc.decompress(blk[2:])
    return out


def _xof_names(d):
    kind, body = d[8:12], d[16:]
    if kind == b'txt ':
        return [m.decode('latin1') for m in re.findall(rb'AnimationSet\s+(\S+)\s*\{', body)]
    if kind == b'bzip':
        body = _mszip(body)
    elif kind != b'bin ':
        raise ValueError(f'unknown .x kind {kind!r}')
    # binary tokens: 1 NAME, 2 STRING, 3 INTEGER, 5 GUID, 6 INT_LIST, 7 FLOAT_LIST (32-bit),
    # 10 OBRACE, 31 TEMPLATE; a data object is NAME <type> [NAME <name>] OBRACE
    i, res, names, template = 0, [], [], False
    while i + 2 <= len(body):
        t = struct.unpack_from('<H', body, i)[0]
        i += 2
        if t == 1:
            n = struct.unpack_from('<I', body, i)[0]
            names.append(body[i + 4:i + 4 + n])
            i += 4 + n
            continue
        if t == 10:
            if not template and len(names) >= 2 and names[-2] == b'AnimationSet':
                res.append(names[-1].decode('latin1'))
            template = False
        elif t == 31:
            template = True
        elif t == 2:
            i += 4 + struct.unpack_from('<I', body, i)[0] + 2
        elif t == 3:
            i += 4
        elif t == 5:
            i += 16
        elif t in (6, 7):
            i += 4 + 4 * struct.unpack_from('<I', body, i)[0]
        names = []
    return res


def anim_names(path):
    with open(path, 'rb') as f:
        d = f.read()
    if d[:4] == b'KSM ':
        return _ksm_names(d)
    if d[:4] == b'xof ':
        return _xof_names(d)
    raise ValueError(f'{path}: not a motion file')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--diff', action='store_true')
    ap.add_argument('files', nargs='+')
    a = ap.parse_args()
    if a.diff:
        if len(a.files) != 2:
            ap.error('--diff takes two files')
        x, y = (set(anim_names(f)) for f in a.files)
        print(f'only in {a.files[0]}:', ' '.join(sorted(x - y)))
        print(f'only in {a.files[1]}:', ' '.join(sorted(y - x)))
        return 0
    for f in a.files:
        n = anim_names(f)
        print(f'{f}: {len(n)}')
        for s in n:
            print(f'  {s}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
