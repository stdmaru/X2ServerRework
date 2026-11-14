# Tools (decision T32: src/tools/<Tool>). GUI tools (T29) use the SDL3 + ImGui foundation (src/app).
add_subdirectory(AssetTool)
add_subdirectory(ServerManager)
# The UI editor links the client's KTDXLIB and reads the client_data's loose files (dev builds only, T46).
if(X2_BUILD_CLIENT AND X2_DEV_TOOLS AND TARGET x2::app_imgui)
    add_subdirectory(UIEditor)
endif()
