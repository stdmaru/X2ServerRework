# Client (docs/porting/client.md). Layers mirror the original projects (decision T14) and are added here as they
# compile: so far LuaCommon, the DXUT replacement, KTDXLIB's input layer, mass file manager and web pages, the movie
# player, X2ServerProtocol and the executable's skeleton. KTDXLIB and X2Lib come next (roadmap Phase 3) and link
# x2_client_includes.

# Include directories of the original projects (KTDXLIB, X2Lib, X2 and X2ServerProtocol .vcxproj), for every layer:
# the layers include each other's headers by bare name. KTDXLIB/DXUT is replaced by DXUT/. The servers' Common (packets,
# event IDs, enums) is shared with the client (decision T33).
add_library(x2_client_includes INTERFACE)
target_include_directories(x2_client_includes INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/KTDXLIB
    ${CMAKE_CURRENT_SOURCE_DIR}/DXUT
    ${CMAKE_CURRENT_SOURCE_DIR}/X2Lib
    ${CMAKE_CURRENT_SOURCE_DIR}/X2Lib/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/X2
    ${CMAKE_CURRENT_SOURCE_DIR}/X2ServerProtocol
    ${PROJECT_SOURCE_DIR}/src/server/Common)

# LuaCommon: the client's Lua binding and its own lua_tinker (docs/porting/client-luacommon.md), on upstream LuaJIT (T21).
if(TARGET x2::luajit)
    add_library(x2_client_luacommon STATIC
        LuaCommon/KLuabinder.cpp
        LuaCommon/KLuaManager.cpp
        LuaCommon/lua_tinker.cpp)
    add_library(x2::client_luacommon ALIAS x2_client_luacommon)
    target_include_directories(x2_client_luacommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/LuaCommon)
    target_link_libraries(x2_client_luacommon PUBLIC x2::platform x2::luajit)
else()
    message(WARNING "LuaJIT not found: the client's LuaCommon is skipped (docs/project/decisions.md, T21)")
endif()

# DXUT replacement: window, message pump and frame loop on x2::app and x2::render, SDL3 events as Win32 messages
# (docs/porting/client-dxut.md).
add_library(x2_client_dxut STATIC DXUT/DXUT.cpp DXUT/Ime.cpp DXUT/SdlInput.cpp DXUT/SdlMessages.cpp)
add_library(x2::client_dxut ALIAS x2_client_dxut)
target_include_directories(x2_client_dxut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/DXUT)
target_link_libraries(x2_client_dxut PUBLIC x2::platform x2::app x2::render)

# KTDXLIB's input layer (CKTDIManager, CKTDIDevice) on the DXUT replacement's SDL3 state instead of DirectInput
# (docs/porting/client-input.md). Its files join the KTDXLIB target once the engine builds; CKTDXStage's base classes
# come from there too (the tests supply them).
add_library(x2_client_ktdi STATIC KTDXLIB/KTDIDevice.cpp KTDXLIB/KTDIManager.cpp)
add_library(x2::client_ktdi ALIAS x2_client_ktdi)
target_link_libraries(x2_client_ktdi PUBLIC x2_client_includes x2::client_dxut)

# KTDXLIB's mass file manager (KGCMassFileManager, KGCMassFileBufferManager): the client's files by name from the .kom
# archives on x2::kom and, with X2_DEV_TOOLS, loose files (decision T35, docs/porting/client-ktdxlib.md#mass-files).
# Its files join the KTDXLIB target once the engine builds.
add_library(x2_client_massfile STATIC KTDXLIB/KGCMassFileBufferManager.cpp KTDXLIB/KGCMassFileManager.cpp)
add_library(x2::client_massfile ALIAS x2_client_massfile)
target_include_directories(x2_client_massfile PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/KTDXLIB)
target_link_libraries(x2_client_massfile PUBLIC x2::platform PRIVATE x2::kom x2::kncsdk)

# KTDXLIB's web pages in the system browser (KBrowserWrapper on SDL_OpenURL, in place of the embedded Internet Explorer;
# docs/porting/client-ktdxlib.md#web-pages). Its files join the KTDXLIB target once the engine builds.
add_library(x2_client_browser STATIC KTDXLIB/KBrowserWrapper.cpp)
add_library(x2::client_browser ALIAS x2_client_browser)
target_include_directories(x2_client_browser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/KTDXLIB)
target_link_libraries(x2_client_browser PUBLIC x2::platform PRIVATE SDL3::SDL3)

# The client's MPEG-1 movies on pl_mpeg, replacing BandiVideo (docs/porting/client-movie.md). KTDXLIB's CMovieData plays
# them; new code with no equivalent file in the original.
add_library(x2_client_movie STATIC Movie/MoviePlayer.cpp)
add_library(x2::client_movie ALIAS x2_client_movie)
target_include_directories(x2_client_movie PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Movie)
target_link_libraries(x2_client_movie PUBLIC x2::options PRIVATE x2::pl_mpeg)

# X2ServerProtocol: the client's connections to the servers (docs/porting/client-serverprotocol.md). The event/actor
# model and the TCP layer are the servers' x2::common_net (decision T34); only the client's proxies and the
# CX2ServerProtocol facade are here.
add_library(x2_client_serverprotocol STATIC
    X2ServerProtocol/ActorProxy.cpp
    X2ServerProtocol/EventNames.cpp
    X2ServerProtocol/TRUser.cpp
    X2ServerProtocol/UserProxy.cpp
    X2ServerProtocol/X2ServerProtocol.cpp)
add_library(x2::client_serverprotocol ALIAS x2_client_serverprotocol)
target_include_directories(x2_client_serverprotocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/X2ServerProtocol)
target_link_libraries(x2_client_serverprotocol PUBLIC x2::common_net)

# The executable (x2.exe in the original). X2/X2Skeleton.cpp stands in for X2.cpp until KTDXLIB and X2Lib build
# (docs/porting/client-x2.md).
add_executable(x2_client X2/X2Skeleton.cpp)
target_link_libraries(x2_client PRIVATE x2::client_dxut)
set_target_properties(x2_client PROPERTIES OUTPUT_NAME x2 WIN32_EXECUTABLE TRUE)  # SDL_main.h provides WinMain
x2_windows_icon(x2_client ${CMAKE_CURRENT_SOURCE_DIR}/X2/Elsword.ico)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
