add_executable(x2_render_tests main.cpp test_null.cpp test_sdlgpu.cpp test_boundary.cpp)
target_link_libraries(x2_render_tests PRIVATE x2::render_sdlgpu doctest::doctest)
target_compile_definitions(x2_render_tests PRIVATE X2_SOURCE_ROOT="${PROJECT_SOURCE_DIR}/src")
add_test(NAME render COMMAND x2_render_tests)
