# x2askpass: ssh's SSH_ASKPASS, which hands ssh's questions to the ServerManager window (docs/tools/servermanager.md#ssh-prompts).
# A console program: ssh runs it with the console of its own (hidden, CREATE_NO_WINDOW on Windows), so no window shows.
add_executable(x2askpass main.cpp)
target_link_libraries(x2askpass PRIVATE x2::servermanager_core)
