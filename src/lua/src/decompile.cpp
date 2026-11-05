// A port of tools/kom/kom.py's LuaJIT 2.0 decompiler, which stays the reference: both must write the same text
// for the same bytecode (tested over every script of the client). Hence the Python-shaped structure: expressions
// and pending temporaries are shared (shared_ptr, like Python references) between the state snapshots the analysis
// rewinds to, and rendering keeps kom.py's order of evaluation, since rendering a function changes the indent that
// later long tables use.
#include "x2/lua/decompile.h"

#include "x2/lua/bytecode.h"

#include "reader.h"

#include <algorithm>
#include <bit>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <variant>

namespace x2::lua {

namespace {

// ---------------------------------------------------------------- opcodes (LuaJIT 2.0)

enum Op : int
{
    ISLT, ISGE, ISLE, ISGT, ISEQV, ISNEV, ISEQS, ISNES, ISEQN, ISNEN, ISEQP, ISNEP,
    ISTC, ISFC, IST, ISF, MOV, NOT, UNM, LEN,
    ADDVN, SUBVN, MULVN, DIVVN, MODVN, ADDNV, SUBNV, MULNV, DIVNV, MODNV, ADDVV, SUBVV, MULVV, DIVVV, MODVV,
    POW, CAT, KSTR, KCDATA, KSHORT, KNUM, KPRI, KNIL, UGET, USETV, USETS, USETN, USETP, UCLO,
    FNEW, TNEW, TDUP, GGET, GSET, TGETV, TGETS, TGETB, TSETV, TSETS, TSETB, TSETM,
    CALLM, CALL, CALLMT, CALLT, ITERC, ITERN, VARG, ISNEXT, RETM, RET, RET0, RET1,
    FORI, JFORI, FORL, IFORL, JFORL, ITERL, IITERL, JITERL, LOOP, ILOOP, JLOOP, JMP,
    FUNCF, IFUNCF, JFUNCF, FUNCV, IFUNCV, JFUNCV, FUNCC, FUNCCW,
    kNumOps
};

constexpr const char* kOpNames[kNumOps] = {
    "ISLT", "ISGE", "ISLE", "ISGT", "ISEQV", "ISNEV", "ISEQS", "ISNES", "ISEQN", "ISNEN", "ISEQP", "ISNEP",
    "ISTC", "ISFC", "IST", "ISF", "MOV", "NOT", "UNM", "LEN",
    "ADDVN", "SUBVN", "MULVN", "DIVVN", "MODVN", "ADDNV", "SUBNV", "MULNV", "DIVNV", "MODNV",
    "ADDVV", "SUBVV", "MULVV", "DIVVV", "MODVV",
    "POW", "CAT", "KSTR", "KCDATA", "KSHORT", "KNUM", "KPRI", "KNIL", "UGET", "USETV", "USETS", "USETN", "USETP",
    "UCLO", "FNEW", "TNEW", "TDUP", "GGET", "GSET", "TGETV", "TGETS", "TGETB", "TSETV", "TSETS", "TSETB", "TSETM",
    "CALLM", "CALL", "CALLMT", "CALLT", "ITERC", "ITERN", "VARG", "ISNEXT", "RETM", "RET", "RET0", "RET1",
    "FORI", "JFORI", "FORL", "IFORL", "JFORL", "ITERL", "IITERL", "JITERL", "LOOP", "ILOOP", "JLOOP", "JMP",
    "FUNCF", "IFUNCF", "JFUNCF", "FUNCV", "IFUNCV", "JFUNCV", "FUNCC", "FUNCCW"};

bool IsTest(int op) { return op <= ISF; }

bool IsAnyOf(int op, std::initializer_list<int> ops) { return std::find(ops.begin(), ops.end(), op) != ops.end(); }

constexpr int kProtoVararg = 0x02;
constexpr int kProtoHasReturn = 0x20;

// ---------------------------------------------------------------- errors

// What kom.py reports as DecompileError: the script is kept as bytecode. Some analysis steps catch it and try
// another reading.
struct DecompileError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Where kom.py would stop with a Python error (IndexError, KeyError, RecursionError, ...): never caught inside.
struct Fatal : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Restart the function's analysis with the temporary defined at `pc` made a local.
struct ForceLocal
{
    int pc;
};

std::string Str(long long v) { return std::to_string(v); }

// ---------------------------------------------------------------- constants

struct Value
{
    enum Kind { Nil, False, True, Num, Str } kind = Nil;
    double num = 0;
    std::string str;

    static Value Number(double v) { return {Num, v, {}}; }
    static Value String(std::string s) { return {Str, 0, std::move(s)}; }
    static Value Primitive(long d)
    {
        if (d < 0 || d > 2)
            throw Fatal("bad primitive constant");
        return {static_cast<Kind>(d), 0, {}};
    }
};

struct Proto;
using ProtoP = std::shared_ptr<Proto>;

struct TableConst
{
    std::vector<Value> arr;
    std::vector<std::pair<Value, Value>> hash;
};

using Kgc = std::variant<std::string, TableConst, ProtoP>;

struct Proto
{
    int flags = 0, nparams = 0, framesize = 0;
    std::vector<std::uint32_t> bc;
    std::vector<std::uint16_t> uv;
    std::vector<Kgc> kgc;
    std::vector<double> kn;
};

double Dbl(std::uint32_t lo, std::uint32_t hi)
{
    const std::uint64_t bits = (static_cast<std::uint64_t>(hi) << 32) | lo;
    return std::bit_cast<double>(bits);
}

std::int32_t I32(std::uint32_t v) { return static_cast<std::int32_t>(v); }

Value TabKey(detail::Reader& r)
{
    const std::uint32_t tp = r.Uleb();
    if (tp >= 5)
    {
        const auto b = r.Bytes(tp - 5);
        return Value::String(std::string(b.begin(), b.end()));
    }
    if (tp == 3)
        return Value::Number(I32(r.Uleb()));
    if (tp == 4)
    {
        const std::uint32_t lo = r.Uleb();
        return Value::Number(Dbl(lo, r.Uleb()));
    }
    return Value::Primitive(tp);
}

ProtoP ParseBytecode(std::span<const std::uint8_t> data)
{
    detail::Reader r(data);
    try
    {
        const auto magic = r.Bytes(3);
        if (!(magic[0] == 0x1B && ((magic[1] == 'K' && magic[2] == 'L') || (magic[1] == 'L' && magic[2] == 'J'))))
            throw DecompileError("not a LuaJIT bytecode dump");
        r.Byte();
        const std::uint32_t flags = r.Uleb();
        if (flags & 1)
            throw DecompileError("big-endian dump not supported");
        if (!(flags & 2))
            r.Skip(r.Uleb());  // chunk name
        std::vector<ProtoP> stack;
        for (;;)
        {
            const std::uint32_t length = r.Uleb();
            if (length == 0)
                break;
            const std::size_t end = r.Pos() + length;
            auto p = std::make_shared<Proto>();
            p->flags = r.Byte();
            p->nparams = r.Byte();
            p->framesize = r.Byte();
            const int nuv = r.Byte();
            const std::uint32_t nkgc = r.Uleb(), nkn = r.Uleb(), nbc = r.Uleb();
            if (!(flags & 2) && r.Uleb())
                throw DecompileError("debug info not supported");
            for (std::uint32_t i = 0; i < nbc; ++i)
            {
                const std::uint32_t ins = r.U32();
                if ((ins & 0xFF) >= kNumOps)
                    throw DecompileError("unknown opcode " + Str(ins & 0xFF));
                p->bc.push_back(ins);
            }
            for (int i = 0; i < nuv; ++i)
                p->uv.push_back(r.U16());
            std::vector<Kgc> kgc;
            for (std::uint32_t i = 0; i < nkgc; ++i)
            {
                const std::uint32_t tp = r.Uleb();
                if (tp >= 5)
                {
                    const auto b = r.Bytes(tp - 5);
                    kgc.emplace_back(std::string(b.begin(), b.end()));
                }
                else if (tp == 1)
                {
                    const std::uint32_t na = r.Uleb(), nh = r.Uleb();
                    TableConst t;
                    for (std::uint32_t j = 0; j < na; ++j)
                        t.arr.push_back(TabKey(r));
                    for (std::uint32_t j = 0; j < nh; ++j)
                    {
                        Value k = TabKey(r);
                        Value v = TabKey(r);
                        t.hash.emplace_back(std::move(k), std::move(v));
                    }
                    kgc.emplace_back(std::move(t));
                }
                else if (tp == 0)
                {
                    if (stack.empty())
                        throw DecompileError("child prototype missing");
                    kgc.emplace_back(stack.back());
                    stack.pop_back();
                }
                else
                {
                    throw DecompileError("cdata constants not supported");
                }
            }
            p->kgc.assign(kgc.rbegin(), kgc.rend());
            for (std::uint32_t i = 0; i < nkn; ++i)
            {
                const bool isnum = r.Peek() & 1;
                const std::uint32_t lo = r.Uleb33();
                p->kn.push_back(isnum ? Dbl(lo, r.Uleb()) : static_cast<double>(I32(lo)));
            }
            if (r.Pos() != end)
                throw DecompileError("bad prototype length");
            stack.push_back(std::move(p));
        }
        if (r.Pos() != data.size() || stack.size() != 1)
            throw DecompileError("trailing data in dump");
        return stack.front();
    }
    catch (const detail::Truncated&)
    {
        throw DecompileError("truncated bytecode");
    }
}

// ---------------------------------------------------------------- text helpers

constexpr int PREC_OR = 1, PREC_AND = 2, PREC_CMP = 3, PREC_CAT = 5, PREC_ADD = 6, PREC_MUL = 7, PREC_UN = 8,
              PREC_POW = 10, PREC_ATOM = 12;

bool IsKeyword(std::string_view s)
{
    static constexpr std::string_view kKeywords[] = {"and", "break", "do", "else", "elseif", "end", "false",
                                                     "for", "function", "goto", "if", "in", "local", "nil",
                                                     "not", "or", "repeat", "return", "then", "true", "until",
                                                     "while"};
    return std::find(std::begin(kKeywords), std::end(kKeywords), s) != std::end(kKeywords);
}

// kom.py's is_ident: ^[A-Za-z_][A-Za-z0-9_]*$ on ASCII, not a keyword. Python's $ also matches before one
// trailing newline, so "name\n" passes as well.
bool IsIdent(std::string_view s)
{
    for (const char c : s)
        if (static_cast<unsigned char>(c) >= 0x80)
            return false;
    std::string_view core = s;
    if (!core.empty() && core.back() == '\n')
        core.remove_suffix(1);
    if (core.empty())
        return false;
    const auto alpha = [](char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_'; };
    if (!alpha(core[0]))
        return false;
    for (const char c : core.substr(1))
        if (!alpha(c) && !(c >= '0' && c <= '9'))
            return false;
    return !IsKeyword(s);
}

// Length of the valid UTF-8 sequence at b[i] (Python's strict decoder), or 0.
std::size_t Utf8Sequence(std::string_view b, std::size_t i)
{
    const auto c = static_cast<unsigned char>(b[i]);
    const std::size_t n = (c >= 0xC2 && c <= 0xDF) ? 2 : (c >= 0xE0 && c <= 0xEF) ? 3 : (c >= 0xF0 && c <= 0xF4) ? 4 : 0;
    if (!n || i + n > b.size())
        return 0;
    unsigned char lo = 0x80, hi = 0xBF;
    if (c == 0xE0)
        lo = 0xA0;
    else if (c == 0xED)
        hi = 0x9F;
    else if (c == 0xF0)
        lo = 0x90;
    else if (c == 0xF4)
        hi = 0x8F;
    const auto c1 = static_cast<unsigned char>(b[i + 1]);
    if (c1 < lo || c1 > hi)
        return 0;
    for (std::size_t k = 2; k < n; ++k)
    {
        const auto ck = static_cast<unsigned char>(b[i + k]);
        if (ck < 0x80 || ck > 0xBF)
            return 0;
    }
    return n;
}

std::string LuaStr(std::string_view b)
{
    std::string out = "\"";
    std::size_t i = 0;
    const std::size_t n = b.size();
    while (i < n)
    {
        const auto c = static_cast<unsigned char>(b[i]);
        if (c >= 0x80)
        {
            if (const std::size_t len = Utf8Sequence(b, i))
            {
                out.append(b.substr(i, len));
                i += len;
                continue;
            }
            out += "\\" + Str(c);
        }
        else if (c == '"')
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
            const unsigned char next = i + 1 < n ? static_cast<unsigned char>(b[i + 1]) : 0;
            if (next >= '0' && next <= '9')
            {
                char buf[8];
                std::snprintf(buf, sizeof buf, "\\%03d", c);
                out += buf;
            }
            else
                out += "\\" + Str(c);
        }
        else
            out += static_cast<char>(c);
        ++i;
    }
    out += '"';
    return out;
}

// Python's repr(float): shortest round-trip digits, exponent form when the decimal point falls outside (-4, 16].
std::string PyRepr(double v)
{
    char buf[64];
    const auto res = std::to_chars(buf, buf + sizeof buf, v, std::chars_format::scientific);
    std::string_view s(buf, static_cast<std::size_t>(res.ptr - buf));
    std::string sign;
    if (!s.empty() && s[0] == '-')
    {
        sign = "-";
        s.remove_prefix(1);
    }
    const std::size_t e = s.find('e');
    std::string digits;
    for (const char c : s.substr(0, e))
        if (c != '.')
            digits += c;
    int exp = 0;
    std::from_chars(s.data() + e + 1 + (s[e + 1] == '+' ? 1 : 0), s.data() + s.size(), exp);
    const int decpt = exp + 1;
    const int nd = static_cast<int>(digits.size());
    std::string out;
    if (decpt > -4 && decpt <= 16)
    {
        if (decpt <= 0)
            out = "0." + std::string(static_cast<std::size_t>(-decpt), '0') + digits;
        else if (decpt >= nd)
            out = digits + std::string(static_cast<std::size_t>(decpt - nd), '0') + ".0";
        else
            out = digits.substr(0, static_cast<std::size_t>(decpt)) + "." + digits.substr(static_cast<std::size_t>(decpt));
    }
    else
    {
        out = digits.substr(0, 1);
        if (nd > 1)
            out += "." + digits.substr(1);
        const int ae = std::abs(exp);
        out += exp < 0 ? "e-" : "e+";
        if (ae < 10)
            out += '0';
        out += Str(ae);
    }
    return sign + out;
}

// Python's `v == int(v)` (int() of NaN or infinity raises).
bool PyIsInt(double v)
{
    if (!std::isfinite(v))
        throw Fatal("integer conversion of a non-finite number");
    return v == std::trunc(v);
}

std::string LuaNum(double v)
{
    if (std::isnan(v))
        return "(0/0)";
    if (v == HUGE_VAL)
        return "(1/0)";
    if (v == -HUGE_VAL)
        return "(-1/0)";
    if (v == std::trunc(v) && std::fabs(v) < 1e15)
        return Str(static_cast<long long>(v));
    return PyRepr(v);
}

// Characters (code points) of UTF-8 text, as Python's len() of the decoded string.
std::size_t CodePoints(std::string_view s)
{
    std::size_t n = 0;
    for (const char c : s)
        if ((static_cast<unsigned char>(c) & 0xC0) != 0x80)
            ++n;
    return n;
}

std::string ReplaceAll(std::string s, std::string_view from, std::string_view to)
{
    std::size_t at = 0;
    while ((at = s.find(from, at)) != std::string::npos)
    {
        s.replace(at, from.size(), to);
        at += to.size();
    }
    return s;
}

// ---------------------------------------------------------------- AST

// Nesting limit for the recursive analysis and rendering: deeper scripts are reported, not decompiled, instead of
// overflowing the stack (kom.py raises its recursion limit to 20000 frames).
constexpr int kMaxDepth = 400;

struct Depth
{
    explicit Depth(int& d) : d_(d)
    {
        if (++d_ > kMaxDepth)
        {
            --d_;
            throw Fatal("nested too deeply");
        }
    }
    ~Depth() { --d_; }
    Depth(const Depth&) = delete;
    Depth& operator=(const Depth&) = delete;

private:
    int& d_;
};

// kom.py's _CUR_INDENT: the indent of the statement being rendered, which long tables use and function bodies
// change as they render.
struct Renderer
{
    std::string cur;
    int depth = 0;
};

enum class EK { Const, Name, LocalRef, Global, Vararg, Index, MethodRef, SelfArg, Call, BinOp, UnOp, Concat, Not,
                Cmp, Logic, Truth, Func, Table };

struct Expr;
using ExprP = std::shared_ptr<Expr>;

struct Expr
{
    explicit Expr(EK k) : kind(k) {}
    virtual ~Expr() = default;
    EK kind;
    int prec = PREC_ATOM;
    bool multi = false;  // a call/vararg that may yield several values

    virtual std::string Render(Renderer& R) = 0;
    virtual std::string R(Renderer& R, int p)
    {
        std::string s = Render(R);
        return prec < p ? "(" + s + ")" : s;
    }
    bool Is(EK k) const { return kind == k; }
};

struct Var
{
    std::string name;
};
using VarP = std::shared_ptr<Var>;

struct ConstE : Expr
{
    explicit ConstE(Value val) : Expr(EK::Const), v(std::move(val))
    {
        if (v.kind == Value::Num && (v.num < 0 || (v.num == 0 && std::signbit(v.num))))
            prec = PREC_UN;
    }
    Value v;
    std::string Render(Renderer&) override
    {
        switch (v.kind)
        {
        case Value::Nil: return "nil";
        case Value::True: return "true";
        case Value::False: return "false";
        case Value::Str: return LuaStr(v.str);
        case Value::Num: break;
        }
        return LuaNum(v.num);
    }
    bool IsNil() const { return v.kind == Value::Nil; }
};

ExprP Const(Value v) { return std::make_shared<ConstE>(std::move(v)); }
ExprP Num(double v) { return Const(Value::Number(v)); }
ExprP Nil() { return Const(Value()); }

bool IsNilConst(const ExprP& e) { return e->Is(EK::Const) && static_cast<ConstE&>(*e).IsNil(); }

struct NameE : Expr
{
    NameE(EK k, std::string n) : Expr(k), name(std::move(n)) {}
    std::string name;
    std::string Render(Renderer&) override { return name; }
};

struct LocalRefE : Expr
{
    explicit LocalRefE(VarP v) : Expr(EK::LocalRef), var(std::move(v)) {}
    VarP var;
    std::string Render(Renderer&) override { return var->name; }
};

struct VarargE : Expr
{
    explicit VarargE(bool m) : Expr(EK::Vararg) { multi = m; }
    std::string Render(Renderer&) override { return "..."; }
};

bool IsAtomicWrapped(const ExprP& e) { return e->Is(EK::Const) || e->Is(EK::Func) || e->Is(EK::Table); }

struct IndexE : Expr
{
    IndexE(ExprP o, ExprP k) : Expr(EK::Index), obj(std::move(o)), key(std::move(k)) {}
    ExprP obj, key;
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        std::string o = obj->R(R, PREC_ATOM);
        if (IsAtomicWrapped(obj) || o.starts_with("("))
            if (!o.starts_with("("))
                o = "(" + obj->Render(R) + ")";
        if (key->Is(EK::Const))
        {
            const Value& kv = static_cast<ConstE&>(*key).v;
            if (kv.kind == Value::Str && IsIdent(kv.str))
                return o + "." + kv.str;
        }
        return o + "[" + key->Render(R) + "]";
    }
};

struct MethodRefE : Expr
{
    MethodRefE(ExprP o, std::string n) : Expr(EK::MethodRef), obj(std::move(o)), name(std::move(n)) {}
    ExprP obj;
    std::string name;
    std::string Render(Renderer&) override { throw Fatal("a method reference has no rendering"); }
};

struct SelfArgE : Expr
{
    SelfArgE() : Expr(EK::SelfArg) {}
    std::string Render(Renderer&) override { throw Fatal("a self argument has no rendering"); }
};

struct CallE : Expr
{
    CallE(ExprP f, std::vector<ExprP> a) : Expr(EK::Call), func(std::move(f)), args(std::move(a)) {}
    ExprP func;
    std::vector<ExprP> args;
    std::string Args(Renderer& R)
    {
        std::string s;
        for (std::size_t i = 0; i < args.size(); ++i)
            s += (i ? ", " : "") + args[i]->Render(R);
        return s;
    }
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        if (func->Is(EK::MethodRef))
        {
            auto& m = static_cast<MethodRefE&>(*func);
            std::string o = m.obj->R(R, PREC_ATOM);
            if (IsAtomicWrapped(m.obj) && !o.starts_with("("))
                o = "(" + o + ")";
            return o + ":" + m.name + "(" + Args(R) + ")";
        }
        std::string f = func->R(R, PREC_ATOM);
        if (IsAtomicWrapped(func) && !f.starts_with("("))
            f = "(" + f + ")";
        return f + "(" + Args(R) + ")";
    }
};

struct BinOpE : Expr
{
    BinOpE(std::string o, ExprP x, ExprP y, int p, bool r) : Expr(EK::BinOp), op(std::move(o)), a(std::move(x)),
                                                              b(std::move(y)), right(r)
    {
        prec = p;
    }
    std::string op;
    ExprP a, b;
    bool right;
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        if (right)
        {
            std::string sa = a->R(R, prec + 1);
            return sa + " " + op + " " + b->R(R, prec);
        }
        std::string sa = a->R(R, prec);
        return sa + " " + op + " " + b->R(R, prec + 1);
    }
};

struct UnOpE : Expr
{
    UnOpE(std::string o, ExprP x) : Expr(EK::UnOp), op(std::move(o)), a(std::move(x)) { prec = PREC_UN; }
    std::string op;
    ExprP a;
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        std::string inner = a->R(R, PREC_UN);
        if (op == "-" && inner.starts_with("-"))
            inner = "(" + inner + ")";
        return op + (op == "not" ? " " : "") + inner;
    }
};

struct ConcatE : Expr
{
    explicit ConcatE(std::vector<ExprP> p) : Expr(EK::Concat), parts(std::move(p)) { prec = PREC_CAT; }
    std::vector<ExprP> parts;
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        std::string s;
        for (std::size_t i = 0; i < parts.size(); ++i)
            s += (i ? " .. " : "") + parts[i]->R(R, PREC_CAT + 1);
        return s;
    }
};

struct NotE : Expr
{
    explicit NotE(ExprP x) : Expr(EK::Not), a(std::move(x)) { prec = PREC_UN; }
    ExprP a;
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        return "not " + a->R(R, PREC_UN);
    }
};

// A comparison as the test instruction emits it (op is the bytecode op).
struct CmpE : Expr
{
    CmpE(int o, ExprP x, ExprP y, bool s) : Expr(EK::Cmp), op(o), a(std::move(x)), b(std::move(y)), swap(s)
    {
        prec = PREC_CMP;
    }
    int op;
    ExprP a, b;
    bool swap;
    std::shared_ptr<CmpE> Negate() const { return std::make_shared<CmpE>(op ^ 1, a, b, swap); }
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        if (op == ISLT || op == ISLE)
        {
            const std::string sym = op == ISLT ? "<" : "<=";
            if (swap)
            {
                std::string sb = b->R(R, PREC_CMP + 1);
                return sb + " " + (op == ISLT ? ">" : ">=") + " " + a->R(R, PREC_CMP + 1);
            }
            std::string sa = a->R(R, PREC_CMP + 1);
            return sa + " " + sym + " " + b->R(R, PREC_CMP + 1);
        }
        if (op == ISGE || op == ISGT)
            return "not (" + Negate()->Render(R) + ")";
        const std::string sym = (op == ISEQV || op == ISEQS || op == ISEQN || op == ISEQP) ? "==" : "~=";
        std::string sa = a->R(R, PREC_CMP + 1);
        return sa + " " + sym + " " + b->R(R, PREC_CMP + 1);
    }
};

struct LogicE : Expr
{
    LogicE(std::string o, ExprP x, ExprP y) : Expr(EK::Logic), op(std::move(o)), a(std::move(x)), b(std::move(y))
    {
        prec = op == "and" ? PREC_AND : PREC_OR;
    }
    std::string op;
    ExprP a, b;
    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        std::string sa = a->R(R, prec);
        return sa + " " + op + " " + b->R(R, prec + 1);
    }
};

// Truthiness test of a value (inside condition trees only).
struct TruthE : Expr
{
    TruthE(ExprP x, bool n) : Expr(EK::Truth), e(std::move(x)), neg(n) {}
    ExprP e;
    bool neg;
    std::string Render(Renderer&) override { throw Fatal("a truth test has no rendering"); }
};

// ---------------------------------------------------------------- statements

struct Stmt;
using StmtP = std::shared_ptr<Stmt>;
using Block = std::shared_ptr<std::vector<StmtP>>;

Block NewBlock() { return std::make_shared<std::vector<StmtP>>(); }

std::string RenderBlock(Renderer& R, const Block& stmts, const std::string& ind);

struct FuncE : Expr
{
    FuncE(std::vector<std::string> p, bool va, Block b) : Expr(EK::Func), params(std::move(p)), vararg(va),
                                                          body(std::move(b))
    {
    }
    std::vector<std::string> params;
    bool vararg;
    Block body;
    bool selfref = false;
    std::function<void(const VarP&)> fixSelf;

    std::string RenderFunc(Renderer& R, const std::string& name, const std::string& indent)
    {
        Depth guard(R.depth);
        std::string ps;
        for (std::size_t i = 0; i < params.size(); ++i)
            ps += (i ? ", " : "") + params[i];
        if (vararg)
            ps += params.empty() ? "..." : ", ...";
        const std::string head = "function" + (name.empty() ? "" : " " + name) + "(" + ps + ")";
        const std::string inner = RenderBlock(R, body, indent + "\t");
        if (inner.empty())
            return head + " end";
        return head + "\n" + inner + indent + "end";
    }
    std::string Render(Renderer& R) override { return RenderFunc(R, "", ""); }
    std::string R(Renderer& R, int) override
    {
        const std::string indent = R.cur;
        return RenderFunc(R, "", indent);
    }
};

struct TableE : Expr
{
    TableE() : Expr(EK::Table) {}
    struct Event
    {
        bool pos;
        int idx;
        ExprP key, value;
    };
    std::map<int, ExprP> arr;                      // positional index (1..n) -> value
    std::vector<std::pair<ExprP, ExprP>> hash;     // template (constant) hash entries
    std::vector<Event> events;                     // dynamic fills in evaluation order
    ExprP tail;                                    // multres expression appended positionally
    std::optional<int> hint;                       // TNEW size hint

    static std::string Field(Renderer& R, const ExprP& k, const ExprP& v)
    {
        if (k->Is(EK::Const))
        {
            const Value& kv = static_cast<ConstE&>(*k).v;
            if (kv.kind == Value::Str && IsIdent(kv.str))
                return kv.str + " = " + v->Render(R);
        }
        std::string sk = k->Render(R);
        return "[" + sk + "] = " + v->Render(R);
    }

    std::string Render(Renderer& R) override
    {
        Depth guard(R.depth);
        const int n = arr.empty() ? 0 : arr.rbegin()->first;
        const auto lastIt = arr.find(n);
        const ExprP last = lastIt == arr.end() ? nullptr : lastIt->second;
        // a single-valued call/vararg must not end up as the last positional item, or it would expand to all its
        // results
        const bool singleTail = !tail && last && (last->Is(EK::Call) || last->Is(EK::Vararg)) && !last->multi &&
                                (events.empty() || (events.back().pos && events.back().idx == n));
        std::set<std::string> dynKeys;
        for (const Event& ev : events)
            if (!ev.pos)
                dynKeys.insert(ev.key->Render(R));
        std::vector<std::pair<ExprP, ExprP>> late, early;
        for (const auto& kv : hash)
            if (dynKeys.count(kv.first->Render(R)))
                late.push_back(kv);
        for (const auto& kv : hash)
            if (!dynKeys.count(kv.first->Render(R)))
                early.push_back(kv);
        std::vector<std::string> items;
        if (!(singleTail && !early.empty()))
            for (const auto& [k, v] : early)
                items.push_back(Field(R, k, v));
        int done = 0;
        const auto upto = [&](int i) {
            while (done < i)
            {
                ++done;
                const auto it = arr.find(done);
                items.push_back(it == arr.end() ? "nil" : it->second->Render(R));
            }
        };
        for (const Event& ev : events)
        {
            if (ev.pos)
                upto(ev.idx);
            else
                items.push_back(Field(R, ev.key, ev.value));
        }
        upto(n);
        for (const auto& [k, v] : late)
            items.push_back(Field(R, k, v));
        if (singleTail)
        {
            if (!early.empty())
                for (const auto& [k, v] : early)
                    items.push_back(Field(R, k, v));
            else
                items.back() = "(" + items.back() + ")";
        }
        if (tail)
            items.push_back(tail->Render(R));
        if (items.empty())
            return "{}";
        std::string s = "{ ";
        for (std::size_t i = 0; i < items.size(); ++i)
            s += (i ? ", " : "") + items[i];
        s += " }";
        if (CodePoints(s) > 100)
        {
            const std::string& ind = R.cur;
            std::string out = "{\n";
            for (const std::string& it : items)
                out += ind + "\t" + ReplaceAll(it, "\n", "\n\t") + ",\n";
            return out + ind + "}";
        }
        return s;
    }
};

struct Stmt
{
    enum Kind { Local, LocalFunc, Assign, CallStmt, Return, Break, If, While, Repeat, NumFor, GenFor, Do } kind;
    explicit Stmt(Kind k) : kind(k) {}
    std::vector<VarP> vars;           // Local, GenFor; LocalFunc: vars[0]; NumFor: vars[0]
    std::vector<ExprP> targets;       // Assign
    std::vector<ExprP> values;        // Local, Assign, Return; GenFor: its expressions
    ExprP expr;                       // CallStmt: the call; If/While/Repeat: the condition; LocalFunc: the function
    ExprP start, stop, step;          // NumFor
    Block body, orelse;               // If: then/else; loops and Do: body
};

StmtP MakeStmt(Stmt::Kind k) { return std::make_shared<Stmt>(k); }

std::string RenderList(Renderer& R, const std::vector<ExprP>& exprs)
{
    std::string s;
    for (std::size_t i = 0; i < exprs.size(); ++i)
        s += (i ? ", " : "") + exprs[i]->Render(R);
    return s;
}

std::string VarNames(const std::vector<VarP>& vars)
{
    std::string s;
    for (std::size_t i = 0; i < vars.size(); ++i)
        s += (i ? ", " : "") + vars[i]->name;
    return s;
}

// kom.py's _func_name: the dotted name of a function assignment target, if it has one.
std::string FuncName(Renderer& R, const ExprP& t)
{
    if (t->Is(EK::Global))
        return static_cast<NameE&>(*t).name;
    if (t->Is(EK::Index))
    {
        auto& ix = static_cast<IndexE&>(*t);
        if (ix.key->Is(EK::Const))
        {
            const Value& kv = static_cast<ConstE&>(*ix.key).v;
            if (kv.kind == Value::Str && IsIdent(kv.str))
            {
                const std::string base = ix.obj->Is(EK::LocalRef) ? ix.obj->Render(R) : FuncName(R, ix.obj);
                if (!base.empty())
                    return base + "." + kv.str;
            }
        }
    }
    return {};
}

std::string RenderStmt(Renderer& R, const Stmt& s, const std::string& ind)
{
    Depth guard(R.depth);
    R.cur = ind;
    switch (s.kind)
    {
    case Stmt::Local:
        if (s.values.empty())
            return ind + "local " + VarNames(s.vars) + "\n";
        return ind + "local " + VarNames(s.vars) + " = " + RenderList(R, s.values) + "\n";
    case Stmt::LocalFunc:
        return ind + "local " + static_cast<FuncE&>(*s.expr).RenderFunc(R, s.vars[0]->name, ind) + "\n";
    case Stmt::Assign:
        if (s.targets.size() == 1 && s.values.size() == 1 && s.values[0]->Is(EK::Func))
            if (const std::string name = FuncName(R, s.targets[0]); !name.empty())
                return ind + static_cast<FuncE&>(*s.values[0]).RenderFunc(R, FuncName(R, s.targets[0]), ind) +
                       "\n";
        {
            std::string targets = RenderList(R, s.targets);
            return ind + targets + " = " + RenderList(R, s.values) + "\n";
        }
    case Stmt::CallStmt:
        return ind + s.expr->Render(R) + "\n";
    case Stmt::Return:
        if (s.values.empty())
            return ind + "return\n";
        return ind + "return " + RenderList(R, s.values) + "\n";
    case Stmt::Break:
        return ind + "break\n";
    case Stmt::If: {
        std::string cond = s.expr->Render(R);
        std::string out = ind + "if " + cond + " then\n" + RenderBlock(R, s.body, ind + "\t");
        Block orelse = s.orelse;
        while (orelse && orelse->size() == 1 && (*orelse)[0]->kind == Stmt::If)
        {
            const Stmt& e = *(*orelse)[0];
            R.cur = ind;
            std::string c = e.expr->Render(R);
            out += ind + "elseif " + c + " then\n" + RenderBlock(R, e.body, ind + "\t");
            orelse = e.orelse;
        }
        if (orelse && !orelse->empty())
            out += ind + "else\n" + RenderBlock(R, orelse, ind + "\t");
        return out + ind + "end\n";
    }
    case Stmt::While: {
        std::string cond = s.expr->Render(R);
        std::string body = RenderBlock(R, s.body, ind + "\t");
        return ind + "while " + cond + " do\n" + body + ind + "end\n";
    }
    case Stmt::Repeat: {
        std::string body = RenderBlock(R, s.body, ind + "\t");
        R.cur = ind;
        return ind + "repeat\n" + body + ind + "until " + s.expr->Render(R) + "\n";
    }
    case Stmt::NumFor: {
        std::string start = s.start->Render(R);
        std::string range = start + ", " + s.stop->Render(R);
        if (s.step)
            range += ", " + s.step->Render(R);
        std::string body = RenderBlock(R, s.body, ind + "\t");
        return ind + "for " + s.vars[0]->name + " = " + range + " do\n" + body + ind + "end\n";
    }
    case Stmt::GenFor: {
        std::string exprs = RenderList(R, s.values);
        std::string body = RenderBlock(R, s.body, ind + "\t");
        return ind + "for " + VarNames(s.vars) + " in " + exprs + " do\n" + body + ind + "end\n";
    }
    case Stmt::Do: {
        std::string body = RenderBlock(R, s.body, ind + "\t");
        return ind + "do\n" + body + ind + "end\n";
    }
    }
    throw DecompileError("cannot render a statement");
}

std::string RenderBlock(Renderer& R, const Block& stmts, const std::string& ind)
{
    std::string out;
    for (const StmtP& s : *stmts)
        out += RenderStmt(R, *s, ind);
    return out;
}

// ---------------------------------------------------------------- conditions

ExprP Negate(const ExprP& c)
{
    if (c->Is(EK::Cmp))
        return static_cast<CmpE&>(*c).Negate();
    if (c->Is(EK::Truth))
    {
        auto& t = static_cast<TruthE&>(*c);
        return std::make_shared<TruthE>(t.e, !t.neg);
    }
    if (c->Is(EK::Logic))
    {
        auto& l = static_cast<LogicE&>(*c);
        ExprP a = Negate(l.a);
        return std::make_shared<LogicE>(l.op == "and" ? "or" : "and", a, Negate(l.b));
    }
    throw DecompileError("cannot negate");
}

// A condition tree (Cmp/Truth/Logic) as a renderable expression.
ExprP CondExpr(const ExprP& c)
{
    if (c->Is(EK::Truth))
    {
        auto& t = static_cast<TruthE&>(*c);
        return t.neg ? std::make_shared<NotE>(t.e) : t.e;
    }
    if (c->Is(EK::Logic))
    {
        auto& l = static_cast<LogicE&>(*c);
        ExprP a = CondExpr(l.a);
        return std::make_shared<LogicE>(l.op, a, CondExpr(l.b));
    }
    return c;
}

// ---------------------------------------------------------------- decompiler

struct Group
{
    enum Kind { None, Call, Part } kind = None;
    int slot = 0;
    bool operator==(const Group& o) const { return kind == o.kind && (kind != Part || slot == o.slot); }
};

struct Pending
{
    Pending(ExprP e, int p, int s) : expr(std::move(e)), pc(p), start(s) {}
    ExprP expr;
    int pc;
    Group group;
    int start;
};
using PendingP = std::shared_ptr<Pending>;

struct Ins
{
    int op, a, c, b, d;
};

struct Snapshot
{
    std::map<int, PendingP> pend;
    int nact;
    std::map<int, VarP> slotvars;
};

struct ChainItem
{
    ExprP cond;
    int target, start, next;
    Snapshot snap;
};

using Loop = std::optional<int>;  // the break target of the enclosing loop
using Ends = std::set<int>;

class FuncDecompiler
{
public:
    FuncDecompiler(const Proto& proto, std::vector<std::string> uvnames, int depth, int& nesting)
        : p_(proto), uvnames_(std::move(uvnames)), depth_(depth), nesting_(nesting)
    {
        for (const Kgc& k : p_.kgc)
            if (const auto* s = std::get_if<std::string>(&k))
                strs_.insert(*s);
    }

    std::shared_ptr<FuncE> Decompile()
    {
        Depth guard(nesting_);
        for (int attempt = 0; attempt < 500; ++attempt)
        {
            try
            {
                return DecompileOnce();
            }
            catch (const ForceLocal& e)
            {
                if (forced_.count(e.pc))
                    throw DecompileError("local inference did not converge at pc " + Str(e.pc));
                forced_.insert(e.pc);
            }
        }
        throw DecompileError("too many restarts");
    }

private:
    const Proto& p_;
    std::vector<std::string> uvnames_;
    int depth_;
    int& nesting_;
    std::set<int> forced_;
    std::set<std::string> strs_;

    std::set<std::string> uvset_;
    std::map<int, VarP> slotvars_;
    std::vector<std::string> params_;
    std::map<int, PendingP> pend_;
    std::optional<int> skipGap_;
    std::map<int, std::pair<Block, std::size_t>> declPos_;
    Ends curEnds_;
    std::map<int, int> consumed_;
    std::vector<int> cstarts_;
    int nact_ = 0;
    int n_ = 0;

    // -- helpers (bytecode indices follow Python's list indexing: negative counts from the end)
    std::uint32_t Bc(long pc) const
    {
        const long n = static_cast<long>(p_.bc.size());
        if (pc < 0)
            pc += n;
        if (pc < 0 || pc >= n)
            throw Fatal("bytecode index out of range");
        return p_.bc[static_cast<std::size_t>(pc)];
    }
    Ins I(long pc) const
    {
        const std::uint32_t i = Bc(pc);
        return {static_cast<int>(i & 0xFF), static_cast<int>((i >> 8) & 0xFF), static_cast<int>((i >> 16) & 0xFF),
                static_cast<int>(i >> 24), static_cast<int>(i >> 16)};
    }
    int OpAt(long pc) const { return static_cast<int>(Bc(pc) & 0xFF); }
    int Jt(long pc) const { return static_cast<int>(pc + 1 + static_cast<long>(Bc(pc) >> 16) - 0x8000); }

    const std::string& Kstr(long idx) const
    {
        if (idx < 0 || static_cast<std::size_t>(idx) >= p_.kgc.size())
            throw Fatal("constant index out of range");
        const auto* s = std::get_if<std::string>(&p_.kgc[static_cast<std::size_t>(idx)]);
        if (!s)
            throw DecompileError("expected string constant");
        return *s;
    }
    double Kn(long idx) const
    {
        if (idx < 0 || static_cast<std::size_t>(idx) >= p_.kn.size())
            throw Fatal("number constant index out of range");
        return p_.kn[static_cast<std::size_t>(idx)];
    }
    const std::string& UvName(long idx) const
    {
        if (idx < 0 || static_cast<std::size_t>(idx) >= uvnames_.size())
            throw Fatal("upvalue index out of range");
        return uvnames_[static_cast<std::size_t>(idx)];
    }
    PendingP& PendAt(int slot)
    {
        const auto it = pend_.find(slot);
        if (it == pend_.end())
            throw Fatal("no pending value in slot " + Str(slot));
        return it->second;
    }
    PendingP PendGet(int slot) const
    {
        const auto it = pend_.find(slot);
        return it == pend_.end() ? nullptr : it->second;
    }

    VarP GetVar(int slot)
    {
        const auto it = slotvars_.find(slot);
        return it != slotvars_.end() ? it->second : NewVar(slot);
    }

    VarP NewVar(int slot, std::string prefix = {})
    {
        if (prefix.empty())
            prefix = slot < p_.nparams ? "p" : "v";
        std::string base = (depth_ == 0 || prefix != "p") ? prefix + Str(slot)
                                                         : prefix + Str(slot) + "_" + Str(depth_);
        if (depth_ && prefix == "v")
            base = "v" + Str(slot) + "_" + Str(depth_);
        std::string name = base;
        for (int k = 1; strs_.count(name) || uvset_.count(name); ++k)
            name = base + "_" + Str(k);
        auto v = std::make_shared<Var>(Var{name});
        slotvars_[slot] = v;
        return v;
    }

    // -- entry
    std::shared_ptr<FuncE> DecompileOnce()
    {
        uvset_ = std::set<std::string>(uvnames_.begin(), uvnames_.end());
        slotvars_.clear();
        params_.clear();
        for (int i = 0; i < p_.nparams; ++i)
            params_.push_back(NewVar(i)->name);
        pend_.clear();
        skipGap_.reset();
        declPos_.clear();
        curEnds_.clear();
        consumed_.clear();
        cstarts_.clear();
        nact_ = p_.nparams;
        n_ = static_cast<int>(p_.bc.size());
        Block body = MakeBlock(0, n_, std::nullopt, {});
        // drop the implicit trailing RET0
        if (!body->empty() && body->back()->kind == Stmt::Return && body->back()->values.empty() &&
            OpAt(-1) == RET0 && !Ret0Explicit())
            body->pop_back();
        return std::make_shared<FuncE>(params_, (p_.flags & kProtoVararg) != 0, body);
    }

    bool Ret0Explicit() const
    {
        if (!(p_.flags & kProtoHasReturn))
            return false;
        for (std::size_t i = 0; i + 1 < p_.bc.size(); ++i)
            if (IsAnyOf(static_cast<int>(p_.bc[i] & 0xFF), {RET0, RET1, RET, RETM, CALLT, CALLMT}))
                return false;
        return true;
    }

    // -- pending temp management
    int Top() const
    {
        int t = nact_;
        while (pend_.count(t))
            ++t;
        return t;
    }

    ExprP Take(int slot, int pc)
    {
        if (slot < nact_)
            return std::make_shared<LocalRefE>(GetVar(slot));
        const auto it = pend_.find(slot);
        if (it == pend_.end())
        {
            if (const auto c = consumed_.find(slot); c != consumed_.end())
                throw ForceLocal{c->second};
            throw DecompileError("read of undefined slot " + Str(slot) + " at pc " + Str(pc));
        }
        PendingP pe = it->second;
        pend_.erase(it);
        // stack discipline: nothing above may remain pending
        if (!pend_.empty() && pend_.rbegin()->first > slot)
        {
            pend_[slot] = pe;
            throw ForceLocal{pe->pc};
        }
        if (pe->expr->Is(EK::SelfArg) || pe->expr->Is(EK::MethodRef) || pe->group.kind != Group::None)
            consumed_.erase(slot);
        else
            consumed_[slot] = pe->pc;
        cstarts_.push_back(pe->start);
        return pe->expr;
    }

    // Reads several operand slots; temps are consumed top-down.
    std::vector<ExprP> TakeMany(const std::vector<int>& slots, int pc)
    {
        std::set<int> unique(slots.begin(), slots.end());
        std::map<int, ExprP> vals;
        for (auto it = unique.rbegin(); it != unique.rend(); ++it)
            vals[*it] = Take(*it, pc);
        std::vector<ExprP> out;
        for (const int s : slots)
            out.push_back(vals[s]);
        return out;
    }

    void Define(int slot, ExprP expr, int pc, const Block& stmts)
    {
        if (slot < nact_ && pend_.empty() && TempUse(pc, slot))
            CloseDo(slot, stmts);
        if (slot < nact_)
        {
            Flush(stmts);
            auto s = MakeStmt(Stmt::Assign);
            s->targets = {std::make_shared<LocalRefE>(GetVar(slot))};
            s->values = {std::move(expr)};
            stmts->push_back(s);
            return;
        }
        if (const auto pe = PendGet(slot))
            throw ForceLocal{pe->pc};
        const int t = Top();
        if (slot > t && consumed_.count(t) && !pend_.count(t) && !LateFill(pc, slot, t) && skipGap_ != pc)
            throw ForceLocal{consumed_[t]};
        int start = pc;
        for (const int c : cstarts_)
            start = std::min(start, c);
        pend_[slot] = std::make_shared<Pending>(std::move(expr), pc, start);
        if (forced_.count(pc))
            Promote(slot, stmts);
    }

    // Is the value written to `slot` at pc used as a call frame base (so `slot` is a temporary, not a local)?
    bool TempUse(int pc, int slot) const
    {
        for (int q = pc + 1; q < n_ && q < pc + 400; ++q)
        {
            const Ins in = I(q);
            if (IsAnyOf(in.op, {CALL, CALLM, CALLT, CALLMT}) && in.a == slot)
                return true;
            if ((in.op == CALL || in.op == CALLM) && in.a < slot &&
                slot < in.a + 1 + (in.op == CALLM ? in.c : in.c - 1) + 1)
                return false;
            if (IsTest(in.op) || IsAnyOf(in.op, {JMP, FORI, FORL, LOOP, ITERL, UCLO, ISNEXT, RET, RET0, RET1, RETM,
                                                 GSET, TSETS, TSETB, TSETV, USETV, TSETM}))
                return false;
            if (in.op == MOV && in.d == slot)
                return false;
        }
        return false;
    }

    // Locals from `slot` up went out of scope: wrap their statements in an explicit do ... end block.
    void CloseDo(int slot, const Block& stmts)
    {
        const auto pos = declPos_.find(slot);
        if (pos == declPos_.end() || pos->second.first != stmts)
            throw DecompileError("scope end of local " + Str(slot) + " not in current block");
        const std::size_t idx = pos->second.second;
        if (idx > stmts->size())
            throw Fatal("bad declaration position");
        Block body = NewBlock();
        body->assign(stmts->begin() + static_cast<long>(idx), stmts->end());
        stmts->erase(stmts->begin() + static_cast<long>(idx), stmts->end());
        auto d = MakeStmt(Stmt::Do);
        d->body = body;
        stmts->push_back(d);
        for (auto it = declPos_.begin(); it != declPos_.end();)
            it = it->first >= slot ? declPos_.erase(it) : std::next(it);
        nact_ = slot;
    }

    // A relocatable expression may be emitted before a constant that gets the lower register (e.g. 'K > a.b'
    // compiles a.b first).
    bool LateFill(int pc, int slot, int t) const
    {
        if (slot != t + 1 || pc + 1 >= n_)
            return false;
        const Ins next = I(pc + 1);
        return IsAnyOf(next.op, {KSHORT, KNUM, KSTR, KPRI}) && next.a == t;
    }

    // Turns pending temps with slot <= upto into local declarations.
    void Promote(int upto, const Block& stmts)
    {
        std::vector<int> slots;
        for (const auto& [s, pe] : pend_)
            if (s <= upto)
                slots.push_back(s);
        if (slots.empty())
            return;
        if (slots[0] != nact_)
        {
            // gap: the slots in between hold locals without a visible definition
            for (int s = nact_; s < slots[0]; ++s)
            {
                declPos_[s] = {stmts, stmts->size()};
                auto l = MakeStmt(Stmt::Local);
                l->vars = {NewVar(s)};
                stmts->push_back(l);
            }
        }
        for (std::size_t i = 0; i < slots.size(); ++i)
        {
            const int s = slots[i];
            PendingP pe = PendAt(s);
            pend_.erase(s);
            std::vector<int> grp{s};
            std::vector<ExprP> vals{pe->expr};
            if (pe->group.kind == Group::Call)
            {
                while (i + 1 < slots.size() && PendAt(slots[i + 1])->group == Group{Group::Part, s})
                {
                    ++i;
                    grp.push_back(slots[i]);
                    pend_.erase(slots[i]);
                }
            }
            else if (IsNilConst(pe->expr))
            {
                while (i + 1 < slots.size() && IsNilConst(PendAt(slots[i + 1])->expr) &&
                       PendAt(slots[i + 1])->group.kind != Group::Call)
                {
                    ++i;
                    grp.push_back(slots[i]);
                    pend_.erase(slots[i]);
                }
                vals.clear();
            }
            if (pe->expr->Is(EK::Func) && static_cast<FuncE&>(*pe->expr).selfref)
            {
                auto& fe = static_cast<FuncE&>(*pe->expr);
                VarP v = NewVar(s);
                fe.fixSelf(v);
                declPos_[s] = {stmts, stmts->size()};
                auto lf = MakeStmt(Stmt::LocalFunc);
                lf->vars = {v};
                lf->expr = pe->expr;
                stmts->push_back(lf);
            }
            else
            {
                std::vector<VarP> vs;
                for (const int x : grp)
                    vs.push_back(NewVar(x));
                for (const int x : grp)
                    declPos_[x] = {stmts, stmts->size()};
                auto l = MakeStmt(Stmt::Local);
                l->vars = std::move(vs);
                l->values = std::move(vals);
                stmts->push_back(l);
            }
            nact_ = grp.back() + 1;
        }
    }

    // Statement boundary: every remaining pending temp is a local.
    void Flush(const Block& stmts)
    {
        if (!pend_.empty())
            Promote(pend_.rbegin()->first, stmts);
    }

    // -- blocks

    // Decompiles [start, end). ends: jump targets equivalent to `end` (LuaJIT merges the pending jumps of a
    // trailing if into the following JMP).
    Block MakeBlock(int start, int end, Loop loop, Ends ends)
    {
        Depth guard(nesting_);
        const Ends saved = curEnds_;
        ends.insert(end);
        // a JMP right at the block end makes its target equivalent to the end
        if (end < n_ && OpAt(end) == JMP)
            ends.insert(Jt(end));
        curEnds_ = ends;
        Block stmts = NewBlock();
        int pc = start;
        while (pc < end)
            pc = Step(pc, end, stmts, loop);
        if (!pend_.empty())
            Flush(stmts);
        curEnds_ = saved;
        return stmts;
    }

    Block SubBlock(int start, int end, Loop loop, Ends ends)
    {
        const int savedNact = nact_;
        const auto savedVars = slotvars_;
        if (!pend_.empty())
            throw DecompileError("pending temps across block boundary");
        Block body = MakeBlock(start, end, loop, std::move(ends));
        nact_ = savedNact;
        // restore outer names (inner declarations go out of scope)
        for (auto& [s, v] : slotvars_)
            if (const auto it = savedVars.find(s); it != savedVars.end())
                v = it->second;
        return body;
    }

    int Step(int pc, int end, const Block& stmts, Loop loop)
    {
        const Ins in = I(pc);
        if (IsTest(in.op))
            return DoCond(pc, end, stmts, loop);
        if (in.op == FORI)
            return DoNumFor(pc, stmts);
        if (in.op == LOOP)
            return DoLoop(pc, stmts);
        if (in.op == JMP || in.op == ISNEXT || in.op == UCLO)
        {
            const int tgt = Jt(pc);
            if (in.op == UCLO && tgt == pc + 1)
            {
                Flush(stmts);
                return pc + 1;
            }
            if ((in.op == JMP || in.op == ISNEXT) && tgt < n_ && IsAnyOf(OpAt(tgt), {ITERC, ITERN}) &&
                tgt + 1 < n_ && OpAt(tgt + 1) == ITERL && Jt(tgt + 1) == pc + 1)
                return DoGenFor(pc, stmts);
            if (loop && tgt == *loop)
            {
                Flush(stmts);
                stmts->push_back(MakeStmt(Stmt::Break));
                return pc + 1;
            }
            if (tgt == pc + 1)
            {
                Flush(stmts);
                return pc + 1;
            }
            throw DecompileError("unstructured jump at pc " + Str(pc));
        }
        cstarts_.clear();
        const std::optional<int> r = Simple(pc, stmts);
        return r ? *r : pc + 1;
    }

    // -- straight-line instructions
    void SimpleStep(int pc, const Block& stmts)
    {
        cstarts_.clear();
        Simple(pc, stmts);
    }

    std::optional<int> Simple(int pc, const Block& stmts)
    {
        const auto [op, a, c, b, d] = I(pc);
        switch (op)
        {
        case MOV: {
            if (pc + 1 < n_)
            {
                const Ins next = I(pc + 1);
                if (next.op == TGETS && next.a + 1 == a && next.b == d)
                    return std::nullopt;  // method call setup; handled by TGETS
                if (next.op == KSTR && pc + 2 < n_)
                {
                    const Ins n2 = I(pc + 2);
                    if (n2.op == TGETV && n2.a + 1 == a && n2.b == d && n2.c == a + 1)
                    {
                        skipGap_ = pc + 1;
                        return std::nullopt;
                    }
                }
            }
            if (const auto pe = PendGet(a); pe && a < d)
                throw ForceLocal{pe->pc};  // MOV into an unconsumed pending slot: that slot is a local
            if (d >= nact_)
                if (const auto pe = PendGet(d))
                    // x = f() with a local x: the call result is copied by MOV
                    if (!(a < nact_ && (pe->expr->Is(EK::Call) || pe->expr->Is(EK::Vararg)) &&
                          pe->group.kind == Group::None))
                        throw ForceLocal{pe->pc};
            ExprP v = Take(d, pc);
            Define(a, v, pc, stmts);
            break;
        }
        case KSTR:
            Define(a, Const(Value::String(Kstr(d))), pc, stmts);
            break;
        case KSHORT:
            Define(a, Num((d & 0x8000) ? d - 0x10000 : d), pc, stmts);
            break;
        case KNUM:
            Define(a, Num(Kn(d)), pc, stmts);
            break;
        case KPRI:
            Define(a, Const(Value::Primitive(d)), pc, stmts);
            break;
        case KNIL:
            for (int s = a; s <= d; ++s)
                Define(s, Nil(), pc, stmts);
            break;
        case UGET:
            Define(a, std::make_shared<NameE>(EK::Name, UvName(d)), pc, stmts);
            break;
        case GGET:
            Define(a, std::make_shared<NameE>(EK::Global, GName(d)), pc, stmts);
            break;
        case FNEW:
            Define(a, Closure(d, a), pc, stmts);
            break;
        case TNEW: {
            auto t = std::make_shared<TableE>();
            t->hint = d;
            Define(a, t, pc, stmts);
            break;
        }
        case TDUP:
            Define(a, Template(d), pc, stmts);
            break;
        case TGETS:
        case TGETB:
        case TGETV: {
            bool method = false;
            std::optional<Ins> prev;
            if (pc > 0)
                prev = I(pc - 1);
            if (op == TGETS && prev && prev->op == MOV && prev->a == a + 1 && prev->d == b)
                method = true;
            if (op == TGETV && pc >= 2 && prev->op == KSTR && prev->a == a + 2)
            {
                const Ins p2 = I(pc - 2);
                if (p2.op == MOV && p2.a == a + 1 && p2.d == b && c == a + 2)
                {
                    pend_.erase(c);
                    ExprP obj = Take(b, pc);
                    cstarts_.push_back(pc - 2);
                    Define(a, std::make_shared<MethodRefE>(obj, Kstr(prev->d)), pc, stmts);
                    pend_[a + 1] = std::make_shared<Pending>(std::make_shared<SelfArgE>(), pc, pc);
                    return std::nullopt;
                }
            }
            if (method)
            {
                if (a < nact_ && pend_.empty())
                    CloseDo(a, stmts);
                ExprP obj = Take(b, pc);
                cstarts_.push_back(pc - 1);
                Define(a, std::make_shared<MethodRefE>(obj, Kstr(c)), pc, stmts);
                pend_[a + 1] = std::make_shared<Pending>(std::make_shared<SelfArgE>(), pc, pc);
                return std::nullopt;
            }
            ExprP obj, key;
            if (op == TGETV)
            {
                auto v = TakeMany({b, c}, pc);
                obj = v[0];
                key = v[1];
            }
            else
            {
                key = op == TGETS ? Const(Value::String(Kstr(c))) : Num(c);
                obj = Take(b, pc);
            }
            Define(a, std::make_shared<IndexE>(obj, key), pc, stmts);
            break;
        }
        case UNM:
        case NOT:
        case LEN: {
            ExprP v = Take(d, pc);
            ExprP e = op == NOT ? ExprP(std::make_shared<NotE>(v))
                                : ExprP(std::make_shared<UnOpE>(op == UNM ? "-" : "#", v));
            Define(a, e, pc, stmts);
            break;
        }
        case ADDVN: case SUBVN: case MULVN: case DIVVN: case MODVN:
        case ADDNV: case SUBNV: case MULNV: case DIVNV: case MODNV:
        case ADDVV: case SUBVV: case MULVV: case DIVVV: case MODVV: {
            static constexpr const char* kSym[] = {"+", "-", "*", "/", "%"};
            const int k = op - ADDVN;
            const std::string sym = kSym[k % 5];
            const int prec = (sym == "+" || sym == "-") ? PREC_ADD : PREC_MUL;
            ExprP x, y;
            if (k / 5 == 2)
            {
                auto v = TakeMany({b, c}, pc);
                x = v[0];
                y = v[1];
            }
            else if (k / 5 == 0)
            {
                x = Take(b, pc);
                y = Num(Kn(c));
            }
            else
            {
                y = Take(b, pc);
                x = Num(Kn(c));
            }
            Define(a, std::make_shared<BinOpE>(sym, x, y, prec, false), pc, stmts);
            break;
        }
        case POW: {
            auto v = TakeMany({b, c}, pc);
            Define(a, std::make_shared<BinOpE>("^", v[0], v[1], PREC_POW, true), pc, stmts);
            break;
        }
        case CAT: {
            std::vector<ExprP> parts(static_cast<std::size_t>(std::max(c - b + 1, 0)));
            for (int s = c; s >= b; --s)
                parts[static_cast<std::size_t>(s - b)] = Take(s, pc);
            Define(a, std::make_shared<ConcatE>(std::move(parts)), pc, stmts);
            break;
        }
        case VARG:
            if (b == 0)
                Define(a, std::make_shared<VarargE>(true), pc, stmts);
            else if (b == 2)
                Define(a, std::make_shared<VarargE>(false), pc, stmts);
            else
                for (int s = a; s < a + b - 1; ++s)
                {
                    if (s == a)
                    {
                        Define(s, std::make_shared<VarargE>(true), pc, stmts);
                        PendAt(s)->group = {Group::Call, 0};
                    }
                    else
                    {
                        Define(s, Nil(), pc, stmts);
                        PendAt(s)->group = {Group::Part, a};
                    }
                }
            break;
        case GSET: case TSETS: case TSETB: case TSETV: case USETV: case USETS: case USETN: case USETP:
            return Store(pc, stmts);
        case TSETM: {
            const auto tbl = PendGet(a - 1);
            if (!tbl || !tbl->expr->Is(EK::Table))
                throw DecompileError("TSETM outside constructor");
            ExprP val = Take(a, pc);
            static_cast<TableE&>(*tbl->expr).tail = val;
            break;
        }
        case CALL:
        case CALLM:
            Call(pc, stmts);
            break;
        case CALLT:
        case CALLMT: {
            ExprP call = CallExpr(pc, a, d - 1, op == CALLMT);
            Flush(stmts);
            auto r = MakeStmt(Stmt::Return);
            r->values = {call};
            stmts->push_back(r);
            break;
        }
        case RET0: case RET1: case RET: case RETM: {
            std::vector<ExprP> vals;
            if (op == RET1)
            {
                const auto pe = PendGet(a);
                if (pe && pe->expr->Is(EK::Call))
                    throw ForceLocal{pe->pc};
                vals = {Take(a, pc)};
            }
            else if (op == RET)
            {
                if (d - 1 < 0)
                    throw Fatal("bad RET count");
                vals.resize(static_cast<std::size_t>(d - 1));
                for (int s = a + d - 2; s >= a; --s)
                    vals[static_cast<std::size_t>(s - a)] = Take(s, pc);
            }
            else if (op == RETM)
            {
                vals.resize(static_cast<std::size_t>(d + 1));
                vals[static_cast<std::size_t>(d)] = TakeMulti(a + d, pc);
                for (int s = a + d - 1; s >= a; --s)
                    vals[static_cast<std::size_t>(s - a)] = Take(s, pc);
            }
            Flush(stmts);
            auto r = MakeStmt(Stmt::Return);
            r->values = std::move(vals);
            stmts->push_back(r);
            break;
        }
        default:
            throw DecompileError(std::string("unhandled opcode ") + kOpNames[op] + " at pc " + Str(pc));
        }
        return std::nullopt;
    }

    std::string GName(int d) const
    {
        const std::string& name = Kstr(d);
        if (!IsIdent(name))
            throw DecompileError("non-identifier global name");
        return name;
    }

    ExprP TakeMulti(int slot, int pc)
    {
        const auto pe = PendGet(slot);
        if (!pe || !pe->expr->multi)
            throw DecompileError("expected multi-result at slot " + Str(slot) + " pc " + Str(pc));
        return Take(slot, pc);
    }

    ExprP CallExpr(int pc, int a, int nargs, bool multres)
    {
        // as kom.py: a negative count gives no arguments
        std::vector<ExprP> args(static_cast<std::size_t>(std::max(nargs, 0)));
        ExprP tail;
        if (multres)
            tail = TakeMulti(a + 1 + nargs, pc);
        for (int s = a + nargs; s > a; --s)
            args[static_cast<std::size_t>(s - a - 1)] = Take(s, pc);
        ExprP func = Take(a, pc);
        if (func->Is(EK::MethodRef))
        {
            if (args.empty() || !args[0]->Is(EK::SelfArg))
                throw DecompileError("broken method call");
            args.erase(args.begin());
        }
        if (tail)
            args.push_back(tail);
        for (const ExprP& x : args)
            if (x->Is(EK::SelfArg))
                throw DecompileError("stray self");
        return std::make_shared<CallE>(func, std::move(args));
    }

    void Call(int pc, const Block& stmts)
    {
        const auto [op, a, c, b, d] = I(pc);
        const int nargs = op == CALL ? c - 1 : c;
        ExprP call = CallExpr(pc, a, nargs, op == CALLM);
        if (b == 1)
        {
            Flush(stmts);
            auto s = MakeStmt(Stmt::CallStmt);
            s->expr = call;
            stmts->push_back(s);
        }
        else if (b == 0)
        {
            call->multi = true;
            Define(a, call, pc, stmts);
        }
        else if (b == 2)
        {
            Define(a, call, pc, stmts);
        }
        else
        {
            call->multi = true;
            Define(a, call, pc, stmts);
            PendAt(a)->group = {Group::Call, 0};
            for (int s = a + 1; s < a + b - 1; ++s)
            {
                Define(s, Nil(), pc, stmts);
                PendAt(s)->group = {Group::Part, a};
            }
        }
    }

    ExprP Template(int d) const
    {
        if (d < 0 || static_cast<std::size_t>(d) >= p_.kgc.size())
            throw Fatal("constant index out of range");
        const auto* k = std::get_if<TableConst>(&p_.kgc[static_cast<std::size_t>(d)]);
        if (!k)
            throw Fatal("TDUP of a constant that is not a table");
        auto t = std::make_shared<TableE>();
        if (!k->arr.empty() && k->arr[0].kind != Value::Nil)
            t->hash.emplace_back(Num(0.0), Const(k->arr[0]));
        for (std::size_t i = 1; i < k->arr.size(); ++i)
            if (k->arr[i].kind != Value::Nil)
                t->arr[static_cast<int>(i)] = Const(k->arr[i]);
        for (const auto& [kk, v] : k->hash)
        {
            ExprP key = Const(kk);
            t->hash.emplace_back(key, Const(v));
        }
        return t;
    }

    int Store(int pc, const Block& stmts)
    {
        {
            const auto [op, a, c, b, d] = I(pc);
            // table constructor fill
            if (op == TSETS || op == TSETB || op == TSETV)
            {
                const auto tp = PendGet(b);
                if (tp && tp->expr->Is(EK::Table) && (a > b || a < nact_))
                {
                    ExprP val, key;
                    if (op == TSETV)
                    {
                        if (c == b)
                            throw DecompileError("odd TSETV");
                        auto v = TakeMany({a, c}, pc);
                        val = v[0];
                        key = v[1];
                    }
                    else
                    {
                        val = Take(a, pc);
                        key = op == TSETS ? Const(Value::String(Kstr(c))) : Num(c);
                    }
                    auto& t = static_cast<TableE&>(*tp->expr);
                    if (!FillFits(t, op, key))
                        throw ForceLocal{tp->pc};
                    std::optional<int> idx;
                    if (op == TSETB)
                        idx = c;
                    if (op == TSETV && key->Is(EK::Const))
                    {
                        const Value& kv = static_cast<ConstE&>(*key).v;
                        if (kv.kind == Value::Num && PyIsInt(kv.num) && kv.num > 255 &&
                            static_cast<long long>(kv.num) == (t.arr.empty() ? 0 : t.arr.rbegin()->first) + 1)
                            idx = static_cast<int>(kv.num);
                    }
                    if (idx && *idx >= 1 && !t.arr.count(*idx) && !t.tail)
                    {
                        t.arr[*idx] = val;
                        t.events.push_back({true, *idx, nullptr, nullptr});
                    }
                    else
                    {
                        t.events.push_back({false, 0, key, val});
                    }
                    return pc + 1;
                }
            }
        }
        std::vector<ExprP> targets, values;
        for (;;)
        {
            const auto [op, a, c, b, d] = I(pc);
            ExprP val, tgt;
            if (op == GSET)
            {
                val = Take(a, pc);
                tgt = std::make_shared<NameE>(EK::Global, GName(d));
            }
            else if (op == USETV || op == USETS || op == USETN || op == USETP)
            {
                if (op == USETV)
                    val = Take(d, pc);
                else if (op == USETS)
                    val = Const(Value::String(Kstr(d)));
                else if (op == USETN)
                    val = Num(Kn(d));
                else
                    val = Const(Value::Primitive(d));
                tgt = std::make_shared<NameE>(EK::Name, UvName(a));
            }
            else
            {
                ExprP obj, key;
                if (op == TSETV)
                {
                    auto v = TakeMany({a, b, c}, pc);
                    val = v[0];
                    obj = v[1];
                    key = v[2];
                }
                else
                {
                    auto v = TakeMany({a, b}, pc);
                    val = v[0];
                    obj = v[1];
                    key = op == TSETS ? Const(Value::String(Kstr(c))) : Num(c);
                }
                tgt = std::make_shared<IndexE>(obj, key);
            }
            targets.push_back(tgt);
            values.push_back(val);
            // a multi-assignment continues while further stores consume the next pending value
            if (pend_.empty() || pc + 1 >= n_)
                break;
            const Ins next = I(pc + 1);
            const int topslot = pend_.rbegin()->first;
            if (IsAnyOf(next.op, {GSET, TSETS, TSETB, TSETV}) && next.a == topslot &&
                !PendAt(topslot)->expr->Is(EK::Table))
            {
                ++pc;
                continue;
            }
            if (next.op == USETV && next.d == topslot)
            {
                ++pc;
                continue;
            }
            break;
        }
        Flush(stmts);
        auto s = MakeStmt(Stmt::Assign);
        s->targets.assign(targets.rbegin(), targets.rend());
        s->values.assign(values.rbegin(), values.rend());
        stmts->push_back(s);
        return pc + 1;
    }

    // Checks a constructor fill against the TNEW size hint.
    static bool FillFits(const TableE& t, int op, const ExprP& key)
    {
        if (!t.hint)
            return true;
        const int narr = *t.hint & 0x7FF, hbits = *t.hint >> 11;
        bool positional = op == TSETB;
        if (!positional && key->Is(EK::Const))
        {
            const Value& kv = static_cast<ConstE&>(*key).v;
            positional = kv.kind == Value::Num && PyIsInt(kv.num) && 0 <= kv.num && op == TSETV && kv.num > 255;
        }
        if (positional)
            return narr > 0;
        int nh = 1;
        for (const auto& ev : t.events)
            if (!ev.pos)
                ++nh;
        const int need = nh == 0 ? 0 : nh == 1 ? 1 : 1 + static_cast<int>(std::bit_width(static_cast<unsigned>(nh - 1)));
        return need <= hbits;
    }

    ExprP Closure(int d, int slot)
    {
        if (d < 0 || static_cast<std::size_t>(d) >= p_.kgc.size())
            throw Fatal("constant index out of range");
        const auto* childp = std::get_if<ProtoP>(&p_.kgc[static_cast<std::size_t>(d)]);
        if (!childp)
            throw Fatal("FNEW of a constant that is not a prototype");
        const ProtoP child = *childp;
        std::vector<std::optional<std::string>> names;
        bool selfref = false;
        for (const std::uint16_t u : child->uv)
        {
            if (u & 0x8000)
            {
                const int s = u & 0xFF;
                if (s == slot && !pend_.count(s) && s >= nact_)
                {
                    selfref = true;
                    names.emplace_back(std::nullopt);
                }
                else
                    names.emplace_back(s < nact_ ? GetVar(s)->name : PendingLocalName(s));
            }
            else
                names.emplace_back(UvName(u));
        }
        std::vector<std::string> uv;
        for (const auto& nm : names)
            uv.push_back(nm ? *nm : "__self__");
        auto fe = FuncDecompiler(*child, uv, depth_ + 1, nesting_).Decompile();
        if (selfref)
        {
            fe->selfref = true;
            FuncE* raw = fe.get();
            const int depth = depth_ + 1;
            int* nesting = &nesting_;
            fe->fixSelf = [raw, child, names, depth, nesting](const VarP& var) {
                std::vector<std::string> nm;
                for (const auto& n : names)
                    nm.push_back(n ? *n : var->name);
                auto fe2 = FuncDecompiler(*child, nm, depth, *nesting).Decompile();
                raw->body = fe2->body;
                raw->params = fe2->params;
            };
        }
        return fe;
    }

    std::string PendingLocalName(int s) const
    {
        if (const auto pe = PendGet(s))
            throw ForceLocal{pe->pc};
        throw DecompileError("upvalue refers to unknown slot " + Str(s));
    }

    // -- control flow
    Snapshot Snap() const { return {pend_, nact_, slotvars_}; }

    int DoCond(int pc, int end, const Block& stmts, Loop loop)
    {
        cstarts_.clear();
        Snapshot snap = Snap();
        ExprP cond = TestCond(pc);
        int start = pc;
        for (const int c : cstarts_)
            start = std::min(start, c);
        if (OpAt(pc + 1) != JMP)
            throw DecompileError("test without jump at pc " + Str(pc));
        // comparison used as a value:  test; JMP Lt; KPRI r false; JMP L; Lt: KPRI r true; L:
        const int T = pc + 2;
        if (T + 2 < n_ && Jt(pc + 1) == T + 2)
        {
            const Ins i0 = I(T), i1 = I(T + 1), i2 = I(T + 2);
            if (i0.op == KPRI && i0.d == 1 && i1.op == JMP && Jt(T + 1) == T + 3 && i2.op == KPRI && i2.d == 2 &&
                i2.a == i0.a)
            {
                cstarts_ = {start};
                Define(i0.a, CondExpr(cond), T + 2, stmts);
                return T + 3;
            }
        }
        Flush(stmts);
        std::vector<ChainItem> items{{cond, Jt(pc + 1), start, pc + 2, std::move(snap)}};
        ParseChain(pc + 2, end, items);
        // choose the longest valid prefix
        for (std::size_t k = items.size(); k > 0; --k)
        {
            if (auto res = ReduceChain(items, k))
            {
                const int t = items[k - 1].next;
                if (k < items.size())
                {
                    const Snapshot& s = items[k].snap;
                    pend_ = s.pend;
                    nact_ = s.nact;
                    slotvars_ = s.slotvars;
                }
                return BuildIfOrLoop(start, res->first, res->second, t, end, stmts, loop);
            }
        }
        throw DecompileError("irreducible condition at pc " + Str(pc));
    }

    // Returns the pc after the last condition parsed.
    int ParseChain(int pc, int end, std::vector<ChainItem>& items)
    {
        while (pc < end)
        {
            const int st = pc;
            // operand computation: simple instructions until a test
            const auto savedPend = pend_;
            const int savedNact = nact_;
            Snapshot snap = Snap();
            int q = pc;
            bool ok = true;
            while (q < end && !IsTest(OpAt(q)))
            {
                const Ins in = I(q);
                if (IsAnyOf(in.op, {JMP, FORI, LOOP, UCLO, ISNEXT, CALLT, CALLMT, RET0, RET1, RET, RETM, GSET, TSETS,
                                    TSETB, TSETV, USETV, USETS, USETN, USETP, TSETM}) ||
                    ((in.op == CALL || in.op == CALLM) && in.b == 1))
                {
                    ok = false;
                    break;
                }
                ++q;
            }
            if (!ok || q >= end || (!items.empty() && q + 1 >= end))
                break;
            if (OpAt(q + 1) != JMP)
                throw DecompileError("test without jump at pc " + Str(q));
            Block sub = NewBlock();
            ExprP cond;
            try
            {
                for (int x = pc; x < q; ++x)
                    SimpleStep(x, sub);
                cstarts_.clear();
                cond = TestCond(q);
            }
            catch (const DecompileError&)
            {
                if (items.empty())
                    throw;
                pend_ = savedPend;
                nact_ = savedNact;
                break;
            }
            if (!sub->empty() || !pend_.empty())
            {
                if (items.empty())
                    throw DecompileError("statements inside condition at pc " + Str(pc));
                pend_ = savedPend;
                nact_ = savedNact;
                break;
            }
            items.push_back({cond, Jt(q + 1), st, q + 2, std::move(snap)});
            pc = q + 2;
        }
        if (items.empty())
            throw DecompileError("empty condition at pc " + Str(pc));
        return pc;
    }

    ExprP TestCond(int q)
    {
        ExprP cond = TestCondInner(q);
        // The JMP after a test records the free register: anything below it that we consumed as a temporary was
        // really a local variable.
        if (q + 1 < n_ && OpAt(q + 1) == JMP)
        {
            const int ja = I(q + 1).a;
            const int t = Top();
            const Ins in = I(q);
            const int allow = ((in.op == IST || in.op == ISF) && in.d == t) ? t + 1 : t;
            if (ja > allow && consumed_.count(t) && !pend_.count(t))
                throw ForceLocal{consumed_[t]};
        }
        return cond;
    }

    ExprP TestCondInner(int q)
    {
        const auto [op, a, c, b, d] = I(q);
        if (op == IST || op == ISF)
            return std::make_shared<TruthE>(Take(d, q), op == ISF);
        if (op == ISTC || op == ISFC)
            throw DecompileError("value-context and/or not supported");
        if (op <= ISNEV)
        {
            const bool da = pend_.count(a) != 0, dd = pend_.count(d) != 0;
            auto v = TakeMany({a, d}, q);
            return std::make_shared<CmpE>(op, v[0], v[1], da && dd && a > d);
        }
        ExprP k;
        if (op == ISEQS || op == ISNES)
            k = Const(Value::String(Kstr(d)));
        else if (op == ISEQN || op == ISNEN)
            k = Num(Kn(d));
        else
            k = Const(Value::Primitive(d));
        ExprP x = Take(a, q);
        return std::make_shared<CmpE>(op, x, k, false);
    }

    // Reduces the first k chain items to one condition: (cond, target), or nothing.
    static std::optional<std::pair<ExprP, int>> ReduceChain(const std::vector<ChainItem>& items, std::size_t k)
    {
        struct It
        {
            ExprP cond;
            int target, start, next;
        };
        std::vector<It> it;
        for (std::size_t i = 0; i < k; ++i)
            it.push_back({items[i].cond, items[i].target, items[i].start, items[i].next});
        while (it.size() > 1)
        {
            bool done = false;
            for (long i = static_cast<long>(it.size()) - 2; i >= 0; --i)
            {
                const It a = it[static_cast<std::size_t>(i)], b = it[static_cast<std::size_t>(i) + 1];
                bool labelUsed = false;
                for (long j = 0; j < static_cast<long>(it.size()); ++j)
                    if (j != i && j != i + 1 && it[static_cast<std::size_t>(j)].target == b.start)
                        labelUsed = true;
                if (labelUsed)
                    continue;
                if (a.target == b.target)
                {
                    it[static_cast<std::size_t>(i)] = {std::make_shared<LogicE>("or", a.cond, b.cond), b.target,
                                                       a.start, b.next};
                    it.erase(it.begin() + i + 1);
                    done = true;
                    break;
                }
                if (a.target == b.next)
                {
                    ExprP na = Negate(a.cond);
                    it[static_cast<std::size_t>(i)] = {std::make_shared<LogicE>("and", na, b.cond), b.target, a.start,
                                                       b.next};
                    it.erase(it.begin() + i + 1);
                    done = true;
                    break;
                }
            }
            if (!done)
                return std::nullopt;
        }
        return std::make_pair(it[0].cond, it[0].target);
    }

    static Ends Union(Ends a, const Ends& b)
    {
        a.insert(b.begin(), b.end());
        return a;
    }

    StmtP MakeIf(ExprP cond, Block then, Block orelse)
    {
        auto s = MakeStmt(Stmt::If);
        s->expr = std::move(cond);
        s->body = std::move(then);
        s->orelse = std::move(orelse);
        return s;
    }

    static Block BreakBlock()
    {
        Block b = NewBlock();
        b->push_back(MakeStmt(Stmt::Break));
        return b;
    }

    // jcond: "jump to F if jcond", fallthrough T
    int BuildIfOrLoop(int start, const ExprP& jcond, int F, int T, int end, const Block& stmts, Loop loop)
    {
        const Ends ends = curEnds_;
        const auto norm = [&](int x) { return ends.count(x) ? end : x; };
        // while loop
        if (F > T && T < end && OpAt(T) == LOOP && Jt(T) == F && OpAt(F - 1) == JMP && Jt(F - 1) == start &&
            F <= end)
        {
            Block body = SubBlock(T + 1, F - 1, F, {F - 1, start});
            auto w = MakeStmt(Stmt::While);
            w->expr = CondExpr(Negate(jcond));
            w->body = body;
            stmts->push_back(w);
            return F;
        }
        const int Fn = norm(F);
        if (loop && F == *loop && Fn != end && end - 1 > T && OpAt(end - 1) == JMP && Jt(end - 1) == F)
        {
            // the jump was merged into a trailing 'break' of this block
            Block then = SubBlock(T, end - 1, loop, {end - 1, F});
            stmts->push_back(MakeIf(CondExpr(Negate(jcond)), then, NewBlock()));
            return end - 1;
        }
        if (loop && F == *loop && Fn != end)
        {
            // 'if c then ... else break end': the false jump was merged into the break that forms the else block
            for (int P = end - 1; P > T; --P)
            {
                if (OpAt(P) == JMP && Jt(P) == F && OpAt(P - 1) == JMP)
                {
                    const int nj = norm(Jt(P - 1));
                    if ((nj == P + 1 || nj == end) && P - 1 >= T)
                    {
                        const int E = P + 1;
                        Block then = SubBlock(T, P - 1, loop, Union({P - 1, Jt(P - 1)}, E == end ? ends : Ends{}));
                        stmts->push_back(MakeIf(CondExpr(Negate(jcond)), then, BreakBlock()));
                        return E;
                    }
                }
            }
        }
        if (loop && F == *loop && Fn != end)
        {
            // the condition jumps straight out of the loop
            stmts->push_back(MakeIf(CondExpr(jcond), BreakBlock(), NewBlock()));
            return T;
        }
        if (Fn < T || Fn > end)
            throw DecompileError("condition jumps outside block at pc " + Str(start));
        const Ends outer = Fn == end ? ends : Ends{};
        if ((Fn - 1 >= T && OpAt(Fn - 1) == JMP && Fn != end) ||
            (Fn == end && Fn - 1 >= T && OpAt(Fn - 1) == JMP && F == end))
        {
            const int E = Jt(Fn - 1);
            const int En = norm(E);
            const bool isBreak = loop && E == *loop && En != end;
            if (!isBreak && Fn < En && En <= end)
            {
                Block then = SubBlock(T, Fn - 1, loop, Union({Fn - 1, E}, En == end ? ends : Ends{}));
                Block orelse = SubBlock(Fn, En, loop, En == end ? ends : Ends{});
                stmts->push_back(MakeIf(CondExpr(Negate(jcond)), then, orelse));
                return En;
            }
        }
        Block then = SubBlock(T, Fn, loop, outer);
        stmts->push_back(MakeIf(CondExpr(Negate(jcond)), then, NewBlock()));
        return Fn;
    }

    int DoLoop(int pc, const Block& stmts)
    {
        Flush(stmts);
        const int exit = Jt(pc);
        if (OpAt(exit - 1) == JMP && Jt(exit - 1) == pc && !IsTest(OpAt(exit - 2)))
        {
            Block body = SubBlock(pc + 1, exit - 1, exit, {exit - 1, pc});
            auto w = MakeStmt(Stmt::While);
            w->expr = Const(Value::Primitive(2));
            w->body = body;
            stmts->push_back(w);
            return exit;
        }
        // repeat ... until cond: find the trailing condition chain jumping back to pc
        const int cstart = FindUntil(pc, exit);
        const int savedNact = nact_;
        const auto savedVars = slotvars_;
        Block body = MakeBlock(pc + 1, cstart, exit, {cstart});
        std::vector<ChainItem> items;
        const int after = ParseChain(cstart, exit, items);
        if (after != exit)
            throw DecompileError("bad repeat condition at pc " + Str(pc));
        const auto res = ReduceChain(items, items.size());
        if (!res || res->second != pc)
            throw DecompileError("bad repeat jump at pc " + Str(pc));
        ExprP cond = CondExpr(Negate(res->first));
        nact_ = savedNact;
        for (auto& [s, v] : slotvars_)
            if (const auto it = savedVars.find(s); it != savedVars.end())
                v = it->second;
        auto r = MakeStmt(Stmt::Repeat);
        r->body = body;
        r->expr = cond;
        stmts->push_back(r);
        return exit;
    }

    int FindUntil(int pc, int exit) const
    {
        // the last JMP before exit jumps back to pc; walk back over the chain
        const int q = exit - 1;
        if (OpAt(q) != JMP || Jt(q) != pc)
            throw DecompileError("repeat without back jump at pc " + Str(pc));
        // find the earliest test whose chain ends at exit: scan backwards for a run of test+jmp groups with pure
        // expression code in between
        int best = q - 1;
        int k = q - 1;
        while (k > pc)
        {
            if (IsTest(OpAt(k)) && OpAt(k + 1) == JMP)
            {
                best = k;
                // move to the start of the operand code
                int j = k - 1;
                const int ja = I(k + 1).a;
                while (j > pc && OpAt(j) != JMP && !IsTest(OpAt(j)) && !IsStmt(j) && I(j).a >= ja)
                    --j;
                if (j > pc && OpAt(j) == JMP && IsTest(OpAt(j - 1)) && pc < Jt(j) && Jt(j) <= exit &&
                    (Jt(j) == pc || Jt(j) >= j + 1))
                {
                    k = j - 1;
                    best = j + 1;
                    continue;
                }
                return j + 1;
            }
            break;
        }
        return best;
    }

    bool IsStmt(int j) const
    {
        const Ins in = I(j);
        if (IsAnyOf(in.op, {GSET, TSETS, TSETB, TSETV, USETV, USETS, USETN, USETP, RET0, RET1, RET, RETM, CALLT,
                            CALLMT, FORL, ITERL, LOOP, FORI, UCLO}))
            return true;
        return (in.op == CALL || in.op == CALLM) && in.b == 1;
    }

    int DoNumFor(int pc, const Block& stmts)
    {
        const int a = I(pc).a;
        const int exit = Jt(pc);
        if (OpAt(exit - 1) != FORL || Jt(exit - 1) != pc + 1)
            throw DecompileError("bad FORI at pc " + Str(pc));
        ExprP step = Take(a + 2, pc);
        ExprP stop = Take(a + 1, pc);
        ExprP start = Take(a, pc);
        Flush(stmts);
        if (a != nact_)
            throw DecompileError("for base mismatch at pc " + Str(pc));
        const int saved = nact_;
        nact_ = a + 3;
        VarP var = NewVar(a + 3, "i");
        nact_ = a + 4;
        Block body = SubBlock(pc + 1, exit - 1, exit, {exit - 1});
        nact_ = saved;
        if (step->Is(EK::Const))
        {
            const Value& v = static_cast<ConstE&>(*step).v;
            // Python: True == 1.0
            if (((v.kind == Value::Num && v.num == 1.0) || v.kind == Value::True) && StepDefault(pc, a))
                step = nullptr;
        }
        auto f = MakeStmt(Stmt::NumFor);
        f->vars = {var};
        f->start = start;
        f->stop = stop;
        f->step = step;
        f->body = body;
        stmts->push_back(f);
        return exit;
    }

    // the default step is emitted as KSHORT base+2, 1 right before FORI
    bool StepDefault(int pc, int a) const
    {
        const Ins in = I(pc - 1);
        return in.op == KSHORT && in.a == a + 2 && in.d == 1;
    }

    int DoGenFor(int pc, const Block& stmts)
    {
        const int a = I(pc).a;
        const int iterc = Jt(pc);
        const int exit = iterc + 2;
        const int nvars = I(iterc).b - 1;
        ExprP e3 = Take(a - 1, pc);
        ExprP e2 = Take(a - 2, pc);
        ExprP e1 = Take(a - 3, pc);
        std::vector<ExprP> exprs{e1, e2, e3};
        // collapse an adjusted multi-result call: pairs(t) -> call with 3 results
        if (e1->Is(EK::Call) && e1->multi && IsNilConst(e2) && IsNilConst(e3))
            exprs = {e1};
        else if (e2->Is(EK::Call) && e2->multi && IsNilConst(e3))
            exprs = {e1, e2};
        else
            while (exprs.size() > 1 && IsNilConst(exprs.back()))  // trailing nils from adjustment
                exprs.pop_back();
        Flush(stmts);
        if (a - 3 != nact_)
            throw DecompileError("for-in base mismatch at pc " + Str(pc));
        const int saved = nact_;
        nact_ = a;
        std::vector<VarP> vars;
        for (int i = 0; i < nvars; ++i)
            vars.push_back(NewVar(a + i, i == 0 ? "k" : "v"));
        nact_ = a + nvars;
        Block body = SubBlock(pc + 1, iterc, exit, {iterc});
        nact_ = saved;
        auto f = MakeStmt(Stmt::GenFor);
        f->vars = std::move(vars);
        f->values = std::move(exprs);
        f->body = body;
        stmts->push_back(f);
        return exit;
    }
};

} // namespace

bool Decompile(std::span<const std::uint8_t> bytecode, std::string& source, std::string& error)
{
    try
    {
        std::vector<std::uint8_t> bc81;
        if (!ToLayout(bytecode, 0x81, bc81, error))
            return false;
        const ProtoP proto = ParseBytecode(bc81);
        int nesting = 0;
        const auto fe = FuncDecompiler(*proto, {}, 0, nesting).Decompile();
        Renderer R;
        source = RenderBlock(R, fe->body, "");
        return true;
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return false;
    }
}

} // namespace x2::lua
