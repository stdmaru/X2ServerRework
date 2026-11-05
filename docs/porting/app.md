[Home](../README.md) › [Porting](README.md) › App foundation

# App foundation (`src/app`, targets `x2::app`, `x2::app_imgui`)

New code, with no equivalent in the original (which used Win32 + Direct3D 9 directly). The SDL3 foundation shared by the client
(Phase 3, T19) and the GUI tools (T29), written first for ServerManager (T30) on 2026-11-04 (`tools/sdl3-imgui-base`). Where it
lives and how its dependencies enter the build is decision T32.

| Target | For | Contents |
|---|---|---|
| `x2::app` (`x2/app/app.h`) | client and tools | `Sdl` (process-wide `SDL_Init`/`SDL_Quit`), `Window` (SDL3 window with its own SDL_GPU device; `BeginFrame`/`EndFrame` acquire the command buffer and swapchain texture), `PollEvents` (quit on `SDL_EVENT_QUIT` or the main window's close request), `WaitForEvents`/`WakeEventLoop` (tools that redraw only on change) |
| `x2::app_imgui` (`x2/app/imgui_layer.h`) | tools only | `ImGuiLayer` (ImGui context with the `sdl3` platform and `sdlgpu3` renderer backends), `ConfigureImGui` (style, DPI scale, fonts), `DefaultFontFiles` |

- **Rendering is SDL_GPU only** (T19), never SDL_Renderer or OpenGL, so the client inherits the same device. The device accepts
  SPIR-V, DXIL, and MSL shaders (Vulkan, D3D12, Metal); `X2_DEV_TOOLS` builds request validation layers.
- **High DPI**: the window size is multiplied by the display content scale, and ImGui scales its style and fonts by it.
- **Fonts**: server logs contain Korean, and ImGui's built-in font is ASCII only. `DefaultFontFiles` tries Malgun Gothic (Windows) or
  Noto Sans CJK / Nanum Gothic (Linux); glyphs are rasterized on demand, so a CJK font costs nothing until used. Without any of them,
  the built-in font is used and Hangul shows as `?`. No font is shipped with the repository.
- SDL headers may appear in `x2/app` headers: this is the SDL layer, and SDL does not include `windows.h`. Process spawning and other
  OS APIs the tools need still belong to [`x2::platform`](platform.md).

## Dependencies (T32)

| Library | Version (2026-11-04) | Source |
|---|---|---|
| SDL3 | 3.4.18 | pinned `FetchContent`, built from source and linked **static** on Linux and Windows (no `find_package`) |
| Dear ImGui | 1.93.0 | pinned `FetchContent`, compiled into `x2::imgui` with the `null`, `sdl3`, and `sdlgpu3` backends |

- Static everywhere (user's choice, 2026-11-04): executables need no SDL library next to them, and one built in the container runs on
  the desktop (it depends only on libc/libstdc++). SDL loads X11, Wayland, KMSDRM, ALSA, PulseAudio, and PipeWire at run time; it is
  built against their headers (`docker/build/Dockerfile`). Configured with video drivers x11, wayland, kmsdrm, offscreen, dummy and
  GPU driver vulkan (Linux, 2026-11-04). On 2026-11-04 it briefly used Ubuntu's shared `libsdl3-dev`, copied next to the executable.
- `X2_BUILD_TOOLS` (on by default) builds `src/app` and the tools; the servers never link SDL.
- ImGui and SDL are third-party: compiled without the project's warning flags, and their headers are `SYSTEM`.

## Tests and demo

- `x2_app_tests` (`ctest -R app`): run without a display, in the container and in CI. SDL with the `dummy` video driver (event loop,
  close request for the main window only, `WakeEventLoop`; window creation without a GPU fails with a message instead of crashing) and
  ImGui with its `null` backends (font fallback, first existing font file, a frame with Korean text and DPI scale).
- `x2_app_demo`: manual test on a desktop (window, SDL_GPU driver, input, DPI, Hangul font) with the ImGui demo. Built in the container,
  it runs on the host from `build/<preset>/src/app/`. Tested on Arch and Windows on 2026-11-04 (works; Linux opens a terminal only when
  started from a file manager, since it is a console program).

## Pending

- Shader handling for the client (SDL_shadercross, T19) and the Win32-style `MsgProc` event translation (Phase 3) build on `x2::app`.
- Windows GUI subsystem (`WinMain`, no console) for the tools, once ServerManager has a UI.
