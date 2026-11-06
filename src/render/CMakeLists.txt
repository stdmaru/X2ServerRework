# x2::render: the graphics boundary (decision T36, docs/porting/render.md). Backends: null (headless tests) and sdlgpu.
# SDL_GPU is used only under backends/sdlgpu; x2::render_sdlgpu adds the escape hatch header (x2/render/sdlgpu.h).

add_library(x2_render STATIC
    src/render.cpp
    backends/null/null_device.cpp
    backends/sdlgpu/sdlgpu_device.cpp)
add_library(x2::render ALIAS x2_render)
target_include_directories(x2_render PUBLIC include PRIVATE backends/sdlgpu/include)
target_link_libraries(x2_render PUBLIC x2::options PRIVATE SDL3::SDL3)

add_library(x2_render_sdlgpu INTERFACE)
add_library(x2::render_sdlgpu ALIAS x2_render_sdlgpu)
target_include_directories(x2_render_sdlgpu INTERFACE backends/sdlgpu/include)
target_link_libraries(x2_render_sdlgpu INTERFACE x2_render SDL3::SDL3)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
