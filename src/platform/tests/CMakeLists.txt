add_executable(x2_platform_tests
    main.cpp
    test_crt.cpp
    test_d3dx_math.cpp
    test_net_io.cpp
    test_odbc.cpp
    test_format.cpp
    test_string.cpp
    test_sync.cpp
    test_text.cpp
    test_time.cpp
    test_unicode.cpp)
target_link_libraries(x2_platform_tests PRIVATE x2::platform x2::odbc doctest::doctest)
add_test(NAME platform COMMAND x2_platform_tests)
