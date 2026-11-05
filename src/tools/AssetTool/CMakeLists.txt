# AssetTool (decision T35, docs/tools/client-data.md): the client's .kom archives as an organized client_data folder.
# x2::assets: import, pack and status (tested headless); x2_assets: its command line.

add_library(x2_assets_core STATIC
    src/client_data.cpp
    src/layout.cpp
    src/manifest.cpp)
add_library(x2::assets ALIAS x2_assets_core)
target_include_directories(x2_assets_core PUBLIC include)
target_link_libraries(x2_assets_core PUBLIC x2::kom_write PRIVATE x2::kncsdk)

add_executable(x2_assets src/main.cpp)
target_link_libraries(x2_assets PRIVATE x2::assets x2::platform)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
