# SDL3 application foundation (decision T32, docs/porting/app.md).
# x2::app: window and events (client and tools; the GPU device is x2::render's, T36). x2::app_imgui: Dear ImGui on top of it (tools only).

add_library(x2_app STATIC src/app.cpp)
add_library(x2::app ALIAS x2_app)
target_include_directories(x2_app PUBLIC include)
target_link_libraries(x2_app PUBLIC x2::options SDL3::SDL3)

add_library(x2_app_imgui STATIC src/imgui_layer.cpp)
add_library(x2::app_imgui ALIAS x2_app_imgui)
target_link_libraries(x2_app_imgui PUBLIC x2::app x2::render x2::imgui PRIVATE x2::render_sdlgpu)

# Manual test: a window with the ImGui demo (docs/porting/app.md).
add_executable(x2_app_demo demo/main.cpp)
target_link_libraries(x2_app_demo PRIVATE x2::app_imgui)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
