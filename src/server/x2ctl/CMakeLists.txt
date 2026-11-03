# x2ctl: client of the servers' control socket (decision T30, docs/porting/kncsdk.md)
add_executable(x2ctl main.cpp)
target_link_libraries(x2ctl PRIVATE x2::platform)

# Utility copied to the root of the assembled package: dist/x2ctl
set(_x2ctl_all)
if(X2_DIST_FOLDERS)
    set(_x2ctl_all ALL)
endif()
add_custom_target(x2_run_x2ctl ${_x2ctl_all}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${X2_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:x2ctl>" "${X2_DIST_DIR}"
    DEPENDS x2ctl
    COMMENT "Copying x2ctl to ${X2_DIST_DIR}" VERBATIM)
