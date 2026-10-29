cmake_minimum_required(VERSION 3.28)

project(X2Rework
    VERSION 0.1.0
    DESCRIPTION "Elsword server/client port (Windows + Linux)"
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(X2_DEV_TOOLS    "Enable in-game/server debug, development and admin tools" OFF)
option(X2_BUILD_SERVER "Build the servers" ON)
option(X2_BUILD_CLIENT "Build the client" OFF)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "X2Rework targets 64-bit only (see docs/core/decisoes.md, T6)")
endif()

include(X2Compiler)

add_subdirectory(src)
