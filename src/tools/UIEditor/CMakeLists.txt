# UI editor (decisions T29, T32, T46; docs/tools/ui-editor.md): the client's dialogs drawn by KTDXLIB's own GUI code
# in an SDL3 + ImGui window. Dev builds only: it reads the client_data's loose files (T35 (3)).

add_library(x2_ui_editor_core STATIC src/anchors_file.cpp src/engine.cpp src/live.cpp src/record.cpp src/script_edit.cpp src/tet_file.cpp)
target_include_directories(x2_ui_editor_core PUBLIC src)
target_link_libraries(x2_ui_editor_core PUBLIC x2::render PRIVATE x2::client_ktdxlib)

add_executable(x2_ui_editor src/main.cpp)
target_link_libraries(x2_ui_editor PRIVATE x2_ui_editor_core x2::app_imgui)
set_target_properties(x2_ui_editor PROPERTIES WIN32_EXECUTABLE TRUE)  # SDL_main.h provides WinMain

if(X2_BUILD_TESTS)
    add_executable(x2_ui_editor_tests tests/main.cpp tests/test_anchors_file.cpp tests/test_engine.cpp tests/test_live.cpp
                                      tests/test_record.cpp tests/test_script_edit.cpp tests/test_tet_file.cpp)
    target_link_libraries(x2_ui_editor_tests PRIVATE x2_ui_editor_core doctest::doctest)
    add_test(NAME ui_editor COMMAND x2_ui_editor_tests)
endif()
