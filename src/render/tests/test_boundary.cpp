// Decision T36 (4): the SDL_GPU API is used only by the sdlgpu backend, plus the one escape-hatch user (the ImGui layer's
// sdlgpu3 renderer) until an ImGui renderer on x2::render replaces it. SDL.h includes SDL_gpu.h, so this scans for the
// API's identifiers (SDL_*GPU*, the header's name, ImGui's sdlgpu3 backend), not for the #include alone.

#include <doctest/doctest.h>

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

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

bool IsIdentifier(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

// The first SDL_GPU identifier in `text`, or an empty string.
std::string FirstGpuUse(const std::string& text)
{
    const std::string prefix = "SDL_";
    for (size_t at = text.find(prefix); at != std::string::npos; at = text.find(prefix, at + 1)) {
        if (at > 0 && IsIdentifier(text[at - 1]))
            continue;
        size_t end = at + prefix.size();
        while (end < text.size() && (IsIdentifier(text[end]) || text[end] == '.'))
            ++end;
        const std::string word = text.substr(at, end - at);
        if (word.find("GPU") != std::string::npos || word == std::string("SDL_") + "gpu.h")
            return word;
    }
    const std::string imgui = std::string("ImGui_ImplSDL") + "GPU3";
    if (text.find(imgui) != std::string::npos || text.find("imgui_impl_sdl" "gpu3") != std::string::npos)
        return imgui;
    return {};
}

}  // namespace

TEST_CASE("render boundary: the SDL GPU API only in src/render/backends/sdlgpu")
{
    const std::filesystem::path root = X2_SOURCE_ROOT;
    const std::vector<std::filesystem::path> allowed = {
        root / "render" / "backends" / "sdlgpu",
        root / "app" / "src" / "imgui_layer.cpp",     // the escape hatch's user (x2::render::sdlgpu::Native)
    };
    int files = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        const std::filesystem::path& path = entry.path();
        const std::string ext = path.extension().string();
        if (!entry.is_regular_file() || (ext != ".cpp" && ext != ".h" && ext != ".hpp" && ext != ".inl" && ext != ".c"))
            continue;
        bool skip = false;
        for (const std::filesystem::path& a : allowed) {
            const std::string p = path.generic_string(), q = a.generic_string();
            skip = skip || p == q || p.starts_with(q + "/");
        }
        if (skip)
            continue;
        ++files;
        std::ifstream file(path, std::ios::binary);
        const std::string use =
            FirstGpuUse(WithoutComments({std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()}));
        CAPTURE(path.lexically_relative(root).generic_string());
        CHECK_MESSAGE(use.empty(), use);
    }
    CHECK(files > 1000);
}
