#include "x2/assets/layout.h"

#include "x2/assets/manifest.h"

#include <fstream>
#include <iterator>

namespace x2::assets {

namespace {

char Lower(char c)
{
    return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c;
}

bool IsSpace(char c)
{
    return c == ' ' || c == '\t';
}

std::vector<std::string_view> SplitFields(std::string_view line)
{
    std::vector<std::string_view> fields;
    std::size_t i = 0;
    while (i < line.size())
    {
        while (i < line.size() && IsSpace(line[i]))
            ++i;
        if (i == line.size() || line[i] == '#')
            break;
        const std::size_t start = i;
        while (i < line.size() && !IsSpace(line[i]))
            ++i;
        fields.push_back(line.substr(start, i - start));
    }
    return fields;
}

// A relative folder: `/`-separated, no empty, `.` or `..` parts, no backslash, not under the metadata folder.
bool ValidFolder(std::string_view folder)
{
    if (folder.empty() || folder.front() == '/' || folder.back() == '/' ||
        folder.find_first_of("\\:") != std::string_view::npos)
        return false;
    std::size_t start = 0;
    for (;;)
    {
        const std::size_t slash = folder.find('/', start);
        const std::string_view part = folder.substr(start, slash == std::string_view::npos ? slash : slash - start);
        if (part.empty() || part == "." || part == ".." || (start == 0 && part == kMetaFolder))
            return false;
        if (slash == std::string_view::npos)
            return true;
        start = slash + 1;
    }
}

} // namespace

namespace {

// Matches one character against the token at pattern[p] (`?`, a `[...]` class or a literal); `next` gets the index
// after the token. A `[` with no closing `]` is a literal.
bool MatchOne(std::string_view pattern, std::size_t p, char c, std::size_t& next)
{
    if (pattern[p] == '?')
    {
        next = p + 1;
        return true;
    }
    if (pattern[p] == '[')
    {
        const std::size_t close = pattern.find(']', p + 2);
        if (close != std::string_view::npos)
        {
            next = close + 1;
            const char lc = Lower(c);
            for (std::size_t k = p + 1; k < close; ++k)
                if (k + 2 < close && pattern[k + 1] == '-')
                {
                    if (lc >= Lower(pattern[k]) && lc <= Lower(pattern[k + 2]))
                        return true;
                    k += 2;
                }
                else if (Lower(pattern[k]) == lc)
                {
                    return true;
                }
            return false;
        }
    }
    next = p + 1;
    return Lower(pattern[p]) == Lower(c);
}

} // namespace

bool GlobMatch(std::string_view pattern, std::string_view text)
{
    // Iterative wildcard matching with backtracking to the last `*`.
    std::size_t p = 0, t = 0, star = std::string_view::npos, mark = 0, next = 0;
    while (t < text.size())
    {
        if (p < pattern.size() && pattern[p] == '*')
        {
            star = p++;
            mark = t;
        }
        else if (p < pattern.size() && MatchOne(pattern, p, text[t], next))
        {
            p = next;
            ++t;
        }
        else if (star != std::string_view::npos)
        {
            p = star + 1;
            t = ++mark;
        }
        else
        {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
        ++p;
    return p == pattern.size();
}

bool Layout::Load(const std::filesystem::path& path, std::string& error)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
    {
        error = "cannot open " + PathToUtf8(path);
        return false;
    }
    const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (!Parse(text, error))
    {
        error = PathToUtf8(path) + ":" + error;
        return false;
    }
    return true;
}

bool Layout::Parse(std::string_view text, std::string& error)
{
    rules_.clear();
    int number = 0;
    while (!text.empty())
    {
        ++number;
        const std::size_t end = text.find('\n');
        std::string_view line = text.substr(0, end);
        text = end == std::string_view::npos ? std::string_view() : text.substr(end + 1);
        if (!line.empty() && line.back() == '\r')
            line.remove_suffix(1);
        const auto fields = SplitFields(line);
        if (fields.empty())
            continue;
        if (fields.size() < 2 || fields.size() > 3)
        {
            error = std::to_string(number) + ": expected <folder> <name pattern> [<archive pattern>]";
            return false;
        }
        if (!ValidFolder(fields[0]))
        {
            error = std::to_string(number) + ": \"" + std::string(fields[0]) + "\" is not a relative folder";
            return false;
        }
        Rule rule;
        rule.folder = std::string(fields[0]);
        rule.name = std::string(fields[1]);
        if (fields.size() == 3)
            rule.archive = std::string(fields[2]);
        rule.line = number;
        rules_.push_back(std::move(rule));
    }
    return true;
}

std::string Layout::Classify(std::string_view name, std::string_view archive) const
{
    std::string_view folder = kFallback;
    for (const Rule& rule : rules_)
        if (GlobMatch(rule.name, name) && (rule.archive.empty() || GlobMatch(rule.archive, archive)))
        {
            folder = rule.folder;
            break;
        }

    std::string_view stem = archive;
    if (stem.size() > 4 && GlobMatch("*.kom", stem))
        stem.remove_suffix(4);
    std::string out;
    constexpr std::string_view kPlaceholder = "{archive}";
    for (std::size_t i = 0; i < folder.size();)
        if (folder.substr(i, kPlaceholder.size()) == kPlaceholder)
        {
            out += stem;
            i += kPlaceholder.size();
        }
        else
        {
            out += folder[i++];
        }
    return out;
}

} // namespace x2::assets
