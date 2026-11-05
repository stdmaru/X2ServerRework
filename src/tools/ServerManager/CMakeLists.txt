# ServerManager (decisions T29, T30, T32): orchestrates and monitors the servers, locally and over SSH.
add_subdirectory(core)
add_subdirectory(askpass)
if(TARGET x2::app_imgui)
    add_subdirectory(ui)
endif()
