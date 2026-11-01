#!/usr/bin/env python3
"""Generates the ClientScript folder the servers read (SimLayer:AddPath) from the client's data files.

The servers load client scripts (Enum.lua, DungeonEnum.lua, item tables...) as Lua source. They are taken from
the client's .kom archives: every .lua entry is decrypted and its KOG LuaJIT bytecode decompiled (tools/kom/kom.py),
and Item.kim becomes Item.lua + SetItem.lua (tools/kom/kim.py). The output is flat: the servers search the
folder and its subfolders by name (KAutoPath), and no two client scripts share a name.

Usage: tools/runtime/gen_clientscript.py <client data dir> <out dir> [--luajit PATH]

Nothing is rewritten when the inputs did not change (a stamp file in <out dir> records them).
See docs/tools/runtime.md.
"""

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "kom"))
import kom  # noqa: E402

STAMP = ".x2-clientscript.json"


def inputs_digest(kom_files):
    h = hashlib.sha256()
    for tool in (HERE / "gen_clientscript.py", HERE.parent / "kom" / "kom.py", HERE.parent / "kom" / "kim.py"):
        h.update(tool.read_bytes())
    for p in kom_files:
        st = p.stat()
        h.update(f"{p.name}\0{st.st_size}\0{st.st_mtime_ns}\0".encode())
    return h.hexdigest()


def generate(data_dir, out, luajit):
    kom_files = sorted(p for p in data_dir.iterdir() if p.suffix.lower() == ".kom")
    if not kom_files:
        raise SystemExit(f"error: no .kom files in {data_dir}")
    digest = inputs_digest(kom_files)
    stamp = out / STAMP
    if stamp.is_file() and json.loads(stamp.read_text()).get("inputs") == digest:
        print(f"ClientScript up to date ({out})")
        return

    tmp = Path(tempfile.mkdtemp(prefix=".clientscript-", dir=out.parent))
    try:
        sources = {}
        kim = None
        for path in kom_files:
            for e in kom.read_kom(str(path)):
                low = e.name.lower()
                if not (low.endswith(".lua") or low == "item.kim"):
                    continue
                if os.path.basename(e.name) != e.name or e.name in ("", ".", ".."):
                    raise SystemExit(f"error: {path.name}: unsafe entry name {e.name!r}")
                if e.name in sources:
                    raise SystemExit(f"error: {e.name} is in {sources[e.name]} and {path.name}")
                sources[e.name] = path.name
                data = kom.decode_entry(e)
                if data is None:
                    raise SystemExit(f"error: {path.name}: {e.name} could not be decoded")
                if low == "item.kim":
                    kim = data
                    continue
                if kom.is_bytecode(data):
                    data = kom.BOM + kom.decompile(kom.bytecode_to_81(data)).encode("utf-8", "surrogateescape")
                (tmp / e.name).write_bytes(data)
        if kim is None:
            raise SystemExit("error: Item.kim not found in the client data")

        # Item.kim -> Item.lua + SetItem.lua (needs the enums and a LuaJIT to read them)
        kim_path = tmp / ".Item.kim"
        kim_path.write_bytes(kim)
        subprocess.run([sys.executable, "-I", str(HERE.parent / "kom" / "kim.py"), "--luajit", luajit,
                        "decompile", str(kim_path), str(tmp), "--enum", str(tmp / "Enum.lua")], check=True, stdout=subprocess.DEVNULL)
        kim_path.unlink()

        count = sum(1 for p in tmp.iterdir() if p.suffix.lower() == ".lua")
        (tmp / STAMP).write_text(json.dumps({"inputs": digest, "files": count, "source": str(data_dir)}, indent=1))
        if out.exists():
            shutil.rmtree(out)
        tmp.rename(out)
        tmp = None
        print(f"ClientScript: {count} scripts from {len(kom_files)} .kom files -> {out}")
    finally:
        if tmp is not None:
            shutil.rmtree(tmp, ignore_errors=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("data", type=Path, help="client data folder (with the data*.kom files)")
    ap.add_argument("out", type=Path, help="output folder (replaced)")
    ap.add_argument("--luajit", default=shutil.which("luajit"), help="LuaJIT executable (default: luajit on PATH)")
    args = ap.parse_args()
    if not args.luajit:
        raise SystemExit("error: LuaJIT not found (install it or pass --luajit)")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    generate(args.data.resolve(), args.out.resolve(), args.luajit)


if __name__ == "__main__":
    main()
