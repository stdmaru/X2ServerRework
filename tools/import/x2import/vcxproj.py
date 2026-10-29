"""Reads the list of files that a .vcxproj really builds for one configuration."""

import re
import xml.etree.ElementTree as ET
from pathlib import Path, PureWindowsPath

_NS = "{http://schemas.microsoft.com/developer/msbuild/2003}"
_COND_RE = re.compile(r"'\$\(Configuration\)\|\$\(Platform\)'\s*==\s*'([^']+)'")


def _applies(cond: str | None, config: str) -> bool:
    if not cond:
        return True
    m = _COND_RE.search(cond)
    return m is None or m.group(1) == config


def _is_excluded(item: ET.Element, config: str) -> bool:
    excluded = False
    for ex in item.findall(_NS + "ExcludedFromBuild"):
        if _applies(ex.get("Condition"), config) and (ex.text or "").strip().lower() == "true":
            excluded = True
    return excluded


def project_files(vcxproj: Path, config: str) -> tuple[list[Path], list[Path]]:
    """Returns (compiled sources, listed headers) as absolute paths, for `config` ("MD|Win32")."""
    root = ET.parse(vcxproj).getroot()
    sources: list[Path] = []
    headers: list[Path] = []
    for group in root.findall(_NS + "ItemGroup"):
        if not _applies(group.get("Condition"), config):
            continue
        for item in group:
            tag = item.tag.removeprefix(_NS)
            inc = item.get("Include")
            if not inc or tag not in ("ClCompile", "ClInclude"):
                continue
            path = (vcxproj.parent / Path(*PureWindowsPath(inc).parts)).resolve()
            if tag == "ClInclude" or path.suffix.lower() in (".h", ".hpp", ".inl"):
                headers.append(path)
            elif not _is_excluded(item, config):
                sources.append(path)
    return sources, headers
