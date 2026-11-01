# EPublisher service (docs/porte/server-epublisher.md): login and cash for the servers' DEB client.
# It uses the Common (KODBC, NetError, log, the DEB SOAP messages); nothing in the Common uses it.

add_library(x2_epublisher STATIC
    EPublisherServer.cpp
    Http.cpp
    OdbcAccountStore.cpp
    PublisherService.cpp)
target_include_directories(x2_epublisher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(x2_epublisher PUBLIC x2::server_common)

add_executable(x2_epublisher_server main.cpp)
x2_windows_icon(x2_epublisher_server ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set_target_properties(x2_epublisher_server PROPERTIES OUTPUT_NAME EPublisherServer)
target_link_libraries(x2_epublisher_server PRIVATE x2_epublisher)

x2_runtime_server(epublisher x2_epublisher_server EPublisherServer)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
