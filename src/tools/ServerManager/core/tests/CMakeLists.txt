add_executable(x2_servermanager_tests main.cpp test_log_store.cpp test_profile.cpp test_protocol.cpp test_session.cpp)
target_link_libraries(x2_servermanager_tests PRIVATE x2::servermanager_core doctest::doctest)
# against real server consoles: KServerConsole in this process, and x2_terminal_demo / x2ctl as children
if(TARGET x2::kncsdk_lua)
    target_sources(x2_servermanager_tests PRIVATE test_console.cpp test_orchestrator.cpp)
    target_link_libraries(x2_servermanager_tests PRIVATE x2::kncsdk_lua)
    target_compile_definitions(x2_servermanager_tests PRIVATE
        X2_TERMINAL_DEMO="$<TARGET_FILE:x2_terminal_demo>"
        X2_PROCESS_HELPER="$<TARGET_FILE:x2_process_helper>")
    add_dependencies(x2_servermanager_tests x2_terminal_demo x2_process_helper)
    if(TARGET x2ctl)
        # x2_fake_ssh plays ssh: the "remote" command runs here
        add_executable(x2_fake_ssh fake_ssh.cpp)
        target_link_libraries(x2_fake_ssh PRIVATE x2::platform)
        target_compile_definitions(x2_servermanager_tests PRIVATE
            X2_X2CTL="$<TARGET_FILE:x2ctl>"
            X2_FAKE_SSH="$<TARGET_FILE:x2_fake_ssh>")
        add_dependencies(x2_servermanager_tests x2ctl x2_fake_ssh)
    endif()
endif()
add_test(NAME servermanager COMMAND x2_servermanager_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Manual test against real servers (docs/tools/servermanager.md)
add_executable(x2_servermanager_probe probe.cpp)
target_link_libraries(x2_servermanager_probe PRIVATE x2::servermanager_core)
