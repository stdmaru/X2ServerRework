# The ServerManager window (decisions T29, T30, T32; docs/tools/servermanager.md#window). The UI is a library so a
# headless test can draw it with ImGui's null backends.

add_library(x2_servermanager_ui STATIC src/manager_ui.cpp src/profile_editor.cpp)
target_include_directories(x2_servermanager_ui PUBLIC src)
target_link_libraries(x2_servermanager_ui PUBLIC x2::servermanager_core x2::imgui)

add_executable(ServerManager src/main.cpp)
target_link_libraries(ServerManager PRIVATE x2_servermanager_ui x2::app_imgui)
# no console window on Windows (SDL_main.h provides WinMain)
set_target_properties(ServerManager PROPERTIES WIN32_EXECUTABLE TRUE)

if(NOT WIN32)
    # A launcher for the desktop, without a terminal: profile.json next to the program, else the server package of this
    # build (dist/ in the working folder, X2_DIST_DIR's parent)
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ServerManager.desktop" CONTENT
"[Desktop Entry]
Type=Application
Name=X2 ServerManager
Comment=Start, stop and follow the X2 servers
Exec=\"$<TARGET_FILE:ServerManager>\"
Path=${X2_DIST_DIR}/..
Terminal=false
Categories=Development;
")
endif()

if(X2_BUILD_TESTS)
    add_executable(x2_servermanager_ui_tests tests/main.cpp tests/test_ui.cpp)
    target_link_libraries(x2_servermanager_ui_tests PRIVATE x2_servermanager_ui x2::app_imgui doctest::doctest)
    if(TARGET x2_terminal_demo)
        target_compile_definitions(x2_servermanager_ui_tests PRIVATE X2_TERMINAL_DEMO="$<TARGET_FILE:x2_terminal_demo>")
        add_dependencies(x2_servermanager_ui_tests x2_terminal_demo)
    endif()
    add_test(NAME servermanager_ui COMMAND x2_servermanager_ui_tests)
endif()
