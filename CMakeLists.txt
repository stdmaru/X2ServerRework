cmake_minimum_required(VERSION 3.28)

project(X2Rework
    VERSION 0.1.0
    DESCRIPTION "Elsword server/client port (Windows + Linux)"
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(X2_DEV_TOOLS    "Enable in-game/server debug, development and admin tools" OFF)
option(X2_BUILD_SERVER "Build the servers" ON)
option(X2_BUILD_CLIENT "Build the client" OFF)
option(X2_BUILD_TESTS  "Build unit tests" ON)

# x86_64 and arm64 (decisions T6, T12)
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "X2Rework targets 64-bit only (see docs/projeto/decisoes.md, T6)")
endif()

include(X2Compiler)
include(X2Dependencies)

if(X2_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(src)
