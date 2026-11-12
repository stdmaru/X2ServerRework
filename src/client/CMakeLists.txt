# Client (docs/porting/client.md). Layers mirror the original projects (decision T14) and are added here as they
# compile: so far LuaCommon, the DXUT replacement, KTDXLIB (with its mass file manager and web pages), the movie
# player, X2ServerProtocol and the executable's skeleton. X2Lib comes next (roadmap Phase 3) and links
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

# KTDXLIB's D3D9 device: the engine's D3D9 calls translated onto x2::render (decisions T36 (5), T38), with
# fixed-function shaders compiled offline (T39, D3D9/shaders). docs/porting/client-d3d9.md
add_library(x2_client_d3d9 STATIC KTDXLIB/D3D9/Animation.cpp KTDXLIB/D3D9/Device.cpp KTDXLIB/D3D9/D3DX.cpp KTDXLIB/D3D9/D3DXTexture.cpp
    KTDXLIB/D3D9/Effect.cpp KTDXLIB/D3D9/Image.cpp KTDXLIB/D3D9/ImageConvert.cpp KTDXLIB/D3D9/Mesh.cpp KTDXLIB/D3D9/Resources.cpp
    KTDXLIB/D3D9/SkinInfo.cpp KTDXLIB/D3D9/XFile.cpp KTDXLIB/D3D9/XMesh.cpp)
add_library(x2::client_d3d9 ALIAS x2_client_d3d9)
target_include_directories(x2_client_d3d9 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/KTDXLIB/D3D9)
target_link_libraries(x2_client_d3d9 PUBLIC x2::platform x2::render PRIVATE SDL3::SDL3 x2::stb ZLIB::ZLIB)

# DXUT replacement: window, message pump and frame loop on x2::app and x2::render, SDL3 events as Win32 messages
# (docs/porting/client-dxut.md).
add_library(x2_client_dxut STATIC DXUT/Camera.cpp DXUT/DXUT.cpp DXUT/Ime.cpp DXUT/MessageBox.cpp DXUT/SdlInput.cpp DXUT/SdlMessages.cpp
                              DXUT/WindowFunctions.cpp)
add_library(x2::client_dxut ALIAS x2_client_dxut)
target_include_directories(x2_client_dxut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/DXUT)
target_link_libraries(x2_client_dxut PUBLIC x2::platform x2::app x2::render x2::client_d3d9)

# KTDXLIB's mass file manager (KGCMassFileManager, KGCMassFileBufferManager): the client's files by name from the .kom
# archives on x2::kom and, with X2_DEV_TOOLS, loose files (decision T35, docs/porting/client-ktdxlib.md#mass-files).
# KTDXLIB links it.
add_library(x2_client_massfile STATIC KTDXLIB/KGCMassFileBufferManager.cpp KTDXLIB/KGCMassFileManager.cpp)
add_library(x2::client_massfile ALIAS x2_client_massfile)
target_include_directories(x2_client_massfile PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/KTDXLIB)
target_link_libraries(x2_client_massfile PUBLIC x2::platform PRIVATE x2::kom x2::kncsdk)

# KTDXLIB's web pages in the system browser (KBrowserWrapper on SDL_OpenURL, in place of the embedded Internet Explorer;
# docs/porting/client-ktdxlib.md#web-pages). KTDXLIB links it.
add_library(x2_client_browser STATIC KTDXLIB/KBrowserWrapper.cpp)
add_library(x2::client_browser ALIAS x2_client_browser)
target_include_directories(x2_client_browser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/KTDXLIB)
target_link_libraries(x2_client_browser PUBLIC x2::platform PRIVATE SDL3::SDL3)

# KTDXLIB, the engine (docs/porting/client-ktdxlib.md), on the D3D9 device (T38), with its input layer on the DXUT
# replacement's SDL3 state (docs/porting/client-input.md). Every file includes stdafx.h -> KTDX.h, which includes all of
# the engine's headers. The mass file manager and the web pages are libraries of their own (tested alone).
add_library(x2_client_ktdxlib STATIC
    KTDXLIB/CRC_32.cpp KTDXLIB/KExceptionSafeUtil.cpp KTDXLIB/KFileSerializer.cpp KTDXLIB/KHangulPPWSelector.cpp
    KTDXLIB/KHelpSystem.cpp KTDXLIB/KLuaManagerProxy.cpp KTDXLIB/KProtectRefTable.cpp KTDXLIB/KSmartNull.cpp
    KTDXLIB/KSuperAssert.cpp KTDXLIB/KSystemTimer.cpp KTDXLIB/KTDGCamera.cpp KTDXLIB/KTDGDynamicTexture.cpp
    KTDXLIB/KTDGDynamicVBManager.cpp KTDXLIB/KTDGEffectStateManager.cpp KTDXLIB/KTDGFontManager.cpp
    KTDXLIB/KTDGFontUtil.cpp KTDXLIB/KTDGFreeTypeFont.cpp KTDXLIB/KTDGFrustum.cpp KTDXLIB/KTDGGlow.cpp
    KTDXLIB/KTDGLensFlare.cpp KTDXLIB/KTDGLineMap.cpp KTDXLIB/KTDGManager.cpp
    KTDXLIB/KTDGMatrix.cpp KTDXLIB/KTDGMotionBlur.cpp KTDXLIB/KTDGObject.cpp KTDXLIB/KTDGParticleSystem.cpp
    KTDXLIB/KTDGPicChar.cpp KTDXLIB/KTDGPlanShadow.cpp KTDXLIB/KTDGSkyDome.cpp
    KTDXLIB/KTDGSlashTrace.cpp KTDXLIB/KTDGStateManager.cpp KTDXLIB/KTDGTargetArrow.cpp KTDXLIB/KTDGUIButton.cpp
    KTDXLIB/KTDGUICheckBox.cpp KTDXLIB/KTDGUIComboBox.cpp KTDXLIB/KTDGUIControl.cpp KTDXLIB/KTDGUIControlList.cpp
    KTDXLIB/KTDGUIDialog.cpp KTDXLIB/KTDGUIDialogHandle.cpp KTDXLIB/KTDGUIDialogManager.cpp
    KTDXLIB/KTDGUIEditBox.cpp KTDXLIB/KTDGUIIMEEditBox.cpp KTDXLIB/KTDGUIItem.cpp KTDXLIB/KTDGUIListBox.cpp
    KTDXLIB/KTDGUIListControl.cpp KTDXLIB/KTDGUINavigation.cpp KTDXLIB/KTDGUIRadioButton.cpp
    KTDXLIB/KTDGUIScrollBar.cpp KTDXLIB/KTDGUISlider.cpp KTDXLIB/KTDGUISlot.cpp KTDXLIB/KTDGUIStatic.cpp
    KTDXLIB/KTDGUIUniBuffer.cpp KTDXLIB/KTDGXMeshPlayer.cpp KTDXLIB/KTDGXRenderer.cpp
    KTDXLIB/KTDGXSkinAfterImage.cpp KTDXLIB/KTDGXSkinAnim.cpp KTDXLIB/KTDIDevice.cpp KTDXLIB/KTDIManager.cpp
    KTDXLIB/KTDNDP.cpp KTDXLIB/KTDNManager.cpp KTDXLIB/KTDNPacket.cpp KTDXLIB/KTDNTransfer.cpp KTDXLIB/KTDNUDP.cpp
    KTDXLIB/KTDNUDPStatisticData.cpp KTDXLIB/KTDSMP3Play.cpp KTDXLIB/KTDSManager.cpp
    KTDXLIB/KTDXAnimationController.cpp KTDXLIB/KTDXApp.cpp KTDXLIB/KTDXCollision.cpp KTDXLIB/KTDXDevice.cpp
    KTDXLIB/KTDXDeviceBaseTexture.cpp KTDXLIB/KTDXDeviceHolder.cpp KTDXLIB/KTDXDeviceManager.cpp
    KTDXLIB/KTDXDeviceRenderTargetTexture.cpp KTDXLIB/KTDXDeviceSound.cpp KTDXLIB/KTDXDeviceTET_Preprocessing.cpp
    KTDXLIB/KTDXDeviceTexture.cpp KTDXLIB/KTDXDeviceXET.cpp KTDXLIB/KTDXDeviceXET_Preprocessing.cpp
    KTDXLIB/KTDXDeviceXMesh.cpp KTDXLIB/KTDXDeviceXSkinMesh.cpp KTDXLIB/KTDXEnumToLua.cpp KTDXLIB/KTDXLog.cpp
    KTDXLIB/KTDXRandomNumbers.cpp KTDXLIB/KTDXRecyclingArray.cpp KTDXLIB/KTDXSimpleHandleInterface.cpp
    KTDXLIB/KTDXStage.cpp KTDXLIB/KTDXStringLoader.cpp KTDXLIB/KTDXStringTable.cpp KTDXLIB/KTDXThread.cpp
    KTDXLIB/KTDXTimer.cpp KTDXLIB/KVersionControl.cpp KTDXLIB/MinMax.cpp KTDXLIB/debug.cpp KTDXLIB/stdafx.cpp)
add_library(x2::client_ktdxlib ALIAS x2_client_ktdxlib)
target_link_libraries(x2_client_ktdxlib PUBLIC
    x2_client_includes x2::platform x2::kncsdk x2::client_d3d9 x2::client_dxut x2::client_luacommon
    x2::client_massfile x2::client_browser x2::client_movie Freetype::Freetype ZLIB::ZLIB)

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

# X2Lib, the game (docs/porting/client-x2lib.md). Every file includes stdafx.h -> X2Main.h, which includes the engine and
# almost every game header. The servers' Common is shared (T33): X2UDPPacket.* compiles through X2Packet.cpp, the packet
# files through X2ServerPacket.cpp, and NetError.cpp (the original client had its own copy) is listed here.
file(GLOB X2_CLIENT_X2LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/X2Lib/*.cpp)
add_library(x2_client_x2lib STATIC ${X2_CLIENT_X2LIB_SOURCES} ${PROJECT_SOURCE_DIR}/src/server/Common/NetError.cpp)
add_library(x2::client_x2lib ALIAS x2_client_x2lib)
target_include_directories(x2_client_x2lib PUBLIC ${PROJECT_SOURCE_DIR}/src/server/X2Lib)
target_link_libraries(x2_client_x2lib PUBLIC x2::client_ktdxlib x2::client_serverprotocol)
# IDC_CURSOR1 of x2.exe's resources (CX2Cursor).
x2_embed_file(x2_client_x2lib ${CMAKE_CURRENT_SOURCE_DIR}/X2/DefaultCursor.cur X2DefaultCursorFile)

# The executable (x2.exe in the original): X2.cpp sets up DXUT's callbacks and runs CKTDXApp/CX2Main
# (docs/porting/client-x2.md).
add_executable(x2_client X2/X2.cpp)
target_include_directories(x2_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/X2/Common)
target_link_libraries(x2_client PRIVATE x2::client_x2lib)
set_target_properties(x2_client PROPERTIES OUTPUT_NAME x2 WIN32_EXECUTABLE TRUE)  # SDL_main.h provides WinMain
x2_windows_icon(x2_client ${CMAKE_CURRENT_SOURCE_DIR}/X2/Elsword.ico)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
