// A port of tools/kom/kim.py (its reference: both write the same scripts and the same Item.kim). Text follows
// Python's str: code points, with lone UTF-16 surrogates kept (surrogatepass) and invalid UTF-8 read as U+FFFD.
#include "x2/lua/kim.h"

#include "x2/lua/bytecode.h"

#include "lua_state.h"

#include <algorithm>
#include <bit>
#include <cfloat>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <variant>

namespace x2::lua {

namespace {

using PyStr = std::u32string;  // a Python str: code points

struct KimError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

constexpr std::uint32_t kVersion = 0x1201;
constexpr std::size_t kTempletSize = 188, kSetSize = 20, kSaSize = 24;
const char* const kChars[] = {"", "Elsword", "Aisha", "Rena", "Raven", "Eve", "Chung", "Ara", "Elesis", "Add"};
constexpr int kNumChars = 10;
const char* const kCaps[] = {"", "ELSWORD", "AISHA", "RENA", "RAVEN", "EVE", "CHUNG", "ARA", "ELESIS", "ADD"};

// ---------------------------------------------------------------- numbers

// Python's f32(): through a float32, overflowing to infinity.
double F32(double x)
{
    const float y = static_cast<float>(x);
    if (std::isinf(y) && !std::isinf(x))
        return std::copysign(HUGE_VAL, x);
    return y;
}

const double kD3dxPi = F32(3.141592654);

// C cast of a double to an integer type (truncation, MSVC-style wrap), as kim.py's c_int.
std::int64_t CInt(double x, int bits = 32, bool isSigned = true)
{
    std::uint64_t u;
    if (!std::isfinite(x))
        u = 0x8000000000000000ull;
    else
    {
        const double t = std::trunc(x);
        const double m = std::fmod(t, 18446744073709551616.0);
        u = m >= 0 ? static_cast<std::uint64_t>(m) : 0 - static_cast<std::uint64_t>(-m);
    }
    if (bits < 64)
        u &= (std::uint64_t{1} << bits) - 1;
    if (isSigned && bits < 64 && u >= (std::uint64_t{1} << (bits - 1)))
        return static_cast<std::int64_t>(u) - (std::int64_t{1} << bits);
    return static_cast<std::int64_t>(u);
}

std::string Str(long long v) { return std::to_string(v); }

std::string Fmt(const char* fmt, int p, double x)
{
    char buf[512];
    std::snprintf(buf, sizeof buf, fmt, p, x);
    return buf;
}

double ParseDouble(const std::string& s)
{
    double v = 0;
    const char* b = s.data();
    if (!s.empty() && s[0] == '+')
        ++b;
    std::from_chars(b, s.data() + s.size(), v);
    return v;
}

// Python's repr(float) for the rare value %g cannot write in 17 digits (never in practice).
std::string Repr(double v)
{
    char buf[64];
    const auto r = std::to_chars(buf, buf + sizeof buf, v);
    return std::string(buf, r.ptr);
}

// Shortest decimal that reads back as the same float32.
std::string NumF32(double x)
{
    if (!std::isfinite(x))
        throw KimError("a non-finite number cannot be written to a script");
    if (x == std::trunc(x) && std::fabs(x) < 1e15)
        return (x == 0 && std::signbit(x)) ? "-0" : Str(static_cast<long long>(x));
    for (int p = 1; p < 18; ++p)
    {
        const std::string s = Fmt("%.*g", p, x);
        if (F32(ParseDouble(s)) == x)
            return s;
    }
    return Repr(x);
}

// Shortest fixed-point decimal (no exponent) accepted by ok().
template <class Ok>
std::string Fixed(double x, Ok ok)
{
    for (int p = 0; p < 20; ++p)
    {
        std::string s = Fmt("%.*f", p, x);
        if (s.find('.') != std::string::npos)
        {
            while (!s.empty() && s.back() == '0')
                s.pop_back();
            if (!s.empty() && s.back() == '.')
                s.pop_back();
        }
        if (s == "-0" && !std::signbit(x))
            s = "0";
        if (ok(s))
            return s;
    }
    throw KimError("cannot represent a scale or rotation");
}

double RotOf(const std::string& s) { return F32((ParseDouble(s) / 180.0) * kD3dxPi); }

// ---------------------------------------------------------------- text

PyStr DecodeUtf16(std::span<const std::uint8_t> b)
{
    PyStr out;
    const std::size_t n = b.size() / 2;
    for (std::size_t i = 0; i < n; ++i)
    {
        const char32_t u = b[2 * i] | (b[2 * i + 1] << 8);
        if (u >= 0xD800 && u <= 0xDBFF && i + 1 < n)
        {
            const char32_t l = b[2 * i + 2] | (b[2 * i + 3] << 8);
            if (l >= 0xDC00 && l <= 0xDFFF)
            {
                out += 0x10000 + ((u - 0xD800) << 10) + (l - 0xDC00);
                ++i;
                continue;
            }
        }
        out += u;
    }
    return out;
}

std::u16string EncodeUtf16(const PyStr& s)
{
    std::u16string out;
    for (const char32_t c : s)
        if (c >= 0x10000)
        {
            out += static_cast<char16_t>(0xD800 + ((c - 0x10000) >> 10));
            out += static_cast<char16_t>(0xDC00 + ((c - 0x10000) & 0x3FF));
        }
        else
            out += static_cast<char16_t>(c);
    return out;
}

// UTF-8, writing lone surrogates as 3 bytes (Python's surrogatepass).
void AppendUtf8(std::string& out, char32_t c)
{
    if (c < 0x80)
        out += static_cast<char>(c);
    else if (c < 0x800)
    {
        out += static_cast<char>(0xC0 | (c >> 6));
        out += static_cast<char>(0x80 | (c & 0x3F));
    }
    else if (c < 0x10000)
    {
        out += static_cast<char>(0xE0 | (c >> 12));
        out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (c & 0x3F));
    }
    else
    {
        out += static_cast<char>(0xF0 | (c >> 18));
        out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (c & 0x3F));
    }
}

std::string ToUtf8(const PyStr& s)
{
    std::string out;
    for (const char32_t c : s)
        AppendUtf8(out, c);
    return out;
}

PyStr Ascii(std::string_view s) { return PyStr(s.begin(), s.end()); }

// UTF-8 -> code points, each maximal invalid subpart becoming U+FFFD (Python's "replace"; MultiByteToWideChar).
PyStr DecodeUtf8(std::string_view b)
{
    PyStr out;
    std::size_t i = 0;
    while (i < b.size())
    {
        const auto c = static_cast<unsigned char>(b[i]);
        if (c < 0x80)
        {
            out += c;
            ++i;
            continue;
        }
        const std::size_t n = (c >= 0xC2 && c <= 0xDF) ? 2 : (c >= 0xE0 && c <= 0xEF) ? 3 : (c >= 0xF0 && c <= 0xF4) ? 4 : 0;
        if (!n)
        {
            out += 0xFFFD;
            ++i;
            continue;
        }
        unsigned char lo = 0x80, hi = 0xBF;
        if (c == 0xE0)
            lo = 0xA0;
        else if (c == 0xED)
            hi = 0x9F;
        else if (c == 0xF0)
            lo = 0x90;
        else if (c == 0xF4)
            hi = 0x8F;
        char32_t cp = c & (n == 2 ? 0x1F : n == 3 ? 0x0F : 0x07);
        std::size_t k = 1;
        for (; k < n && i + k < b.size(); ++k)
        {
            const auto ck = static_cast<unsigned char>(b[i + k]);
            if (ck < (k == 1 ? lo : 0x80) || ck > (k == 1 ? hi : 0xBF))
                break;
            cp = (cp << 6) | (ck & 0x3F);
        }
        if (k == n)
            out += cp;
        else
            out += 0xFFFD;
        i += k;
    }
    return out;
}

std::string LuaStr(const PyStr& s)
{
    std::string out = "\"";
    for (const char32_t c : s)
    {
        if (c == '"')
            out += "\\\"";
        else if (c == '\\')
            out += "\\\\";
        else if (c == '\n')
            out += "\\n";
        else if (c == '\r')
            out += "\\r";
        else if (c == '\t')
            out += "\\t";
        else if (c < 0x20 || c == 0x7F)
        {
            char buf[8];
            std::snprintf(buf, sizeof buf, "\\%03d", static_cast<int>(c));
            out += buf;
        }
        else
            AppendUtf8(out, c);
    }
    return out + "\"";
}

std::string LuaStr(std::string_view ascii) { return LuaStr(Ascii(ascii)); }

// ---------------------------------------------------------------- binary

std::uint32_t U32(std::span<const std::uint8_t> d, std::size_t off)
{
    if (off > d.size() || d.size() - off < 4)
        throw KimError("Item.kim is truncated");
    return d[off] | (d[off + 1] << 8) | (d[off + 2] << 16) | (static_cast<std::uint32_t>(d[off + 3]) << 24);
}
std::uint16_t U16(std::span<const std::uint8_t> d, std::size_t off)
{
    if (off > d.size() || d.size() - off < 2)
        throw KimError("Item.kim is truncated");
    return static_cast<std::uint16_t>(d[off] | (d[off + 1] << 8));
}
std::int32_t I32(std::span<const std::uint8_t> d, std::size_t off) { return static_cast<std::int32_t>(U32(d, off)); }
std::int16_t I16(std::span<const std::uint8_t> d, std::size_t off) { return static_cast<std::int16_t>(U16(d, off)); }
double F(std::span<const std::uint8_t> d, std::size_t off) { return std::bit_cast<float>(U32(d, off)); }
std::uint8_t U8(std::span<const std::uint8_t> d, std::size_t off)
{
    if (off >= d.size())
        throw KimError("Item.kim is truncated");
    return d[off];
}

PyStr WStr(std::span<const std::uint8_t> d, std::size_t off)
{
    if (off == 0)
        return {};
    const std::size_t n = U16(d, off);
    const std::size_t at = off + 2;
    const std::size_t len = std::min(2 * n, d.size() > at ? d.size() - at : 0);
    return DecodeUtf16(d.subspan(std::min(at, d.size()), len));
}

// Templet fields (KItemFormatTemplet, #pragma pack(1)), in order.
enum Tf
{
    ItemID, Flags, Flags2, ItemLevel, Quantity, RepairED, RepairVP, Price, PricePvPPoint, SetID, AddMaxMP,
    oName, oDescription, oDescriptionInShop, oAttach0, oAttach1, oAttach2, oAttach3, oAttach4, oAttach5,
    oModel0, oModel1, oModel2, oModel3, oModel4, oModel5, oTextureChangeXETName, oAniXETName, oAniName,
    oShopImage, oDropViewer, oDescriptionInSkillNote, oStat, oBuffFactor, oSpecialAbility, oSocket,
    oParticle, oMesh, oExtra, oRandomSocket, oStatRelLV,
    Endurance, EnduranceDamageMin, EnduranceDamageMax, SummonNpc,
    BuyPvpRank, UseLevel, UnitType, UnitClass, BuffID, EqipPosition, CoolTime, MaxSealCount,
    MaxAttribEnchantCount, AttributeLevel, nFrameOffsets, nRenderScales, nRenderRotates,
    nCommonModelNames, nCommonXETNames, nSlashTraces,
    kNumTf
};

// Field type: I u32, i i32, f float, h i16, H u16, B u8.
char TfType(int f)
{
    if (f <= Flags2 || (f >= oName && f <= oStatRelLV))
        return 'I';
    if (f == RepairED || f == AddMaxMP)
        return 'f';
    if (f < oName)
        return 'i';
    if (f <= EnduranceDamageMax)
        return 'h';
    if (f == SummonNpc)
        return 'H';
    return 'B';
}

std::array<double, kNumTf> ReadTemplet(std::span<const std::uint8_t> d, std::size_t off)
{
    std::array<double, kNumTf> t{};
    std::size_t p = off;
    for (int f = 0; f < kNumTf; ++f)
        switch (TfType(f))
        {
        case 'I': t[f] = U32(d, p); p += 4; break;
        case 'i': t[f] = I32(d, p); p += 4; break;
        case 'f': t[f] = F(d, p); p += 4; break;
        case 'h': t[f] = I16(d, p); p += 2; break;
        case 'H': t[f] = U16(d, p); p += 2; break;
        default: t[f] = U8(d, p); p += 1; break;
        }
    return t;
}

struct Writer
{
    std::vector<std::uint8_t> buf;
    std::map<PyStr, std::size_t> wmap;
    std::map<std::array<double, 3>, std::size_t> vmap;

    void Align(std::size_t n)
    {
        if (buf.size() % n)
            buf.resize(buf.size() + n - buf.size() % n);
    }
    std::size_t Append(const std::vector<std::uint8_t>& b)
    {
        const std::size_t off = buf.size();
        buf.insert(buf.end(), b.begin(), b.end());
        return off;
    }
    void PutU32(std::uint32_t v)
    {
        for (int i = 0; i < 4; ++i)
            buf.push_back(static_cast<std::uint8_t>(v >> (8 * i)));
    }
    std::size_t WStr(const PyStr& s, bool nomap)
    {
        if (s.empty())
            return 0;
        Align(2);
        if (!nomap)
            if (const auto it = wmap.find(s); it != wmap.end())
                return it->second;
        const std::u16string u = EncodeUtf16(s);
        const std::size_t off = buf.size();
        if (!nomap)
            wmap[s] = off;
        const auto n = static_cast<std::uint16_t>(u.size() & 0xFFFF);
        buf.push_back(static_cast<std::uint8_t>(n));
        buf.push_back(static_cast<std::uint8_t>(n >> 8));
        for (const char16_t c : u)
        {
            buf.push_back(static_cast<std::uint8_t>(c));
            buf.push_back(static_cast<std::uint8_t>(c >> 8));
        }
        buf.push_back(0);
        buf.push_back(0);
        return off;
    }
    std::size_t Vec(const std::array<double, 3>& v0, bool defaultOne)
    {
        std::array<double, 3> v{F32(v0[0]), F32(v0[1]), F32(v0[2])};
        const double d = defaultOne ? 1.0 : 0.0;
        if (v[0] == d && v[1] == d && v[2] == d)
            return 0;
        const bool nan = std::isnan(v[0]) || std::isnan(v[1]) || std::isnan(v[2]);
        // Python tuples compare -0.0 equal to 0.0 and NaN to nothing
        std::array<double, 3> key = v;
        for (double& x : key)
            if (x == 0)
                x = 0;
        if (!nan)
            if (const auto it = vmap.find(key); it != vmap.end())
                return it->second;
        Align(4);
        const std::size_t off = buf.size();
        for (const double x : v)
            PutU32(std::bit_cast<std::uint32_t>(static_cast<float>(x)));
        if (!nan)
            vmap[key] = off;
        return off;
    }
};

void PutAt(std::vector<std::uint8_t>& buf, std::size_t off, std::uint64_t v, int bytes)
{
    for (int i = 0; i < bytes; ++i)
        buf[off + i] = static_cast<std::uint8_t>(v >> (8 * i));
}

std::vector<std::uint8_t> Le32(std::initializer_list<std::int64_t> values)
{
    std::vector<std::uint8_t> out;
    for (const std::int64_t v : values)
        for (int i = 0; i < 4; ++i)
            out.push_back(static_cast<std::uint8_t>(static_cast<std::uint64_t>(v) >> (8 * i)));
    return out;
}

// ---------------------------------------------------------------- items

const std::pair<const char*, int> kFlagBits[] = {{"m_bFashion", 0},        {"m_bVested", 1}, {"m_bCanEnchant", 2},
                                                 {"m_bCanUseInventory", 3}, {"m_bNoEquip", 4}, {"m_bPcBang", 5},
                                                 {"m_bCanHyperMode", 6},    {"m_bHideSetDesc", 7}};
const std::pair<const char*, int> kFlagFields[] = {{"m_UseType", 8},     {"m_UseCondition", 12}, {"m_ItemType", 16},
                                                   {"m_ItemGrade", 20},  {"m_PeriodType", 24},   {"m_PriceType", 28}};
struct Scalar
{
    const char* key;
    int field;
    double def;
};
const Scalar kScalars[] = {
    {"m_ItemLevel", ItemLevel, 0}, {"m_Quantity", Quantity, 0}, {"m_RepairED", RepairED, 0.0},
    {"m_RepairVP", RepairVP, 0}, {"m_Price", Price, 0}, {"m_PricePvPPoint", PricePvPPoint, 0}, {"m_SetID", SetID, 0},
    {"m_fAddMaxMP", AddMaxMP, 0.0}, {"m_Endurance", Endurance, -1}, {"m_EnduranceDamageMin", EnduranceDamageMin, 0},
    {"m_EnduranceDamageMax", EnduranceDamageMax, 0}, {"m_Summon_Npc", SummonNpc, 0},
    {"m_BuyPvpRankCondition", BuyPvpRank, 0}, {"m_UseLevel", UseLevel, 0}, {"m_UnitType", UnitType, 0},
    {"m_UnitClass", UnitClass, 0}, {"m_Buff_ID", BuffID, 0}, {"m_EqipPosition", EqipPosition, 1},
    {"m_CoolTime", CoolTime, 0}, {"m_ucMaxSealCount", MaxSealCount, 0},
    {"m_iMaxAttribEnchantCount", MaxAttribEnchantCount, 0}, {"m_iAttributeLevel", AttributeLevel, 0}};
bool IsFloatKey(std::string_view k) { return k == "m_RepairED" || k == "m_fAddMaxMP"; }
const char* const kStringsNoMap[] = {"m_Description", "m_DescriptionInShop", "m_DescriptionInSkillNote"};
const char* const kStringsMap[] = {"m_ModelName", "m_ModelName1", "m_TextureChangeXETName", "m_AniXETName",
                                   "m_AniName", "m_ShopImage", "m_DropViewer", "m_AttachFrameName",
                                   "m_AttachFrameName1"};
const std::pair<const char*, int> kStringField[] = {
    {"m_Description", oDescription}, {"m_DescriptionInShop", oDescriptionInShop},
    {"m_DescriptionInSkillNote", oDescriptionInSkillNote}, {"m_ModelName", oModel0}, {"m_ModelName1", oModel1},
    {"m_TextureChangeXETName", oTextureChangeXETName}, {"m_AniXETName", oAniXETName}, {"m_AniName", oAniName},
    {"m_ShopImage", oShopImage}, {"m_DropViewer", oDropViewer}, {"m_AttachFrameName", oAttach0},
    {"m_AttachFrameName1", oAttach1}};
int StringField(std::string_view k)
{
    for (const auto& [key, f] : kStringField)
        if (k == key)
            return f;
    throw KimError("unknown string field");
}
const char* const kStatKeys[] = {"m_fBaseHP", "m_fAtkPhysic", "m_fAtkMagic", "m_fDefPhysic", "m_fDefMagic"};
const char* const kStatRelKeys[] = {"m_byBaseHPRelLV", "m_byAtkPhysicRelLV", "m_byAtkMagicRelLV",
                                    "m_byDefPhysicRelLV", "m_byDefMagicRelLV"};
const char* const kSttKeys[] = {"SLASH_TRACE_COLOR", "SLASH_TRACE_TIP_COLOR", "HYPER_SLASH_TRACE_COLOR",
                                "HYPER_SLASH_TRACE_TIP_COLOR"};
const std::pair<const char*, const char*> kEnumOf[] = {
    {"m_ItemType", "ITEM_TYPE"}, {"m_ItemGrade", "ITEM_GRADE"}, {"m_UseType", "USE_TYPE"},
    {"m_UseCondition", "USE_CONDITION"}, {"m_PeriodType", "PERIOD_TYPE"}, {"m_PriceType", "SHOP_PRICE_TYPE"},
    {"m_UnitType", "UNIT_TYPE"}, {"m_UnitClass", "UNIT_CLASS"}, {"m_EqipPosition", "EQIP_POSITION"},
    {"m_BuyPvpRankCondition", "PVP_RANK"}, {"m_Summon_Npc", "NPC_UNIT_ID"}, {"TYPE", "SPECIAL_ABILITY_TYPE"},
    {"BUFF_FACTOR", "BUFF_FACTOR_ID"}};
const char* EnumOf(std::string_view key)
{
    for (const auto& [k, e] : kEnumOf)
        if (key == k)
            return e;
    return nullptr;
}
std::vector<std::string> WantedEnums()
{
    std::set<std::string> s;
    for (const auto& [k, e] : kEnumOf)
        s.insert(e);
    return {s.begin(), s.end()};
}

struct SpecialAbility
{
    std::int64_t type = 0, coolTime = 0, value1 = 0, value2 = 0, value3 = 0;
    PyStr stringValue1;
};

struct Attached
{
    PyStr name, bone;
};

using Vec3 = std::array<double, 3>;

struct Item
{
    std::map<std::string, double> num;  // m_ItemID, flags, flag fields, scalars
    std::map<std::string, PyStr> str;   // m_Name and the string fields
    std::optional<std::array<double, 5>> stat;
    std::optional<std::array<std::int64_t, 5>> statRel;
    std::vector<std::int64_t> buffFactor;
    std::vector<SpecialAbility> specialAbilities;
    std::vector<std::int64_t> socket, randomSocket;
    std::vector<Attached> particles, meshes;
    std::map<int, Vec3> frameOffsets, scales, rotates;
    std::map<int, PyStr> commonModels, commonXets;
    std::map<int, std::uint32_t> slash;
    std::optional<std::pair<long long, int>> order;

    double N(const std::string& k) const
    {
        const auto it = num.find(k);
        if (it == num.end())
            throw KimError("missing item field " + k);
        return it->second;
    }
    std::int64_t I(const std::string& k) const { return static_cast<std::int64_t>(N(k)); }
    const PyStr& S(const std::string& k) const
    {
        static const PyStr kEmpty;
        const auto it = str.find(k);
        return it == str.end() ? kEmpty : it->second;
    }
};

struct SetData
{
    std::uint32_t id = 0;
    PyStr name;
    std::map<std::uint32_t, std::vector<std::int64_t>> options;  // need -> options
    long long order = 0;
};

struct SetCall
{
    std::uint32_t id = 0;
    PyStr name;
    std::uint32_t need = 0;
    std::vector<std::int64_t> options;
};

// ---------------------------------------------------------------- Item.kim -> items

struct Parsed
{
    std::vector<Item> items;
    std::vector<SetData> sets;
    std::vector<std::pair<long long, PyStr>> orphans;
};

Parsed ParseKim(std::span<const std::uint8_t> data)
{
    if (data.size() < 16 || std::memcmp(data.data(), "KIM ", 4) != 0)
        throw KimError("not an Item.kim file");
    const std::uint32_t version = U32(data, 4), nitems = U32(data, 8), nsets = U32(data, 12);
    if (version != kVersion)
    {
        char buf[96];
        std::snprintf(buf, sizeof buf, "unsupported KIM version 0x%x (expected 0x%x)", version, kVersion);
        throw KimError(buf);
    }
    const std::size_t tbase = 16 + 4 * std::size_t{nitems};
    const std::size_t sbase = tbase + kTempletSize * nitems;
    const std::size_t sdbase = sbase + 4 * std::size_t{nsets};
    if (sdbase + kSetSize * std::size_t{nsets} > data.size())
        throw KimError("Item.kim is truncated");
    Parsed out;
    for (std::uint32_t i = 0; i < nitems; ++i)
    {
        const auto t = ReadTemplet(data, tbase + i * kTempletSize);
        if (t[ItemID] != U32(data, 16 + 4 * std::size_t{i}))
            throw KimError("item " + Str(i) + " (id " + Str(U32(data, 16 + 4 * std::size_t{i})) +
                           ") has no templet; file not produced by the compiler");
        Item it;
        it.num["m_ItemID"] = t[ItemID];
        it.str["m_Name"] = WStr(data, static_cast<std::size_t>(t[oName]));
        const auto flags = static_cast<std::uint32_t>(t[Flags]), flags2 = static_cast<std::uint32_t>(t[Flags2]);
        for (const auto& [k, bit] : kFlagBits)
            it.num[k] = (flags >> bit) & 1;
        for (const auto& [k, pos] : kFlagFields)
            it.num[k] = (flags >> pos) & 0xF;
        it.num["m_bIsPvpItem"] = flags2 & 1;
        if (flags2 & ~1u)
            throw KimError("item " + Str(static_cast<long long>(t[ItemID])) + " uses unknown m_dwFlags2 bits");
        for (const Scalar& s : kScalars)
            it.num[s.key] = t[s.field];
        for (const auto& [k, f] : kStringField)
            it.str[k] = WStr(data, static_cast<std::size_t>(t[f]));
        const auto off = [&](int f) { return static_cast<std::size_t>(t[f]); };
        if (off(oStat))
        {
            std::array<double, 5> st{};
            for (int j = 0; j < 5; ++j)
                st[j] = F(data, off(oStat) + 4 * j);
            it.stat = st;
        }
        if (off(oStatRelLV))
        {
            std::array<std::int64_t, 5> sr{};
            for (int j = 0; j < 5; ++j)
                sr[j] = U8(data, off(oStatRelLV) + j);
            it.statRel = sr;
        }
        if (off(oBuffFactor))
        {
            const std::size_t n = U16(data, off(oBuffFactor));
            for (std::size_t j = 0; j < n; ++j)
                it.buffFactor.push_back(U16(data, off(oBuffFactor) + 2 + 2 * j));
        }
        if (off(oSpecialAbility))
        {
            const std::size_t n = U32(data, off(oSpecialAbility));
            for (std::size_t j = 0; j < n; ++j)
            {
                const std::size_t o = off(oSpecialAbility) + 4 + j * kSaSize;
                it.specialAbilities.push_back({U32(data, o), I32(data, o + 4), I32(data, o + 8), I32(data, o + 12),
                                               I32(data, o + 16), WStr(data, U32(data, o + 20))});
            }
        }
        for (const auto& [list, f] : {std::pair{&it.socket, oSocket}, std::pair{&it.randomSocket, oRandomSocket}})
            if (off(f))
            {
                const std::size_t n = U32(data, off(f));
                for (std::size_t j = 0; j < n; ++j)
                    list->push_back(I32(data, off(f) + 4 + 4 * j));
            }
        for (const auto& [list, f] : {std::pair{&it.particles, oParticle}, std::pair{&it.meshes, oMesh}})
            if (off(f))
            {
                const std::size_t n = U32(data, off(f));
                for (std::size_t j = 0; j < n; ++j)
                {
                    const std::uint32_t a = U32(data, off(f) + 4 + 8 * j), b = U32(data, off(f) + 8 + 8 * j);
                    list->push_back({WStr(data, a & 0x7FFFFFFF), WStr(data, b)});
                }
            }
        const int counts[] = {static_cast<int>(t[nFrameOffsets]), static_cast<int>(t[nRenderScales]),
                              static_cast<int>(t[nRenderRotates]), static_cast<int>(t[nCommonModelNames]),
                              static_cast<int>(t[nCommonXETNames]), static_cast<int>(t[nSlashTraces])};
        std::vector<std::pair<std::uint32_t, std::uint32_t>> ex;
        if (off(oExtra))
            for (int j = 0; j < counts[0] + counts[1] + counts[2] + counts[3] + counts[4] + counts[5]; ++j)
                ex.emplace_back(U32(data, off(oExtra) + 8 * j), U32(data, off(oExtra) + 8 * j + 4));
        std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> groups;
        std::size_t p = 0;
        for (const int n : counts)
        {
            // ex[p:p + n]
            const std::size_t b = std::min(p, ex.size()), e = std::min(p + static_cast<std::size_t>(n), ex.size());
            groups.emplace_back(ex.begin() + static_cast<long>(b), ex.begin() + static_cast<long>(e));
            p += static_cast<std::size_t>(n);
        }
        const auto vec = [&](std::size_t o) { return Vec3{F(data, o), F(data, o + 4), F(data, o + 8)}; };
        for (const auto& [ty, o] : groups[0])
            it.frameOffsets[static_cast<int>(ty)] = vec(o);
        for (const auto& [ty, o] : groups[1])
            it.scales[static_cast<int>(ty)] = vec(o);
        for (const auto& [ty, o] : groups[2])
            it.rotates[static_cast<int>(ty)] = vec(o);
        for (const auto& [ty, o] : groups[3])
            it.commonModels[static_cast<int>(ty)] = WStr(data, o);
        for (const auto& [ty, o] : groups[4])
            it.commonXets[static_cast<int>(ty)] = WStr(data, o);
        for (const auto& [ty, v] : groups[5])
            it.slash[static_cast<int>(ty)] = v;
        std::vector<long long> offs;
        for (const int f : {oName, oDescription, oDescriptionInShop, oDescriptionInSkillNote, oStat, oStatRelLV,
                            oBuffFactor, oSpecialAbility, oSocket, oRandomSocket, oParticle, oMesh, oExtra})
            if (off(f))
                offs.push_back(static_cast<long long>(off(f)));
        if (!offs.empty())
            it.order = std::pair{*std::min_element(offs.begin(), offs.end()), 0};
        else
        {
            // only shared strings/vectors: whatever this item added to the data area was appended last, so it
            // sits at its latest reference
            std::vector<long long> refs;
            for (int f = oName; f <= oStatRelLV; ++f)
                if (off(f))
                    refs.push_back(static_cast<long long>(off(f)));
            for (int g = 0; g < 5; ++g)
                for (const auto& [ty, o] : groups[static_cast<std::size_t>(g)])
                    refs.push_back(o);
            if (!refs.empty())
                it.order = std::pair{*std::max_element(refs.begin(), refs.end()), 1};
        }
        out.items.push_back(std::move(it));
    }
    std::map<std::uint32_t, std::vector<std::uint32_t>> setlists;
    std::vector<std::uint32_t> lists;
    for (std::uint32_t i = 0; i < nsets; ++i)
    {
        const std::size_t o = sdbase + i * kSetSize;
        const std::uint32_t sid = U32(data, o), oname = U32(data, o + 8), oids = U32(data, o + 12),
                            oopts = U32(data, o + 16);
        SetData s;
        s.id = sid;
        s.name = WStr(data, oname);
        s.order = oname;
        if (oopts)
        {
            const std::uint32_t n = U32(data, oopts);
            for (std::uint32_t j = 0; j < n; ++j)
                s.options[U32(data, oopts + 4 + 8 * j)].push_back(I32(data, oopts + 8 + 8 * j));
            if (n == 0)
                s.options[1];  // m_NeedPartsNum given without any m_Option
        }
        out.sets.push_back(std::move(s));
        if (oids)
        {
            auto& l = setlists[sid];
            l.clear();
            const std::uint32_t n = U32(data, oids);
            for (std::uint32_t j = 0; j < n; ++j)
                l.push_back(U32(data, oids + 4 + 4 * j));
        }
        for (const std::uint32_t x : {oids, oopts})
            if (x)
                lists.push_back(x);
    }
    // items without any data keep their place in their set's item list
    std::unordered_map<std::uint32_t, std::size_t> byid;
    for (std::size_t i = 0; i < out.items.size(); ++i)
        byid[static_cast<std::uint32_t>(out.items[i].N("m_ItemID"))] = i;
    for (Item& it : out.items)
    {
        if (it.order)
            continue;
        std::pair<long long, int> prev{0, -1};
        const auto sl = setlists.find(static_cast<std::uint32_t>(it.N("m_SetID")));
        if (sl != setlists.end())
        {
            const auto& lst = sl->second;
            const auto at = std::find(lst.begin(), lst.end(), static_cast<std::uint32_t>(it.N("m_ItemID")));
            if (at != lst.end())
                for (auto o = lst.begin(); o != at; ++o)
                {
                    const auto b = byid.find(*o);
                    if (b == byid.end())
                        throw KimError("set list names an unknown item");
                    if (const auto& oo = out.items[b->second].order)
                        prev = std::max(prev, *oo);
                }
        }
        it.order = prev != std::pair<long long, int>{0, -1} ? std::pair{prev.first, prev.second + 1}
                                                            : std::pair{static_cast<long long>(data.size()), 9};
    }
    std::stable_sort(out.items.begin(), out.items.end(), [](const Item& a, const Item& b) { return *a.order < *b.order; });
    std::stable_sort(out.sets.begin(), out.sets.end(), [](const SetData& a, const SetData& b) {
        return std::pair{a.order, a.id} < std::pair{b.order, b.id};
    });
    // Names written by AddSetItemData_LUA calls that a later call of the same set overrode are still in the data
    // area, between the item data and the set lists; keep them so the scripts rebuild the identical file.
    std::set<long long> used;
    long long first = -1;
    for (const SetData& s : out.sets)
        if (s.order)
        {
            used.insert(s.order);
            first = first < 0 ? s.order : std::min(first, s.order);
        }
    if (first >= 0)
    {
        const long long end = lists.empty() ? static_cast<long long>(data.size())
                                            : *std::min_element(lists.begin(), lists.end());
        long long o = first;
        while (o + 2 <= end)
        {
            const long long n = U16(data, static_cast<std::size_t>(o));
            if (!used.count(o))
                out.orphans.emplace_back(o, WStr(data, static_cast<std::size_t>(o)));
            o += 4 + 2 * n;
        }
    }
    return out;
}

// ---------------------------------------------------------------- items -> Item.kim

std::vector<std::uint8_t> BuildKim(const std::vector<Item>& items, const std::vector<SetCall>& setcalls,
                                   std::vector<std::string>& log)
{
    std::set<std::uint32_t> idset, setidset;
    for (const Item& it : items)
        idset.insert(static_cast<std::uint32_t>(it.N("m_ItemID")));
    for (const SetCall& s : setcalls)
        setidset.insert(s.id);
    const std::vector<std::uint32_t> ids(idset.begin(), idset.end()), setids(setidset.begin(), setidset.end());
    Writer w;
    w.buf = {'K', 'I', 'M', ' '};
    w.PutU32(kVersion);
    w.PutU32(static_cast<std::uint32_t>(ids.size()));
    w.PutU32(static_cast<std::uint32_t>(setids.size()));
    for (const std::uint32_t id : ids)
        w.PutU32(id);
    const std::size_t tbase = w.buf.size();
    w.buf.resize(w.buf.size() + kTempletSize * ids.size());
    for (const std::uint32_t id : setids)
        w.PutU32(id);
    const std::size_t sdbase = w.buf.size();
    w.buf.resize(w.buf.size() + kSetSize * setids.size());
    std::unordered_map<std::uint32_t, std::size_t> index;
    for (std::size_t i = 0; i < ids.size(); ++i)
        index[ids[i]] = i;
    struct Sd
    {
        std::int64_t maxLevel = 0;
        std::vector<std::uint32_t> ids;
        std::map<std::uint32_t, std::vector<std::int64_t>> options;
    };
    std::map<std::int64_t, Sd> setdata;
    std::set<std::uint32_t> done;
    for (const Item& it : items)
    {
        const auto iid = static_cast<std::uint32_t>(it.N("m_ItemID"));
        if (!done.insert(iid).second)
        {
            log.push_back("ITEMID_" + Str(iid) + ": DUPLICATED_ITEM_TEMPLET (ignored)");
            continue;
        }
        std::array<double, kNumTf> t{};
        t[ItemID] = iid;
        t[oName] = static_cast<double>(w.WStr(it.S("m_Name"), true));
        std::int64_t fl = 0;
        for (const auto& [k, bit] : kFlagBits)
            fl |= it.I(k) ? (std::int64_t{1} << bit) : 0;
        for (const auto& [k, pos] : kFlagFields)
            fl |= (it.I(k) & 0xF) << pos;
        t[Flags] = static_cast<double>(fl);
        t[Flags2] = it.I("m_bIsPvpItem") ? 1 : 0;
        for (const Scalar& s : kScalars)
            t[s.field] = it.N(s.key);
        for (const int f : {BuyPvpRank, UseLevel, UnitType, UnitClass, BuffID, EqipPosition, CoolTime, MaxSealCount,
                            MaxAttribEnchantCount, AttributeLevel})
            t[f] = static_cast<double>(static_cast<std::int64_t>(t[f]) & 0xFF);
        for (const int f : {Endurance, EnduranceDamageMin, EnduranceDamageMax})
            t[f] = static_cast<double>(((static_cast<std::int64_t>(t[f]) + 0x8000) & 0xFFFF) - 0x8000);
        t[SummonNpc] = static_cast<double>(static_cast<std::int64_t>(t[SummonNpc]) & 0xFFFF);
        // data is appended in the order AddItemTemplet_LUA reads the script
        for (const char* k : {"m_Description", "m_DescriptionInShop"})
            t[StringField(k)] = static_cast<double>(w.WStr(it.S(k), true));
        for (const char* k : {"m_ModelName", "m_ModelName1", "m_TextureChangeXETName", "m_AniXETName", "m_AniName",
                              "m_ShopImage", "m_DropViewer", "m_AttachFrameName", "m_AttachFrameName1"})
            t[StringField(k)] = static_cast<double>(w.WStr(it.S(k), false));
        std::vector<std::pair<std::uint32_t, std::uint32_t>> extra;
        const std::tuple<const std::map<int, Vec3>*, bool, int> vecs[] = {
            {&it.frameOffsets, false, nFrameOffsets}, {&it.scales, true, nRenderScales},
            {&it.rotates, false, nRenderRotates}};
        for (const auto& [m, defaultOne, cnt] : vecs)
            for (int i = 1; i < kNumChars; ++i)
            {
                const auto f = m->find(i);
                const double d = defaultOne ? 1.0 : 0.0;
                const std::size_t o = w.Vec(f != m->end() ? f->second : Vec3{d, d, d}, defaultOne);
                if (o)
                {
                    extra.emplace_back(i, static_cast<std::uint32_t>(o));
                    t[cnt] += 1;
                }
            }
        t[oDescriptionInSkillNote] = static_cast<double>(w.WStr(it.S("m_DescriptionInSkillNote"), true));
        if (it.I("m_UseCondition") == 1 && it.I("m_UseType") != 0)
        {
            std::map<int, std::size_t> mo, xo;
            for (int i = 1; i < kNumChars; ++i)
            {
                const auto m = it.commonModels.find(i);
                if (m != it.commonModels.end() && !m->second.empty())
                {
                    mo[i] = w.WStr(m->second, false);
                    const auto x = it.commonXets.find(i);
                    if (x != it.commonXets.end() && !x->second.empty())
                        xo[i] = w.WStr(x->second, false);
                }
            }
            for (const auto& [i, o] : mo)
            {
                extra.emplace_back(i, static_cast<std::uint32_t>(o));
                t[nCommonModelNames] += 1;
            }
            for (const auto& [i, o] : xo)
            {
                extra.emplace_back(i, static_cast<std::uint32_t>(o));
                t[nCommonXETNames] += 1;
            }
        }
        if (it.stat)
        {
            w.Align(4);
            t[oStat] = static_cast<double>(w.buf.size());
            for (const double v : *it.stat)
                w.PutU32(std::bit_cast<std::uint32_t>(static_cast<float>(v)));
        }
        if (it.statRel)
        {
            w.Align(4);
            t[oStatRelLV] = static_cast<double>(w.buf.size());
            for (const std::int64_t v : *it.statRel)
                w.buf.push_back(static_cast<std::uint8_t>(v & 0xFF));
        }
        std::vector<std::uint16_t> bf;
        for (const std::int64_t x : it.buffFactor)
            if (x)
                bf.push_back(static_cast<std::uint16_t>(x & 0xFFFF));
        if (!bf.empty())
        {
            w.Align(2);
            t[oBuffFactor] = static_cast<double>(w.buf.size());
            for (const std::uint16_t v : std::initializer_list<std::uint16_t>{static_cast<std::uint16_t>(bf.size())})
                w.buf.insert(w.buf.end(), {static_cast<std::uint8_t>(v), static_cast<std::uint8_t>(v >> 8)});
            for (const std::uint16_t v : bf)
                w.buf.insert(w.buf.end(), {static_cast<std::uint8_t>(v), static_cast<std::uint8_t>(v >> 8)});
        }
        if (!it.specialAbilities.empty())
        {
            std::vector<std::uint8_t> recs = Le32({static_cast<std::int64_t>(it.specialAbilities.size())});
            for (const SpecialAbility& sa : it.specialAbilities)
            {
                const PyStr& sv = sa.stringValue1;
                const std::size_t so = (!sv.empty() && sv != Ascii("Group 0")) ? w.WStr(sv, false) : 0;
                const auto r = Le32({sa.type & 0xFFFFFFFF, sa.coolTime, sa.value1, sa.value2, sa.value3,
                                     static_cast<std::int64_t>(so)});
                recs.insert(recs.end(), r.begin(), r.end());
            }
            w.Align(4);
            t[oSpecialAbility] = static_cast<double>(w.Append(recs));
        }
        for (const auto& [v, f] : {std::pair{&it.socket, oSocket}, std::pair{&it.randomSocket, oRandomSocket}})
            if (!v->empty())
            {
                std::vector<std::uint8_t> b = Le32({static_cast<std::int64_t>(v->size())});
                for (const std::int64_t x : *v)
                {
                    const auto e = Le32({x});
                    b.insert(b.end(), e.begin(), e.end());
                }
                w.Align(4);
                t[f] = static_cast<double>(w.Append(b));
            }
        for (const auto& [v, f, jiggle] :
             {std::tuple{&it.particles, oParticle, false}, std::tuple{&it.meshes, oMesh, true}})
        {
            std::vector<std::uint8_t> recs;
            std::size_t count = 0;
            for (const Attached& e : *v)
                if (!e.name.empty() && !e.bone.empty())
                {
                    std::uint64_t a = w.WStr(e.name, false);
                    const std::uint64_t b = w.WStr(e.bone, false);
                    if (jiggle && ToUtf8(e.name).find("Jiggle_Bone") != std::string::npos)
                        a |= 0x80000000;
                    const auto r = Le32({static_cast<std::int64_t>(a), static_cast<std::int64_t>(b)});
                    recs.insert(recs.end(), r.begin(), r.end());
                    ++count;
                }
            if (count)
            {
                std::vector<std::uint8_t> b = Le32({static_cast<std::int64_t>(count)});
                b.insert(b.end(), recs.begin(), recs.end());
                w.Align(4);
                t[f] = static_cast<double>(w.Append(b));
            }
        }
        for (int ty = 0; ty < 4; ++ty)
            if (const auto s = it.slash.find(ty); s != it.slash.end())
            {
                extra.emplace_back(ty, s->second);
                t[nSlashTraces] += 1;
            }
        if (!extra.empty())
        {
            std::vector<std::uint8_t> b;
            for (const auto& [x, y] : extra)
            {
                const auto e = Le32({x, y});
                b.insert(b.end(), e.begin(), e.end());
            }
            w.Align(4);
            t[oExtra] = static_cast<double>(w.Append(b));
        }
        std::size_t p = tbase + kTempletSize * index.at(iid);
        for (int f = 0; f < kNumTf; ++f)
        {
            const char type = TfType(f);
            if (type == 'f')
            {
                PutAt(w.buf, p, std::bit_cast<std::uint32_t>(static_cast<float>(t[f])), 4);
                p += 4;
                continue;
            }
            const auto v = static_cast<std::int64_t>(t[f]);
            const int size = (type == 'I' || type == 'i') ? 4 : (type == 'h' || type == 'H') ? 2 : 1;
            const bool ok = type == 'I'   ? (v >= 0 && v <= 0xFFFFFFFFll)
                            : type == 'i' ? (v >= INT32_MIN && v <= INT32_MAX)
                            : type == 'h' ? (v >= INT16_MIN && v <= INT16_MAX)
                            : type == 'H' ? (v >= 0 && v <= 0xFFFF)
                                          : (v >= 0 && v <= 0xFF);
            if (!ok)
                throw KimError("item " + Str(iid) + ": a field is out of range");
            PutAt(w.buf, p, static_cast<std::uint64_t>(v), size);
            p += static_cast<std::size_t>(size);
        }
        const auto setId = static_cast<std::int64_t>(t[SetID]);
        if (setId)
        {
            if (setId < 0 || !setidset.count(static_cast<std::uint32_t>(setId)))
                log.push_back("ITEMID_" + Str(iid) + ": INVALID_SETID_" + Str(setId));
            Sd& sd = setdata[setId];
            sd.ids.push_back(iid);
            const auto useLevel = static_cast<std::int64_t>(t[UseLevel]);
            if (0 < useLevel && sd.maxLevel < useLevel)
                sd.maxLevel = useLevel;
        }
    }
    std::map<std::uint32_t, std::array<std::int64_t, 5>> setrec;
    for (const SetCall& s : setcalls)
    {
        auto& rec = setrec.try_emplace(s.id, std::array<std::int64_t, 5>{s.id, 0, 0, 0, 0}).first->second;
        rec[2] = static_cast<std::int64_t>(w.WStr(s.name, false));
        if (s.need > 0)
        {
            Sd& sd = setdata[s.id];
            if (!sd.options.count(s.need))
            {
                auto& o = sd.options[s.need];
                for (const std::int64_t x : s.options)
                    if (x != 0)
                        o.push_back(x);
            }
        }
    }
    for (std::size_t i = 0; i < setids.size(); ++i)
    {
        auto rec = setrec.at(setids[i]);
        const auto f = setdata.find(setids[i]);
        const Sd sd = f == setdata.end() ? Sd{} : f->second;
        rec[1] = sd.maxLevel;
        if (!sd.ids.empty())
        {
            std::vector<std::uint8_t> b = Le32({static_cast<std::int64_t>(sd.ids.size())});
            for (const std::uint32_t id : sd.ids)
            {
                const auto e = Le32({id});
                b.insert(b.end(), e.begin(), e.end());
            }
            w.Align(4);
            rec[3] = static_cast<std::int64_t>(w.Append(b));
        }
        if (!sd.options.empty())
        {
            std::size_t total = 0;
            for (const auto& [need, opts] : sd.options)
                total += opts.size();
            w.Align(4);
            rec[4] = static_cast<std::int64_t>(w.Append(Le32({static_cast<std::int64_t>(total)})));
            for (const auto& [need, opts] : sd.options)
                for (const std::int64_t o : opts)
                {
                    const auto e = Le32({need, o});
                    w.buf.insert(w.buf.end(), e.begin(), e.end());
                }
        }
        for (int k = 0; k < 5; ++k)
            PutAt(w.buf, sdbase + kSetSize * i + 4 * static_cast<std::size_t>(k), static_cast<std::uint64_t>(rec[k]), 4);
    }
    return std::move(w.buf);
}

// ---------------------------------------------------------------- Lua output

class EnumNames
{
public:
    explicit EnumNames(const Enums& e)
    {
        for (const auto& [name, kv] : e.tables)
        {
            auto& rev = rev_[name];
            for (const auto& [k, v] : kv)  // sorted by name: the first name of a value wins
                if (!std::isnan(v))
                    rev.try_emplace(v, k);
        }
        const auto g = e.tables.find("_G");
        hasBool_ = g != e.tables.end() && g->second.count("True");
    }
    std::string Val(std::string_view key, std::int64_t v) const
    {
        if (const char* en = EnumOf(key))
            if (const auto t = rev_.find(en); t != rev_.end())
                if (const auto n = t->second.find(static_cast<double>(v)); n != t->second.end())
                    return std::string(en) + "[\"" + n->second + "\"]";
        return Str(v);
    }
    std::string Boolean(std::int64_t v) const
    {
        if (hasBool_)
            return v ? "True" : "False";
        return v ? "1" : "0";
    }

private:
    std::map<std::string, std::map<double, std::string>> rev_;
    bool hasBool_ = false;
};

const std::string kInd = "       ";  // Item.lua / SetItem.lua indent (7 spaces)
// per-character keys follow this order in the original scripts
constexpr int kCharOrder[] = {1, 2, 3, 4, 8, 5, 6, 7, 9};
double DefaultOf(std::string_view k)
{
    if (k == "m_UseCondition" || k == "m_EqipPosition")
        return 1;
    if (k == "m_ItemGrade")
        return 4;
    if (k == "m_PriceType")
        return 2;
    if (k == "m_Endurance")
        return -1;
    return 0;
}

std::string ScalarText(const Item& it, const EnumNames& en, const std::string& k)
{
    const double v = it.N(k);
    if (EnumOf(k))
        return en.Val(k, static_cast<std::int64_t>(v));
    if (IsFloatKey(k))
        return NumF32(v);
    if (k.starts_with("m_b"))
        return en.Boolean(static_cast<std::int64_t>(v));
    return Str(static_cast<long long>(v));
}

std::vector<std::string> Block(const std::string& key, const std::vector<std::string>& lines)
{
    std::vector<std::string> out{kInd + key + " =", kInd + "{"};
    out.insert(out.end(), lines.begin(), lines.end());
    out.push_back(kInd + "},");
    return out;
}

std::string ScaleRotateString(const Vec3& scale, const Vec3& rot)
{
    std::string parts[6];
    for (int i = 0; i < 3; ++i)
    {
        const double v = scale[static_cast<std::size_t>(i)];
        parts[i] = Fixed(v, [v](const std::string& s) { return F32(ParseDouble(s)) == v; });
    }
    for (int i = 0; i < 3; ++i)
    {
        const double v = rot[static_cast<std::size_t>(i)];
        parts[3 + i] = Fixed(v * 180.0 / kD3dxPi, [v](const std::string& s) { return RotOf(s) == v; });
    }
    return "SX:" + parts[0] + " SY:" + parts[1] + " SZ:" + parts[2] + " RX:" + parts[3] + " RY:" + parts[4] +
           " RZ:" + parts[5];
}

std::array<std::uint32_t, 4> ColorRgba(std::uint32_t v)
{
    return {(v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, (v >> 24) & 0xFF};
}

// One AddItemTemplet call in the layout of the original Item.lua.
std::string ItemToLua(const Item& it, const EnumNames& en)
{
    std::vector<std::string> L;
    const std::string I2 = kInd + kInd, I3 = I2 + kInd;
    const auto scalar = [&](const std::string& k, bool always = false) {
        if (always || it.N(k) != DefaultOf(k))
            L.push_back(kInd + k + " = " + ScalarText(it, en, k) + ",");
    };
    const auto string = [&](const std::string& k, bool always = false) {
        if (always || !it.S(k).empty())
            L.push_back(kInd + k + " = " + LuaStr(it.S(k)) + ",");
    };
    scalar("m_ItemID", true);
    string("m_Name", true);
    for (const char* k : {"m_Description", "m_DescriptionInShop", "m_DescriptionInSkillNote"})
        string(k);
    L.emplace_back();
    for (const char* k : {"m_ModelName", "m_ModelName1", "m_TextureChangeXETName", "m_AniXETName", "m_AniName"})
        string(k);
    scalar("m_UseType");
    for (const char* k : {"m_AttachFrameName", "m_AttachFrameName1"})
        string(k);
    L.emplace_back();
    string("m_ShopImage");
    string("m_DropViewer");
    L.emplace_back();
    for (const char* k : {"m_BuyPvpRankCondition", "m_UseCondition", "m_UnitType", "m_UnitClass", "m_UseLevel",
                          "m_ItemLevel"})
        scalar(k);
    L.emplace_back();
    scalar("m_ItemType", true);
    for (const char* k : {"m_ItemGrade", "m_bFashion", "m_bPcBang", "m_bVested", "m_bCanEnchant", "m_fAddMaxMP",
                          "m_bCanUseInventory", "m_bNoEquip", "m_bHideSetDesc", "m_bIsPvpItem", "m_ucMaxSealCount",
                          "m_iMaxAttribEnchantCount", "m_iAttributeLevel"})
        scalar(k);
    L.emplace_back();
    for (const int i : kCharOrder)
    {
        if (const auto f = it.frameOffsets.find(i); f != it.frameOffsets.end())
            for (int a = 0; a < 3; ++a)
                if (f->second[static_cast<std::size_t>(a)] != 0.0)
                    L.push_back(kInd + "m_FrameOffset" + kChars[i] + "XYZ"[a] + " = " +
                                NumF32(f->second[static_cast<std::size_t>(a)]) + ",");
        const auto s = it.scales.find(i);
        const auto r = it.rotates.find(i);
        if (s != it.scales.end() || r != it.rotates.end())
            L.push_back(kInd + kCaps[i] + "_ITEM_SCALE_ROTATE = " +
                        LuaStr(ScaleRotateString(s != it.scales.end() ? s->second : Vec3{1.0, 1.0, 1.0},
                                                 r != it.rotates.end() ? r->second : Vec3{0.0, 0.0, 0.0})) +
                        ",");
    }
    for (const char* k : {"m_PeriodType", "m_bCanHyperMode"})
        scalar(k);
    scalar("m_Endurance", true);
    for (const char* k : {"m_Quantity", "m_EnduranceDamageMin", "m_EnduranceDamageMax", "m_RepairED"})
        scalar(k);
    L.emplace_back();
    for (const char* k : {"m_PriceType", "m_Price", "m_PricePvPPoint"})
        scalar(k);
    L.emplace_back();
    scalar("m_EqipPosition");
    L.emplace_back();
    const auto add = [&](const std::vector<std::string>& lines) { L.insert(L.end(), lines.begin(), lines.end()); };
    if (it.statRel)
    {
        std::vector<std::string> lines;
        for (int j = 0; j < 5; ++j)
            if (const auto v = (*it.statRel)[static_cast<std::size_t>(j)])
                lines.push_back(I2 + kStatRelKeys[j] + " = " + Str(v) + ",");
        add(Block("m_Stat_Relation_Level", lines));
    }
    scalar("m_Buff_ID");
    for (const auto& [ty, v] : it.slash)
    {
        if (ty < 0 || ty >= 4)
            throw KimError("unknown slash trace type " + Str(ty));
        const auto c = ColorRgba(v);
        L.push_back(kInd + kSttKeys[ty] + " = { " + Str(c[0]) + ", " + Str(c[1]) + ", " + Str(c[2]) + ", " +
                    Str(c[3]) + " },");
    }
    if (it.stat)
    {
        std::vector<std::string> lines;
        for (int j = 0; j < 5; ++j)
            if (const double v = (*it.stat)[static_cast<std::size_t>(j)]; v != 0)
                lines.push_back(I2 + kStatKeys[j] + " = " + NumF32(v) + ",");
        add(Block("m_Stat", lines));
    }
    if (!it.specialAbilities.empty())
    {
        std::vector<std::string> sal;
        for (const SpecialAbility& sa : it.specialAbilities)
        {
            sal.push_back(I2 + "{");
            sal.push_back(I3 + "TYPE = " + en.Val("TYPE", sa.type) + ",");
            const std::pair<const char*, std::int64_t> fields[] = {
                {"COOL_TIME", sa.coolTime}, {"VALUE1", sa.value1}, {"VALUE2", sa.value2}, {"VALUE3", sa.value3}};
            for (const auto& [k, v] : fields)
                if (v)
                    sal.push_back(I3 + k + " = " + Str(v) + ",");
            if (!sa.stringValue1.empty())
                sal.push_back(I3 + "STRING_VALUE1 = " + LuaStr(sa.stringValue1) + ",");
            sal.push_back(I2 + "},");
        }
        add(Block("m_SpecialAbilityList", sal));
    }
    for (const auto& [key, v] : {std::pair{"SOCKET_OPTION", &it.socket}, std::pair{"RANDOM_SOCKET_GROUP_ID", &it.randomSocket}})
        if (!v->empty())
        {
            std::vector<std::string> lines;
            for (const std::int64_t x : *v)
                lines.push_back(I2 + Str(x) + ", ");
            add(Block(key, lines));
        }
    const auto attached = [&](const char* key, const char* nameKey, const std::vector<Attached>& list) {
        std::vector<std::string> lines;
        for (const Attached& e : list)
        {
            lines.push_back(I2 + "{ ");
            lines.push_back(I3 + nameKey + " = " + LuaStr(e.name) + ",");
            lines.push_back(I3 + "BONE_NAME = " + LuaStr(e.bone) + ",");
            lines.push_back(I2 + "},");
        }
        add(Block(key, lines));
    };
    if (!it.particles.empty())
        attached("ATTACHED_PARTICLE", "PARTICLE_NAME", it.particles);
    scalar("m_CoolTime");
    if (!it.buffFactor.empty())
    {
        std::vector<std::string> lines;
        for (const std::int64_t v : it.buffFactor)
            lines.push_back(I2 + en.Val("BUFF_FACTOR", v) + ",");
        add(Block("BUFF_FACTOR", lines));
    }
    if (!it.meshes.empty())
        attached("ATTACHED_MESH", "MESH_NAME", it.meshes);
    scalar("m_SetID");
    scalar("m_Summon_Npc");
    if (!it.commonModels.empty())
    {
        L.emplace_back();
        for (const int i : kCharOrder)
            if (const auto m = it.commonModels.find(i); m != it.commonModels.end())
            {
                L.push_back(kInd + kCaps[i] + "_MODEL_NAME = " + LuaStr(m->second) + ",");
                if (const auto x = it.commonXets.find(i); x != it.commonXets.end())
                    L.push_back(kInd + kCaps[i] + "_XET_NAME = " + LuaStr(x->second) + ",");
            }
    }
    // collapse runs of blank lines
    std::vector<std::string> body;
    for (std::string& line : L)
    {
        if (line.empty() && (body.empty() || body.back().empty()))
            continue;
        body.push_back(std::move(line));
    }
    while (!body.empty() && body.back().empty())
        body.pop_back();
    std::string out = "g_pItemManager:AddItemTemplet\n{\n";
    for (std::size_t i = 0; i < body.size(); ++i)
        out += (i ? "\n" : "") + body[i];
    return out + "\n}\n\n\n";
}

std::string NameCall(std::uint32_t sid, const PyStr& name, const std::string& comment = "")
{
    return "g_pCX2SetItemManager:AddSetItemData_LUA\n{\n" + kInd + "m_SetID = " + Str(sid) + ",\n" + kInd +
           "m_SetName = " + LuaStr(name) + "," + comment + "\n}\n\n";
}

std::string SetsToLua(const Parsed& p)
{
    struct Event
    {
        long long order;
        int kind;  // 0 orphan name, 1 set
        std::size_t index;
    };
    std::vector<Event> events;
    for (std::size_t i = 0; i < p.sets.size(); ++i)
        events.push_back({p.sets[i].order, 1, i});
    for (std::size_t i = 0; i < p.orphans.size(); ++i)
        events.push_back({p.orphans[i].first, 0, i});
    std::stable_sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return std::pair{a.order, a.kind} < std::pair{b.order, b.kind};
    });
    const std::string note = "         -- overridden below; kept so Item.kim rebuilds identically";
    std::string out;
    const SetData* prev = nullptr;
    std::vector<const PyStr*> pending;
    for (const Event& e : events)
    {
        if (e.kind == 0)
        {
            const PyStr& name = p.orphans[e.index].second;
            if (prev)
            {
                out += NameCall(prev->id, name, note);
                out += NameCall(prev->id, prev->name);
            }
            else
                pending.push_back(&name);
            continue;
        }
        const SetData& s = p.sets[e.index];
        for (const PyStr* name : pending)
            out += NameCall(s.id, *name, note);
        pending.clear();
        prev = &s;
        std::vector<std::pair<std::uint32_t, std::vector<std::int64_t>>> groups(s.options.begin(), s.options.end());
        if (groups.empty())
            groups.emplace_back(0, std::vector<std::int64_t>{});
        for (const auto& [need, opts] : groups)
        {
            std::vector<std::string> L{kInd + "m_SetID = " + Str(s.id) + ",", kInd + "m_SetName = " + LuaStr(s.name) + ","};
            if (need)
                L.push_back(kInd + "m_NeedPartsNum = " + Str(need) + ",");
            for (std::size_t j = 0; j < opts.size(); ++j)
                L.push_back(kInd + "m_Option" + Str(static_cast<long long>(j + 1)) + " = " + Str(opts[j]) + ",");
            out += "g_pCX2SetItemManager:AddSetItemData_LUA\n{\n";
            for (std::size_t j = 0; j < L.size(); ++j)
                out += (j ? "\n" : "") + L[j];
            out += "\n}\n\n";
        }
    }
    return out;
}

std::string Crlf(const std::string& s)
{
    std::string out;
    out.reserve(s.size() + s.size() / 16);
    for (const char c : s)
    {
        if (c == '\n')
            out += '\r';
        out += c;
    }
    return out;
}

// ---------------------------------------------------------------- Lua input

// A value of a script table as kim.py dumps it (tables up to 4 levels deep).
struct LValue;
using LTable = std::shared_ptr<std::pair<std::map<double, LValue>, std::map<std::string, LValue>>>;
struct LValue
{
    enum Kind { Num, String, Bool, Table, Other } kind = Other;
    double num = 0;
    std::string str;  // String: the bytes; Num: tostring()
    LTable table;
};

LValue ReadValue(lua_State* L, int idx, int depth)
{
    LValue v;
    switch (lua_type(L, idx))
    {
    case LUA_TNUMBER:
        v.kind = LValue::Num;
        v.num = lua_tonumber(L, idx);
        lua_pushvalue(L, idx);
        {
            std::size_t n = 0;
            const char* s = lua_tolstring(L, -1, &n);
            v.str.assign(s, n);
        }
        lua_pop(L, 1);
        return v;
    case LUA_TSTRING: {
        v.kind = LValue::String;
        std::size_t n = 0;
        const char* s = lua_tolstring(L, idx, &n);
        v.str.assign(s, n);
        return v;
    }
    case LUA_TBOOLEAN:
        v.kind = LValue::Bool;
        v.num = lua_toboolean(L, idx);
        return v;
    case LUA_TTABLE:
        if (depth >= 4)
            return v;
        break;
    default:
        return v;
    }
    v.kind = LValue::Table;
    v.table = std::make_shared<LTable::element_type>();
    idx = detail::lua_absindex(L, idx);
    lua_pushnil(L);
    while (lua_next(L, idx))
    {
        if (lua_type(L, -2) == LUA_TNUMBER)
            v.table->first[lua_tonumber(L, -2)] = ReadValue(L, -1, depth + 1);
        else if (lua_type(L, -2) == LUA_TSTRING)
        {
            std::size_t n = 0;
            const char* s = lua_tolstring(L, -2, &n);
            v.table->second[std::string(s, n)] = ReadValue(L, -1, depth + 1);
        }
        lua_pop(L, 1);
    }
    return v;
}

// String -> number as KLuaManager reads a string field asked for as a number.
std::optional<double> ToNumber(std::string s)
{
    const auto space = [](char c) { return c == ' ' || (c >= '\t' && c <= '\r') || (c >= 0x1C && c <= 0x1F); };
    while (!s.empty() && space(s.front()))
        s.erase(s.begin());
    while (!s.empty() && space(s.back()))
        s.pop_back();
    std::string lower;
    for (const char c : s)
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    bool neg = false;
    std::string_view body = s;
    if (lower.starts_with("0x") || lower.starts_with("-0x") || lower.starts_with("+0x"))
    {
        if (body[0] == '-' || body[0] == '+')
        {
            neg = body[0] == '-';
            body.remove_prefix(1);
        }
        body.remove_prefix(2);
        if (body.empty())
            return std::nullopt;
        double v = 0;
        for (const char c : body)
        {
            const int d = (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'f') ? c - 'a' + 10
                        : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : -1;
            if (d < 0)
                return std::nullopt;
            v = v * 16 + d;
        }
        return neg ? -v : v;
    }
    if (body.empty())
        return std::nullopt;
    if (body[0] == '+')
        body.remove_prefix(1);
    if (body.empty() || body[0] == '+' || (s[0] == '+' && body[0] == '-'))
        return std::nullopt;
    double v = 0;
    const auto r = std::from_chars(body.data(), body.data() + body.size(), v);
    if (r.ec != std::errc() || r.ptr != body.data() + body.size())
        return std::nullopt;
    return v;
}

class LuaTable
{
public:
    explicit LuaTable(LTable t) : t_(std::move(t)) {}

    const LValue* Raw(const std::string& key) const
    {
        const auto it = t_->second.find(key);
        return it == t_->second.end() ? nullptr : &it->second;
    }
    const LValue* Raw(double key) const
    {
        const auto it = t_->first.find(key);
        return it == t_->first.end() ? nullptr : &it->second;
    }
    template <class K>
    std::optional<double> Number(const K& key) const
    {
        const LValue* v = Raw(key);
        if (!v)
            return std::nullopt;
        if (v->kind == LValue::Num)
            return v->num;
        if (v->kind == LValue::String)
            return ToNumber(v->str);
        return std::nullopt;
    }
    std::int64_t Int(const std::string& key, std::int64_t def, int bits = 32, bool isSigned = true) const
    {
        const auto n = Number(key);
        return n ? CInt(*n, bits, isSigned) : def;
    }
    double Float(const std::string& key, double def) const
    {
        const auto n = Number(key);
        return n ? F32(*n) : def;
    }
    int Bool(const std::string& key) const
    {
        const auto n = Number(key);
        return n ? (CInt(*n) != 0 ? 1 : 0) : 0;
    }
    bool HasNumber(const std::string& key) const { return Number(key).has_value(); }
    bool HasString(const std::string& key) const
    {
        const LValue* v = Raw(key);
        return v && (v->kind == LValue::String || v->kind == LValue::Num);
    }
    const std::string* Bytes(const std::string& key) const
    {
        const LValue* v = Raw(key);
        return v && (v->kind == LValue::String || v->kind == LValue::Num) ? &v->str : nullptr;
    }
    PyStr WStr(const std::string& key, const PyStr& def = {}) const
    {
        const std::string* b = Bytes(key);
        return b ? DecodeUtf8(*b) : def;
    }
    template <class K>
    std::optional<LuaTable> Table(const K& key) const
    {
        const LValue* v = Raw(key);
        if (v && v->kind == LValue::Table)
            return LuaTable(v->table);
        return std::nullopt;
    }
    std::vector<LuaTable> SeqTables() const
    {
        std::vector<LuaTable> out;
        for (double i = 1;; ++i)
        {
            auto t = Table(i);
            if (!t)
                return out;
            out.push_back(*t);
        }
    }
    std::vector<std::int64_t> SeqInts(bool isSigned) const
    {
        std::vector<std::int64_t> out;
        for (double i = 1;; ++i)
        {
            const auto n = Number(i);
            if (!n)
                return out;
            out.push_back(CInt(*n, 32, isSigned));
        }
    }

private:
    LTable t_;
};

// CX2ItemManager::TokenizeByScaleRotate.
std::pair<Vec3, Vec3> TokenizeScaleRotate(const PyStr& s)
{
    Vec3 scale{1.0, 1.0, 1.0}, rot{0.0, 0.0, 0.0};
    if (s.size() < 19)
        return {scale, rot};
    static const char* const kNames[] = {"SX", "SY", "SZ", "RX", "RY", "RZ"};
    std::string tok[6], cur;
    for (const char32_t ch : s)
    {
        if (ch == 'R' || ch == 'S' || ch == 'X' || ch == 'Y' || ch == 'Z')
            cur += static_cast<char>(ch);
        else if (ch == ' ')
            cur.clear();
        else if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-')
            for (int i = 0; i < 6; ++i)
                if (cur == kNames[i])
                {
                    tok[i] += static_cast<char>(ch);
                    break;
                }
    }
    // Python's float() on [0-9.-]: -?(digits[.digits]|.digits)
    const auto isFloat = [](const std::string& t) {
        std::size_t i = 0;
        if (i < t.size() && t[i] == '-')
            ++i;
        std::size_t intDigits = 0, fracDigits = 0;
        while (i < t.size() && t[i] >= '0' && t[i] <= '9')
            ++i, ++intDigits;
        if (i < t.size() && t[i] == '.')
        {
            ++i;
            while (i < t.size() && t[i] >= '0' && t[i] <= '9')
                ++i, ++fracDigits;
        }
        return i == t.size() && (intDigits || fracDigits);
    };
    const auto atof = [&](const std::string& t) {
        std::string m;
        for (const char ch : t)
        {
            std::string cand = m + ch;
            if (!isFloat(cand + ((cand.back() == '.' || cand.back() == '-') ? "0" : "")))
                break;
            m = cand;
        }
        if (isFloat(m))
            return ParseDouble(m);
        if (isFloat(m + "0"))
            return ParseDouble(m + "0");
        return 0.0;
    };
    for (int i = 0; i < 3; ++i)
    {
        if (!tok[i].empty())
            scale[static_cast<std::size_t>(i)] = F32(atof(tok[i]));
        if (!tok[3 + i].empty())
            rot[static_cast<std::size_t>(i)] = F32((atof(tok[3 + i]) / 180.0) * kD3dxPi);
    }
    return {scale, rot};
}

// KProxy2::AddItemTemplet_LUA.
std::optional<Item> ReadItem(const LuaTable& t)
{
    const std::int64_t uc = t.Int("m_UseCondition", 1);
    if (uc == 0)
        return std::nullopt;
    if (!t.HasNumber("m_ItemID") || !t.HasString("m_Name") || !t.HasNumber("m_ItemType"))
        return std::nullopt;
    Item it;
    it.num["m_ItemID"] = static_cast<double>(t.Int("m_ItemID", 0, 32, false));
    it.str["m_Name"] = t.WStr("m_Name");
    it.num["m_ItemType"] = static_cast<double>(t.Int("m_ItemType", 0));
    it.num["m_UseCondition"] = static_cast<double>(uc);
    for (const char* k : {"m_UseType", "m_PeriodType"})
        it.num[k] = static_cast<double>(t.Int(k, 0));
    it.num["m_ItemGrade"] = static_cast<double>(t.Int("m_ItemGrade", 4));
    it.num["m_PriceType"] = static_cast<double>(t.Int("m_PriceType", 2));
    for (const auto& [k, bit] : kFlagBits)
        it.num[k] = t.Bool(k);
    it.num["m_bIsPvpItem"] = t.Bool("m_bIsPvpItem");
    for (const Scalar& s : kScalars)
        it.num[s.key] = IsFloatKey(s.key) ? t.Float(s.key, s.def)
                                          : static_cast<double>(t.Int(s.key, static_cast<std::int64_t>(s.def)));
    for (const char* k : kStringsNoMap)
        it.str[k] = t.WStr(k);
    for (const char* k : kStringsMap)
        it.str[k] = t.WStr(k);
    for (int i = 1; i < kNumChars; ++i)
    {
        Vec3 v{};
        for (int a = 0; a < 3; ++a)
            v[static_cast<std::size_t>(a)] = t.Float(std::string("m_FrameOffset") + kChars[i] + "XYZ"[a], 0.0);
        if (v != Vec3{0.0, 0.0, 0.0})
            it.frameOffsets[i] = v;
        const std::string* b = t.Bytes(std::string(kCaps[i]) + "_ITEM_SCALE_ROTATE");
        const auto [sc, rt] = TokenizeScaleRotate(b && !b->empty() ? DecodeUtf8(*b) : PyStr());
        if (sc != Vec3{1.0, 1.0, 1.0})
            it.scales[i] = sc;
        if (rt != Vec3{0.0, 0.0, 0.0})
            it.rotates[i] = rt;
    }
    for (int i = 1; i < kNumChars; ++i)
    {
        const PyStr m = t.WStr(std::string(kCaps[i]) + "_MODEL_NAME");
        if (!m.empty())
        {
            it.commonModels[i] = m;
            const PyStr x = t.WStr(std::string(kCaps[i]) + "_XET_NAME");
            if (!x.empty())
                it.commonXets[i] = x;
        }
    }
    if (const auto st = t.Table(std::string("m_Stat")))
    {
        std::array<double, 5> v{};
        for (int j = 0; j < 5; ++j)
            v[static_cast<std::size_t>(j)] = st->Float(kStatKeys[j], 0.0);
        it.stat = v;
    }
    if (const auto sr = t.Table(std::string("m_Stat_Relation_Level")))
    {
        std::array<std::int64_t, 5> v{};
        for (int j = 0; j < 5; ++j)
            v[static_cast<std::size_t>(j)] = sr->Int(kStatRelKeys[j], 0) & 0xFF;
        it.statRel = v;
    }
    if (const auto bf = t.Table(std::string("BUFF_FACTOR")))
        it.buffFactor = bf->SeqInts(false);
    if (const auto sal = t.Table(std::string("m_SpecialAbilityList")))
        for (const LuaTable& sa : sal->SeqTables())
            it.specialAbilities.push_back({sa.Int("TYPE", 0), sa.Int("COOL_TIME", 0), sa.Int("VALUE1", 0),
                                           sa.Int("VALUE2", 0), sa.Int("VALUE3", 0),
                                           sa.WStr("STRING_VALUE1", Ascii("Group 0"))});
    if (const auto so = t.Table(std::string("SOCKET_OPTION")))
        it.socket = so->SeqInts(true);
    if (const auto so = t.Table(std::string("RANDOM_SOCKET_GROUP_ID")))
        it.randomSocket = so->SeqInts(true);
    for (const auto& [key, nk, list] : {std::tuple{"ATTACHED_PARTICLE", "PARTICLE_NAME", &it.particles},
                                        std::tuple{"ATTACHED_MESH", "MESH_NAME", &it.meshes}})
        if (const auto ap = t.Table(std::string(key)))
            for (const LuaTable& e : ap->SeqTables())
                list->push_back({e.WStr(nk), e.WStr("BONE_NAME")});
    for (int ty = 0; ty < 4; ++ty)
        if (const auto c = t.Table(std::string(kSttKeys[ty])))
        {
            double vals[4] = {1.0, 0.0, 0.0, 0.0};
            for (int j = 0; j < 4; ++j)
            {
                const auto n = c->Number(static_cast<double>(j + 1));
                if (!n)
                    break;
                vals[j] = F32(F32(*n) / F32(255.0));
            }
            const auto comp = [](double x) -> std::uint32_t {
                if (x >= 1.0)
                    return 0xFF;
                if (x <= 0.0)
                    return 0;
                return static_cast<std::uint32_t>(CInt(F32(F32(x * 255.0) + 0.5), 32, false));
            };
            const std::uint32_t r = comp(vals[0]), g = comp(vals[1]), b = comp(vals[2]), a = comp(vals[3]);
            it.slash[ty] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    return it;
}

std::optional<SetCall> ReadSet(const LuaTable& t)
{
    if (!t.HasNumber("m_SetID"))
        return std::nullopt;
    SetCall s;
    s.id = static_cast<std::uint32_t>(t.Int("m_SetID", 0, 32, false));
    if (s.id == 0)
        return std::nullopt;
    for (int i = 1; i < 16; ++i)
        if (const std::int64_t o = t.Int("m_Option" + Str(i), 0))
            s.options.push_back(o);
    s.name = t.WStr("m_SetName");
    s.need = static_cast<std::uint32_t>(t.Int("m_NeedPartsNum", 0, 32, false));
    return s;
}

// The scripts' manager calls, as kim.py's dump driver records them.
constexpr const char* kDriver = R"(
local rec, want = ...
local missing, seen = {}, {}
for _, name in ipairs(want) do
  local t = rawget(_G, name)
  if type(t) == "table" and getmetatable(t) == nil then
    setmetatable(t, { __index = function(_, k)
      local key = name .. '["' .. tostring(k) .. '"]'
      if not seen[key] then seen[key] = true; missing[#missing + 1] = key end
    end })
  end
end
if TRUE == nil then TRUE = 1 end
if FALSE == nil then FALSE = 0 end
if True == nil then True = 1 end
if False == nil then False = 0 end
local function mk(kind)
  return function(self, t)
    if type(t) ~= "table" then t = self end
    rec(kind, t)
  end
end
local mgr = { AddItemTemplet = mk("I"), AddSetItemData_LUA = mk("S"),
              AddItemTempletTrans = mk("T"), AddSetItemDataTrans_LUA = mk("U") }
g_pItemManager = mgr
g_pCX2SetItemManager = mgr
return missing
)";

struct Recorder
{
    std::vector<Item> items;
    std::vector<SetCall> sets;
    std::size_t trans = 0;
};

int Record(lua_State* L)
{
    auto* r = static_cast<Recorder*>(lua_touserdata(L, lua_upvalueindex(1)));
    const std::string kind = luaL_checkstring(L, 1);
    std::string error;
    try
    {
        const LValue v = ReadValue(L, 2, 0);
        if (v.kind != LValue::Table)
            return 0;
        const LuaTable t(v.table);
        if (kind == "I")
        {
            if (auto it = ReadItem(t))
                r->items.push_back(std::move(*it));
        }
        else if (kind == "S")
        {
            if (auto s = ReadSet(t))
                r->sets.push_back(std::move(*s));
        }
        else
            ++r->trans;
        return 0;
    }
    catch (const std::exception& e)
    {
        error = e.what();
    }
    // no C++ exception may cross Lua frames: raise a Lua error instead
    lua_pushstring(L, error.c_str());
    return lua_error(L);
}

// The pieces kim.py feeds LuaJIT for one script: Lua 5.1 escapes rewritten and, for big item scripts, cut at the
// top-level manager calls (one LuaJIT chunk holds at most 65536 constants).
std::vector<std::string> ScriptPieces(std::string_view src, bool split)
{
    if (src.starts_with(kBom))
        src.remove_prefix(kBom.size());
    const std::string d = detail::Lua51Escapes(src);
    if (!split || d.size() <= (1u << 20))
        return {d};
    std::vector<std::string> pieces;
    std::string cur;
    int n = 0;
    std::size_t i = 0;
    while (i < d.size())
    {
        std::size_t e = i;
        while (e < d.size() && d[e] != '\n' && d[e] != '\r')
            ++e;
        if (e < d.size())
            e += (d[e] == '\r' && e + 1 < d.size() && d[e + 1] == '\n') ? 2 : 1;
        const std::string_view line(d.data() + i, e - i);
        if (line.starts_with("g_pItemManager") || line.starts_with("g_pCX2SetItemManager"))
        {
            if (++n > 1000)
            {
                pieces.push_back(std::move(cur));
                cur.clear();
                n = 1;
            }
        }
        cur.append(line);
        i = e;
    }
    pieces.push_back(std::move(cur));
    return pieces;
}

bool RunEnums(lua_State* L, const std::vector<std::string>& enumSources, std::string& error)
{
    for (const std::string& src : enumSources)
        for (const std::string& piece : ScriptPieces(src, false))
            if (!detail::RunChunk(L, piece, "=Enum.lua", error))
                return false;
    return true;
}

} // namespace

bool LoadEnums(const std::vector<std::string>& enumSources, Enums& enums, std::string& error)
{
    enums = {};
    detail::LuaState L;
    if (!L || !RunEnums(L, enumSources, error))
        return error.empty() ? (error = "cannot create a Lua state", false) : false;
    for (const std::string& name : WantedEnums())
    {
        lua_getglobal(L, name.c_str());
        if (lua_istable(L, -1))
        {
            auto& table = enums.tables[name];
            lua_pushnil(L);
            while (lua_next(L, -2))
            {
                if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TNUMBER)
                {
                    std::size_t n = 0;
                    const char* k = lua_tolstring(L, -2, &n);
                    table[std::string(k, n)] = lua_tonumber(L, -1);
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
    }
    lua_getglobal(L, "True");
    if (lua_type(L, -1) == LUA_TNUMBER)
        enums.tables["_G"]["True"] = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return true;
}

bool DecompileKim(std::span<const std::uint8_t> kim, const Enums& enums, ItemScripts& scripts, std::string& error)
{
    try
    {
        const Parsed p = ParseKim(kim);
        const EnumNames en(enums);
        const std::string note = std::string("-- Generated from Item.kim by x2_assets") +
                                 (enums.Empty() ? "." : "; uses the enum names of Enum.lua.") + "\n\n\n";
        std::string items = std::string(kBom) + Crlf(note);
        for (const Item& it : p.items)
            items += Crlf(ItemToLua(it, en));
        scripts.items = std::move(items);
        scripts.setItems = std::string(kBom) + Crlf(note) + Crlf(SetsToLua(p));
        scripts.itemCount = p.items.size();
        scripts.setCount = p.sets.size();
        return true;
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return false;
    }
}

bool CompileKim(const std::vector<std::string>& scripts, const std::vector<std::string>& enumSources,
                std::vector<std::uint8_t>& kim, std::vector<std::string>& warnings, std::string& error)
{
    try
    {
        detail::LuaState L;
        if (!L)
        {
            error = "cannot create a Lua state";
            return false;
        }
        if (!RunEnums(L, enumSources, error))
            return false;
        Recorder rec;
        if (luaL_loadbuffer(L, kDriver, std::strlen(kDriver), "=kim driver") != 0)
        {
            error = lua_tostring(L, -1);
            return false;
        }
        lua_pushlightuserdata(L, &rec);
        lua_pushcclosure(L, Record, 1);
        lua_newtable(L);
        int i = 1;
        for (const std::string& name : WantedEnums())
        {
            lua_pushstring(L, name.c_str());
            lua_rawseti(L, -2, i++);
        }
        if (lua_pcall(L, 2, 1, 0) != 0)
        {
            error = lua_tostring(L, -1);
            return false;
        }
        const int missing = lua_gettop(L);
        for (std::size_t s = 0; s < scripts.size(); ++s)
            for (const std::string& piece : ScriptPieces(scripts[s], true))
                if (!detail::RunChunk(L, piece, s == 0 ? "=Item.lua" : "=SetItem.lua", error))
                    return false;
        std::vector<std::string> missingNames;
        for (int k = 1;; ++k)
        {
            lua_rawgeti(L, missing, k);
            if (!lua_isstring(L, -1))
            {
                lua_pop(L, 1);
                break;
            }
            missingNames.push_back(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        if (!missingNames.empty())
        {
            std::string list;
            for (std::size_t k = 0; k < missingNames.size() && k < 10; ++k)
                list += (k ? ", " : "") + missingNames[k];
            warnings.push_back(Str(static_cast<long long>(missingNames.size())) +
                               " enum name(s) not defined by Enum.lua (the field falls back to its default, as in "
                               "the official tool): " + list);
        }
        if (rec.trans)
            warnings.push_back(Str(static_cast<long long>(rec.trans)) +
                               " AddItemTempletTrans/AddSetItemDataTrans_LUA calls ignored");
        std::vector<std::string> log;
        kim = BuildKim(rec.items, rec.sets, log);
        warnings.insert(warnings.end(), log.begin(), log.end());
        return true;
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return false;
    }
}

bool CheckSyntax(std::string_view source, std::string_view chunkName, std::string& error)
{
    detail::LuaState L;
    if (!L)
    {
        error = "cannot create a Lua state";
        return false;
    }
    if (source.starts_with(kBom))
        source.remove_prefix(kBom.size());
    const std::string name = "=" + std::string(chunkName);
    if (luaL_loadbuffer(L, source.data(), source.size(), name.c_str()) != 0)
    {
        error = lua_tostring(L, -1);
        return false;
    }
    return true;
}

} // namespace x2::lua
