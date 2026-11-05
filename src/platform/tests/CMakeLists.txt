add_executable(x2_platform_tests
    main.cpp
    test_control.cpp
    test_crt.cpp
    test_d3dx_math.cpp
    test_file.cpp
    test_net_io.cpp
    test_odbc.cpp
    test_format.cpp
    test_json.cpp
    test_string.cpp
    test_sync.cpp
    test_terminal.cpp
    test_text.cpp
    test_time.cpp
    test_unicode.cpp)
target_link_libraries(x2_platform_tests PRIVATE x2::platform x2::odbc doctest::doctest)
add_test(NAME platform COMMAND x2_platform_tests)

# child process for test_process.cpp
add_executable(x2_process_helper process_helper.cpp)
target_link_libraries(x2_process_helper PRIVATE x2::platform)
target_sources(x2_platform_tests PRIVATE test_process.cpp)
target_compile_definitions(x2_platform_tests PRIVATE X2_PROCESS_HELPER="$<TARGET_FILE:x2_process_helper>")
add_dependencies(x2_platform_tests x2_process_helper)
