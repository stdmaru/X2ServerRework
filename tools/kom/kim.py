#!/usr/bin/env python3
"""kim.py - Elsword Item.kim <-> Item.lua / SetItem.lua, stdlib only.

Item.kim is what X2ScriptPreprocessingTool builds from Item.lua and
SetItem.lua (source: X2Lib/X2ItemManager_Preprocessing.cpp, KProxy2).  The
client loads it instead of interpreting the item scripts.

  decompile  Item.kim -> <dir>/Item.lua + <dir>/SetItem.lua
  compile    Item.lua SetItem.lua ... -> Item.kim (runs the scripts with the
             client's LuaJIT, then lays the data out exactly like KProxy2)

kim -> lua -> kim reproduces the original file byte for byte: the scripts are
written in the order the compiler originally processed them (recovered from
the offsets of each item's unshared data).

Format (version 0x1201, little endian, #pragma pack(1))
-------------------------------------------------------
  header   "KIM " u32 version, u32 numItems, u32 numSetIDs
  u32      itemID[numItems]                 sorted
  templet  KItemFormatTemplet[numItems]     188 bytes each, same order
  u32      setID[numSetIDs]                 sorted
  setdata  KItemFormatSetItemData[numSetIDs] 20 bytes each
  data     strings, vectors and lists referenced by offset (0 = none),
           appended in script order:
             string  u16 length, UTF-16LE chars, u16 0 (2-aligned); names
                     and descriptions are unique, every other string is
                     interned
             vector  3 x f32 (4-aligned, interned); (0,0,0) - or (1,1,1)
                     for scales - is stored as offset 0
             lists   u32 count + elements (4-aligned; buff factors use u16)
  The templet packs USE_TYPE/USE_CONDITION/ITEM_TYPE/ITEM_GRADE/PERIOD_TYPE/
  SHOP_PRICE_TYPE into 4-bit fields of m_dwFlags (bits 8..31) and the bools
  into bits 0..7; per-character frame offsets, scales, rotations, common
  model/XET names and slash trace colours go into an "extra" list of
  (type, offset-or-value) pairs.

Enum names: pass the client's Enum.lua (--enum, e.g. extracted from
data036.kom with kom.py) to get ITEM_TYPE["IT_WEAPON"] instead of 1 when
decompiling; compiling scripts that use those names needs the same file.
"""

import argparse
import json
import math
import os
import struct
import subprocess
import sys
import tempfile

BOM = b"\xef\xbb\xbf"
MAGIC = b"KIM "
VERSION = 0x1201
CHARS = ["", "Elsword", "Aisha", "Rena", "Raven", "Eve", "Chung", "Ara", "Elesis", "Add"]
CAPS = [c.upper() for c in CHARS]
D3DX_PI = struct.unpack("<f", struct.pack("<f", 3.141592654))[0]

TEMPLET_FMT = "<3I2ifiiiif30I3hH16B"
TEMPLET_SIZE = struct.calcsize(TEMPLET_FMT)
SET_FMT = "<IiIII"
SET_SIZE = struct.calcsize(SET_FMT)
SA_FMT = "<I4iI"
SA_SIZE = struct.calcsize(SA_FMT)
TF = ("ItemID Flags Flags2 ItemLevel Quantity RepairED RepairVP Price PricePvPPoint SetID AddMaxMP "
      "oName oDescription oDescriptionInShop oAttach0 oAttach1 oAttach2 oAttach3 oAttach4 oAttach5 "
      "oModel0 oModel1 oModel2 oModel3 oModel4 oModel5 oTextureChangeXETName oAniXETName oAniName "
      "oShopImage oDropViewer oDescriptionInSkillNote oStat oBuffFactor oSpecialAbility oSocket "
      "oParticle oMesh oExtra oRandomSocket oStatRelLV "
      "Endurance EnduranceDamageMin EnduranceDamageMax SummonNpc "
      "BuyPvpRank UseLevel UnitType UnitClass BuffID EqipPosition CoolTime MaxSealCount "
      "MaxAttribEnchantCount AttributeLevel nFrameOffsets nRenderScales nRenderRotates "
      "nCommonModelNames nCommonXETNames nSlashTraces").split()
assert TEMPLET_SIZE == 188 and len(TF) == 61

FLAG_BITS = [("m_bFashion", 0), ("m_bVested", 1), ("m_bCanEnchant", 2), ("m_bCanUseInventory", 3),
             ("m_bNoEquip", 4), ("m_bPcBang", 5), ("m_bCanHyperMode", 6), ("m_bHideSetDesc", 7)]
FLAG_FIELDS = [("m_UseType", 8), ("m_UseCondition", 12), ("m_ItemType", 16), ("m_ItemGrade", 20),
               ("m_PeriodType", 24), ("m_PriceType", 28)]
# scalar templet fields: lua key -> (templet field, default)
SCALARS = [("m_ItemLevel", "ItemLevel", 0), ("m_Quantity", "Quantity", 0), ("m_RepairED", "RepairED", 0.0),
           ("m_RepairVP", "RepairVP", 0), ("m_Price", "Price", 0), ("m_PricePvPPoint", "PricePvPPoint", 0),
           ("m_SetID", "SetID", 0), ("m_fAddMaxMP", "AddMaxMP", 0.0), ("m_Endurance", "Endurance", -1),
           ("m_EnduranceDamageMin", "EnduranceDamageMin", 0), ("m_EnduranceDamageMax", "EnduranceDamageMax", 0),
           ("m_Summon_Npc", "SummonNpc", 0), ("m_BuyPvpRankCondition", "BuyPvpRank", 0),
           ("m_UseLevel", "UseLevel", 0), ("m_UnitType", "UnitType", 0), ("m_UnitClass", "UnitClass", 0),
           ("m_Buff_ID", "BuffID", 0), ("m_EqipPosition", "EqipPosition", 1), ("m_CoolTime", "CoolTime", 0),
           ("m_ucMaxSealCount", "MaxSealCount", 0), ("m_iMaxAttribEnchantCount", "MaxAttribEnchantCount", 0),
           ("m_iAttributeLevel", "AttributeLevel", 0)]
FLOAT_KEYS = {"m_RepairED", "m_fAddMaxMP"}
STRINGS_NOMAP = ["m_Description", "m_DescriptionInShop", "m_DescriptionInSkillNote"]
STRINGS_MAP = ["m_ModelName", "m_ModelName1", "m_TextureChangeXETName", "m_AniXETName", "m_AniName",
               "m_ShopImage", "m_DropViewer", "m_AttachFrameName", "m_AttachFrameName1"]
STRING_FIELD = {"m_Description": "oDescription", "m_DescriptionInShop": "oDescriptionInShop",
                "m_DescriptionInSkillNote": "oDescriptionInSkillNote", "m_ModelName": "oModel0",
                "m_ModelName1": "oModel1", "m_TextureChangeXETName": "oTextureChangeXETName",
                "m_AniXETName": "oAniXETName", "m_AniName": "oAniName", "m_ShopImage": "oShopImage",
                "m_DropViewer": "oDropViewer", "m_AttachFrameName": "oAttach0", "m_AttachFrameName1": "oAttach1"}
STAT_KEYS = ["m_fBaseHP", "m_fAtkPhysic", "m_fAtkMagic", "m_fDefPhysic", "m_fDefMagic"]
STATREL_KEYS = ["m_byBaseHPRelLV", "m_byAtkPhysicRelLV", "m_byAtkMagicRelLV", "m_byDefPhysicRelLV",
                "m_byDefMagicRelLV"]
STT_KEYS = ["SLASH_TRACE_COLOR", "SLASH_TRACE_TIP_COLOR", "HYPER_SLASH_TRACE_COLOR", "HYPER_SLASH_TRACE_TIP_COLOR"]
ENUM_OF = {"m_ItemType": "ITEM_TYPE", "m_ItemGrade": "ITEM_GRADE", "m_UseType": "USE_TYPE",
           "m_UseCondition": "USE_CONDITION", "m_PeriodType": "PERIOD_TYPE", "m_PriceType": "SHOP_PRICE_TYPE",
           "m_UnitType": "UNIT_TYPE", "m_UnitClass": "UNIT_CLASS", "m_EqipPosition": "EQIP_POSITION",
           "m_BuyPvpRankCondition": "PVP_RANK", "m_Summon_Npc": "NPC_UNIT_ID", "TYPE": "SPECIAL_ABILITY_TYPE",
           "BUFF_FACTOR": "BUFF_FACTOR_ID"}


def f32(x):
    try:
        return struct.unpack("<f", struct.pack("<f", x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def c_int(x, bits=32, signed=True):
    """C cast of a double to an integer type (truncation, MSVC-style wrap)."""
    if x != x or x in (math.inf, -math.inf):
        v = -(1 << 63)
    else:
        v = int(x)
    v &= (1 << bits) - 1
    if signed and v >= 1 << (bits - 1):
        v -= 1 << bits
    return v


# ================================================================ binary

def _wstr(data, off):
    if off == 0:
        return ""
    n = struct.unpack_from("<H", data, off)[0]
    return data[off + 2:off + 2 + 2 * n].decode("utf-16-le", "surrogatepass")


def parse_kim(data):
    """Decode Item.kim into (items, sets).  Items come back in the order the
    compiler processed them; sets as one dict per set id."""
    if data[:4] != MAGIC:
        raise SystemExit("error: not an Item.kim file")
    _, version, nitems, nsets = struct.unpack_from("<4sIII", data, 0)
    if version != VERSION:
        raise SystemExit("error: unsupported KIM version 0x%x (expected 0x%x)" % (version, VERSION))
    ids = struct.unpack_from("<%dI" % nitems, data, 16)
    tbase = 16 + 4 * nitems
    sbase = tbase + TEMPLET_SIZE * nitems
    sdbase = sbase + 4 * nsets
    items = []
    for i in range(nitems):
        t = dict(zip(TF, struct.unpack_from(TEMPLET_FMT, data, tbase + i * TEMPLET_SIZE)))
        if t["ItemID"] != ids[i]:
            raise SystemExit("error: item %d (id %d) has no templet; file not produced by the compiler"
                             % (i, ids[i]))
        it = {"m_ItemID": t["ItemID"], "m_Name": _wstr(data, t["oName"])}
        for k, bit in FLAG_BITS:
            it[k] = (t["Flags"] >> bit) & 1
        for k, pos in FLAG_FIELDS:
            it[k] = (t["Flags"] >> pos) & 0xf
        it["m_bIsPvpItem"] = t["Flags2"] & 1
        if t["Flags2"] & ~1:
            raise SystemExit("error: item %d uses unknown m_dwFlags2 bits" % t["ItemID"])
        for k, f, _ in SCALARS:
            it[k] = t[f]
        for k, f in STRING_FIELD.items():
            it[k] = _wstr(data, t[f])
        if t["oStat"]:
            it["m_Stat"] = list(struct.unpack_from("<5f", data, t["oStat"]))
        if t["oStatRelLV"]:
            it["m_Stat_Relation_Level"] = list(struct.unpack_from("<5B", data, t["oStatRelLV"]))
        if t["oBuffFactor"]:
            n = struct.unpack_from("<H", data, t["oBuffFactor"])[0]
            it["BUFF_FACTOR"] = list(struct.unpack_from("<%dH" % n, data, t["oBuffFactor"] + 2))
        if t["oSpecialAbility"]:
            n = struct.unpack_from("<I", data, t["oSpecialAbility"])[0]
            sas = []
            for j in range(n):
                ty, ct, v1, v2, v3, so = struct.unpack_from(SA_FMT, data, t["oSpecialAbility"] + 4 + j * SA_SIZE)
                sas.append({"TYPE": ty, "COOL_TIME": ct, "VALUE1": v1, "VALUE2": v2, "VALUE3": v3,
                            "STRING_VALUE1": _wstr(data, so)})
            it["m_SpecialAbilityList"] = sas
        for key, ok in (("SOCKET_OPTION", "oSocket"), ("RANDOM_SOCKET_GROUP_ID", "oRandomSocket")):
            if t[ok]:
                n = struct.unpack_from("<I", data, t[ok])[0]
                it[key] = list(struct.unpack_from("<%di" % n, data, t[ok] + 4))
        for key, ok, nk in (("ATTACHED_PARTICLE", "oParticle", "PARTICLE_NAME"),
                            ("ATTACHED_MESH", "oMesh", "MESH_NAME")):
            if t[ok]:
                n = struct.unpack_from("<I", data, t[ok])[0]
                lst = []
                for j in range(n):
                    a, b = struct.unpack_from("<2I", data, t[ok] + 4 + 8 * j)
                    lst.append({nk: _wstr(data, a & 0x7fffffff), "BONE_NAME": _wstr(data, b)})
                it[key] = lst
        counts = [t["nFrameOffsets"], t["nRenderScales"], t["nRenderRotates"], t["nCommonModelNames"],
                  t["nCommonXETNames"], t["nSlashTraces"]]
        ex = [struct.unpack_from("<2I", data, t["oExtra"] + 8 * j) for j in range(sum(counts))] if t["oExtra"] else []
        groups, p = [], 0
        for n in counts:
            groups.append(ex[p:p + n])
            p += n
        vec = lambda o: list(struct.unpack_from("<3f", data, o))
        it["frame_offsets"] = {ty: vec(o) for ty, o in groups[0]}
        it["scales"] = {ty: vec(o) for ty, o in groups[1]}
        it["rotates"] = {ty: vec(o) for ty, o in groups[2]}
        it["common_models"] = {ty: _wstr(data, o) for ty, o in groups[3]}
        it["common_xets"] = {ty: _wstr(data, o) for ty, o in groups[4]}
        it["slash"] = {ty: v for ty, v in groups[5]}
        offs = [t[k] for k in ("oName", "oDescription", "oDescriptionInShop", "oDescriptionInSkillNote", "oStat",
                               "oStatRelLV", "oBuffFactor", "oSpecialAbility", "oSocket", "oRandomSocket",
                               "oParticle", "oMesh", "oExtra") if t[k]]
        if offs:
            it["_order"] = (min(offs), 0)
        else:
            # only shared strings/vectors: whatever this item added to the
            # data area was appended last, so it sits at its latest reference
            refs = [t[k] for k in TF if k.startswith("o") and t[k]]
            refs += [o for g in groups[:5] for _, o in g]
            it["_order"] = (max(refs), 1) if refs else None
        items.append(it)
    sets = []
    for i in range(nsets):
        sid, _maxlv, oname, oids, oopts = struct.unpack_from(SET_FMT, data, sdbase + i * SET_SIZE)
        opts = {}
        if oopts:
            n = struct.unpack_from("<I", data, oopts)[0]
            for j in range(n):
                need, opt = struct.unpack_from("<Ii", data, oopts + 4 + 8 * j)
                opts.setdefault(need, []).append(opt)
            if n == 0:
                opts[1] = []      # m_NeedPartsNum given without any m_Option
        sets.append({"m_SetID": sid, "m_SetName": _wstr(data, oname), "options": opts, "_order": oname})
    # items without any data keep their place in their set's item list
    setlists = {}
    for i in range(nsets):
        sid, _maxlv, oname, oids, oopts = struct.unpack_from(SET_FMT, data, sdbase + i * SET_SIZE)
        if oids:
            setlists[sid] = list(struct.unpack_from("<%dI" % struct.unpack_from("<I", data, oids)[0], data, oids + 4))
    byid = {it["m_ItemID"]: it for it in items}
    for it in items:
        if it["_order"] is None:
            prev = (0, -1)
            lst = setlists.get(it["m_SetID"], [])
            if it["m_ItemID"] in lst:
                for other in lst[:lst.index(it["m_ItemID"])]:
                    o = byid[other]["_order"]
                    if o is not None:
                        prev = max(prev, o)
            it["_order"] = (prev[0], prev[1] + 1) if prev != (0, -1) else (len(data), 9)
    items.sort(key=lambda it: it["_order"])
    sets.sort(key=lambda s: (s["_order"], s["m_SetID"]))
    # Names written by AddSetItemData_LUA calls that a later call of the same
    # set overrode are still in the data area, between the item data and the
    # set lists; keep them so the scripts rebuild the identical file.
    named = [s["_order"] for s in sets if s["_order"]]
    lists = [o for i in range(nsets) for o in struct.unpack_from(SET_FMT, data, sdbase + i * SET_SIZE)[3:] if o]
    orphans = []
    if named:
        o, end = min(named), (min(lists) if lists else len(data))
        used = set(named)
        while o + 2 <= end:
            n = struct.unpack_from("<H", data, o)[0]
            if o not in used:
                orphans.append((o, _wstr(data, o)))
            o += 4 + 2 * n
    sets_orphans = orphans
    return items, (sets, sets_orphans)


class _Writer(object):
    def __init__(self):
        self.buf = bytearray()
        self.wmap = {}
        self.vmap = {}

    def align(self, n):
        if len(self.buf) % n:
            self.buf.extend(bytes(n - len(self.buf) % n))

    def append(self, b):
        off = len(self.buf)
        self.buf.extend(b)
        return off

    def wstr(self, s, nomap):
        if not s:
            return 0
        self.align(2)
        if not nomap and s in self.wmap:
            return self.wmap[s]
        u = s.encode("utf-16-le", "surrogatepass")
        off = len(self.buf)
        if not nomap:
            self.wmap[s] = off
        self.buf.extend(struct.pack("<H", (len(u) // 2) & 0xffff) + u + b"\0\0")
        return off

    def vec(self, v, default_one):
        v = tuple(f32(x) for x in v)
        if v == ((1.0, 1.0, 1.0) if default_one else (0.0, 0.0, 0.0)):
            return 0
        if v in self.vmap:
            return self.vmap[v]
        self.align(4)
        off = self.append(struct.pack("<3f", *v))
        self.vmap[v] = off
        return off


def build_kim(items, setcalls, log=None):
    """Lay out Item.kim exactly like KProxy2.  items: item dicts in script
    order; setcalls: AddSetItemData_LUA calls in script order."""
    warnings = []
    log = log or warnings.append
    ids = sorted(set(it["m_ItemID"] for it in items))
    setids = sorted(set(s["m_SetID"] for s in setcalls))
    w = _Writer()
    w.buf.extend(struct.pack("<4sIII", MAGIC, VERSION, len(ids), len(setids)))
    w.buf.extend(struct.pack("<%dI" % len(ids), *ids))
    tbase = len(w.buf)
    w.buf.extend(bytes(TEMPLET_SIZE * len(ids)))
    w.buf.extend(struct.pack("<%dI" % len(setids), *setids))
    sdbase = len(w.buf)
    w.buf.extend(bytes(SET_SIZE * len(setids)))
    index = {iid: i for i, iid in enumerate(ids)}
    setdata = {}
    done = set()
    for it in items:
        iid = it["m_ItemID"]
        if iid in done:
            log("ITEMID_%d: DUPLICATED_ITEM_TEMPLET (ignored)" % iid)
            continue
        done.add(iid)
        t = dict.fromkeys(TF, 0)
        t["ItemID"] = iid
        t["oName"] = w.wstr(it["m_Name"], True)
        fl = 0
        for k, bit in FLAG_BITS:
            fl |= (1 << bit) if it[k] else 0
        for k, pos in FLAG_FIELDS:
            fl |= (it[k] & 0xf) << pos
        t["Flags"] = fl
        t["Flags2"] = 1 if it["m_bIsPvpItem"] else 0
        for k, f, _ in SCALARS:
            t[f] = it[k]
        for f in ("BuyPvpRank", "UseLevel", "UnitType", "UnitClass", "BuffID", "EqipPosition", "CoolTime",
                  "MaxSealCount", "MaxAttribEnchantCount", "AttributeLevel"):
            t[f] &= 0xff
        for f in ("Endurance", "EnduranceDamageMin", "EnduranceDamageMax"):
            t[f] = ((t[f] + 0x8000) & 0xffff) - 0x8000
        t["SummonNpc"] &= 0xffff
        # data is appended in the order AddItemTemplet_LUA reads the script
        for k in ("m_Description", "m_DescriptionInShop"):
            t[STRING_FIELD[k]] = w.wstr(it[k], True)
        for k in ("m_ModelName", "m_ModelName1", "m_TextureChangeXETName", "m_AniXETName", "m_AniName",
                  "m_ShopImage", "m_DropViewer", "m_AttachFrameName", "m_AttachFrameName1"):
            t[STRING_FIELD[k]] = w.wstr(it[k], False)
        extra = []
        for key, default_one, cnt in (("frame_offsets", False, "nFrameOffsets"), ("scales", True, "nRenderScales"),
                                      ("rotates", False, "nRenderRotates")):
            for i in range(1, len(CHARS)):
                o = w.vec(it[key].get(i, (1.0, 1.0, 1.0) if default_one else (0.0, 0.0, 0.0)), default_one)
                if o:
                    extra.append((i, o))
                    t[cnt] += 1
        t["oDescriptionInSkillNote"] = w.wstr(it["m_DescriptionInSkillNote"], True)
        if it["m_UseCondition"] == 1 and it["m_UseType"] != 0:
            mo, xo = {}, {}
            for i in range(1, len(CHARS)):
                if it["common_models"].get(i):
                    mo[i] = w.wstr(it["common_models"][i], False)
                    if it["common_xets"].get(i):
                        xo[i] = w.wstr(it["common_xets"][i], False)
            for i in sorted(mo):
                extra.append((i, mo[i]))
                t["nCommonModelNames"] += 1
            for i in sorted(xo):
                extra.append((i, xo[i]))
                t["nCommonXETNames"] += 1
        if "m_Stat" in it:
            w.align(4)
            t["oStat"] = w.append(struct.pack("<5f", *it["m_Stat"]))
        if "m_Stat_Relation_Level" in it:
            w.align(4)
            t["oStatRelLV"] = w.append(struct.pack("<5B", *[x & 0xff for x in it["m_Stat_Relation_Level"]]))
        bf = [x & 0xffff for x in it.get("BUFF_FACTOR", ()) if x]
        if bf:
            w.align(2)
            t["oBuffFactor"] = w.append(struct.pack("<H%dH" % len(bf), len(bf), *bf))
        if it.get("m_SpecialAbilityList"):
            recs = []
            for sa in it["m_SpecialAbilityList"]:
                sv = sa["STRING_VALUE1"]
                so = w.wstr(sv, False) if sv and sv != "Group 0" else 0
                recs.append(struct.pack(SA_FMT, sa["TYPE"] & 0xffffffff, sa["COOL_TIME"], sa["VALUE1"],
                                        sa["VALUE2"], sa["VALUE3"], so))
            w.align(4)
            t["oSpecialAbility"] = w.append(struct.pack("<I", len(recs)) + b"".join(recs))
        for key, ok in (("SOCKET_OPTION", "oSocket"), ("RANDOM_SOCKET_GROUP_ID", "oRandomSocket")):
            v = it.get(key)
            if v:
                w.align(4)
                t[ok] = w.append(struct.pack("<I%di" % len(v), len(v), *v))
        for key, ok, nk in (("ATTACHED_PARTICLE", "oParticle", "PARTICLE_NAME"),
                            ("ATTACHED_MESH", "oMesh", "MESH_NAME")):
            recs = []
            for e in it.get(key, ()):
                if e[nk] and e["BONE_NAME"]:
                    a = w.wstr(e[nk], False)
                    b = w.wstr(e["BONE_NAME"], False)
                    if key == "ATTACHED_MESH" and "Jiggle_Bone" in e[nk]:
                        a |= 0x80000000
                    recs.append(struct.pack("<2I", a, b))
            if recs:
                w.align(4)
                t[ok] = w.append(struct.pack("<I", len(recs)) + b"".join(recs))
        for ty in range(len(STT_KEYS)):
            if ty in it["slash"]:
                extra.append((ty, it["slash"][ty]))
                t["nSlashTraces"] += 1
        if extra:
            w.align(4)
            t["oExtra"] = w.append(b"".join(struct.pack("<2I", a, b) for a, b in extra))
        struct.pack_into(TEMPLET_FMT, w.buf, tbase + TEMPLET_SIZE * index[iid], *[t[k] for k in TF])
        if t["SetID"]:
            if t["SetID"] not in setids:
                log("ITEMID_%d: INVALID_SETID_%d" % (iid, t["SetID"]))
            sd = setdata.setdefault(t["SetID"], [0, [], {}])
            sd[1].append(iid)
            if 0 < t["UseLevel"] and sd[0] < t["UseLevel"]:
                sd[0] = t["UseLevel"]
    setrec = {}
    for s in setcalls:
        sid = s["m_SetID"]
        rec = setrec.setdefault(sid, [sid, 0, 0, 0, 0])
        rec[2] = w.wstr(s["m_SetName"], False)
        need = s["m_NeedPartsNum"]
        if need > 0:
            sd = setdata.setdefault(sid, [0, [], {}])
            if need not in sd[2]:
                sd[2][need] = [o for o in s["options"] if o != 0]
    for i, sid in enumerate(setids):
        rec = setrec[sid]
        sd = setdata.get(sid, [0, [], {}])
        rec[1] = sd[0]
        if sd[1]:
            w.align(4)
            rec[3] = w.append(struct.pack("<I%dI" % len(sd[1]), len(sd[1]), *sd[1]))
        if sd[2]:
            w.align(4)
            rec[4] = w.append(struct.pack("<I", sum(len(v) for v in sd[2].values())))
            for need in sorted(sd[2]):
                for o in sd[2][need]:
                    w.buf.extend(struct.pack("<Ii", need, o))
        struct.pack_into(SET_FMT, w.buf, sdbase + SET_SIZE * i, *rec)
    if warnings:
        print("warning: %d issue(s) the official tool also reports, e.g.:\n  %s"
              % (len(warnings), "\n  ".join(warnings[:5])), file=sys.stderr)
    return bytes(w.buf)


# ================================================================ Lua output

def lua_str(s):
    out = ['"']
    for ch in s:
        o = ord(ch)
        if ch == '"':
            out.append('\\"')
        elif ch == "\\":
            out.append("\\\\")
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\r":
            out.append("\\r")
        elif ch == "\t":
            out.append("\\t")
        elif o < 0x20 or o == 0x7f:
            out.append("\\%03d" % o)
        else:
            out.append(ch)
    out.append('"')
    return "".join(out)


def num_f32(x):
    """Shortest decimal that reads back as the same float32."""
    if x == int(x) and abs(x) < 1e15:
        return str(int(x)) if not (x == 0 and math.copysign(1, x) < 0) else "-0"
    for p in range(1, 18):
        s = "%.*g" % (p, x)
        if f32(float(s)) == x:
            return s
    return repr(x)


def _fixed(x, ok):
    """Shortest fixed-point decimal (no exponent) accepted by ok()."""
    for p in range(0, 20):
        s = ("%.*f" % (p, x))
        if "." in s:
            s = s.rstrip("0").rstrip(".")
        if s == "-0":
            s = "0" if math.copysign(1, x) > 0 else s
        if ok(s):
            return s
    raise ValueError("cannot represent %r" % x)


def _rot_of(s):
    return f32((float(s) / 180.0) * D3DX_PI)


def scale_rotate_string(scale, rot):
    sc = [_fixed(v, lambda s, v=v: f32(float(s)) == v) for v in scale]
    rt = [_fixed(v * 180.0 / D3DX_PI, lambda s, v=v: _rot_of(s) == v) for v in rot]
    return "SX:%s SY:%s SZ:%s RX:%s RY:%s RZ:%s" % tuple(sc + rt)


class Enums(object):
    def __init__(self, tables=None):
        self.tables = tables or {}
        self.rev = {}
        for name, kv in self.tables.items():
            rev = {}
            for k in sorted(kv):
                rev.setdefault(kv[k], k)
            self.rev[name] = rev
        self.has_bool = "True" in self.tables.get("_G", {})

    def val(self, key, v):
        en = ENUM_OF.get(key)
        if en and v in self.rev.get(en, {}):
            return '%s["%s"]' % (en, self.rev[en][v])
        return str(v)

    def boolean(self, v):
        if self.has_bool:
            return "True" if v else "False"
        return "1" if v else "0"


IND = "       "          # Item.lua / SetItem.lua indent (7 spaces)
# per-character keys follow this order in the original scripts
CHAR_ORDER = [1, 2, 3, 4, 8, 5, 6, 7, 9]
DEFAULTS = {"m_UseCondition": 1, "m_ItemGrade": 4, "m_PriceType": 2, "m_EqipPosition": 1, "m_Endurance": -1}


def _scalar(it, en, k):
    v = it[k]
    if k in ENUM_OF:
        return en.val(k, v)
    if k in FLOAT_KEYS:
        return num_f32(v)
    if k.startswith("m_b"):
        return en.boolean(v)
    return str(v)


def _block(key, lines, ind=IND):
    return ["%s%s =" % (ind, key), ind + "{"] + lines + [ind + "},"]


def item_to_lua(it, en):
    """One AddItemTemplet call in the layout of the original Item.lua."""
    L = []
    I2 = IND * 2
    I3 = IND * 3

    def scalar(k, always=False):
        if always or it[k] != DEFAULTS.get(k, 0):
            L.append("%s%s = %s," % (IND, k, _scalar(it, en, k)))

    def string(k, always=False):
        if always or it[k]:
            L.append("%s%s = %s," % (IND, k, lua_str(it[k])))

    scalar("m_ItemID", True)
    string("m_Name", True)
    for k in ("m_Description", "m_DescriptionInShop", "m_DescriptionInSkillNote"):
        string(k)
    L.append("")
    for k in ("m_ModelName", "m_ModelName1", "m_TextureChangeXETName", "m_AniXETName", "m_AniName"):
        string(k)
    scalar("m_UseType")
    for k in ("m_AttachFrameName", "m_AttachFrameName1"):
        string(k)
    L.append("")
    string("m_ShopImage")
    string("m_DropViewer")
    L.append("")
    for k in ("m_BuyPvpRankCondition", "m_UseCondition", "m_UnitType", "m_UnitClass", "m_UseLevel", "m_ItemLevel"):
        scalar(k)
    L.append("")
    scalar("m_ItemType", True)
    for k in ("m_ItemGrade", "m_bFashion", "m_bPcBang", "m_bVested", "m_bCanEnchant", "m_fAddMaxMP",
              "m_bCanUseInventory", "m_bNoEquip", "m_bHideSetDesc", "m_bIsPvpItem", "m_ucMaxSealCount",
              "m_iMaxAttribEnchantCount", "m_iAttributeLevel"):
        scalar(k)
    L.append("")
    for i in CHAR_ORDER:
        if i in it["frame_offsets"]:
            for axis, v in zip("XYZ", it["frame_offsets"][i]):
                if v != 0.0:
                    L.append("%sm_FrameOffset%s%s = %s," % (IND, CHARS[i], axis, num_f32(v)))
        if i in it["scales"] or i in it["rotates"]:
            L.append("%s%s_ITEM_SCALE_ROTATE = %s," % (IND, CAPS[i], lua_str(scale_rotate_string(
                it["scales"].get(i, (1.0, 1.0, 1.0)), it["rotates"].get(i, (0.0, 0.0, 0.0))))))
    for k in ("m_PeriodType", "m_bCanHyperMode"):
        scalar(k)
    scalar("m_Endurance", True)
    for k in ("m_Quantity", "m_EnduranceDamageMin", "m_EnduranceDamageMax", "m_RepairED"):
        scalar(k)
    L.append("")
    for k in ("m_PriceType", "m_Price", "m_PricePvPPoint"):
        scalar(k)
    L.append("")
    scalar("m_EqipPosition")
    L.append("")
    if "m_Stat_Relation_Level" in it:
        L += _block("m_Stat_Relation_Level", ["%s%s = %d," % (I2, k, v) for k, v in
                                              zip(STATREL_KEYS, it["m_Stat_Relation_Level"]) if v])
    scalar("m_Buff_ID")
    for ty in sorted(it["slash"]):
        L.append("%s%s = { %s }," % (IND, STT_KEYS[ty], ", ".join(map(str, _color_rgba(it["slash"][ty])))))
    if "m_Stat" in it:
        L += _block("m_Stat", ["%s%s = %s," % (I2, k, num_f32(v)) for k, v in zip(STAT_KEYS, it["m_Stat"]) if v])
    if it.get("m_SpecialAbilityList"):
        sal = []
        for sa in it["m_SpecialAbilityList"]:
            sal.append(I2 + "{")
            sal.append("%sTYPE = %s," % (I3, en.val("TYPE", sa["TYPE"])))
            for k in ("COOL_TIME", "VALUE1", "VALUE2", "VALUE3"):
                if sa[k]:
                    sal.append("%s%s = %d," % (I3, k, sa[k]))
            if sa["STRING_VALUE1"]:
                sal.append("%sSTRING_VALUE1 = %s," % (I3, lua_str(sa["STRING_VALUE1"])))
            sal.append(I2 + "},")
        L += _block("m_SpecialAbilityList", sal)
    for key in ("SOCKET_OPTION", "RANDOM_SOCKET_GROUP_ID"):
        if it.get(key):
            L += _block(key, ["%s%d, " % (I2, v) for v in it[key]])
    if it.get("ATTACHED_PARTICLE"):
        L += _block("ATTACHED_PARTICLE", sum(([I2 + "{ ", "%sPARTICLE_NAME = %s," % (I3, lua_str(e["PARTICLE_NAME"])),
                                               "%sBONE_NAME = %s," % (I3, lua_str(e["BONE_NAME"])), I2 + "},"]
                                              for e in it["ATTACHED_PARTICLE"]), []))
    scalar("m_CoolTime")
    if it.get("BUFF_FACTOR"):
        L += _block("BUFF_FACTOR", ["%s%s," % (I2, en.val("BUFF_FACTOR", v)) for v in it["BUFF_FACTOR"]])
    if it.get("ATTACHED_MESH"):
        L += _block("ATTACHED_MESH", sum(([I2 + "{ ", "%sMESH_NAME = %s," % (I3, lua_str(e["MESH_NAME"])),
                                           "%sBONE_NAME = %s," % (I3, lua_str(e["BONE_NAME"])), I2 + "},"]
                                          for e in it["ATTACHED_MESH"]), []))
    scalar("m_SetID")
    scalar("m_Summon_Npc")
    if it["common_models"]:
        L.append("")
        for i in CHAR_ORDER:
            if i in it["common_models"]:
                L.append("%s%s_MODEL_NAME = %s," % (IND, CAPS[i], lua_str(it["common_models"][i])))
                if i in it["common_xets"]:
                    L.append("%s%s_XET_NAME = %s," % (IND, CAPS[i], lua_str(it["common_xets"][i])))
    # collapse runs of blank lines
    body = []
    for line in L:
        if line == "" and (not body or body[-1] == ""):
            continue
        body.append(line)
    while body and body[-1] == "":
        body.pop()
    return "g_pItemManager:AddItemTemplet\n{\n" + "\n".join(body) + "\n}\n\n\n"


def _color_rgba(v):
    return [(v >> 16) & 0xff, (v >> 8) & 0xff, v & 0xff, (v >> 24) & 0xff]


def _name_call(sid, name, comment=""):
    return "g_pCX2SetItemManager:AddSetItemData_LUA\n{\n%sm_SetID = %d,\n%sm_SetName = %s,%s\n}\n\n" % (
        IND, sid, IND, lua_str(name), comment)


def sets_to_lua(sets_and_orphans):
    sets, orphans = sets_and_orphans
    events = sorted([(s["_order"], 1, i) for i, s in enumerate(sets)] + [(o, 0, name) for o, name in orphans],
                    key=lambda e: (e[0], e[1]))
    out, prev, pending = [], None, []
    note = "         -- overridden below; kept so Item.kim rebuilds identically"
    for _, kind, x in events:
        if kind == 0:
            if prev is not None:
                out.append(_name_call(prev["m_SetID"], x, note))
                out.append(_name_call(prev["m_SetID"], prev["m_SetName"]))
            else:
                pending.append(x)
            continue
        s = sets[x]
        for name in pending:
            out.append(_name_call(s["m_SetID"], name, note))
        pending = []
        prev = s
        groups = sorted(s["options"].items()) or [(0, [])]
        for need, opts in groups:
            L = ["%sm_SetID = %d," % (IND, s["m_SetID"]), "%sm_SetName = %s," % (IND, lua_str(s["m_SetName"]))]
            if need:
                L.append("%sm_NeedPartsNum = %d," % (IND, need))
            for j, o in enumerate(opts, 1):
                L.append("%sm_Option%d = %d," % (IND, j, o))
            out.append("g_pCX2SetItemManager:AddSetItemData_LUA\n{\n" + "\n".join(L) + "\n}\n\n")
    return "".join(out)


# ================================================================ Lua input

DUMP_LUA = r"""
local out = assert(io.open(DUMP_PATH, "wb"))
local function sesc(s)
  return '"' .. s:gsub('[%z\1-\31"\\\128-\255]', function(c) return string.format("\\u%04x", c:byte()) end) .. '"'
end
local ser
ser = function(v, depth)
  local tv = type(v)
  if tv == "number" then
    return '["n",' .. sesc(string.format("%.17g", v)) .. ',' .. sesc(tostring(v)) .. ']'
  elseif tv == "string" then
    return '["s",' .. sesc(v) .. ']'
  elseif tv == "boolean" then
    return '["b",' .. (v and "true" or "false") .. ']'
  elseif tv == "table" and depth < 4 then
    local parts = {}
    for k, x in pairs(v) do
      local ks
      if type(k) == "number" then ks = sesc("#" .. string.format("%.17g", k))
      elseif type(k) == "string" then ks = sesc("s" .. k) end
      if ks then parts[#parts + 1] = ks .. ':' .. ser(x, depth + 1) end
    end
    return '["t",{' .. table.concat(parts, ",") .. '}]'
  end
  return '["x"]'
end
local function rec(kind)
  return function(self, t)
    if type(t) ~= "table" then t = self end
    out:write(kind, "\t", ser(t, 0), "\n")
  end
end
local mgr = { AddItemTemplet = rec("I"), AddSetItemData_LUA = rec("S"),
              AddItemTempletTrans = rec("T"), AddSetItemDataTrans_LUA = rec("U") }
for _, f in ipairs(ENUM_FILES) do dofile(f) end
local missing, seen = assert(io.open(DUMP_PATH .. ".missing", "wb")), {}
for _, name in ipairs(WANT) do
  local t = rawget(_G, name)
  if type(t) == "table" and getmetatable(t) == nil then
    setmetatable(t, { __index = function(_, k)
      local key = name .. '["' .. tostring(k) .. '"]'
      if not seen[key] then seen[key] = true; missing:write(key, "\n") end
    end })
  end
end
if TRUE == nil then TRUE = 1 end
if FALSE == nil then FALSE = 0 end
if True == nil then True = 1 end
if False == nil then False = 0 end
g_pItemManager = mgr
g_pCX2SetItemManager = mgr
for _, f in ipairs(SCRIPT_FILES) do dofile(f) end
out:close()
missing:close()
"""

ENUM_DUMP_LUA = r"""
for _, f in ipairs(ENUM_FILES) do dofile(f) end
local out = assert(io.open(DUMP_PATH, "wb"))
for _, name in ipairs(WANT) do
  local t = _G[name]
  if type(t) == "table" then
    for k, v in pairs(t) do
      if type(k) == "string" and type(v) == "number" then out:write(name, "\t", k, "\t", string.format("%.17g", v), "\n") end
    end
  end
end
if type(True) == "number" then out:write("_G\tTrue\t", True, "\n") end
out:close()
"""


def find_luajit(explicit=None):
    here = os.path.dirname(os.path.abspath(__file__))
    for cand in (explicit, os.environ.get("KOM_LUAJIT"), os.path.join(here, "luajit-kog", "luajit")):
        if cand and os.path.isfile(cand) and os.access(cand, os.X_OK):
            return cand
    raise SystemExit("error: the client LuaJIT was not found (luajit-kog/luajit, --luajit or $KOM_LUAJIT)")


def _lua_list(paths):
    return "{" + ",".join(lua_str(p) for p in paths) + "}"


def run_lua(luajit, body, tmp, **globs):
    """Run a Lua chunk with the client LuaJIT (files need a UTF-8 BOM)."""
    head = "".join("%s = %s\n" % (k, v) for k, v in globs.items())
    drv = os.path.join(tmp, "driver.lua")
    with open(drv, "wb") as fp:
        fp.write(BOM + (head + body).encode("utf-8"))
    r = subprocess.run([luajit, drv], capture_output=True, text=True)
    if r.returncode != 0:
        raise SystemExit("error: Lua failed:\n" + (r.stderr or r.stdout).strip())


CHUNK_STATEMENTS = 1000
LUA51_ESCAPES = b"abfnrtv\\\"'\n\r0123456789"


def lua51_escapes(src):
    """Lua 5.1 (used by X2ScriptPreprocessingTool) reads an unknown escape
    such as "\\R" as the plain character; LuaJIT rejects it.  Rewrite those
    escapes (and LuaJIT-only \\x / \\z) inside string literals."""
    if b"\\" not in src:
        return src
    out = bytearray()
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == 0x2d and src[i:i + 2] == b"--":               # comment
            j = i + 2
            if src[j:j + 1] == b"[":
                k = j + 1
                while k < n and src[k] == 0x3d:
                    k += 1
                if k < n and src[k] == 0x5b:
                    end = src.find(b"]" + b"=" * (k - j - 1) + b"]", k)
                    end = n if end < 0 else end + k - j + 1
                    out += src[i:end]
                    i = end
                    continue
            end = src.find(b"\n", i)
            end = n if end < 0 else end
            out += src[i:end]
            i = end
        elif c == 0x5b and src[i + 1:i + 2] in (b"[", b"="):     # long string
            k = i + 1
            while k < n and src[k] == 0x3d:
                k += 1
            if k < n and src[k] == 0x5b:
                end = src.find(b"]" + b"=" * (k - i - 1) + b"]", k)
                end = n if end < 0 else end + k - i + 1
                out += src[i:end]
                i = end
            else:
                out.append(c)
                i += 1
        elif c in (0x22, 0x27):                                   # quoted string
            out.append(c)
            i += 1
            while i < n and src[i] != c and src[i] != 0x0a:
                if src[i] == 0x5c and i + 1 < n:
                    if src[i + 1] in LUA51_ESCAPES:
                        out += src[i:i + 2]
                    else:
                        out.append(src[i + 1])
                    i += 2
                    continue
                out.append(src[i])
                i += 1
            if i < n:
                out.append(src[i])
                i += 1
        else:
            out.append(c)
            i += 1
    return bytes(out)


def with_bom(paths, tmp, split=False, tag="in"):
    """Return paths readable by the client LuaJIT (UTF-8 text needs a BOM).
    With split, big item scripts are cut into pieces at the top-level
    manager calls: one LuaJIT chunk holds at most 65536 constants."""
    out = []
    for i, p in enumerate(paths):
        with open(p, "rb") as fp:
            d = fp.read()
        if d.startswith(b"\x1b"):
            out.append(os.path.abspath(p))
            continue
        if d.startswith(BOM):
            d = d[3:]
        d = lua51_escapes(d)
        pieces = [d]
        if split and len(d) > 1 << 20:
            pieces, cur, n = [], [], 0
            for line in d.splitlines(True):
                if line.startswith((b"g_pItemManager", b"g_pCX2SetItemManager")):
                    n += 1
                    if n > CHUNK_STATEMENTS:
                        pieces.append(b"".join(cur))
                        cur, n = [], 1
                cur.append(line)
            pieces.append(b"".join(cur))
        for j, piece in enumerate(pieces):
            q = os.path.join(tmp, "%s%d_%d.lua" % (tag, i, j))
            with open(q, "wb") as fp:
                fp.write(BOM + piece)
            out.append(q)
    return out


def load_enums(luajit, enum_files):
    if not enum_files:
        return Enums()
    with tempfile.TemporaryDirectory() as tmp:
        dump = os.path.join(tmp, "enum.txt")
        want = sorted(set(ENUM_OF.values()))
        run_lua(luajit, ENUM_DUMP_LUA, tmp, ENUM_FILES=_lua_list(with_bom(enum_files, tmp, tag="enum")),
                DUMP_PATH=lua_str(dump), WANT=_lua_list(want))
        tables = {}
        with open(dump, encoding="utf-8", errors="replace") as fp:
            for line in fp:
                name, k, v = line.rstrip("\n").split("\t")
                v = float(v)
                tables.setdefault(name, {})[k] = int(v) if v == int(v) else v
    return Enums(tables)


# -- KLuaManager semantics on dumped values

def _lua_tonumber(s):
    s = s.strip()
    try:
        if s.lower().startswith(("0x", "-0x", "+0x")):
            return float(int(s, 16))
        return float(s)
    except ValueError:
        return None


def _unpack(v):
    """JSON dump -> python value: ('n', float, str) / ('s', bytes) / ('t', dict) / ..."""
    tag = v[0]
    if tag == "n":
        return ("n", float(v[1]), v[2].encode("latin-1"))
    if tag == "s":
        return ("s", v[1].encode("latin-1"))
    if tag == "t":
        d = {}
        for k, x in v[1].items():
            kb = k.encode("latin-1")
            d[(float(kb[1:]) if kb[:1] == b"#" else kb[1:].decode("latin-1"))] = _unpack(x)
        return ("t", d)
    return (tag,)


class LuaTable(object):
    def __init__(self, d):
        self.d = d

    def raw(self, key):
        return self.d.get(key)

    def number(self, key):
        v = self.d.get(key)
        if v is None:
            return None
        if v[0] == "n":
            return v[1]
        if v[0] == "s":
            return _lua_tonumber(v[1].decode("latin-1"))
        return None

    def int(self, key, default, bits=32, signed=True):
        n = self.number(key)
        return default if n is None else c_int(n, bits, signed)

    def float(self, key, default):
        n = self.number(key)
        return default if n is None else f32(n)

    def bool(self, key):
        n = self.number(key)
        return 0 if n is None else (1 if c_int(n) != 0 else 0)

    def has(self, key, kind):
        v = self.d.get(key)
        if v is None:
            return False
        if kind == "number":
            return self.number(key) is not None
        if kind == "string":
            return v[0] in ("s", "n")
        return v[0] == "t"

    def bytes(self, key):
        v = self.d.get(key)
        if v is None:
            return None
        if v[0] == "s":
            return v[1]
        if v[0] == "n":
            return v[2]
        return None

    def wstr(self, key, default=""):
        b = self.bytes(key)
        # MultiByteToWideChar(CP_UTF8): invalid sequences become U+FFFD
        return default if b is None else b.decode("utf-8", "replace")

    def table(self, key):
        v = self.d.get(key)
        return LuaTable(v[1]) if v is not None and v[0] == "t" else None

    def seq(self, getter):
        out, i = [], 1
        while True:
            v = getter(float(i))
            if v is None:
                return out
            out.append(v)
            i += 1


def tokenize_scale_rotate(s):
    """CX2ItemManager::TokenizeByScaleRotate."""
    scale, rot = [1.0, 1.0, 1.0], [0.0, 0.0, 0.0]
    if len(s) < 19:
        return scale, rot
    tok = [""] * 6
    cur = ""
    for ch in s:
        if ch in "RSXYZ":
            cur += ch
        elif ch == " ":
            cur = ""
        elif ch.isdigit() or ch in ".-":
            names = ["SX", "SY", "SZ", "RX", "RY", "RZ"]
            if cur in names:
                tok[names.index(cur)] += ch

    def atof(t):
        m = ""
        for i, ch in enumerate(t):
            cand = m + ch
            if _lua_tonumber(cand + ("0" if cand.endswith((".", "-")) else "")) is None:
                break
            m = cand
        try:
            return float(m)
        except ValueError:
            try:
                return float(m + "0")
            except ValueError:
                return 0.0
    for i in range(3):
        if tok[i]:
            scale[i] = f32(atof(tok[i]))
        if tok[3 + i]:
            rot[i] = f32((atof(tok[3 + i]) / 180.0) * D3DX_PI)
    return scale, rot


def read_item(t, log):
    """KProxy2::AddItemTemplet_LUA: returns an item dict or None."""
    uc = t.int("m_UseCondition", 1)
    if uc == 0:
        return None
    if not t.has("m_ItemID", "number") or not t.has("m_Name", "string") or not t.has("m_ItemType", "number"):
        return None
    it = {"m_ItemID": t.int("m_ItemID", 0, signed=False), "m_Name": t.wstr("m_Name"),
          "m_ItemType": t.int("m_ItemType", 0), "m_UseCondition": uc}
    for k in ("m_UseType", "m_PeriodType"):
        it[k] = t.int(k, 0)
    it["m_ItemGrade"] = t.int("m_ItemGrade", 4)
    it["m_PriceType"] = t.int("m_PriceType", 2)
    for k, bit in FLAG_BITS:
        it[k] = t.bool(k)
    it["m_bIsPvpItem"] = t.bool("m_bIsPvpItem")
    for k, f, d in SCALARS:
        it[k] = t.float(k, d) if k in FLOAT_KEYS else t.int(k, d)
    for k in STRINGS_NOMAP + STRINGS_MAP:
        it[k] = t.wstr(k)
    it["frame_offsets"], it["scales"], it["rotates"] = {}, {}, {}
    for i in range(1, len(CHARS)):
        v = [t.float("m_FrameOffset%s%s" % (CHARS[i], a), 0.0) for a in "XYZ"]
        if v != [0.0, 0.0, 0.0]:
            it["frame_offsets"][i] = v
        b = t.bytes("%s_ITEM_SCALE_ROTATE" % CAPS[i])
        sc, rt = tokenize_scale_rotate(b.decode("utf-8", "replace") if b else "")
        if sc != [1.0, 1.0, 1.0]:
            it["scales"][i] = sc
        if rt != [0.0, 0.0, 0.0]:
            it["rotates"][i] = rt
    it["common_models"], it["common_xets"] = {}, {}
    for i in range(1, len(CHARS)):
        m = t.wstr("%s_MODEL_NAME" % CAPS[i])
        if m:
            it["common_models"][i] = m
            x = t.wstr("%s_XET_NAME" % CAPS[i])
            if x:
                it["common_xets"][i] = x
    st = t.table("m_Stat")
    if st is not None:
        it["m_Stat"] = [st.float(k, 0.0) for k in STAT_KEYS]
    sr = t.table("m_Stat_Relation_Level")
    if sr is not None:
        it["m_Stat_Relation_Level"] = [sr.int(k, 0) & 0xff for k in STATREL_KEYS]
    bf = t.table("BUFF_FACTOR")
    if bf is not None:
        it["BUFF_FACTOR"] = bf.seq(lambda i: None if bf.number(i) is None else c_int(bf.number(i), signed=False))
    sal = t.table("m_SpecialAbilityList")
    if sal is not None:
        sas = []
        for sa in sal.seq(sal.table):
            sv = sa.wstr("STRING_VALUE1", "Group 0")
            sas.append({"TYPE": sa.int("TYPE", 0), "COOL_TIME": sa.int("COOL_TIME", 0), "VALUE1": sa.int("VALUE1", 0),
                        "VALUE2": sa.int("VALUE2", 0), "VALUE3": sa.int("VALUE3", 0), "STRING_VALUE1": sv})
        it["m_SpecialAbilityList"] = sas
    for key in ("SOCKET_OPTION", "RANDOM_SOCKET_GROUP_ID"):
        so = t.table(key)
        if so is not None:
            it[key] = so.seq(lambda i, so=so: None if so.number(i) is None else c_int(so.number(i)))
    for key, nk in (("ATTACHED_PARTICLE", "PARTICLE_NAME"), ("ATTACHED_MESH", "MESH_NAME")):
        ap = t.table(key)
        if ap is not None:
            it[key] = [{nk: e.wstr(nk), "BONE_NAME": e.wstr("BONE_NAME")} for e in ap.seq(ap.table)]
    it["slash"] = {}
    for ty, key in enumerate(STT_KEYS):
        c = t.table(key)
        if c is not None:
            vals = [1.0, 0.0, 0.0, 0.0]
            for j in range(4):
                n = c.number(float(j + 1))
                if n is None:
                    break
                vals[j] = f32(f32(n) / f32(255.0))

            def comp(x):
                return 0xff if x >= 1.0 else 0 if x <= 0.0 else c_int(f32(f32(x * 255.0) + 0.5), signed=False)
            r, g, b, a = (comp(x) for x in vals)
            it["slash"][ty] = (a << 24) | (r << 16) | (g << 8) | b
    return it


def read_set(t):
    if not t.has("m_SetID", "number"):
        return None
    sid = t.int("m_SetID", 0, signed=False)
    if sid == 0:
        return None
    opts = []
    for i in range(1, 16):
        o = t.int("m_Option%d" % i, 0)
        if o:
            opts.append(o)
    return {"m_SetID": sid, "m_SetName": t.wstr("m_SetName"), "m_NeedPartsNum": t.int("m_NeedPartsNum", 0, signed=False),
            "options": opts}


def scripts_to_kim(luajit, scripts, enum_files):
    with tempfile.TemporaryDirectory() as tmp:
        dump = os.path.join(tmp, "dump.txt")
        run_lua(luajit, DUMP_LUA, tmp, ENUM_FILES=_lua_list(with_bom(enum_files, tmp, tag="enum")),
                SCRIPT_FILES=_lua_list(with_bom(scripts, tmp, split=True)), DUMP_PATH=lua_str(dump),
                WANT=_lua_list(sorted(set(ENUM_OF.values()))))
        with open(dump + ".missing", encoding="utf-8", errors="replace") as fp:
            missing = fp.read().split()
        if missing:
            print("warning: %d enum name(s) not defined by the --enum files (the field falls back to its "
                  "default, as in the official tool): %s" % (len(missing), ", ".join(missing[:10])), file=sys.stderr)
        items, setcalls, trans = [], [], 0
        with open(dump, "rb") as fp:
            for line in fp:
                kind, js = line.rstrip(b"\n").split(b"\t", 1)
                v = _unpack(json.loads(js.decode("ascii")))
                if v[0] != "t":
                    continue
                t = LuaTable(v[1])
                if kind == b"I":
                    it = read_item(t, None)
                    if it is not None:
                        items.append(it)
                elif kind == b"S":
                    s = read_set(t)
                    if s is not None:
                        setcalls.append(s)
                else:
                    trans += 1
    if trans:
        print("warning: %d AddItemTempletTrans/AddSetItemDataTrans_LUA calls ignored" % trans, file=sys.stderr)
    return build_kim(items, setcalls), len(items), len(set(s["m_SetID"] for s in setcalls))


# ================================================================ CLI

def cmd_decompile(args):
    with open(args.kim, "rb") as fp:
        items, sets = parse_kim(fp.read())
    nsets = len(sets[0])
    en = load_enums(find_luajit(args.luajit), args.enum) if args.enum else Enums()
    os.makedirs(args.out, exist_ok=True)
    note = ("-- lua header. UTF-8 인코딩 인식을 위해 이 줄은 지우지 마세요.\n"
            "-- Generated from Item.kim by kim.py%s\n\n\n" % (
                "; uses the enum names of Enum.lua (compile with --enum)." if args.enum else "."))
    crlf = lambda text: text.replace("\n", "\r\n").encode("utf-8", "surrogatepass")
    with open(os.path.join(args.out, "Item.lua"), "wb") as fp:
        fp.write(BOM + crlf(note))
        for it in items:
            fp.write(crlf(item_to_lua(it, en)))
    with open(os.path.join(args.out, "SetItem.lua"), "wb") as fp:
        fp.write(BOM + crlf(note) + crlf(sets_to_lua(sets)))
    print("%d items, %d sets -> %s/Item.lua, %s/SetItem.lua" % (len(items), nsets, args.out, args.out))


def cmd_compile(args):
    data, nitems, nsets = scripts_to_kim(find_luajit(args.luajit), args.scripts, args.enum or [])
    with open(args.output, "wb") as fp:
        fp.write(data)
    print("%s: %d items, %d sets, %d bytes" % (args.output, nitems, nsets, len(data)))


def cmd_info(args):
    with open(args.kim, "rb") as fp:
        d = fp.read()
    _, version, nitems, nsets = struct.unpack_from("<4sIII", d, 0)
    print("version 0x%x, %d items, %d sets, %d bytes" % (version, nitems, nsets, len(d)))


def main():
    ap = argparse.ArgumentParser(description="Elsword Item.kim <-> Item.lua/SetItem.lua")
    ap.add_argument("--luajit", help="client LuaJIT (default: luajit-kog/luajit next to this file)")
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("decompile", help="Item.kim -> Item.lua + SetItem.lua")
    p.add_argument("kim")
    p.add_argument("out", help="output directory")
    p.add_argument("--enum", action="append", help="Enum.lua for enum names (repeatable)")
    p.set_defaults(func=cmd_decompile)
    p = sub.add_parser("compile", help="Item.lua SetItem.lua ... -> Item.kim")
    p.add_argument("scripts", nargs="+", help="item scripts, in the order the tool was given them")
    p.add_argument("-o", "--output", required=True)
    p.add_argument("--enum", action="append", help="Enum.lua defining the enum names (repeatable)")
    p.set_defaults(func=cmd_compile)
    p = sub.add_parser("info", help="show the header")
    p.add_argument("kim")
    p.set_defaults(func=cmd_info)
    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
