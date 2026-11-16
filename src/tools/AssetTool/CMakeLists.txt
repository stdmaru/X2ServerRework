# AssetTool (decision T35, docs/tools/client-data.md): the client's .kom archives as an organized client_data folder.
# x2::assets: import, pack, status and the manager's model (tested headless); x2_assets: its window and command line.

add_library(x2_assets_core STATIC
    src/client_data.cpp
    src/layout.cpp
    src/manifest.cpp
    src/texture_sizes.cpp
    src/util.cpp
    src/workspace.cpp)
add_library(x2::assets ALIAS x2_assets_core)
target_include_directories(x2_assets_core PUBLIC include)
target_link_libraries(x2_assets_core PUBLIC x2::kom_write PRIVATE x2::kncsdk x2::lua)

add_executable(x2_assets src/main.cpp)
target_link_libraries(x2_assets PRIVATE x2::assets x2::platform)
# The window (T50) where the SDL3 + ImGui foundation is built; the command line (--cli) everywhere.
if(TARGET x2::app_imgui)
    target_sources(x2_assets PRIVATE src/gui.cpp)
    target_link_libraries(x2_assets PRIVATE x2::app_imgui)
    target_compile_definitions(x2_assets PRIVATE X2_ASSETS_WINDOW)
endif()

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
