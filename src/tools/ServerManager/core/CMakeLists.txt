# ServerManager core (decisions T30, T32; docs/tools/servermanager.md): control protocol client, server sessions and
# logs. No GUI, so it is tested without a display.

add_library(x2_servermanager_core STATIC
    src/askpass.cpp
    src/log_store.cpp
    src/orchestrator.cpp
    src/profile.cpp
    src/protocol.cpp
    src/session.cpp
    src/transport.cpp)
add_library(x2::servermanager_core ALIAS x2_servermanager_core)
target_include_directories(x2_servermanager_core PUBLIC include)
target_link_libraries(x2_servermanager_core PUBLIC x2::platform)

if(X2_BUILD_TESTS)
    add_subdirectory(tests)
endif()
