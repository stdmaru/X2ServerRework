// x2::app without a display: SDL's dummy video driver (no GPU), so these run in the container and in CI.

#include "x2/app/app.h"
#include "x2/render/render.h"

#include <doctest/doctest.h>

#include <tuple>
#include <vector>

namespace {

struct DummySdl {
    DummySdl()
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        sdl = x2::app::Sdl::Init(error);
    }
    std::string error;
    std::unique_ptr<x2::app::Sdl> sdl;
};

}  // namespace

TEST_CASE("app: SDL initializes with the dummy video driver")
{
    DummySdl s;
    REQUIRE_MESSAGE(s.sdl, s.error);
    CHECK(std::string(SDL_GetCurrentVideoDriver()) == "dummy");
}

TEST_CASE("app: a window opens without a GPU and the null renderer claims it")
{
    DummySdl s;
    REQUIRE(s.sdl);
    std::string error;
    auto window = x2::app::Window::Create({}, error);
    REQUIRE_MESSAGE(window, error);     // the window no longer owns a GPU device (T36)
    CHECK(window->Id() != 0);

    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::Null;
    auto device = x2::render::Device::Create(options, error);
    REQUIRE(device);
    {
        x2::render::WindowClaim claim(*device, window->Handle());
        CHECK(claim.Ok());
        x2::render::WindowClaim twice(*device, window->Handle());
        CHECK_FALSE(twice.Ok());
    }
    x2::render::WindowClaim again(*device, window->Handle());     // released by the first claim
    CHECK(again.Ok());
}

TEST_CASE("app: PollEvents stops on quit and passes every event")
{
    DummySdl s;
    REQUIRE(s.sdl);

    SDL_Event user{};
    user.type = SDL_EVENT_USER;
    REQUIRE(SDL_PushEvent(&user));
    int users = 0;
    CHECK(x2::app::PollEvents([&](const SDL_Event& e) { users += e.type == SDL_EVENT_USER; }));
    CHECK(users == 1);

    SDL_Event quit{};
    quit.type = SDL_EVENT_QUIT;
    REQUIRE(SDL_PushEvent(&quit));
    CHECK_FALSE(x2::app::PollEvents({}));
}

TEST_CASE("app: a close request ends the loop only for the main window")
{
    DummySdl s;
    REQUIRE(s.sdl);

    SDL_Event close{};
    close.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    close.window.windowID = 42;
    REQUIRE(SDL_PushEvent(&close));
    CHECK(x2::app::PollEvents({}, 7));
    REQUIRE(SDL_PushEvent(&close));
    CHECK_FALSE(x2::app::PollEvents({}, 42));
}

TEST_CASE("app: WakeEventLoop wakes WaitForEvents")
{
    DummySdl s;
    REQUIRE(s.sdl);
    x2::app::WakeEventLoop();
    const Uint64 start = SDL_GetTicks();
    x2::app::WaitForEvents(5000);
    CHECK(SDL_GetTicks() - start < 4000);
    CHECK(x2::app::PollEvents({}));
}

namespace {

// A PNG of `width` x `height` (opaque red), encoded by SDL.
std::vector<unsigned char> EncodePng(int width, int height)
{
    SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    REQUIRE(surface);
    SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGBA(surface, 255, 0, 0, 255));
    SDL_IOStream* stream = SDL_IOFromDynamicMem();
    REQUIRE(stream);
    REQUIRE(SDL_SavePNG_IO(surface, stream, false));
    SDL_DestroySurface(surface);
    std::vector<unsigned char> png(static_cast<std::size_t>(SDL_GetIOSize(stream)));
    SDL_SeekIO(stream, 0, SDL_IO_SEEK_SET);
    REQUIRE(SDL_ReadIO(stream, png.data(), png.size()) == png.size());
    SDL_CloseIO(stream);
    return png;
}

// An .ico whose entries are `images` (width, height, bytes), in that order.
std::vector<unsigned char> MakeIco(const std::vector<std::tuple<int, int, std::vector<unsigned char>>>& images)
{
    std::vector<unsigned char> ico;
    auto put16 = [&ico](std::size_t v) { ico.push_back(v & 0xff); ico.push_back((v >> 8) & 0xff); };
    auto put32 = [&put16](std::size_t v) { put16(v & 0xffff); put16(v >> 16); };
    put16(0);
    put16(1);
    put16(images.size());
    std::size_t offset = 6 + 16 * images.size();
    for (const auto& [width, height, bytes] : images) {
        ico.push_back(static_cast<unsigned char>(width == 256 ? 0 : width));
        ico.push_back(static_cast<unsigned char>(height == 256 ? 0 : height));
        put16(0);   // colors, reserved
        put16(1);   // planes
        put16(32);  // bits per pixel
        put32(bytes.size());
        put32(offset);
        offset += bytes.size();
    }
    for (const auto& image : images) {
        const auto& bytes = std::get<2>(image);
        ico.insert(ico.end(), bytes.begin(), bytes.end());
    }
    return ico;
}

}  // namespace

TEST_CASE("app: LoadIcoImage decodes the largest PNG entry of an .ico")
{
    const std::vector<unsigned char> bmp(40, 0);  // a BMP entry (BITMAPINFOHEADER), skipped
    const auto ico = MakeIco({{48, 48, bmp}, {32, 32, EncodePng(32, 32)}, {256, 256, EncodePng(256, 256)}});
    std::string error;
    SDL_Surface* image = x2::app::LoadIcoImage(ico.data(), ico.size(), error);
    REQUIRE_MESSAGE(image, error);
    CHECK(image->w == 256);
    CHECK(image->h == 256);
    SDL_DestroySurface(image);

    const auto bmpOnly = MakeIco({{48, 48, bmp}});
    CHECK(x2::app::LoadIcoImage(bmpOnly.data(), bmpOnly.size(), error) == nullptr);
    CHECK(x2::app::LoadIcoImage(bmp.data(), bmp.size(), error) == nullptr);
}

TEST_CASE("app: a window's icon fails on bytes that are not an .ico")
{
    // The dummy video driver does not support window icons, so only the decoding error path runs here.
    DummySdl s;
    REQUIRE(s.sdl);
    std::string error;
    auto window = x2::app::Window::Create({}, error);
    REQUIRE_MESSAGE(window, error);
    const unsigned char notIco[] = {'P', 'K', 3, 4, 0, 0};
    CHECK_FALSE(window->SetIcon(notIco, sizeof(notIco), error));
    CHECK(error == "not an .ico file");
}
