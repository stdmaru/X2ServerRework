#include "x2/app/imgui_layer.h"

#include "x2/render/sdlgpu.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include <cstdlib>
#include <filesystem>
#include <string_view>
#include <system_error>

namespace x2::app {

// Korean fonts draw U+005C as the won sign (the KS X 1003 heritage of Malgun Gothic and Gulim), so on Windows a Latin
// font comes first and the Hangul one fills in the glyphs it lacks. Noto Sans CJK and Nanum Gothic draw a backslash.
std::vector<std::string> DefaultFontFiles()
{
#if defined(_WIN32)
    const char* windir = std::getenv("WINDIR");
    const std::string fonts = std::string(windir ? windir : "C:\\Windows") + "\\Fonts\\";
    return {fonts + "segoeui.ttf", fonts + "arial.ttf"};
#else
    return DefaultHangulFontFiles();
#endif
}

std::vector<std::string> DefaultHangulFontFiles()
{
#if defined(_WIN32)
    const char* windir = std::getenv("WINDIR");
    const std::string fonts = std::string(windir ? windir : "C:\\Windows") + "\\Fonts\\";
    return {fonts + "malgun.ttf", fonts + "gulim.ttc"};
#else
    return {
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",   // Debian/Ubuntu fonts-noto-cjk
        "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",        // Arch noto-fonts-cjk
        "/usr/share/fonts/google-noto-cjk/NotoSansCJK-Regular.ttc", // Fedora
        "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
        "/usr/share/fonts/nanum/NanumGothic.ttf",
    };
#endif
}

namespace {

std::string FirstExisting(const std::vector<std::string>& candidates)
{
    for (const std::string& file : candidates) {
        std::error_code ec;
        if (std::filesystem::is_regular_file(
                std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(file.data()), file.size())), ec))
            return file;
    }
    return {};
}

}  // namespace

std::string ConfigureImGui(const ImGuiOptions& options, float contentScale)
{
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScaleAllSizes(contentScale);
    style.FontScaleDpi = contentScale;

    // Glyphs are rasterized on demand (dynamic font atlas), so a CJK font costs nothing until a glyph is used.
    const std::string hangul = FirstExisting(options.hangulFontFiles.empty() ? DefaultHangulFontFiles()
                                                                             : options.hangulFontFiles);
    std::string primary = FirstExisting(options.fontFiles.empty() ? DefaultFontFiles() : options.fontFiles);
    if (primary.empty())
        primary = hangul;
    if (!primary.empty() && io.Fonts->AddFontFromFileTTF(primary.c_str(), options.fontSize)) {
        if (!hangul.empty() && hangul != primary) {
            ImFontConfig merge;
            merge.MergeMode = true;     // only for the glyphs the main font lacks: it keeps its own backslash
            io.Fonts->AddFontFromFileTTF(hangul.c_str(), options.fontSize, &merge);
        }
        return primary;
    }
    ImFontConfig config;
    config.SizePixels = options.fontSize;
    io.Fonts->AddFontDefault(&config);
    return {};
}

std::unique_ptr<ImGuiLayer> ImGuiLayer::Create(Window& window, render::Device& device, const ImGuiOptions& options,
                                               std::string& error)
{
    IMGUI_CHECKVERSION();
    SDL_GPUDevice* native = render::sdlgpu::Native::Device(device);
    if (!native) {
        error = "ImGuiLayer: needs an SDL_GPU device (x2::render backend sdlgpu)";
        return nullptr;
    }
    std::unique_ptr<ImGuiLayer> self(new ImGuiLayer(device));
    self->m_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(self->m_context);

    self->m_iniFile = options.iniFile;
    ImGui::GetIO().IniFilename = self->m_iniFile.empty() ? nullptr : self->m_iniFile.c_str();
    self->m_fontFile = ConfigureImGui(options, window.ContentScale());

    if (!ImGui_ImplSDL3_InitForSDLGPU(window.Handle())) {
        error = "ImGui_ImplSDL3_InitForSDLGPU failed";
        return nullptr;
    }
    self->m_platformInit = true;

    ImGui_ImplSDLGPU3_InitInfo info{};
    info.Device = native;
    info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(native, window.Handle());
    info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    if (!ImGui_ImplSDLGPU3_Init(&info)) {
        error = "ImGui_ImplSDLGPU3_Init failed";
        return nullptr;
    }
    self->m_rendererInit = true;
    return self;
}

ImGuiLayer::~ImGuiLayer()
{
    if (!m_context) {
        return;
    }
    ImGui::SetCurrentContext(m_context);
    if (m_rendererInit) {
        m_device.WaitIdle();
        ImGui_ImplSDLGPU3_Shutdown();
    }
    if (m_platformInit) {
        ImGui_ImplSDL3_Shutdown();
    }
    ImGui::DestroyContext(m_context);
}

void ImGuiLayer::ProcessEvent(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void ImGuiLayer::NewFrame()
{
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::Render(render::CommandBuffer& commands, render::Texture target, const render::Color& clear)
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    const bool empty = drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f;
    SDL_GPUCommandBuffer* native = render::sdlgpu::Native::CommandBuffer(commands);
    if (!native || !target || empty) {
        return;
    }

    // The backend uploads vertices in a copy pass, which must come before the render pass.
    ImGui_ImplSDLGPU3_PrepareDrawData(drawData, native);

    const render::ColorTargetInfo info{.texture = target, .clearColor = clear, .loadOp = render::LoadOp::Clear};
    render::RenderPass* pass = commands.BeginRenderPass({&info, 1}, nullptr);
    if (!pass) {
        return;
    }
    ImGui_ImplSDLGPU3_RenderDrawData(drawData, native, render::sdlgpu::Native::RenderPass(*pass));
    pass->End();
}

}  // namespace x2::app
