"""Helper of test_kompy.cpp: reads and writes .kom archives with tools/kom/kom.py.

  kompy_check.py <kom.py dir> read <archive> <out.txt>   one line per entry: name, algorithm, size, decoded hex
  kompy_check.py <kom.py dir> write <archive>            the archive of test_kompy.cpp's TestEntries()
"""
import sys

sys.path.insert(0, sys.argv[1])
import kom  # noqa: E402


def test_entries():
    # Keep in sync with TestEntries() in test_kompy.cpp: byte k of entry i is (i * 31 + k * 7 + k // 8) & 0xFF.
    specs = (("Alpha.lua", 2, 37), ("beta_TEXTURE.dds", 0, 1037), ("Gamma.lua", 3, 2037), ("empty.txt", 0, 0))
    return [(name, alg, bytes((i * 31 + k * 7 + k // 8) & 0xFF for k in range(size)))
            for i, (name, alg, size) in enumerate(specs)]


def main():
    cmd = sys.argv[2]
    if cmd == "read":
        with open(sys.argv[4], "w") as fp:
            for e in kom.read_kom(sys.argv[3]):
                data = kom.decode_entry(e)
                fp.write("%s\t%d\t%d\t%s\n" % (e.name, e.algorithm, e.size, data.hex()))
    elif cmd == "write":
        kom.write_kom(sys.argv[3], [kom.encode_entry(n, d, a) for n, a, d in test_entries()])
    else:
        raise SystemExit("unknown command " + cmd)


main()
