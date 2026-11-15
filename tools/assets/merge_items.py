#!/usr/bin/env python3
"""Brings items (and their sets) from other clients' Item.lua/SetItem.lua into a client_data's.

Usage:
  merge_items.py <client_data items folder> --enum <our Enum.lua>
                 (--unit-type UT_ADD | --ids 84-89,41759) [...]
                 --source <Item.lua> [--source ...] [--text <Item.lua>] [--dry-run]

- Our Item.lua and SetItem.lua (kim.py's layout, in the given folder) are kept as they are; only items whose ID we do
  not have are added, at the end, taken from the first --source that has them. Items are selected by unit type
  (--unit-type, repeatable) and/or by ID (--ids).
- Sources are kim.py's Item.lua (e.g. BR 2014-08) or the KimTool "exportfull" Item.lua of V5's server ClientScript,
  whose per-character lists (m_FrameOffsetList, m_RenderScaleList, m_RenderRotateList, m_CommonItem*NameList,
  m_AttachedMeshList, m_AttachedParticleList, m_SlashTraceList) are turned into the original keys first.
  Only fields of the 2014 Item.kim layout are kept (the JIGGLE flag of attached meshes, m_bInventoryOnly and
  m_AttachFrameName2..7 are dropped).
- The sets the added items name (m_SetID) that our SetItem.lua lacks come from the SetItem.lua next to a source or
  to --text; an item whose set is in none gets m_SetID 0 (the client asserts on unknown sets).
- Names and descriptions come from --text by ID (a kim.py Item.lua of any layout, e.g. BR 2017), else from the
  first source whose text is not Korean. Items still Korean (released only in Korea) and items naming an enum value
  our Enum.lua lacks (classes or features our code does not have) are left out and listed.
The result is checked by compiling it with kim.py (enum names included). See docs/project/add-character.md.
"""
import argparse
import json
import os
import re
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "kom"))
import kim  # noqa: E402

HANGUL = re.compile("[ᄀ-ᇿ㄰-㆏가-힣]")
TEXT_KEYS = ("m_Name", "m_Description", "m_DescriptionInShop")

# Runs before the source scripts: turns V5's list fields into the keys of the original Item.lua.
SHIM_LUA = r"""
-- an enum name our Enum.lua lacks gives -1, so items that need what we do not have can be left out
for _, name in ipairs(WANT) do
  local t = rawget(_G, name)
  if type(t) == "table" then
    local mt = getmetatable(t) or {}
    local old = mt.__index
    mt.__index = function(tt, k) if type(old) == "function" then old(tt, k) end return -1 end
    setmetatable(t, mt)
  end
end
local NAMES = { "Elsword", "Aisha", "Rena", "Raven", "Eve", "Chung", "Ara", "Elesis", "Add" }
local STT = { "SLASH_TRACE_COLOR", "SLASH_TRACE_TIP_COLOR", "HYPER_SLASH_TRACE_COLOR", "HYPER_SLASH_TRACE_TIP_COLOR" }
local function num(v) return string.format("%.9g", v or 0) end
local rec = g_pItemManager.AddItemTemplet
g_pItemManager.AddItemTemplet = function(self, t)
  if type(t) ~= "table" then t = self end
  local function each(list, f)
    if type(list) ~= "table" then return end
    for _, e in ipairs(list) do
      local u = e.UNIT_TYPE
      if u == nil or (type(u) == "number" and u >= 1 and u <= #NAMES) then f(u, e) end
    end
  end
  each(t.m_FrameOffsetList, function(u, e)
    t["m_FrameOffset" .. NAMES[u] .. "X"], t["m_FrameOffset" .. NAMES[u] .. "Y"], t["m_FrameOffset" .. NAMES[u] .. "Z"] = e.X, e.Y, e.Z
  end)
  local sr = {}
  each(t.m_RenderScaleList, function(u, e) sr[u] = sr[u] or {}; sr[u].s = { e.X, e.Y, e.Z } end)
  -- V5 writes the rotations in radians; the original's "RX:..." strings are in degrees
  each(t.m_RenderRotateList, function(u, e) sr[u] = sr[u] or {}; sr[u].r = { e.X, e.Y, e.Z } end)
  for u, v in pairs(sr) do
    local s, r = v.s or { 1, 1, 1 }, v.r or { 0, 0, 0 }
    t[string.upper(NAMES[u]) .. "_ITEM_SCALE_ROTATE"] = "SX:" .. num(s[1]) .. " SY:" .. num(s[2]) .. " SZ:" .. num(s[3]) ..
      " RX:" .. num(r[1] * 180 / math.pi) .. " RY:" .. num(r[2] * 180 / math.pi) .. " RZ:" .. num(r[3] * 180 / math.pi)
  end
  each(t.m_CommonItemModelNameList, function(u, e) t[string.upper(NAMES[u]) .. "_MODEL_NAME"] = e.NAME end)
  each(t.m_CommonItemXETNameList, function(u, e) t[string.upper(NAMES[u]) .. "_XET_NAME"] = e.NAME end)
  if type(t.m_AttachedMeshList) == "table" then
    t.ATTACHED_MESH = {}
    for _, e in ipairs(t.m_AttachedMeshList) do t.ATTACHED_MESH[#t.ATTACHED_MESH + 1] = { MESH_NAME = e.MESH_NAME, BONE_NAME = e.BONE_NAME } end
  end
  if type(t.m_AttachedParticleList) == "table" then
    t.ATTACHED_PARTICLE = {}
    for _, e in ipairs(t.m_AttachedParticleList) do t.ATTACHED_PARTICLE[#t.ATTACHED_PARTICLE + 1] = { PARTICLE_NAME = e.PARTICLE_NAME, BONE_NAME = e.BONE_NAME } end
  end
  if type(t.m_SlashTraceList) == "table" then
    for _, e in ipairs(t.m_SlashTraceList) do
      local c, k = e.COLOR or 0, STT[(e.TYPE or 0) + 1]
      if k then
        t[k] = { math.floor(c / 65536) % 256, math.floor(c / 256) % 256, c % 256, math.floor(c / 16777216) % 256 }
      end
    end
  end
  t.m_FrameOffsetList, t.m_RenderScaleList, t.m_RenderRotateList = nil, nil, nil
  t.m_CommonItemModelNameList, t.m_CommonItemXETNameList = nil, nil
  t.m_AttachedMeshList, t.m_AttachedParticleList, t.m_SlashTraceList = nil, nil, nil
  return rec(self, t)
end
"""


def load(luajit, scripts, enum_files, shim=True):
    """Runs scripts with kim.py's dumper: (items by ID, set calls by ID), as kim.py reads them."""
    with tempfile.TemporaryDirectory() as tmp:
        dump = os.path.join(tmp, "dump.txt")
        files = kim.with_bom(scripts, tmp, split=True)
        if shim:
            shim_path = os.path.join(tmp, "shim.lua")
            Path(shim_path).write_text(SHIM_LUA)
            files = [shim_path] + files
        kim.run_lua(luajit, kim.DUMP_LUA, tmp, ENUM_FILES=kim._lua_list(kim.with_bom(enum_files, tmp, tag="enum")),
                    SCRIPT_FILES=kim._lua_list(files), DUMP_PATH=kim.lua_str(dump),
                    WANT=kim._lua_list(sorted(set(kim.ENUM_OF.values()))))
        missing = Path(dump + ".missing").read_text(errors="replace").split()
        items, sets = {}, {}
        with open(dump, "rb") as fp:
            for line in fp:
                kind, js = line.rstrip(b"\n").split(b"\t", 1)
                v = kim._unpack(json.loads(js.decode("ascii")))
                if v[0] != "t":
                    continue
                t = kim.LuaTable(v[1])
                if kind == b"I":
                    it = kim.read_item(t, None)
                    if it is not None:
                        items.setdefault(it["m_ItemID"], it)
                elif kind == b"S":
                    s = kim.read_set(t)
                    if s is not None:
                        sets.setdefault(s["m_SetID"], []).append(s)
    return items, sets, missing


def unknown_enum(it):
    """True when the item names an enum value our Enum.lua lacks (the shim made it -1)."""
    bad = (-1, 0xFFFFFFFF)
    if any(it.get(k) in bad for k in kim.ENUM_OF if k.startswith("m_")):
        return True
    if any(sa["TYPE"] in bad for sa in it.get("m_SpecialAbilityList", [])):
        return True
    return any(v in bad for v in it.get("BUFF_FACTOR", []))


def id_list(spec):
    ids = []
    for part in spec.split(","):
        a, _, b = part.partition("-")
        ids += range(int(a), int(b or a) + 1)
    return ids


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("items_dir", help="client_data folder holding Item.lua and SetItem.lua")
    ap.add_argument("--enum", required=True, action="append")
    ap.add_argument("--unit-type", action="append", default=[])
    ap.add_argument("--ids", action="append", default=[])
    ap.add_argument("--source", action="append", required=True)
    ap.add_argument("--text")
    ap.add_argument("--luajit")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    luajit = kim.find_luajit(args.luajit)
    en = kim.load_enums(luajit, args.enum)
    unit_types = {en.tables["UNIT_TYPE"][u] for u in args.unit_type}
    wanted = {i for spec in args.ids for i in id_list(spec)}
    item_lua, set_lua = Path(args.items_dir) / "Item.lua", Path(args.items_dir) / "SetItem.lua"

    ours, our_sets, _ = load(luajit, [str(item_lua), str(set_lua)], args.enum)
    print(f"ours: {len(ours)} items, {len(our_sets)} sets")
    texts, text_set_calls = {}, {}
    if args.text:
        kim.set_layout(kim.script_layout(args.text))
        text_sets = Path(args.text).with_name("SetItem.lua")
        texts, text_set_calls, _ = load(luajit, [args.text] + ([str(text_sets)] if text_sets.is_file() else []), args.enum)
        kim.set_layout("2014")
        print(f"text: {len(texts)} items from {args.text}")

    loaded = []
    for src in args.source:
        sets_src = Path(src).with_name("SetItem.lua")
        items, sets, missing = load(luajit, [src] + ([str(sets_src)] if sets_src.is_file() else []), args.enum)
        if missing:
            print(f"note: {src}: {len(missing)} enum names our Enum.lua lacks (their items are left out): "
                  f"{', '.join(missing[:12])}")
        loaded.append((src, items, sets))
    # text: --text first, then each source in order; never Korean when another has it
    text_chain = [texts] + [items for _, items, _ in loaded]
    set_chain = [sets for _, _, sets in loaded] + [text_set_calls]

    added, added_ids, added_sets, korean, skipped, no_set = [], set(), {}, [], 0, {}
    for src, items, _ in loaded:
        n = 0
        for iid in sorted(items):
            it = items[iid]
            if iid in ours or iid in added_ids:
                continue
            if iid not in wanted and it["m_UnitType"] not in unit_types:
                continue
            if unknown_enum(it):
                skipped += 1
                continue
            for k in TEXT_KEYS:
                for tx in text_chain:
                    t = tx.get(iid)
                    if t and t[k] and not HANGUL.search(t[k]):
                        it[k] = t[k]
                        break
            if any(HANGUL.search(it[k]) for k in TEXT_KEYS):
                korean.append(iid)      # no source has it in another language: never released in the US or Brazil
                continue
            added.append(it)
            added_ids.add(iid)
            n += 1
            sid = it["m_SetID"]
            if sid and sid not in our_sets and sid not in added_sets:
                found = next((sc[sid] for sc in set_chain if sid in sc), None)
                if found:
                    added_sets[sid] = found
                else:
                    no_set.setdefault(sid, []).append(iid)
                    it["m_SetID"] = 0   # CX2ItemManager::GetSetItem asserts on a set it does not have
        print(f"{src}: {n} items")
    if skipped:
        print(f"left out: {skipped} items naming enum values our Enum.lua lacks")
    if korean:
        print(f"left out: {len(korean)} items only in Korean (no other source has their text): {korean}")
    if no_set:
        print(f"note: {len(no_set)} sets in no SetItem.lua; their {sum(map(len, no_set.values()))} items get "
              f"m_SetID 0: {sorted(no_set)}")
    print(f"added: {len(added)} items, {len(added_sets)} sets")
    missing_ids = sorted(wanted - set(ours) - added_ids)
    if missing_ids:
        print(f"warning: {len(missing_ids)} --ids in no source: {missing_ids[:20]}")
    if not added or args.dry_run:
        return

    def append(path, text):
        data = path.read_bytes()
        crlf = b"\r\n" in data[:4096]
        if crlf:
            text = text.replace("\n", "\r\n")
        path.write_bytes(data + text.encode("utf-8", "surrogatepass"))

    append(item_lua, "".join(kim.item_to_lua(it, en) for it in added))
    if added_sets:
        out = []
        for sid in sorted(added_sets):
            for s in added_sets[sid]:
                L = ["%sm_SetID = %d," % (kim.IND, sid), "%sm_SetName = %s," % (kim.IND, kim.lua_str(s["m_SetName"]))]
                if s["m_NeedPartsNum"]:
                    L.append("%sm_NeedPartsNum = %d," % (kim.IND, s["m_NeedPartsNum"]))
                L += ["%sm_Option%d = %d," % (kim.IND, j, o) for j, o in enumerate(s["options"], 1)]
                out.append("g_pCX2SetItemManager:AddSetItemData_LUA\n{\n" + "\n".join(L) + "\n}\n\n")
        append(set_lua, "".join(out))
    # check: our scripts still compile into an Item.kim
    blob, n_items, n_sets = kim.scripts_to_kim(luajit, [str(item_lua), str(set_lua)], args.enum)
    print(f"wrote {item_lua} and {set_lua}; they compile to {n_items} items, {n_sets} sets ({len(blob)} bytes)")


if __name__ == "__main__":
    main()
