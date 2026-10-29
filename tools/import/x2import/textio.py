"""Decoding of the original sources (CP949 / UTF-8 / UTF-16) and text normalization."""

import codecs
from dataclasses import dataclass


@dataclass
class Decoded:
    text: str
    encoding: str      # "ascii", "utf-8", "utf-8-sig", "utf-16", "cp949", "cp949-lossy"
    had_crlf: bool


def decode(data: bytes) -> Decoded:
    had_crlf = b"\r" in data
    if data.startswith((codecs.BOM_UTF16_LE, codecs.BOM_UTF16_BE)):
        return Decoded(data.decode("utf-16"), "utf-16", had_crlf)
    if data.startswith(codecs.BOM_UTF8):
        return Decoded(data[3:].decode("utf-8", errors="replace"), "utf-8-sig", had_crlf)
    try:
        text = data.decode("utf-8")
        return Decoded(text, "ascii" if text.isascii() else "utf-8", had_crlf)
    except UnicodeDecodeError:
        pass
    try:
        return Decoded(data.decode("cp949"), "cp949", had_crlf)
    except UnicodeDecodeError:
        return Decoded(data.decode("cp949", errors="replace"), "cp949-lossy", had_crlf)


def normalize(text: str) -> str:
    """LF line endings, no trailing whitespace, exactly one final newline."""
    text = text.replace("\r\n", "\n").replace("\r", "\n")
    lines = [line.rstrip() for line in text.split("\n")]
    while lines and not lines[-1]:
        lines.pop()
    return "\n".join(lines) + "\n" if lines else ""
