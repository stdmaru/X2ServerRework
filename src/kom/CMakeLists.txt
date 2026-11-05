# .kom archives (decision T35, docs/porting/kom.md).
# x2::kom: reading and mounting (client, launcher, tools). x2::kom_write: writing (tools and launcher only).

add_library(x2_kom STATIC
    src/archive.cpp
    src/codec.cpp
    src/header.cpp
    src/mount.cpp)
add_library(x2::kom ALIAS x2_kom)
target_include_directories(x2_kom PUBLIC include)
target_link_libraries(x2_kom PUBLIC x2::platform PRIVATE ZLIB::ZLIB)

add_library(x2_kom_write STATIC src/writer.cpp)
add_library(x2::kom_write ALIAS x2_kom_write)
target_link_libraries(x2_kom_write PUBLIC x2::kom PRIVATE ZLIB::ZLIB)
target_include_directories(x2_kom_write PRIVATE src)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
