#!/usr/bin/env python3
"""Adds to our DungeonData.lua the per-character boss drops and ending lines another client has and ours lacks.

Usage:
  merge_dungeon_data.py <client_data> --source <DungeonData.lua> [--source ...] [--units ADD,...]
                        [--ess <our general.ess>] [--luajit <path>] [--dry-run]

A dungeon's BOSS_DROP_ITEM (the boss items its info window shows) and its ENDING_SPEECH<n> (the line a character says
when the dungeon is cleared) list entries by character (ELSWORD ... ADD, CX2DungeonManager::OpenScriptFile). Our
2014-03 file has none for Add (UT_ADD = 9). For each dungeon of ours, keyed by dungeonID, and each of those tables,
the characters ours lacks are taken from the first --source that has them; our entries are never changed, and a table
ours lacks entirely is not added. The item IDs must be in our Item.lua and the STR_IDs in our StringID_def.lua (and,
with --ess, have a text in our general.ess); what is missing is reported, and nothing is written.

Sources are read by running them with LuaJIT (globals such as STR_ID_<n> stand for their own names); ours is edited
as text, so everything else in it stays byte for byte.
"""
import argparse
import json
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from client_data import ClientData
from merge_quests import read_ess

# CX2DungeonManager::OpenScriptFile's character keys, in unit type order (UT_ELSWORD = 1 ... UT_ADD = 9).
UNITS = ["ELSWORD", "AISHA", "RENA", "RAVEN", "EVE", "CHUNG", "ARA", "ELESIS", "ADD"]
TABLE = re.compile(r"^BOSS_DROP_ITEM$|^ENDING_SPEECH\d+$")

DUMP_LUA = r"""
local out = {}
local function names(prefix)
  return setmetatable({}, {__index = function(_, k) return prefix .. "." .. k end})
end
local env = setmetatable({}, {__index = function(_, k)
  if k == "g_pDungeonManager" then
    return setmetatable({}, {__index = function(_, m) return function(_, t)
      if m == "AddDungeonData" then out[#out + 1] = t end end end})
  end
  if k == "TRUE" then return true elseif k == "FALSE" then return false end
  if k:match("^[A-Z_]+$") then return names(k) end
  return k
end})
local f = assert(io.open(arg[1], "rb")); local src = f:read("*a"); f:close()
src = src:gsub("^\239\187\191", "")
local chunk = assert(loadstring(src, "=" .. arg[1]))
setfenv(chunk, env)
chunk()
local function enc(v)
  local t = type(v)
  if t == "string" then return (string.format("%q", v):gsub("\\\n", "\\n")) end
  if t ~= "table" then return tostring(v) end
  local n, parts, list = #v, {}, true
  for k in pairs(v) do if type(k) ~= "number" or k > n then list = false end end
  if list then
    for i = 1, n do parts[#parts + 1] = enc(v[i]) end
    return "[" .. table.concat(parts, ",") .. "]"
  end
  for k, x in pairs(v) do parts[#parts + 1] = string.format("%q", tostring(k)) .. ":" .. enc(x) end
  return "{" .. table.concat(parts, ",") .. "}"
end
for _, t in ipairs(out) do print(enc(t)) end
"""


def find_luajit(given):
    path = given or shutil.which("luajit")
    if not path:
        raise SystemExit("error: no luajit (pass --luajit)")
    return path


def load(luajit, path):
    """{dungeonID: table} of a DungeonData.lua; the first entry of an ID wins, as in the client."""
    with tempfile.NamedTemporaryFile("w", suffix=".lua", delete=False) as f:
        f.write(DUMP_LUA)
    try:
        run = subprocess.run([luajit, f.name, str(path)], capture_output=True, text=True)
    finally:
        Path(f.name).unlink()
    if run.returncode:
        raise SystemExit(f"error: {path}: {run.stderr.strip()}")
    dungeons = {}
    for line in run.stdout.splitlines():
        t = json.loads(line)
        dungeons.setdefault(t["dungeonID"], t)
    return dungeons


def lua(v):
    """A value in our file's compact style: { 1, 2 }, { { STR_ID_1 }, { STR_ID_2 } }."""
    if isinstance(v, list):
        return "{ " + ", ".join(lua(x) for x in v) + " }" if v else "{}"
    return str(v)


def leaves(v):
    if isinstance(v, list):
        for x in v:
            yield from leaves(x)
    else:
        yield v


def blocks(lines):
    """[(first, last)] line ranges of each AddDungeonData call."""
    out, start = [], None
    for n, line in enumerate(lines):
        if line.startswith("g_pDungeonManager:AddDungeonData("):
            start = n
        elif start is not None and line.startswith("})"):
            out.append((start, n))
            start = None
    return out


DUNGEON_ID = re.compile(r"^\tdungeonID = (DUNGEON_ID\.\w+),")
KEY = re.compile(r"^\t(\w+) = \{(.*)$")


def edit_block(lines, first, last, add):
    """Inserts add = {table: {unit: value}} into the block's tables; returns the new lines and what was added."""
    out, done = [], []
    n = first
    while n <= last:
        line = lines[n]
        m = KEY.match(line)
        if not (m and m.group(1) in add):
            out.append(line)
            n += 1
            continue
        key, new = m.group(1), add[m.group(1)]
        entries = [f"{u} = {lua(v)}" for u, v in new.items()]
        if line.rstrip().endswith("},"):  # one line: BOSS_DROP_ITEM = { COMMON = { 1 } },
            body = line.rstrip()[: -len(" },")]
            out.append(body + ", " + ", ".join(entries) + " },")
            n += 1
        else:  # one entry per line, closed by "\t},"
            end = n + 1
            while lines[end].rstrip() != "\t},":
                end += 1
            out.extend(lines[n:end])
            out.extend(f"\t\t{e}," for e in entries)
            out.append(lines[end])
            n = end + 1
        done.append(key)
    return out, done


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("client_data")
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--units", default=",".join(UNITS), help="characters to bring (default: all nine)")
    ap.add_argument("--ess")
    ap.add_argument("--luajit")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    units = args.units.split(",")
    if bad := [u for u in units if u not in UNITS]:
        raise SystemExit(f"error: unknown characters {bad}; known: {UNITS}")
    cd = ClientData(args.client_data)
    luajit = find_luajit(args.luajit)
    path = cd.find("DungeonData.lua")
    ours = load(luajit, path)
    sources = [load(luajit, s) for s in args.source]

    # What each dungeon of ours lacks, from the first source that has it.
    adds, counts = {}, {}
    for did, mine in ours.items():
        for key in sorted(k for k in mine if TABLE.match(k)):
            if not isinstance(mine[key], dict):
                continue
            for unit in units:
                if unit in mine[key]:
                    continue
                for src in sources:
                    value = src.get(did, {}).get(key)
                    if isinstance(value, dict) and unit in value:
                        adds.setdefault(did, {}).setdefault(key, {})[unit] = value[unit]
                        kind = "drops" if key == "BOSS_DROP_ITEM" else "ending lines"
                        counts[(unit, kind)] = counts.get((unit, kind), 0) + 1
                        break

    # Everything added must already be in our client.
    items = set(map(int, re.findall(r"m_ItemID\s*=\s*(\d+)", cd.find("Item.lua").read_text("utf-8", "replace"))))
    defined = set(re.findall(r"^\s*(STR_ID_\d+)\s*=", cd.find("StringID_def.lua").read_text("utf-8", "replace"), re.M))
    texts = read_ess(args.ess) if args.ess else None
    missing = set()
    for tables in adds.values():
        for key, new in tables.items():
            for v in leaves(list(new.values())):
                if key == "BOSS_DROP_ITEM":
                    if int(v) not in items:
                        missing.add(f"item {v}")
                elif v not in defined:
                    missing.add(f"{v} (StringID_def.lua)")
                elif texts is not None and not texts.get(int(v[7:])):
                    missing.add(f"{v} (general.ess)")

    for (unit, kind), n in sorted(counts.items()):
        print(f"{unit}: {kind} in {n} tables")
    if missing:
        print(f"error: {len(missing)} not in our client: {', '.join(sorted(missing)[:20])}", file=sys.stderr)
        raise SystemExit(1)

    raw = path.read_bytes()
    bom = raw.startswith(b"\xef\xbb\xbf")
    text = raw[3 if bom else 0:].decode("utf-8")
    newline = "\r\n" if "\r\n" in text else "\n"
    lines = text.split(newline)
    out, pos, edited = [], 0, 0
    for first, last in blocks(lines):
        did = next((m.group(1) for l in lines[first:last] if (m := DUNGEON_ID.match(l))), None)
        out.extend(lines[pos:first])
        if did in adds:
            block, done = edit_block(lines, first, last, adds[did])
            if sorted(done) != sorted(adds[did]):
                raise SystemExit(f"error: {did}: could not place {sorted(set(adds[did]) - set(done))}")
            out.extend(block)
            edited += 1
        else:
            out.extend(lines[first:last + 1])
        pos = last + 1
    out.extend(lines[pos:])

    print(f"{edited} dungeons edited in {path}")
    if not args.dry_run:
        path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + newline.join(out).encode("utf-8"))
        # The result must still load, with the same dungeons.
        if load(luajit, path).keys() != ours.keys():
            raise SystemExit("error: the edited file lost or gained dungeons")


if __name__ == "__main__":
    main()
