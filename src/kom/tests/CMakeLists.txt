add_executable(x2_kom_tests main.cpp test_codec.cpp test_archive.cpp test_mount.cpp test_kompy.cpp test_client_data.cpp)
target_link_libraries(x2_kom_tests PRIVATE x2::kom_write doctest::doctest)
target_include_directories(x2_kom_tests PRIVATE ../src)
add_test(NAME kom COMMAND x2_kom_tests)

# Cross-check against tools/kom/kom.py, the format's reference (decision T35), when Python is available.
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    target_compile_definitions(x2_kom_tests PRIVATE
        X2_KOM_PYTHON="${Python3_EXECUTABLE}"
        X2_KOM_PY_DIR="${PROJECT_SOURCE_DIR}/tools/kom"
        X2_KOM_PY_CHECK="${CMAKE_CURRENT_SOURCE_DIR}/kompy_check.py")
endif()
