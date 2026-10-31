# Channel server (docs/porte/server-channel.md). Common/ is the x2_server_common library.

file(GLOB_RECURSE X2_CHANNEL_SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(x2_channel_server ${X2_CHANNEL_SERVER_SOURCES})
x2_windows_icon(x2_channel_server ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set_target_properties(x2_channel_server PROPERTIES OUTPUT_NAME ChannelServer)
target_include_directories(x2_channel_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(x2_channel_server PRIVATE x2::server_common)
x2_runtime_server(channel x2_channel_server ChannelServer)
