#!/usr/bin/env python3
"""Tests for kom.py's Algorithm 3 keys (detection, unblob, pack) and keyrecover.py, on synthetic data only."""

import os
import random
import subprocess
import sys
import tempfile
import unittest
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import kom  # noqa: E402
import keyrecover  # noqa: E402

KOM = os.path.join(os.path.dirname(HERE), "kom.py")


def run(*args):
    return subprocess.run([sys.executable, "-I", KOM] + list(args), check=True, capture_output=True, text=True).stdout


class Keys(unittest.TestCase):
    def test_detects_each_key(self):
        data = b"-- text entry\n" * 40
        for key in kom.ALGO3_KEYS:
            e = kom.encode_entry("Some_Script.lua", data, 3, key=key)
            self.assertEqual(kom.decode_entry(e), data)
            self.assertEqual(e.key, key)
            self.assertEqual(kom.decode_entry(e, key), data)
            other = [k for k in kom.ALGO3_KEYS if k != key][0]
            with self.assertRaises(zlib.error):
                kom.decode_entry(e, other)

    def test_unblob_and_pack_keep_the_key(self):
        with tempfile.TemporaryDirectory() as tmp:
            blobs = os.path.join(tmp, "blobs")
            os.mkdir(blobs)
            files = {"A.txt": b"alpha " * 100, "B.lua": b"print(1)\n" * 30}
            for name, data in files.items():
                e = kom.encode_entry(name, data, 3, key="br-2014-08")
                with open(os.path.join(blobs, "%s.%d.3.___" % (name, e.size)), "wb") as fp:
                    fp.write(e.blob)
            with open(os.path.join(blobs, "Plain.xml"), "wb") as fp:
                fp.write(b"<x/>")
            out = os.path.join(tmp, "out")
            self.assertIn("br-2014-08 (2 entries)", run("unblob", blobs, out))
            for name, data in files.items():
                with open(os.path.join(out, name), "rb") as fp:
                    self.assertEqual(fp.read(), data)
            packed = os.path.join(tmp, "x.kom")
            run("pack", out, packed)
            entries = {e.name: e for e in kom.read_kom(packed)}
            self.assertEqual(kom.decode_entry(entries["A.txt"], "br-2014-08"), files["A.txt"])
            converted = os.path.join(tmp, "y.kom")
            run("pack", out, converted, "--format", "2014-08")
            entries = {e.name: e for e in kom.read_kom(converted)}
            self.assertEqual(kom.decode_entry(entries["A.txt"], "2014-08"), files["A.txt"])


class ContainerV4(unittest.TestCase):
    def test_round_trip_keeps_header_and_mapped_id(self):
        with tempfile.TemporaryDirectory() as tmp:
            e = kom.encode_entry("Quest.lua", b"q" * 500, 3, key="br-2014-08")
            e.attrs = {"MappedID": "74^2*h58"}
            plain = kom.encode_entry("Plain.xml", b"<x/>", 0)
            src = os.path.join(tmp, "v4.kom")
            kom.write_kom(src, [e, plain], 4)
            with open(src, "rb") as fp:
                raw = fp.read()
            self.assertTrue(raw.startswith(kom.MAGIC_V4))
            self.assertNotIn(b"<Files>", raw)               # the header is XORed
            info = {}
            back = kom.read_kom(src, info)
            self.assertEqual(info["version"], 4)
            self.assertEqual(back[0].attrs, {"MappedID": "74^2*h58"})
            out = os.path.join(tmp, "out")
            run("unpack", src, out)
            repacked = os.path.join(tmp, "again.kom")
            run("pack", out, repacked)
            info = {}
            again = kom.read_kom(repacked, info)
            self.assertEqual(info["version"], 4)
            self.assertEqual(again[0].attrs, {"MappedID": "74^2*h58"})
            self.assertEqual(kom.decode_entry(again[0], "br-2014-08"), b"q" * 500)
            v3 = os.path.join(tmp, "v3.kom")
            run("pack", out, v3, "--container", "3")
            with open(v3, "rb") as fp:
                self.assertTrue(fp.read().startswith(kom.MAGIC_V3))


class Recover(unittest.TestCase):
    def test_random_keys(self):
        rnd = random.Random(7)
        for algorithm in (2, 3):
            key20 = bytes(rnd.randrange(256) for _ in range(20))
            entries = keyrecover.synthetic(algorithm, key20, rnd)
            found, rank, used, dropped = keyrecover.recover(entries, algorithm, algorithm, verbose=False)
            self.assertEqual(rank, 144)
            self.assertEqual(dropped, [])
            ok, bad = keyrecover.verify(entries, algorithm, found)
            self.assertEqual((ok, bad), (len(entries), []))

    def test_known_keys_are_consistent(self):
        # the equations of a real key's archive never contradict each other
        rnd = random.Random(8)
        entries = keyrecover.synthetic(3, kom.ALGO3_KEYS["br-2014-08"], rnd, 20)
        found = keyrecover.recover(entries, 3, 3, verbose=False)
        self.assertEqual(keyrecover.verify(entries, 3, found[0])[1], [])
        self.assertEqual(keyrecover.verify(entries, 3, kom.ALGO3_KEYS["2014-08"])[0], 0)

    def test_symbolic_matches_cipher(self):
        rnd = random.Random(9)
        key20 = bytes(rnd.randrange(256) for _ in range(20))
        blob = bytes(rnd.randrange(256) for _ in range(40))
        for setup in (2, 3):
            name = "Ünïcode_Name.lua"
            real = kom.xorcrc_decrypt(blob, name, setup, key20)
            sym = keyrecover.sym_decrypt(blob, name, setup, len(blob))
            bits = 0
            for b in range(160):
                bits |= ((key20[b // 8] >> (b % 8)) & 1) << b

            def value(form):
                return (bin(form & bits).count("1") + (1 if form & keyrecover.ONE else 0)) & 1
            got = bytes(sum(value(sym[i][j]) << j for j in range(8)) for i in range(len(blob)))
            self.assertEqual(got, real)


if __name__ == "__main__":
    unittest.main()
