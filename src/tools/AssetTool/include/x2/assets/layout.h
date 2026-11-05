// Where import puts each entry inside client_data (docs/tools/client-data.md#layout): versioned rules, first match
// wins, organized by domain rather than by extension. The layout never affects packing (the manifest is keyed by
// name), so rules can change at any time.
//
// Rules file (tools/assets/layout.txt), UTF-8, one rule per line, fields separated by spaces or tabs:
//   <folder>  <name pattern>  [<archive pattern>]
// Patterns are globs (`*`, `?`, `[abc]`, `[0-9]`) compared case-insensitively (ASCII) with the entry name and with the
// source archive's file name. A folder is a relative path with `/`; `{archive}` in it becomes the source archive's
// name without `.kom`. `#` starts a comment. Entries no rule matches go to `_unsorted/{archive}`.
#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace x2::assets {

bool GlobMatch(std::string_view pattern, std::string_view text);  // case-insensitive (ASCII)

class Layout
{
public:
    static constexpr std::string_view kFallback = "_unsorted/{archive}";

    struct Rule
    {
        std::string folder;
        std::string name;
        std::string archive;  // empty: any
        int line = 0;
    };

    bool Load(const std::filesystem::path& path, std::string& error);
    bool Parse(std::string_view text, std::string& error);  // `error` names the line

    // The folder (relative, `/`-separated) for an entry of `archive` (e.g. "data034.kom").
    std::string Classify(std::string_view name, std::string_view archive) const;

    const std::vector<Rule>& Rules() const { return rules_; }

private:
    std::vector<Rule> rules_;
};

} // namespace x2::assets
