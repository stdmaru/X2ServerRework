add_executable(x2_lua_tests main.cpp test_decompile.cpp)
target_link_libraries(x2_lua_tests PRIVATE x2::lua doctest::doctest)
add_test(NAME lua COMMAND x2_lua_tests)
