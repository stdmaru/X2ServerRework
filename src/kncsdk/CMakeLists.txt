# KNCSDK - internal SDK shared by servers and client (docs/porte/kncsdk.md).
# The source tree mirrors the original Include/ folder, which is also the include root.

add_library(kncsdk STATIC
    KncUtil.cpp
    Crypto/Des.cpp
    Crypto/Hash.cpp
    dbg/assert.cpp
    dbg/logfile.cpp
    KncSecurity/ByteStream.cpp
    KncSecurity/KncSecurity.cpp
    KncSecurity/SADatabase.cpp
    KncSecurity/SecureBuffer.cpp
    KncSecurity/SecurityAssociation.cpp
    Serializer/ByteOrder.cpp
    Serializer/SerBuffer.cpp
    Serializer/Serializer.cpp)
add_library(x2::kncsdk ALIAS kncsdk)

target_include_directories(kncsdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(kncsdk PUBLIC x2::platform PRIVATE ZLIB::ZLIB)

# lua_tinker (C++ <-> Lua binding)
if(TARGET x2::luajit)
    add_library(kncsdk_lua STATIC lua_tinker.cpp lua_interpreter.cpp)
    add_library(x2::kncsdk_lua ALIAS kncsdk_lua)
    target_link_libraries(kncsdk_lua PUBLIC x2::kncsdk x2::luajit)
endif()

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
