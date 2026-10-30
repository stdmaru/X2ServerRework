# Common code of the servers (docs/porte/server-common.md). Port in progress (server/common-build):
# Socket/ (network) and ODBC/ (database) come in their own steps.

file(GLOB_RECURSE X2_SERVER_COMMON_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER X2_SERVER_COMMON_SOURCES EXCLUDE REGEX "/(Socket|ODBC|tests)/")
# depend on GameServer/CenterServer headers: compiled with those servers
list(FILTER X2_SERVER_COMMON_SOURCES EXCLUDE REGEX "/KStatisticsLocalLog\\.cpp$")
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
target_link_libraries(x2_server_common PUBLIC x2::kncsdk x2::kncsdk_lua x2::platform PRIVATE ZLIB::ZLIB)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
