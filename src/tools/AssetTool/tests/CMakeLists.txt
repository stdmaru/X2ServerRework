add_executable(x2_assets_tests main.cpp test_layout.cpp test_manifest.cpp test_client_data.cpp test_client_lua.cpp test_client_import.cpp)
target_link_libraries(x2_assets_tests PRIVATE x2::assets x2::lua doctest::doctest)
add_test(NAME assets COMMAND x2_assets_tests)
# The opt-in client test imports with the versioned layout rules.
target_compile_definitions(x2_assets_tests PRIVATE X2_ASSETS_LAYOUT="${PROJECT_SOURCE_DIR}/tools/assets/layout.txt")
