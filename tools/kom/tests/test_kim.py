#!/usr/bin/env python3
"""Tests for kim.py's templet layouts (2014: 188 bytes, 2016: 204, 2017: 228), on synthetic scripts.
Needs a LuaJIT: $KOM_LUAJIT, luajit-kog/luajit or the system's; skipped without one."""

import os
import shutil
import subprocess
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import kim  # noqa: E402

KIM = os.path.join(os.path.dirname(HERE), "kim.py")
BOM = b"\xef\xbb\xbf"
LUAJIT = os.environ.get("KOM_LUAJIT") or (
    os.path.join(os.path.dirname(HERE), "luajit-kog", "luajit")
    if os.path.isfile(os.path.join(os.path.dirname(HERE), "luajit-kog", "luajit")) else shutil.which("luajit"))

ITEMS = """g_pItemManager:AddItemTemplet
{
    m_ItemID = 100,
    m_Name = "Wooden Sword",
    m_ItemType = 1,
    m_ModelName = "Sword.X",
    m_Stat = { m_fBaseHP = 10, m_fAtkPhysic = 2.5 },
    SOCKET_OPTION = { 3, 4 },
    m_SetID = 7,
}
g_pItemManager:AddItemTemplet
{
    m_ItemID = 101,
    m_Name = "Cloth",
    m_ItemType = 2,
    m_UseType = 1,
    m_SetID = 7,
    m_FrameOffsetRoseX = 2,
    ROSE_MODEL_NAME = "Rose_Cloth.X",
%s}
"""
ITEMS_2017 = """    m_ModelName5 = "Cloth5.X",
    m_AttachFrameName7 = "Bip01",
    m_fUnknownA = 41.5,
    m_iUnknownB = 200000101,
    m_iUnknownC = 100,
    m_iUnknownE = 1,
    m_bCielItem = True,
    m_UnknownDate = "2017-09-14 10:00",
"""
ITEMS_2016 = """    m_iUnknownB = 200000101,
    m_iUnknownC = 100,
    m_bCielItem = True,
    m_UnknownDate = "2014-10-23 06:00:00",
"""
SETS = """g_pCX2SetItemManager:AddSetItemData_LUA
{
    m_SetID = 7,
    m_SetName = "Starter",
%s    m_NeedPartsNum = 1,
    m_Option1 = 12,
}
"""


def kimpy(*args):
    return subprocess.run([sys.executable, "-I", KIM, "--luajit", LUAJIT] + list(args), check=True,
                          capture_output=True, text=True).stdout


@unittest.skipUnless(LUAJIT, "no LuaJIT")
class Layouts(unittest.TestCase):
    def round_trip(self, tmp, layout, items, sets):
        src = os.path.join(tmp, layout)
        os.mkdir(src)
        scripts = []
        for name, text in (("Item.lua", items), ("SetItem.lua", sets)):
            scripts.append(os.path.join(src, name))
            with open(scripts[-1], "wb") as fp:
                fp.write(BOM + text.encode())
        first = os.path.join(tmp, layout + ".kim")
        kimpy("compile", *scripts, "-o", first, "--layout", layout)
        with open(first, "rb") as fp:
            data = fp.read()
        self.assertEqual(kim.detect_layout(data), layout)
        out = os.path.join(tmp, layout + "-lua")
        kimpy("decompile", first, out)
        again = os.path.join(tmp, layout + "-again.kim")
        decompiled = [os.path.join(out, "Item.lua"), os.path.join(out, "SetItem.lua")]
        kimpy("compile", *decompiled, "-o", again)      # the layout comes from the header note
        with open(again, "rb") as fp:
            self.assertEqual(fp.read(), data)
        with open(decompiled[0], encoding="utf-8-sig") as fp:
            return data, fp.read()

    def test_2014(self):
        with tempfile.TemporaryDirectory() as tmp:
            data, lua = self.round_trip(tmp, "2014", ITEMS % "", SETS % "")
            self.assertEqual(len(data) - 16 - 8 > 2 * 188, True)
            self.assertNotIn("layout 2017", lua)
            self.assertIn("m_FrameOffsetRoseX = 2,", lua)

    def test_2016(self):
        with tempfile.TemporaryDirectory() as tmp:
            data, lua = self.round_trip(tmp, "2016", ITEMS % ITEMS_2016, SETS % "")
            self.assertIn("layout 2016", lua)
            for line in ITEMS_2016.replace("True", "1").splitlines():
                self.assertIn(line.strip(), lua)
            self.assertNotIn("m_fUnknownA", lua)

    def test_2017(self):
        with tempfile.TemporaryDirectory() as tmp:
            data, lua = self.round_trip(tmp, "2017", ITEMS % ITEMS_2017, SETS % "    m_iUnknown = 3,\n")
            self.assertIn("layout 2017", lua)
            for line in ITEMS_2017.replace("True", "1").splitlines():
                self.assertIn(line.strip(), lua)
            kim.set_layout("2017")
            items, sets = kim.parse_kim(data)
            self.assertEqual(sets[0][0]["m_iUnknown"], 3)
            cloth = [it for it in items if it["m_ItemID"] == 101][0]
            self.assertEqual((cloth["m_bCielItem"], cloth["m_iUnknownC"], cloth["m_fUnknownA"]), (1, 100, 41.5))
            sword = [it for it in items if it["m_ItemID"] == 100][0]
            self.assertEqual(sword["m_iUnknownC"], 100)                     # defaults to the ItemID
            self.assertNotIn("m_iUnknownC", lua.split("m_ItemID = 101")[0])


if __name__ == "__main__":
    unittest.main()
