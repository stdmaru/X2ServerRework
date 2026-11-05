// x2::app without a display: SDL's dummy video driver (no GPU), so these run in the container and in CI.

#include "x2/app/app.h"

#include <doctest/doctest.h>

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

TEST_CASE("app: a window without a GPU fails cleanly")
{
    DummySdl s;
    REQUIRE(s.sdl);
    std::string error;
    auto window = x2::app::Window::Create({}, error);
    // The dummy driver has no surface for a GPU swapchain: either step fails with a message, never a crash.
    if (!window) {
        CHECK_FALSE(error.empty());
    }
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
