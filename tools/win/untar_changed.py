"""Extracts a tar stream from stdin into a folder, writing only the files whose contents differ (tools/win/winbuild.sh).

A file written gets the machine's current time, so ninja rebuilds what depends on it even when the sender's mtime is
older than the objects already built there (an edit made while a build ran); unchanged files keep theirs and are not
rebuilt.
    python untar_changed.py <folder> < files.tar
"""

import os
import sys
import tarfile


def main() -> int:
    root = sys.argv[1]
    written = 0
    with tarfile.open(fileobj=sys.stdin.buffer, mode="r|") as archive:
        for member in archive:
            if not member.isfile():
                continue
            path = os.path.join(root, *member.name.split("/"))
            data = archive.extractfile(member).read()
            try:
                with open(path, "rb") as existing:
                    if existing.read() == data:
                        continue
            except OSError:
                pass
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "wb") as out:
                out.write(data)
            written += 1
    print(f"sent: {written} changed files")
    return 0


if __name__ == "__main__":
    sys.exit(main())
