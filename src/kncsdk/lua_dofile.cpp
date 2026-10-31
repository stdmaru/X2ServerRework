#include "KncLua.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

// LuaJIT allows 65536 constants per function (LJ_MAX_KGC); the original ran on Lua 5.1, which allows 2^18. A few
// data scripts (Item.lua generated from the client, ArcadeResultTable.lua) are a single main function past that limit.
bool IsConstantLimitError(lua_State* L)
{
    const char* msg = lua_tostring(L, -1);
    return msg && std::strstr(msg, "has more than") && std::strstr(msg, "constants");
}

struct StatementStart
{
    size_t offset;
    size_t line; // 1-based
};

// Top-level statement starts: an identifier at column 0 that is not a keyword closing or continuing a block.
// A wrong guess (a block or an expression cut in two) makes one of the pieces fail to compile.
std::vector<StatementStart> StatementStarts(const std::string& src, bool& splittable)
{
    static const char* const kContinuations[] = { "end", "else", "elseif", "until", "and", "or", "then", "do", "in", "not" };

    splittable = src.find("[[") == std::string::npos && src.find("[=") == std::string::npos; // no long strings to cut
    std::vector<StatementStart> starts;
    size_t line = 1;
    for (size_t pos = 0; pos < src.size() && splittable; ++line) {
        const char c = src[pos];
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t end = pos;
            while (end < src.size() && (std::isalnum(static_cast<unsigned char>(src[end])) || src[end] == '_'))
                ++end;
            const std::string word = src.substr(pos, end - pos);
            if (word == "local") {
                splittable = false; // a top-level local would not be seen by the next pieces
                break;
            }
            bool continuation = false;
            for (const char* k : kContinuations)
                continuation = continuation || word == k;
            if (!continuation && pos > 0)
                starts.push_back({ pos, line });
        }
        const size_t nl = src.find('\n', pos);
        if (nl == std::string::npos)
            break;
        pos = nl + 1;
    }
    return starts;
}

// Compiles src as consecutive pieces of about maxLines lines, cut at statement starts, and pushes their functions.
// Each piece is padded with the newlines before it, so error messages keep the line numbers of the file.
int LoadPieces(lua_State* L, const std::string& src, const std::vector<StatementStart>& starts, size_t maxLines,
               const std::string& chunkname, int& pieces)
{
    pieces = 0;
    const size_t lines = static_cast<size_t>(std::count(src.begin(), src.end(), '\n')) + 1;
    StatementStart begin = { 0, 1 };
    size_t next = 0; // first start after begin
    for (;;) {
        // the rest of the file if it fits; otherwise up to the last start within maxLines lines (at least the next one,
        // so every piece makes progress)
        const bool last = next >= starts.size() || lines - begin.line <= maxLines;
        size_t cut = next;
        while (!last && cut + 1 < starts.size() && starts[cut + 1].line - begin.line <= maxLines)
            ++cut;
        const StatementStart end = last ? StatementStart{ src.size(), lines } : starts[cut];

        std::string piece(begin.line - 1, '\n');
        piece.append(src, begin.offset, end.offset - begin.offset);
        if (!lua_checkstack(L, 1))
            return LUA_ERRMEM;
        const int status = luaL_loadbuffer(L, piece.data(), piece.size(), chunkname.c_str());
        if (status != 0)
            return status;
        ++pieces;

        if (last)
            return 0;
        begin = end;
        next = cut + 1;
    }
}

// Splits a file past the constant limit into pieces and runs them in order. Returns false (and leaves the stack as it
// was) when the file cannot be split safely; the caller then reports the original error.
bool DoFileInPieces(lua_State* L, const char* filename, int& status)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open())
        return false;
    const std::string src((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    bool splittable = false;
    const std::vector<StatementStart> starts = StatementStarts(src, splittable);
    if (!splittable || starts.empty())
        return false;

    const std::string chunkname = std::string("@") + filename;
    const int base = lua_gettop(L);
    for (size_t maxLines = 50000; maxLines >= 100; maxLines /= 2) {
        int pieces = 0;
        status = LoadPieces(L, src, starts, maxLines, chunkname, pieces);
        if (status == 0) {
            // all pieces compiled: run them in order, as the single chunk would
            for (int i = 1; i <= pieces; ++i) {
                lua_pushvalue(L, base + i);
                status = lua_pcall(L, 0, 0, 0);
                if (status != 0) {
                    lua_replace(L, base + 1);
                    lua_settop(L, base + 1);
                    return true;
                }
            }
            lua_settop(L, base);
            return true;
        }
        const bool tooBig = status == LUA_ERRSYNTAX && IsConstantLimitError(L);
        lua_settop(L, base);
        if (!tooBig)
            return false;
    }
    return false;
}

} // namespace

int lua_dofile(lua_State* L, const char* filename)
{
    int status = luaL_loadfile(L, filename);
    if (status == 0)
        return lua_pcall(L, 0, LUA_MULTRET, 0);

    if (status == LUA_ERRSYNTAX && IsConstantLimitError(L)) {
        const int base = lua_gettop(L); // the original error, kept if the file cannot be split
        int pieceStatus = 0;
        if (DoFileInPieces(L, filename, pieceStatus)) {
            if (pieceStatus == 0) {
                lua_remove(L, base);
                return 0;
            }
            lua_remove(L, base); // report the error of the piece that failed
            return pieceStatus;
        }
    }
    return status;
}
