"""Small helpers for inserting #ifdef'd blocks into imported sources (encoding-preserving)."""
PATHS = {  # switch: (base, first job, second job, tag)
    "X2_ADD_2ND_PATH": ("UC_ADD_NASOD_RULER", "UC_ADD_ARC_TRACER", "UC_ADD_MASTERMIND"),
    "X2_ARA_3RD_PATH": ("UC_ARA_MARTIAL_ARTIST", "UC_ARA_LITTLE_SPECTER", "UC_ARA_ASURA"),
    "X2_ELESIS_3RD_PATH": ("UC_ELESIS_KNIGHT", "UC_ELESIS_DARK_KNIGHT", "UC_ELESIS_CRIMSON_AVENGER"),
    "X2_ADD_3RD_PATH": ("UC_ADD_NASOD_RULER", "UC_ADD_TIME_TRACER", "UC_ADD_DIABOLIC_ESPER"),
}
CHAR = {"ADD": ["X2_ADD_2ND_PATH", "X2_ADD_3RD_PATH"], "ARA": ["X2_ARA_3RD_PATH"], "ELESIS": ["X2_ELESIS_3RD_PATH"]}


class Src:
    def __init__(self, path):
        self.path = path
        raw = open(path, "rb").read()
        self.crlf = b"\r\n" in raw
        self.s = raw.decode("utf-8", "surrogateescape").replace("\r\n", "\n")

    def save(self):
        s = self.s.replace("\n", "\r\n") if self.crlf else self.s
        open(self.path, "wb").write(s.encode("utf-8", "surrogateescape"))

    def find(self, anchor, start=0, within=None):
        if within is not None:
            start = self.s.index(within)
        i = self.s.find(anchor, start)
        if i < 0:
            raise SystemExit(f"{self.path}: anchor not found: {anchor!r} (within {within!r})")
        return i

    def insert_after_line(self, anchor, text, within=None):
        """Inserts text after the line holding anchor (the first after `within`)."""
        i = self.find(anchor, within=within)
        j = self.s.index("\n", i) + 1
        self.s = self.s[:j] + text + self.s[j:]

    def insert_before_line(self, anchor, text, within=None):
        i = self.find(anchor, within=within)
        j = self.s.rfind("\n", 0, i) + 1
        self.s = self.s[:j] + text + self.s[j:]


def cases(switches, which, fmt="\tcase {cls}:\n", prefix=""):
    """#ifdef'd case labels: which in ('first', 'second', 'both')."""
    out = ""
    for sw in switches:
        base, first, second = PATHS[sw]
        names = {"first": [first], "second": [second], "both": [first, second]}[which]
        out += f"#ifdef {sw}\n" + "".join(fmt.format(cls=prefix + n, base=prefix + base, first=prefix + first, second=prefix + second) for n in names) + "#endif\n"
    return out
