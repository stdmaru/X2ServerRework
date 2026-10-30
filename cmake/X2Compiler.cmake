# Common compiler settings shared by every X2 target, applied through the
# x2::options interface library so third-party targets are left untouched.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# no C++20 modules: skip the dependency scan step (it also stops the whole build on the first scan error)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_library(x2_options INTERFACE)
add_library(x2::options ALIAS x2_options)

# Always defined (0/1) so code can use `#if X2_DEV_TOOLS`.
target_compile_definitions(x2_options INTERFACE
    X2_DEV_TOOLS=$<IF:$<BOOL:${X2_DEV_TOOLS}>,1,0>)

# The original is a Unicode build: TCHAR, LPCTSTR and the Win32 A/W macros are the wide ones.
if(WIN32)
    target_compile_definitions(x2_options INTERFACE UNICODE _UNICODE)
endif()

if(MSVC)
    target_compile_options(x2_options INTERFACE /utf-8 /W4 /permissive- /Zc:__cplusplus)
    target_compile_definitions(x2_options INTERFACE NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
else()
    # -fsigned-char: the original assumes MSVC's signed `char`; it is unsigned by default on Linux arm64.
    target_compile_options(x2_options INTERFACE -Wall -Wextra -finput-charset=UTF-8 -fsigned-char)
endif()
