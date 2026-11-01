# Game server (docs/porting/server-game.md). Common/ is the x2_server_common library.

file(GLOB_RECURSE X2_GAME_SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# The DEB billing of the Common (cash through the EPublisherServer, T23) includes BillManager.h from here, as in the
# original, where every server project compiled the Common files itself.
set(X2_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Common)
list(APPEND X2_GAME_SERVER_SOURCES
    ${X2_COMMON_DIR}/OnlyGlobal/AuthAndBilling/DEBBilling.cpp
    ${X2_COMMON_DIR}/OnlyGlobal/AuthAndBilling/DEBBillingManager.cpp)

add_executable(x2_game_server ${X2_GAME_SERVER_SOURCES})
x2_windows_icon(x2_game_server ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set_target_properties(x2_game_server PROPERTIES OUTPUT_NAME GameServer)
target_include_directories(x2_game_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(x2_game_server PRIVATE x2::server_common)
x2_runtime_server(game x2_game_server GameServer)
