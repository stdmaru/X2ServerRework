# Client tests, headless (SDL's dummy video driver; no window, no GPU). docs/porting/client-dxut.md#tests
set(X2_CLIENT_TEST_SOURCES main.cpp test_messages.cpp test_ime.cpp test_dxut.cpp test_input.cpp test_ktdi.cpp
    test_serverprotocol.cpp test_massfile.cpp test_d3d9.cpp test_movie.cpp test_browser.cpp
    test_removed_integrations.cpp test_ktdxlib.cpp test_d3dx.cpp test_d3dx_texture.cpp test_d3dx_mesh.cpp test_d3dx_skin.cpp
    test_d3dx_animation.cpp
    x2_globals.cpp)
if(TARGET x2::client_luacommon)
    list(APPEND X2_CLIENT_TEST_SOURCES test_luacommon.cpp)
endif()
add_executable(x2_client_tests ${X2_CLIENT_TEST_SOURCES})
target_link_libraries(x2_client_tests PRIVATE x2::client_dxut x2::client_serverprotocol
    x2::client_massfile x2::client_movie x2::client_browser x2::client_ktdxlib x2::kom_write ZLIB::ZLIB doctest::doctest)
# test_input.cpp checks the DIK_* table against every DIK_* spelled in the client's sources.
target_compile_definitions(x2_client_tests PRIVATE X2_CLIENT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/src/client"
    X2_CLIENT_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
if(TARGET x2::client_luacommon)
    target_link_libraries(x2_client_tests PRIVATE x2::client_luacommon)
endif()
add_test(NAME client COMMAND x2_client_tests)
