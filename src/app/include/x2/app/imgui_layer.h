#pragma once

// Dear ImGui on top of x2::app (decision T32): SDL3 platform backend + SDL_GPU renderer backend, which reaches the
// SDL_GPU device through x2::render's escape hatch (T36) until an ImGui renderer on x2::render exists. Tools only; the
// client does not use ImGui. See docs/porting/app.md.

#include "x2/app/app.h"
#include "x2/render/render.h"

#include <memory>
#include <string>
#include <vector>

struct ImGuiContext;

namespace x2::app {

struct ImGuiOptions {
    // Where ImGui keeps window positions; empty disables it.
    std::string iniFile;
    // Fonts to try, in order: the first one that exists is the main font. Empty uses DefaultFontFiles().
    // Falls back to ImGui's built-in font (ASCII only) when none exists.
    std::vector<std::string> fontFiles;
    // Fonts for the Hangul glyphs the main font lacks, merged into it: the first one that exists, unless it is the main
    // font. Empty uses DefaultHangulFontFiles(). It becomes the main font when no main font exists.
    std::vector<std::string> hangulFontFiles;
    float fontSize = 16.0f;     // logical pixels; multiplied by the content scale
};

// System fonts (server logs contain Korean). Windows: Segoe UI with Malgun Gothic merged, as Korean fonts draw the
// backslash as the won sign. Linux: Noto Sans CJK or Nanum Gothic alone, which draw both.
std::vector<std::string> DefaultFontFiles();
std::vector<std::string> DefaultHangulFontFiles();

// Context setup shared by ImGuiLayer and the headless tests: style, scale and fonts. Returns the font file it loaded,
// or an empty string for the built-in font.
std::string ConfigureImGui(const ImGuiOptions& options, float contentScale);

class ImGuiLayer {
public:
    // `device` must be an SdlGpu device that has claimed `window`.
    static std::unique_ptr<ImGuiLayer> Create(Window& window, render::Device& device, const ImGuiOptions& options,
                                              std::string& error);
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    const std::string& FontFile() const { return m_fontFile; }

    void ProcessEvent(const SDL_Event& event);
    void NewFrame();
    // Ends the ImGui frame and draws it into `target` (the swapchain texture; none draws nothing), cleared to `clear`.
    void Render(render::CommandBuffer& commands, render::Texture target, const render::Color& clear);

private:
    explicit ImGuiLayer(render::Device& device) : m_device(device) {}

    render::Device& m_device;
    ImGuiContext* m_context = nullptr;
    std::string m_iniFile;      // ImGui keeps the pointer
    std::string m_fontFile;
    bool m_platformInit = false;
    bool m_rendererInit = false;
};

}  // namespace x2::app
