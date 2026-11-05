# Lua data of the client (decision T35, docs/tools/client-data.md#lua): KOG's LuaJIT bytecode decompiler and
# Item.kim. Kept apart from x2::kom, which only knows the archive format.

add_library(x2_lua STATIC
    src/bytecode.cpp
    src/decompile.cpp
    src/kim.cpp
    src/lua_state.cpp)
add_library(x2::lua ALIAS x2_lua)
target_include_directories(x2_lua PUBLIC include)
target_link_libraries(x2_lua PRIVATE x2::luajit)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
