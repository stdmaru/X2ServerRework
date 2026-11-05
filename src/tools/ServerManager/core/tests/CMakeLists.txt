add_executable(x2_servermanager_tests main.cpp test_log_store.cpp test_protocol.cpp test_session.cpp)
target_link_libraries(x2_servermanager_tests PRIVATE x2::servermanager_core doctest::doctest)
# against a real server console (KServerConsole) over its control socket
if(TARGET x2::kncsdk_lua)
    target_sources(x2_servermanager_tests PRIVATE test_console.cpp)
    target_link_libraries(x2_servermanager_tests PRIVATE x2::kncsdk_lua)
endif()
add_test(NAME servermanager COMMAND x2_servermanager_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
