#pragma once

// What else runs on this machine, for the client's own anti-cheat (CX2Main::CheckWindowInfo): the server sends a list
// of hacking tools by process, window or kernel-object name. Windows answers all of it; elsewhere only processes can be
// listed (X11 and Wayland do not show other clients' windows portably, and there are no named kernel objects).

#include <string>
#include <vector>

namespace x2::platform {

struct ProcessEntry
{
    long long pid = 0;
    std::wstring path; // full executable path; the bare image name when the system hides the path
};

// The processes this one can see, without the current one.
std::vector<ProcessEntry> ListProcesses();

struct DesktopWindow
{
    std::wstring title;     // empty when the window has none
    std::wstring className; // the window class on Windows
    bool owned = false;     // has a parent or owner window
};

// The desktop's top-level windows, in z-order (topmost first). Empty outside Windows.
std::vector<DesktopWindow> ListTopLevelWindows();

// Whether a named event or mutex exists (OpenEvent / OpenMutex; one the process may not open counts as existing).
// Always false outside Windows.
bool NamedEventExists(const std::wstring& name);
bool NamedMutexExists(const std::wstring& name);

} // namespace x2::platform
