// The client's vendored integrations that the importer excluded (BandiVideo, Bandicam, the embedded browser, CxImage)
// and that were removed or replaced by hand: nothing in the client's sources may still include or use them
// (docs/porting/client.md#integrations-removed-by-hand).

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

// The text without // and /* */ comments (the port's comments name what they replace). String literals are not
// parsed: a "//" inside one ends the line early, which only makes the check looser.
std::string WithoutComments(const std::string& text)
{
    std::string out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text.compare(i, 2, "//") == 0) {
            i = text.find('\n', i);
            if (i == std::string::npos)
                break;
            out += '\n';
        } else if (text.compare(i, 2, "/*") == 0) {
            i = text.find("*/", i + 2);
            if (i == std::string::npos)
                break;
            ++i;
        } else {
            out += text[i];
        }
    }
    return out;
}

}  // namespace

TEST_CASE("removed integrations: no source refers to them")
{
    const std::vector<std::string> forbidden = {
        "BandiVideoLibrary.h", "BandiVideoTexture_DX9.h", "BandiVideoFileReader.h", "BandiVideoSound_DS.h", "bandivid.h",
        "bandicap.h", "CBandiVideo", "CBandiCapture", "KPCHWebBrowser", "Browser.c", "CxImage", "ximage.h",
    };
    const std::filesystem::path root = X2_CLIENT_SOURCE_DIR;
    int files = 0;
    for (const char* layer : {"X2", "X2Lib", "KTDXLIB"}) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root / layer)) {
            const std::filesystem::path& path = entry.path();
            if (!entry.is_regular_file() || (path.extension() != ".cpp" && path.extension() != ".h")) {
                continue;
            }
            ++files;
            std::ifstream file(path, std::ios::binary);
            const std::string text =
                WithoutComments({std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()});
            for (const std::string& name : forbidden) {
                CAPTURE(path.lexically_relative(root).generic_string());
                CHECK_MESSAGE(text.find(name) == std::string::npos, name);
            }
        }
    }
    CHECK(files > 800);
}
