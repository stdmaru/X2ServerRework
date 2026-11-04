# Modules are added here as they are ported (docs/project/roadmap.md).
add_subdirectory(platform)
add_subdirectory(kncsdk)
if(X2_BUILD_SERVER)
    add_subdirectory(server)
endif()
if(X2_BUILD_TOOLS OR X2_BUILD_CLIENT)
    add_subdirectory(app)
endif()
