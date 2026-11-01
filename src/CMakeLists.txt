# Modules are added here as they are ported (docs/project/roadmap.md).
add_subdirectory(platform)
add_subdirectory(kncsdk)
if(X2_BUILD_SERVER)
    add_subdirectory(server)
endif()
