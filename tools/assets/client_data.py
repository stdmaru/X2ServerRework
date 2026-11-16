"""Files of a client_data found by name, never by path: its organization is x2_assets' and may change at any time
(docs/tools/client-data.md). `x2_assets --cli where <client_data> <name>` answers the same from the command line."""
import os
from pathlib import Path


class ClientData:
    def __init__(self, root):
        self.root = Path(root)
        if not (self.root / ".kom" / "manifest.tsv").is_file():
            raise SystemExit(f"error: {self.root} is not a client_data (no .kom/manifest.tsv)")
        self._files = None

    def _index(self):
        if self._files is None:
            self._files = {}
            for folder, dirs, names in os.walk(self.root):
                if Path(folder) == self.root:
                    dirs[:] = [d for d in dirs if d != ".kom"]
                for name in names:
                    self._files.setdefault(name.lower(), []).append(Path(folder) / name)
        return self._files

    def find(self, name):
        """The file of that name (ASCII case-insensitive), wherever it is; an error if there is none or more."""
        paths = self._index().get(name.lower(), [])
        if not paths:
            raise SystemExit(f"error: no {name} in the client_data {self.root}")
        if len(paths) > 1:
            raise SystemExit(f"error: {name} is in more than one folder: {', '.join(map(str, paths))}")
        return paths[0]
