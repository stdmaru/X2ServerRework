#!/usr/bin/env python3
"""Imports one module from _reference into src/. Usage: tools/import/import.py <module> [--dry-run] [--force]"""

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from x2import import pipeline  # noqa: E402


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("module", help="name of tools/import/modules/<module>.toml")
    ap.add_argument("--dry-run", action="store_true", help="only write the report")
    ap.add_argument("--force", action="store_true", help="overwrite files edited after the last import")
    args = ap.parse_args()
    try:
        report = pipeline.run(args.module, dry_run=args.dry_run, force=args.force)
    except pipeline.ImportError_ as e:
        print(f"error: {e}", file=sys.stderr)
        return 1
    print(f"report: {report.relative_to(pipeline.REPO)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
