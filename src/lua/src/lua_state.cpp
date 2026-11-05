#include "lua_state.h"

namespace x2::lua::detail {

std::string Lua51Escapes(std::string_view src)
{
    if (src.find('\\') == std::string_view::npos)
        return std::string(src);
    static constexpr std::string_view kKnown = "abfnrtv\\\"'\n\r0123456789";
    std::string out;
    out.reserve(src.size());
    const std::size_t n = src.size();
    std::size_t i = 0;
    // A long bracket with `level` '=': returns the end of its closing bracket, searched from `from`.
    const auto longEnd = [&](std::size_t level, std::size_t from) {
        const std::string close = "]" + std::string(level, '=') + "]";
        const std::size_t e = src.find(close, from);
        return e == std::string_view::npos ? n : e + level + 2;
    };
    while (i < n)
    {
        const char c = src[i];
        if (c == '-' && src.substr(i, 2) == "--")  // comment
        {
            const std::size_t j = i + 2;
            if (j < n && src[j] == '[')
            {
                std::size_t k = j + 1;
                while (k < n && src[k] == '=')
                    ++k;
                if (k < n && src[k] == '[')
                {
                    const std::size_t end = longEnd(k - j - 1, k);
                    out.append(src.substr(i, end - i));
                    i = end;
                    continue;
                }
            }
            std::size_t end = src.find('\n', i);
            end = end == std::string_view::npos ? n : end;
            out.append(src.substr(i, end - i));
            i = end;
        }
        else if (c == '[' && i + 1 < n && (src[i + 1] == '[' || src[i + 1] == '='))  // long string
        {
            std::size_t k = i + 1;
            while (k < n && src[k] == '=')
                ++k;
            if (k < n && src[k] == '[')
            {
                const std::size_t end = longEnd(k - i - 1, k);
                out.append(src.substr(i, end - i));
                i = end;
            }
            else
            {
                out += c;
                ++i;
            }
        }
        else if (c == '"' || c == '\'')  // quoted string
        {
            out += c;
            ++i;
            while (i < n && src[i] != c && src[i] != '\n')
            {
                if (src[i] == '\\' && i + 1 < n)
                {
                    if (kKnown.find(src[i + 1]) != std::string_view::npos)
                        out.append(src.substr(i, 2));
                    else
                        out += src[i + 1];
                    i += 2;
                    continue;
                }
                out += src[i];
                ++i;
            }
            if (i < n)
            {
                out += src[i];
                ++i;
            }
        }
        else
        {
            out += c;
            ++i;
        }
    }
    return out;
}

} // namespace x2::lua::detail
