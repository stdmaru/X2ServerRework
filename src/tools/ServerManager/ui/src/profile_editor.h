#pragma once

// The window's "Profile" tab (docs/tools/servermanager.md#profile-tab): a copy of the profile edited field by field
// (general settings, hosts, servers), checked with Profile::Validate as it changes, and saved as JSON. UI thread only,
// except the file picker's answer, which may arrive on any thread.

#include "x2/servermanager/profile.h"

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace x2::servermanager {

class ProfileEditor {
public:
    enum class PickKind { File, Folder };
    // Shows a file or folder dialog starting at `start`; calls `done` with the chosen path, or an empty one when
    // cancelled, possibly from another thread.
    using Picker = std::function<void(PickKind kind, const std::string& start, std::function<void(std::string)> done)>;
    enum class Action { None, Save, Apply };

    void SetPicker(Picker picker) { m_picker = std::move(picker); }
    // Edits a copy of `profile`, saved to `file`.
    void Open(const Profile& profile, const std::string& file);
    // The tab's content. Apply is offered only when `canApply` (nothing started here is running); `applyBlocked` says why
    // it is not.
    Action Draw(bool canApply, const char* applyBlocked);
    // Takes the picker's answers. True when the draft changed.
    bool Poll();

    const Profile& Draft() const { return m_draft; }
    Profile& Draft() { return m_draft; }
    const std::string& File() const { return m_file; }
    void SetFile(std::string file) { m_file = std::move(file); }
    // The draft differs from what was opened or last saved.
    bool Dirty() const;
    void MarkSaved();
    void SetMessage(std::string message, bool error) { m_message = std::move(message); m_messageIsError = error; }

private:
    enum class Field { ProfileX2ctl, ProfileControlDir, HostIdentity, ServerDirectory, ServerProgram };
    struct Pick {
        Field field;
        int index;
        std::string path;
    };

    void DrawGeneral();
    void DrawHosts();
    void DrawServers();
    void DrawHost(HostSpec& host);
    void DrawServer(int index, ServerSpec& server);
    // A text field with a "..." button that opens the picker for it.
    bool PathField(const char* id, std::string& value, PickKind kind, Field field, int index, const char* hint = "");
    void RequestPick(PickKind kind, const std::string& start, Field field, int index);
    std::string BaseDir() const;

    Profile m_draft;
    std::string m_file;
    std::string m_saved;            // ProfileToJson of what was opened or last saved
    int m_host = 0;                 // selected host and server
    int m_server = 0;
    std::string m_message;
    bool m_messageIsError = false;
    Picker m_picker;
    // answered, not yet applied; shared with the pending answers, which may outlive the editor
    struct Picks {
        std::mutex mutex;
        std::vector<Pick> items;
    };
    std::shared_ptr<Picks> m_picks = std::make_shared<Picks>();
};

}  // namespace x2::servermanager
