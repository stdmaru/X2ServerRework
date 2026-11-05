add_executable(kncsdk_tests
    main.cpp
    test_crypto.cpp
    test_log.cpp
    test_pool.cpp
    test_security.cpp
    test_serializer.cpp
    test_thread.cpp
    test_util.cpp)
target_link_libraries(kncsdk_tests PRIVATE x2::kncsdk doctest::doctest)
add_test(NAME kncsdk COMMAND kncsdk_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(TARGET x2::kncsdk_lua)
    add_executable(kncsdk_lua_tests main.cpp test_lua_tinker.cpp)
    target_link_libraries(kncsdk_lua_tests PRIVATE x2::kncsdk_lua doctest::doctest)
    add_test(NAME kncsdk_lua COMMAND kncsdk_lua_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Manual test of the server terminal (T15) without a database: run it in a console and type Lua.
if(TARGET x2::kncsdk_lua)
    add_executable(x2_terminal_demo terminal_demo.cpp)
    target_link_libraries(x2_terminal_demo PRIVATE x2::kncsdk_lua)
endif()

# test_lua_tinker.cpp runs lua_interpreter in x2_terminal_demo --interpreter, with a real stdin
if(TARGET x2_terminal_demo AND TARGET kncsdk_lua_tests)
    target_compile_definitions(kncsdk_lua_tests PRIVATE X2_TERMINAL_DEMO="$<TARGET_FILE:x2_terminal_demo>")
    add_dependencies(kncsdk_lua_tests x2_terminal_demo)
endif()
