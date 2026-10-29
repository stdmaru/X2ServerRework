#!/usr/bin/env python3
"""Checks the wiki: broken relative links and pages missing from their folder index."""

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
LINK = re.compile(r"\]\(([^)\s]+)\)")


def main() -> int:
    errors = []
    pages = sorted((REPO / "docs").rglob("*.md")) + [REPO / "AGENTS.md"]
    for page in pages:
        text = re.sub(r"```.*?```", "", page.read_text(), flags=re.S)
        text = re.sub(r"`[^`\n]*`", "", text)  # examples inside code are not links
        for target in LINK.findall(text):
            if re.match(r"[a-z]+:", target) or target.startswith("#"):
                continue
            path = (page.parent / target.partition("#")[0]).resolve()
            if not path.exists():
                errors.append(f"{page.relative_to(REPO)}: broken link {target}")
    for page in (REPO / "docs").rglob("*.md"):
        if page.name == "README.md":
            index = page.parent.parent / "README.md"
            if page.parent == REPO / "docs":
                continue
        else:
            index = page.parent / "README.md"
        rel = page.name if page.name != "README.md" else f"{page.parent.name}/README.md"
        if not index.exists() or rel not in index.read_text():
            errors.append(f"{page.relative_to(REPO)}: not listed in {index.relative_to(REPO)}")
    for e in errors:
        print(e)
    print(f"{len(pages)} pages, {len(errors)} problem(s)")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
