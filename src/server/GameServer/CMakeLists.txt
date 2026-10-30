# Game server (docs/porte/server-game.md). Common/ is the x2_server_common library.

file(GLOB_RECURSE X2_GAME_SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(x2_game_server ${X2_GAME_SERVER_SOURCES})
set_target_properties(x2_game_server PROPERTIES OUTPUT_NAME GameServer)
target_include_directories(x2_game_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(x2_game_server PRIVATE x2::server_common)
