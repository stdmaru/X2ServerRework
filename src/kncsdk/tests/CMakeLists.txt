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
