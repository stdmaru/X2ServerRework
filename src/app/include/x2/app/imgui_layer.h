#pragma once

// Dear ImGui on top of x2::app (decision T32): SDL3 platform backend + SDL_GPU renderer backend. Tools only; the client
// does not use ImGui. See docs/porting/app.md.

#include "x2/app/app.h"

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
    float fontSize = 16.0f;     // logical pixels; multiplied by the content scale
};

// System fonts with Latin and Hangul glyphs (server logs contain Korean): Malgun Gothic on Windows; Noto Sans CJK or
// Nanum Gothic on Linux.
std::vector<std::string> DefaultFontFiles();

// Context setup shared by ImGuiLayer and the headless tests: style, scale and fonts. Returns the font file it loaded,
// or an empty string for the built-in font.
std::string ConfigureImGui(const ImGuiOptions& options, float contentScale);

class ImGuiLayer {
public:
    static std::unique_ptr<ImGuiLayer> Create(Window& window, const ImGuiOptions& options, std::string& error);
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    const std::string& FontFile() const { return m_fontFile; }

    void ProcessEvent(const SDL_Event& event);
    void NewFrame();
    // Ends the ImGui frame and draws it into `frame`, cleared to `clear` first.
    void Render(Frame& frame, SDL_FColor clear);

private:
    explicit ImGuiLayer(Window& window) : m_window(window) {}

    Window& m_window;
    ImGuiContext* m_context = nullptr;
    std::string m_iniFile;      // ImGui keeps the pointer
    std::string m_fontFile;
    bool m_platformInit = false;
    bool m_rendererInit = false;
};

}  // namespace x2::app
