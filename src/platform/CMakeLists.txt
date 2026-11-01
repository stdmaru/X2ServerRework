# x2::platform - the only place that talks to the OS directly (docs/porting/platform.md).

add_library(x2_platform STATIC
    src/console.cpp
    src/control.cpp
    src/crt_posix.cpp
    src/d3dx_math.cpp
    src/debug.cpp
    src/ini.cpp
    src/json.cpp
    src/net_io.cpp
    src/string.cpp
    src/terminal.cpp
    src/sync_posix.cpp
    src/text.cpp
    src/time.cpp
    src/unicode.cpp)
add_library(x2::platform ALIAS x2_platform)

target_include_directories(x2_platform PUBLIC include)
target_link_libraries(x2_platform PUBLIC x2::options PRIVATE x2::asio)

if(WIN32)
    target_link_libraries(x2_platform PUBLIC winmm ws2_32 mswsock shell32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(x2_platform PUBLIC Threads::Threads)
endif()

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
