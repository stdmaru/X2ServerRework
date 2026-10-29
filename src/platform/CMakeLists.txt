# x2::platform - the only place that talks to the OS directly (docs/porte/platform.md).

add_library(x2_platform STATIC
    src/crt_posix.cpp
    src/d3dx_math.cpp
    src/debug.cpp
    src/string.cpp
    src/sync_posix.cpp
    src/time.cpp
    src/unicode.cpp)
add_library(x2::platform ALIAS x2_platform)

target_include_directories(x2_platform PUBLIC include)
target_link_libraries(x2_platform PUBLIC x2::options)

if(WIN32)
    target_link_libraries(x2_platform PUBLIC winmm)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(x2_platform PUBLIC Threads::Threads)
endif()

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
