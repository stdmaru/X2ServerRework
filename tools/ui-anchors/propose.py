#!/usr/bin/env python3
"""Propose UIAnchors.lua entries (T44) from what the client recorded.

The client, built with X2_DEV_TOOLS and run with X2_RECORD_UI_ANCHORS=1, writes UIAnchorsRecord.tsv in its game
folder: for every dialog script drawn, each control's box (layout units, relative to the dialog) and the positions the
dialog was drawn at. This tool reads one or more of those files and proposes anchors by where things sit in the
1024x768 layout:

- a dialog held to the left or right edge of the layout becomes LEFT or RIGHT;
- a dialog spanning the width (a HUD) stays CENTER and its controls get their own anchors: held to the left or right
  edge, or lying in the left or right third, LEFT or RIGHT; held to both edges, STRETCH;
- full-screen pictures (backgrounds) and everything in the middle stay CENTER (the default, so not written);
- a dialog drawn at more than one position (moved by the player or placed by code next to something else) stays
  CENTER and is listed for review: its placement is derived, not a layout position.

Usage:
  propose.py UIAnchorsRecord.tsv [more.tsv ...] [-o UIAnchors.proposed.lua] [--scripts <client_data>/ui/scripts]

With --scripts, the report also counts the dialog scripts never recorded.
"""
from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass, field
from pathlib import Path

LAYOUT_W = 1024.0
LAYOUT_H = 768.0
EDGE = 24.0          # within this many units of an edge counts as held to it
THIRD = LAYOUT_W / 3
SCREEN_W = 600.0     # a dialog at least this wide and SCREEN_H tall is a whole screen composed of pieces
SCREEN_H = 450.0
BAND_H = LAYOUT_H / 2  # taller than this, a dialog spanning the width with a background is a screen
BACK_SHARE = 0.4     # a control covering this share of the dialog's box is its background


@dataclass
class Control:
    name: str
    type: int
    min_x: float
    min_y: float
    max_x: float
    max_y: float
    positions: set[tuple[int, int]] = field(default_factory=set)


@dataclass
class Box:
    min_x: float
    min_y: float
    max_x: float
    max_y: float

    @property
    def width(self) -> float:
        return self.max_x - self.min_x

    @property
    def height(self) -> float:
        return self.max_y - self.min_y

    def at_left(self) -> bool:
        return self.min_x <= EDGE

    def at_right(self) -> bool:
        return self.max_x >= LAYOUT_W - EDGE

    def full_screen(self) -> bool:
        return self.at_left() and self.at_right() and self.min_y <= EDGE and self.max_y >= LAYOUT_H - EDGE

    def __str__(self) -> str:
        return f"{self.min_x:.0f}..{self.max_x:.0f} x {self.min_y:.0f}..{self.max_y:.0f}"


@dataclass
class Proposal:
    script: str
    dialog: str = "CENTER"
    controls: dict[str, str] = field(default_factory=dict)
    notes: list[str] = field(default_factory=list)
    review: str | None = None   # why a person should look at it


def read_records(paths: list[Path]) -> dict[str, dict[str, Control]]:
    scripts: dict[str, dict[str, Control]] = {}
    for path in paths:
        for line in path.read_text(encoding="utf-8").splitlines():
            if not line or line.startswith("#"):
                continue
            cols = line.split("\t")
            if len(cols) < 7:
                continue
            script, name = cols[0].lower(), cols[1]
            ctype = int(cols[2])
            min_x, min_y, max_x, max_y = (float(c) for c in cols[3:7])
            positions = set()
            if len(cols) > 7 and cols[7]:
                for pos in cols[7].split(";"):
                    x, y = pos.split(",")
                    positions.add((int(x), int(y)))
            controls = scripts.setdefault(script, {})
            old = controls.get(name)
            if old is None:
                controls[name] = Control(name, ctype, min_x, min_y, max_x, max_y, positions)
            else:   # the same control in another record file: union
                old.min_x, old.min_y = min(old.min_x, min_x), min(old.min_y, min_y)
                old.max_x, old.max_y = max(old.max_x, max_x), max(old.max_y, max_y)
                old.positions |= positions
    return scripts


def absolute(control: Control, pos: tuple[int, int]) -> Box:
    return Box(control.min_x + pos[0], control.min_y + pos[1], control.max_x + pos[0], control.max_y + pos[1])


def control_anchor(box: Box) -> tuple[str, str]:
    """Anchor of a control inside a dialog that spans the width, and why."""
    if box.full_screen():
        return "CENTER", "full-screen picture"
    if box.at_left() and box.at_right():
        return "STRETCH", "held to both edges"
    if box.at_left():
        return "LEFT", "held to the left edge"
    if box.at_right():
        return "RIGHT", "held to the right edge"
    if box.max_x <= THIRD:
        return "LEFT", "in the left third"
    if box.min_x >= LAYOUT_W - THIRD:
        return "RIGHT", "in the right third"
    return "CENTER", "in the middle"


def propose(script: str, controls: dict[str, Control]) -> Proposal:
    proposal = Proposal(script)
    positions = set().union(*(c.positions for c in controls.values())) or {(0, 0)}
    if len(positions) > 1:
        proposal.review = f"drawn at {len(positions)} positions (moved or placed by code): left centered"
        return proposal
    pos = next(iter(positions))
    boxes = {name: absolute(c, pos) for name, c in controls.items()}
    parts = {name: box for name, box in boxes.items() if not box.full_screen()}
    if not parts:
        proposal.notes.append("full-screen picture only")
        return proposal

    union = Box(min(b.min_x for b in parts.values()), min(b.min_y for b in parts.values()),
                max(b.max_x for b in parts.values()), max(b.max_y for b in parts.values()))
    proposal.notes.append(f"at {pos[0]},{pos[1]}; controls {union}")
    # A background covering much of the dialog makes it a window or a whole screen, not a sparse HUD.
    has_back = any(b.width * b.height >= BACK_SHARE * union.width * union.height for b in parts.values())
    if has_back and union.width >= SCREEN_W and union.height >= SCREEN_H:
        proposal.review = f"a whole screen ({union}): left centered; anchor its pieces by hand if wanted"
        return proposal
    if has_back and union.at_left() and union.at_right() and union.height >= BAND_H:
        proposal.review = f"spans the width with a background ({union}): left centered"
        return proposal
    if union.at_left() != union.at_right() and union.width >= SCREEN_W:
        proposal.review = f"wide and at one edge ({union}): left centered"
        return proposal
    if union.at_left() and union.at_right():
        for name, box in sorted(parts.items()):
            anchor, why = control_anchor(box)
            if anchor != "CENTER":
                if name.startswith("#"):
                    proposal.review = f"unnamed control {name} ({box}) would be {anchor}: name it in the script"
                    continue
                proposal.controls[name] = anchor
                proposal.notes.append(f"{name}: {anchor}, {why} ({box})")
    elif union.at_left():
        proposal.dialog = "LEFT"
    elif union.at_right():
        proposal.dialog = "RIGHT"
    elif union.max_x <= THIRD or union.min_x >= LAYOUT_W - THIRD:
        side = "left" if union.max_x <= THIRD else "right"
        proposal.review = f"in the {side} third but not at the edge: left centered"
    return proposal


def lua_string(text: str) -> str:
    return '"' + text.replace("\\", "\\\\").replace('"', '\\"') + '"'


def lua_key(name: str) -> str:
    if name.isidentifier() and name.isascii():
        return name
    return f"[{lua_string(name)}]"


def write_lua(proposals: list[Proposal]) -> str:
    lines = [
        "﻿-- Proposed by tools/ui-anchors/propose.py from UIAnchorsRecord.tsv; review before copying into UIAnchors.lua.",
        "UI_ANCHORS =",
        "{",
    ]
    for p in proposals:
        if p.dialog == "CENTER" and not p.controls:
            continue
        for note in p.notes:
            lines.append(f"\t-- {note}")
        fields = []
        if p.dialog != "CENTER":
            fields.append(f"DIALOG = {lua_string(p.dialog)}")
        if p.controls:
            controls = ", ".join(f"{lua_key(n)} = {lua_string(a)}" for n, a in sorted(p.controls.items()))
            fields.append(f"CONTROLS = {{ {controls} }}")
        lines.append(f"\t[{lua_string(p.script)}] = {{ {', '.join(fields)} }},")
    lines.append("}")
    return "\n".join(lines) + "\n"


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("records", nargs="+", type=Path, help="UIAnchorsRecord.tsv files")
    parser.add_argument("-o", "--output", type=Path, help="write the proposal here (default: stdout)")
    parser.add_argument("--scripts", type=Path, help="client_data folder of the dialog scripts, for coverage")
    args = parser.parse_args(argv)

    scripts = read_records(args.records)
    proposals = [propose(script, controls) for script, controls in sorted(scripts.items())]
    text = write_lua(proposals)
    if args.output:
        args.output.write_text(text, encoding="utf-8")
    else:
        sys.stdout.write(text)

    anchored = [p for p in proposals if p.dialog != "CENTER" or p.controls]
    review = [p for p in proposals if p.review]
    print(f"{len(scripts)} dialog scripts recorded: {len(anchored)} anchored, {len(review)} to review", file=sys.stderr)
    for p in review:
        print(f"  review {p.script}: {p.review}", file=sys.stderr)
    if args.scripts:
        known = {f.name.lower() for f in args.scripts.rglob("DLG_*.lua")}
        missing = sorted(known - set(scripts))
        print(f"{len(missing)} of {len(known)} dialog scripts in {args.scripts} never recorded", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
