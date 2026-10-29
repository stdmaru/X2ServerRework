import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from x2import.condres import Flags, resolve  # noqa: E402

FLAGS = Flags(true={"SERV_ON", "SERV_COUNTRY_US"}, false={"SERV_OFF", "SERV_COUNTRY_JP", "_OPEN_TEST_"},
              dev={"_IN_HOUSE_", "ITEM_CHEAT_POPUP_TEST"}, dev_not={"_SERVICE_"})


def r(text: str) -> str:
    return resolve(text.strip("\n") + "\n", FLAGS).text.strip("\n")


class ResolveTest(unittest.TestCase):
    def test_true_branch_kept_without_directives(self):
        self.assertEqual(r("#ifdef SERV_ON\na\n#else\nb\n#endif"), "a")

    def test_false_branch_dropped(self):
        self.assertEqual(r("#ifdef SERV_OFF\na\n#else\nb\n#endif"), "b")
        self.assertEqual(r("#ifdef SERV_OFF\na\n#endif\nc"), "c")

    def test_unknown_kept_verbatim(self):
        src = "#ifdef _WIN32 // win\na\n#else\nb\n#endif"
        self.assertEqual(r(src), src)

    def test_mixed_expression_simplified(self):
        self.assertEqual(r("#if defined(SERV_ON) && defined(_WIN32)\na\n#endif"), "#if defined(_WIN32)\na\n#endif")
        self.assertEqual(r("#if defined(SERV_OFF) || defined(_WIN32)\na\n#endif"), "#if defined(_WIN32)\na\n#endif")
        self.assertEqual(r("#if defined(SERV_OFF) && defined(_WIN32)\na\n#endif"), "")

    def test_elif_chain(self):
        src = "#if defined(SERV_COUNTRY_JP)\njp\n#elif defined(SERV_COUNTRY_US)\nus\n#else\nother\n#endif"
        self.assertEqual(r(src), "us")
        src = "#if defined(_X)\nx\n#elif defined(SERV_COUNTRY_US)\nus\n#else\nother\n#endif"
        self.assertEqual(r(src), "#if defined(_X)\nx\n#else\nus\n#endif")
        src = "#if defined(SERV_COUNTRY_JP)\njp\n#elif defined(_X)\nx\n#else\nother\n#endif"
        self.assertEqual(r(src), "#if defined(_X)\nx\n#else\nother\n#endif")

    def test_dev_rewrite(self):
        self.assertEqual(r("#ifndef _SERVICE_\nd\n#endif"), "#if X2_DEV_TOOLS\nd\n#endif")
        self.assertEqual(r("#ifdef _SERVICE_\ns\n#else\nd\n#endif"), "#if !X2_DEV_TOOLS\ns\n#else\nd\n#endif")
        self.assertEqual(r("#ifdef ITEM_CHEAT_POPUP_TEST\nd\n#endif"), "#if X2_DEV_TOOLS\nd\n#endif")
        self.assertEqual(r("#if defined(_IN_HOUSE_) || defined(_OPEN_TEST_)\nd\n#endif"), "#if X2_DEV_TOOLS\nd\n#endif")
        res = resolve("#ifdef _IN_HOUSE_\nd\n#endif\n", FLAGS)
        self.assertEqual(len(res.dev_blocks), 1)

    def test_nested(self):
        src = "#ifdef SERV_OFF\n#ifdef _WIN32\na\n#endif\n#else\n#ifdef _WIN32\nb\n#endif\n#endif"
        self.assertEqual(r(src), "#ifdef _WIN32\nb\n#endif")

    def test_endif_trailing_tokens(self):
        self.assertEqual(r("#ifdef _WIN32\na\n#endif _WIN32"), "#ifdef _WIN32\na\n#endif // _WIN32")

    def test_flag_defines_removed(self):
        self.assertEqual(r("#define SERV_ON\n#define SERV_OFF // x\n#define OTHER 1\n#undef SERV_ON"), "#define OTHER 1")

    def test_pragma_lib_removed(self):
        res = resolve('#pragma comment(lib, "ws2_32.lib")\n#pragma once\n', FLAGS)
        self.assertEqual(res.text.strip(), "#pragma once")
        self.assertEqual(res.pragma_libs, ["ws2_32.lib"])

    def test_directives_in_block_comment_ignored(self):
        src = "/*\n#ifdef SERV_OFF\n*/\na"
        self.assertEqual(r(src), src)

    def test_continuation_lines(self):
        self.assertEqual(r("#if defined(SERV_ON) && \\\n    defined(SERV_OFF)\na\n#endif\nb"), "b")

    def test_numeric(self):
        # `#if 0` / `#if 1` are dead/always-on code: resolved as well
        self.assertEqual(r("#if 0\na\n#else\nb\n#endif"), "b")
        self.assertEqual(r("#if 1\na\n#else\nb\n#endif"), "a")
        self.assertEqual(r("#if SERV_OFF\na\n#endif"), "")


if __name__ == "__main__":
    unittest.main()
