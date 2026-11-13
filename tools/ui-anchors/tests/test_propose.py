import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import propose  # noqa: E402


class ProposeTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def proposals(self, *rows):
        path = self.root / "UIAnchorsRecord.tsv"
        path.write_text("# header\n" + "".join("\t".join(str(c) for c in row) + "\n" for row in rows), encoding="utf-8")
        scripts = propose.read_records([path])
        return {s: propose.propose(s, c) for s, c in scripts.items()}

    def test_dialog_at_an_edge(self):
        p = self.proposals(
            ("dlg_left.lua", "Back", 5, 0, 0, 300, 120, "0,0"),
            ("dlg_right.lua", "Back", 5, 0, 0, 200, 100, "820,10"),
            ("dlg_middle.lua", "Back", 5, 0, 0, 400, 300, "300,200"),
        )
        self.assertEqual(p["dlg_left.lua"].dialog, "LEFT")
        self.assertEqual(p["dlg_right.lua"].dialog, "RIGHT")
        self.assertEqual(p["dlg_middle.lua"].dialog, "CENTER")
        self.assertFalse(p["dlg_middle.lua"].controls)

    def test_hud_spanning_the_width(self):
        p = self.proposals(
            ("dlg_hud.lua", "Bar", 5, 0, 740, 1024, 768, "0,0"),
            ("dlg_hud.lua", "Gauge", 5, 30, 10, 250, 60, "0,0"),
            ("dlg_hud.lua", "MiniMap", 5, 860, 0, 1024, 160, "0,0"),
            ("dlg_hud.lua", "Slots", 5, 380, 680, 640, 730, "0,0"),
        )["dlg_hud.lua"]
        self.assertEqual(p.dialog, "CENTER")
        self.assertEqual(p.controls, {"Bar": "STRETCH", "Gauge": "LEFT", "MiniMap": "RIGHT"})

    def test_full_screen_picture_stays_centered(self):
        p = self.proposals(
            ("dlg_login.lua", "Background", 5, 0, 0, 1024, 768, "0,0"),
            ("dlg_login.lua", "Button", 1, 576, 663, 674, 695, "0,0"),
        )["dlg_login.lua"]
        self.assertEqual((p.dialog, p.controls, p.review), ("CENTER", {}, None))

    def test_moved_dialog_is_left_for_review(self):
        p = self.proposals(("dlg_inventory.lua", "Back", 5, 0, 0, 300, 400, "0,0;500,100"))["dlg_inventory.lua"]
        self.assertEqual(p.dialog, "CENTER")
        self.assertIn("2 positions", p.review)

    def test_unnamed_control_is_left_for_review(self):
        p = self.proposals(
            ("dlg_hud.lua", "#3", 5, 0, 0, 100, 50, "0,0"),
            ("dlg_hud.lua", "Right", 5, 900, 0, 1024, 50, "0,0"),
        )["dlg_hud.lua"]
        self.assertEqual(p.controls, {"Right": "RIGHT"})
        self.assertIn("#3", p.review)

    def test_records_merge_and_lua_output(self):
        a = self.root / "a.tsv"
        b = self.root / "b.tsv"
        a.write_text("dlg_x.lua\tBar\t5\t0\t0\t500\t20\t0,0\n", encoding="utf-8")
        b.write_text("DLG_X.lua\tBar\t5\t400\t0\t1024\t20\t0,0\n", encoding="utf-8")
        scripts = propose.read_records([a, b])
        self.assertEqual((scripts["dlg_x.lua"]["Bar"].min_x, scripts["dlg_x.lua"]["Bar"].max_x), (0, 1024))
        lua = propose.write_lua([propose.propose("dlg_x.lua", scripts["dlg_x.lua"])])
        self.assertIn('["dlg_x.lua"] = { CONTROLS = { Bar = "STRETCH" } },', lua)


if __name__ == "__main__":
    unittest.main()
