add_executable(x2_login_alignment_tests
    main.cpp
    test_user_list_alignment.cpp)
target_include_directories(x2_login_alignment_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(x2_login_alignment_tests PRIVATE x2::server_common doctest::doctest)
add_test(NAME login_user_list_alignment COMMAND x2_login_alignment_tests)
