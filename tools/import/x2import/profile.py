"""Build profiles (flags/<name>.profile.toml): which configuration headers an original build reads, with which
definitions, and which code it compiles. Shared by gen_flags.py and check_equivalence.py."""

import tomllib
from dataclasses import dataclass, field
from pathlib import Path

from . import flagsim, pipeline


@dataclass
class Layer:
    """Part of the build that reads a different chain of configuration headers (e.g. the client engine, which
    never includes ServerDefine.h). Its code is checked against its own simulation."""
    name: str
    entries: list[Path]
    code_globs: list[str]


@dataclass
class Profile:
    name: str
    data: dict
    root: Path
    entries: list[Path]
    include_dirs: list[Path]
    defines: list[str]
    dev_defines: list[str]
    dev_undefines: list[str]
    config: set[Path]
    layers: list[Layer] = field(default_factory=list)

    def files(self, globs: list[str] | None = None) -> list[Path]:
        """Code files of the profile (or of `globs`), minus `code_exclude_globs`."""
        globs = self.data["code_globs"] if globs is None else globs
        out = {p for g in globs for p in self.root.glob(g) if p.is_file()}
        out -= {p for g in self.data.get("code_exclude_globs", []) for p in self.root.glob(g)}
        return sorted(out)

    def initial(self, dev: bool) -> dict[str, str]:
        names = [d for d in self.defines if not (dev and d in self.dev_undefines)]
        if dev:
            names += self.dev_defines
        return {d: "" for d in names}

    def simulate(self, dev: bool, read=None, entries: list[Path] | None = None) -> flagsim.SimResult:
        # only configuration headers are entered: the client's KTDX.h also includes ~100 engine headers
        return flagsim.simulate(entries or self.entries, self.initial(dev), self.include_dirs, read,
                                follow=lambda p: p.resolve() in self.config)


def load(name: str) -> Profile:
    data = tomllib.loads((pipeline.TOOL / "flags" / f"{name}.profile.toml").read_text())["profile"]
    root = (pipeline.REFERENCE / data["root"]).resolve()
    entries = data["entries"] if "entries" in data else [data["entry"]]
    config = {p.resolve() for g in data["config_globs"] for p in root.glob(g)}
    layers = [Layer(x["name"], [root / e for e in x["entries"]], x["code_globs"]) for x in data.get("layers", [])]
    return Profile(name=name, data=data, root=root, entries=[root / e for e in entries],
                   include_dirs=[root / d for d in data["include_dirs"]], defines=list(data["defines"]),
                   dev_defines=list(data["dev_defines"]), dev_undefines=list(data.get("dev_undefines", [])),
                   config=config, layers=layers)
