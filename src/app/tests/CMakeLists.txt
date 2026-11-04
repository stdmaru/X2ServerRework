add_executable(x2_app_tests main.cpp test_app.cpp test_imgui.cpp)
target_link_libraries(x2_app_tests PRIVATE x2::app_imgui doctest::doctest)
x2_bundle_sdl(x2_app_tests)
add_test(NAME app COMMAND x2_app_tests)
