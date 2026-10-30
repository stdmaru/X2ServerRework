# Common code of the servers (docs/porte/server-common.md). Socket/ has the TCP (decision T20) and UDP layers,
# ODBC/ the database (decision T17). ../X2Lib/X2UDPPacket.cpp is compiled through Socket/X2Packet.cpp.

file(GLOB_RECURSE X2_SERVER_COMMON_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER X2_SERVER_COMMON_SOURCES EXCLUDE REGEX "/tests/")
# DEB billing (cash): it includes the GameServer's BillManager.h, so the GameServer compiles it (src/server/GameServer)
list(FILTER X2_SERVER_COMMON_SOURCES EXCLUDE REGEX "/OnlyGlobal/AuthAndBilling/DEBBilling(Manager)?\\.cpp$")

add_library(x2_server_common STATIC ${X2_SERVER_COMMON_SOURCES})

# gSOAP 2.7.18 runtime and the code it generated for the DEB auth service: third-party/generated code
# (register, VLAs, unused parameters...), built without warnings like the dependencies in _deps.
# It goes away with the DEB auth server (docs/porte/server-common.md).
set_source_files_properties(
    OnlyGlobal/AuthAndBilling/DEB/stdsoap2.cpp
    OnlyGlobal/AuthAndBilling/DEB/soapC.cpp
    OnlyGlobal/AuthAndBilling/DEB/soapClient.cpp
    PROPERTIES COMPILE_OPTIONS $<IF:$<CXX_COMPILER_ID:MSVC>,/w,-w>)
add_library(x2::server_common ALIAS x2_server_common)

target_include_directories(x2_server_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# The build's feature flags (ServerDefine.h, with ServerDefine_US.h) in every file, as a precompiled header would:
# headers test them with #ifdef (EXTEND_SERVER_GROUP_MASK in Enum.h...), and without the definitions a file that
# included them first silently compiled the other branch of the same inline function.
target_compile_options(x2_server_common PUBLIC
    "$<IF:$<BOOL:${MSVC}>,/FI${CMAKE_CURRENT_SOURCE_DIR}/ServerDefine.h,SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/ServerDefine.h>")
target_link_libraries(x2_server_common PUBLIC x2::kncsdk x2::kncsdk_lua x2::platform x2::odbc ZLIB::ZLIB)  # Socket/KTDNUDP.h includes zlib.h

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
