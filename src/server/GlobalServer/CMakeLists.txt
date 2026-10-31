# Global server (docs/porte/server-global.md). Common/ is the x2_server_common library.

file(GLOB_RECURSE X2_GLOBAL_SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(x2_global_server ${X2_GLOBAL_SERVER_SOURCES})
x2_windows_icon(x2_global_server ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set_target_properties(x2_global_server PROPERTIES OUTPUT_NAME GlobalServer)
target_include_directories(x2_global_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(x2_global_server PRIVATE x2::server_common)
