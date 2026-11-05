#pragma once

// The ServerManager window (decisions T29, T30; docs/tools/servermanager.md#window), drawn with Dear ImGui into the
// whole viewport: the servers with their phase and actions, the selected server's log with filters and search, and
// its Lua prompt. Owns the orchestrator. UI thread only.

#include "profile_editor.h"

#include "x2/servermanager/askpass.h"
#include "x2/servermanager/orchestrator.h"

#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace x2::servermanager {

class ManagerUi {
public:
    using Clock = std::chrono::steady_clock;

    // `wake` is called from the transports' threads when something arrived (WakeEventLoop).
    explicit ManagerUi(std::function<void()> wake = {});
    ~ManagerUi();

    // A profile file, or a dist folder (DefaultProfile, saved to the default profile file). The servers of a previous
    // profile are left as the orchestrator's destructor leaves them. False and `error` when it cannot be read.
    bool Load(const std::string& path, std::string& error);
    // The path or folder given on the command line, else `defaultFile` (profile.json next to the program) when it
    // exists, else dist/ in the current folder.
    static std::string FindProfile(const std::vector<std::string>& args, const std::string& defaultFile = {});
    // Where a profile made here (from a dist folder, or from nothing) is saved: profile.json next to the program.
    void SetDefaultProfileFile(std::string file);
    // The file and folder dialogs of the Profile tab.
    void SetPicker(ProfileEditor::Picker picker) { m_editor.SetPicker(std::move(picker)); }
    ProfileEditor& Editor() { return m_editor; }
    // The Profile tab's Save and "Save and apply".
    bool SaveProfile(std::string& error);
    bool ApplyProfile(std::string& error);
    // A server started here is running: the profile cannot be replaced, nor the window closed without asking.
    bool StartedRunning() const;
    // ssh's password, passphrase and host key questions in a dialog; `helper` is the program ssh runs for them
    // (ServerManager itself). Without it, ssh runs with BatchMode and such a host fails.
    bool EnableAskpass(const std::string& helper, std::string& error);
    AskpassBroker& Askpass() { return m_askpass; }
    // The ssh question dialog: Asking (inputs enabled, maybe with the last error), Waiting (answered: "Connecting...",
    // inputs disabled, until ssh gets in, asks again or ends), Failed (ssh gave up: the error, inputs enabled again).
    enum class AskpassState { None, Asking, Waiting, Failed };
    AskpassState GetAskpassState() const { return m_askpassState; }
    const std::string& AskpassError() const { return m_askpassError; }
    // What the dialog's buttons do: a text (OK, Yes) or nullopt (Cancel, No). Uses the dialog's input when Secret.
    void AnswerAskpass(std::optional<std::string> answer);
    enum class Tab { Servers, Profile };
    void ShowTab(Tab tab) { m_showTab = tab; }
    const std::string& ProfilePath() const { return m_profilePath; }
    Orchestrator* GetOrchestrator() { return m_orchestrator.get(); }

    // Transport events, stop timeouts and the periodic discovery. True when something changed.
    bool Update(Clock::time_point now = Clock::now());
    // One frame of the window's content (between ImGui::NewFrame and Render).
    void Draw();
    // How long the event loop may sleep: shorter while servers start or stop.
    int IdleTimeoutMs() const;

    // Closing the window: servers ServerManager started would stop with it, so it asks first.
    void RequestQuit();
    bool ShouldQuit() const { return m_quit; }

private:
    struct ServerView {
        LogFilter filter;
        LogView view;
        // display rows: (entry seq, line within the entry), for the filter and store generation they were made for
        std::deque<std::pair<std::uint64_t, std::uint32_t>> rows;
        LogFilter rowsFilter;
        unsigned rowsGeneration = 0;
        std::uint64_t rowsLastSeq = 0;
        std::string input;
        std::vector<std::string> history;
        int historyPos = -1;                // -1: editing a new line
        bool follow = true;
        bool focusInput = false;
    };

    void DrawToolbar();
    void DrawServers();
    void DrawSelected();
    void DrawLog(int i, ServerView& view);
    void DrawPrompt(int i, ServerView& view);
    void DrawQuitDialog();
    void DrawAskpassDialog();
    void UpdateRows(const LogStore& logs, ServerView& view);

    std::function<void()> m_wake;
    ProfileEditor m_editor;
    std::string m_defaultFile;
    std::optional<Tab> m_showTab;   // selected on the next frame
    AskpassBroker m_askpass;
    void AskpassUpdate();
    void AskpassShow(const AskpassRequest& request);
    void AskpassFail(std::string error);
    AskpassState m_askpassState = AskpassState::None;
    AskpassRequest m_askpassRequest;    // the question shown (Asking), or answered last
    std::string m_askpassError;
    bool m_askpassRetry = false;        // Asking again after a wrong answer: ssh's complaint may still be on its way
    std::optional<std::string> m_askpassQueued;     // typed after a failure: answers ssh's next same question
    Clock::time_point m_askpassSince{};             // Waiting since
    bool m_askpassOpen = false;         // the popup is open
    bool m_askpassFocus = false;        // put the cursor in the input on the next frame
    std::string m_askpassInput;
    bool m_askpassRemember = false;
    std::unique_ptr<Orchestrator> m_orchestrator;
    std::vector<ServerView> m_views;
    std::string m_profilePath;
    std::string m_pathInput;
    std::string m_loadError;
    int m_selected = 0;
    Clock::time_point m_nextDiscover{};
    bool m_quitAsked = false;       // the dialog is open
    bool m_quitting = false;        // stopping the servers it started, then quitting
    bool m_quit = false;
};

}  // namespace x2::servermanager
