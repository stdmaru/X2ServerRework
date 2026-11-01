# Login server (docs/porting/server-login.md). Common/ is the x2_server_common library.

file(GLOB_RECURSE X2_LOGIN_SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER X2_LOGIN_SERVER_SOURCES EXCLUDE REGEX "/tests/")

add_executable(x2_login_server ${X2_LOGIN_SERVER_SOURCES})
x2_windows_icon(x2_login_server ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set_target_properties(x2_login_server PROPERTIES OUTPUT_NAME LoginServer)
target_include_directories(x2_login_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(x2_login_server PRIVATE x2::server_common)
x2_runtime_server(login x2_login_server LoginServer)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
