import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from x2import import flagsim  # noqa: E402


class SimTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def write(self, name, text):
        p = self.root / name
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text)
        return p

    def test_defines_undefs_and_country_include(self):
        self.write("Global/Define_US.h", "#undef SERV_A\n#define SERV_US_ONLY\n")
        self.write("Global/Define_JP.h", "#define SERV_JP_ONLY\n")
        entry = self.write("Define.h", (
            "#define SERV_A // comment\n"
            "#define SERV_B\n"
            "//#define SERV_DISABLED\n"
            "#define LINE(x) #x\n"
            "#if defined(SERV_COUNTRY_JP)\n#include \"Global/Define_JP.h\"\n"
            "#elif defined(SERV_COUNTRY_US)\n#include \"global/define_us.h\"\n#endif\n"
            "#ifdef _IN_HOUSE_\n#define SERV_HOUSE\n#endif\n"
            "#if VERSION > 2\n#define SERV_NEW\n#endif\n"))
        res = flagsim.simulate(entry, {"SERV_COUNTRY_US": "", "VERSION": "3"}, [self.root])
        d = set(res.defined)
        self.assertIn("SERV_B", d)
        self.assertIn("SERV_US_ONLY", d)
        self.assertIn("SERV_NEW", d)
        self.assertNotIn("SERV_A", d)        # undefined by the country file
        self.assertNotIn("SERV_JP_ONLY", d)
        self.assertNotIn("SERV_HOUSE", d)
        self.assertIn("SERV_DISABLED", res.mentioned)
        self.assertIn("LINE", res.function_like)
        self.assertNotIn("LINE", res.mentioned)

        dev = flagsim.simulate(entry, {"SERV_COUNTRY_US": "", "_IN_HOUSE_": ""}, [self.root])
        self.assertIn("SERV_HOUSE", dev.defined)

    def test_nested_dead_branches(self):
        entry = self.write("D.h", "#if 0\n#ifdef X\n#define A\n#else\n#define B\n#endif\n#else\n#define C\n#endif\n")
        res = flagsim.simulate(entry, {}, [self.root])
        self.assertEqual(set(res.defined), {"C"})


if __name__ == "__main__":
    unittest.main()
