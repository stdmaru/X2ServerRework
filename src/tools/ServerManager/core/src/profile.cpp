#include "x2/servermanager/profile.h"

#include "x2/servermanager/askpass.h"

#include "x2/platform/control.h"
#include "x2/platform/json.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <set>
#include <sstream>

namespace x2::servermanager {

namespace {

std::filesystem::path Utf8Path(std::string_view text)
{
    return std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(text.data()), text.size()));
}

std::string PathUtf8(const std::filesystem::path& path)
{
    const std::u8string text = path.lexically_normal().u8string();
    return std::string(text.begin(), text.end());
}

std::string Resolve(const std::string& baseDir, const std::string& path)
{
    if (baseDir.empty())
        return path;
    const std::filesystem::path p = Utf8Path(path);
    return p.is_absolute() ? path : PathUtf8(Utf8Path(baseDir) / p);
}

bool StringList(const x2::platform::Json* node, std::vector<std::string>& out)
{
    if (!node)
        return true;
    if (node->GetType() != x2::platform::Json::Array)
        return false;
    for (const auto& item : node->Items()) {
        if (item.GetType() != x2::platform::Json::String)
            return false;
        out.push_back(item.Text());
    }
    return true;
}

}  // namespace

std::string ServerSpec::ControlName() const
{
    if (!control.empty())
        return control;
    const std::u8string stem = Utf8Path(program).stem().u8string();   // GameServer (.exe dropped)
    return std::string(stem.begin(), stem.end());
}

std::vector<std::string> DefaultSshCommand()
{
    std::vector<std::string> command = {"ssh", "-T"};
    // BatchMode makes ssh fail instead of asking; with the window's askpass (AskpassBroker) it asks there
    const char* askpass = std::getenv(kAskpassSocketVar);
    if (!askpass || !*askpass)
        command.insert(command.end(), {"-o", "BatchMode=yes"});
    command.insert(command.end(), {"-o", "ConnectTimeout=10"});
#if !defined(_WIN32)
    const std::string controlPath = x2::platform::ControlSocketDir() + "/ssh-%C";
    command.insert(command.end(), {"-o", "ControlMaster=auto", "-o", "ControlPath=" + controlPath, "-o", "ControlPersist=60"});
#endif
    return command;
}

const HostSpec* Profile::FindHost(std::string_view name) const
{
    for (const HostSpec& h : hosts)
        if (h.name == name)
            return &h;
    return nullptr;
}

namespace {

std::string Expand(const std::string& command, const ServerSpec& server)
{
    const std::pair<std::string_view, std::string> values[] = {
        {"{service}", server.service.empty() ? server.name : server.service},
        {"{name}", server.name},
        {"{control}", server.ControlName()},
    };
    std::string out;
    for (std::size_t i = 0; i < command.size();) {
        bool replaced = false;
        for (const auto& [key, value] : values)
            if (command.compare(i, key.size(), key) == 0) {
                out += value;
                i += key.size();
                replaced = true;
                break;
            }
        if (!replaced)
            out += command[i++];
    }
    return out;
}

}  // namespace

std::string Profile::StartCommand(const ServerSpec& server) const
{
    const HostSpec* h = server.host.empty() ? nullptr : FindHost(server.host);
    if (!h)
        return {};
    return Expand(!server.start.empty() ? server.start : h->start, server);
}

std::string Profile::StopCommand(const ServerSpec& server) const
{
    const HostSpec* h = server.host.empty() ? nullptr : FindHost(server.host);
    if (!h)
        return {};
    return Expand(!server.stop.empty() ? server.stop : h->stop, server);
}

x2::platform::ProcessOptions Profile::Shell(std::string_view host, const std::string& commandLine) const
{
    x2::platform::ProcessOptions options;
    const HostSpec* h = FindHost(host);
    if (!h)
        return options;
    std::vector<std::string> command = h->ssh.empty() ? DefaultSshCommand() : h->ssh;
    options.program = command.front();
    options.args.assign(command.begin() + 1, command.end());
    if (h->port != 0) {
        options.args.push_back("-p");
        options.args.push_back(std::to_string(h->port));
    }
    if (!h->identityFile.empty()) {
        options.args.insert(options.args.end(), {"-i", h->identityFile, "-o", "IdentitiesOnly=yes"});
    }
    options.args.push_back(h->destination);
    options.args.push_back(commandLine);
    return options;
}

x2::platform::ProcessOptions Profile::X2ctl(std::string_view host, const std::vector<std::string>& args) const
{
    x2::platform::ProcessOptions options;
    const HostSpec* h = host.empty() ? nullptr : FindHost(host);
    const std::string& dir = h ? h->controlDir : controlDir;
    std::vector<std::string> x2ctlArgs;
    if (!dir.empty())
        x2ctlArgs.push_back("--dir=" + dir);
    x2ctlArgs.insert(x2ctlArgs.end(), args.begin(), args.end());
    if (!h) {
        options.program = x2ctl;
        options.args = std::move(x2ctlArgs);
        return options;
    }
    // ssh joins its arguments into one line for the remote shell: quote each word once, here. cmd reads the program
    // name before the carets, so it only gets the C runtime's quotes.
    const bool cmd = h->shell == HostSpec::Shell::Cmd;
    std::string remote = cmd ? x2::platform::QuoteWindowsArgument(h->x2ctl) : x2::platform::QuoteShellArgument(h->x2ctl);
    for (const std::string& arg : x2ctlArgs)
        remote += ' ' + (cmd ? x2::platform::QuoteCmdArgument(arg) : x2::platform::QuoteShellArgument(arg));
    return Shell(host, remote);
}

int Profile::Find(std::string_view name) const
{
    for (std::size_t i = 0; i < servers.size(); ++i)
        if (servers[i].name == name)
            return static_cast<int>(i);
    return -1;
}

std::string Profile::Validate() const
{
    std::set<std::string> hostNames;
    for (const HostSpec& h : hosts) {
        if (h.name.empty())
            return "a host has no name";
        if (!hostNames.insert(h.name).second)
            return "two hosts are called " + h.name;
        if (h.destination.empty() || h.destination.front() == '-')
            return "host " + h.name + ": no destination";
        if (h.x2ctl.empty())
            return "host " + h.name + ": no x2ctl";
        if (h.port < 0 || h.port > 65535)
            return "host " + h.name + ": port " + std::to_string(h.port) + " is not a TCP port";
    }
    std::set<std::string> names;
    for (const ServerSpec& s : servers) {
        if (s.name.empty())
            return "a server has no name";
        if (!s.host.empty() && !FindHost(s.host))
            return s.name + ": unknown host " + s.host;
        if (s.host.empty() && s.program.empty())
            return s.name + ": no program";
        if (s.host.empty() && (!s.start.empty() || !s.stop.empty() || !s.service.empty()))
            return s.name + ": \"start\", \"stop\" and \"service\" are for remote servers";
        if (!s.host.empty() && s.program.empty() && s.control.empty())
            return s.name + ": a remote server needs \"control\" or \"program\" for its socket name";
        if (!names.insert(s.name).second)
            return "two servers are called " + s.name;
    }
    for (const ServerSpec& s : servers)
        for (const std::string& dep : s.after) {
            if (dep == s.name)
                return s.name + " depends on itself";
            if (Find(dep) < 0)
                return s.name + " depends on " + dep + ", which is not in the profile";
        }
    // cycles: depth-first search with colors
    std::vector<int> color(servers.size(), 0);
    std::string cycle;
    std::function<bool(int)> visit = [&](int i) {
        color[static_cast<std::size_t>(i)] = 1;
        for (const std::string& dep : servers[static_cast<std::size_t>(i)].after) {
            const int d = Find(dep);
            if (color[static_cast<std::size_t>(d)] == 1) {
                cycle = servers[static_cast<std::size_t>(i)].name + " and " + dep + " depend on each other";
                return false;
            }
            if (color[static_cast<std::size_t>(d)] == 0 && !visit(d))
                return false;
        }
        color[static_cast<std::size_t>(i)] = 2;
        return true;
    };
    for (std::size_t i = 0; i < servers.size(); ++i)
        if (color[i] == 0 && !visit(static_cast<int>(i)))
            return "dependency cycle: " + cycle;
    return {};
}

std::vector<int> Profile::StartOrder() const
{
    std::vector<int> order;
    std::vector<bool> placed(servers.size(), false);
    while (order.size() < servers.size()) {
        bool progress = false;
        for (std::size_t i = 0; i < servers.size(); ++i) {
            if (placed[i])
                continue;
            bool ready = true;
            for (const std::string& dep : servers[i].after)
                ready = ready && placed[static_cast<std::size_t>(Find(dep))];
            if (ready) {
                placed[i] = true;
                order.push_back(static_cast<int>(i));
                progress = true;
                break;      // restart from the top: keeps the profile order among independent servers
            }
        }
        if (!progress)
            break;          // invalid profile
    }
    return order;
}

std::optional<Profile> ParseProfile(std::string_view json, const std::string& baseDir, std::string& error)
{
    using x2::platform::Json;
    const auto doc = x2::platform::ParseJson(json);
    if (!doc || doc->GetType() != Json::Object) {
        error = "not a JSON object";
        return std::nullopt;
    }
    Profile profile;
    const std::string x2ctl = doc->GetString("x2ctl", "x2ctl");
    profile.x2ctl = Utf8Path(x2ctl).has_parent_path() ? Resolve(baseDir, x2ctl) : x2ctl;
    if (const std::string dir = doc->GetString("controlDir"); !dir.empty())
        profile.controlDir = Resolve(baseDir, dir);

    if (const Json* hosts = doc->Find("hosts")) {
        if (hosts->GetType() != Json::Array) {
            error = "\"hosts\" is not an array";
            return std::nullopt;
        }
        for (const Json& node : hosts->Items()) {
            if (node.GetType() != Json::Object) {
                error = "a host is not an object";
                return std::nullopt;
            }
            HostSpec h;
            h.name = node.GetString("name");
            h.destination = node.GetString("destination");
            h.x2ctl = node.GetString("x2ctl", "x2ctl");
            h.controlDir = node.GetString("controlDir");   // the other machine's path: not resolved here
            h.start = node.GetString("start");
            h.stop = node.GetString("stop");
            h.identityFile = node.GetString("identityFile");   // this machine's file, as written (ssh expands ~)
            if (const std::string shell = node.GetString("shell", "sh"); shell == "cmd") {
                h.shell = HostSpec::Shell::Cmd;
            } else if (shell != "sh") {
                error = "host " + h.name + ": \"shell\" is \"sh\" or \"cmd\"";
                return std::nullopt;
            }
            if (const x2::platform::Json* port = node.Find("port")) {
                const auto value = port->AsInteger();
                if (!value || *value <= 0 || *value > 65535) {
                    error = "host " + h.name + ": \"port\" is a number from 1 to 65535";
                    return std::nullopt;
                }
                h.port = static_cast<int>(*value);
            }
            if (!StringList(node.Find("ssh"), h.ssh)) {
                error = "host " + h.name + ": \"ssh\" is a list of strings";
                return std::nullopt;
            }
            profile.hosts.push_back(std::move(h));
        }
    }

    const Json* servers = doc->Find("servers");
    if (!servers || servers->GetType() != Json::Array) {
        error = "no \"servers\" array";
        return std::nullopt;
    }
    for (const Json& node : servers->Items()) {
        if (node.GetType() != Json::Object) {
            error = "a server is not an object";
            return std::nullopt;
        }
        ServerSpec s;
        s.name = node.GetString("name");
        s.host = node.GetString("host");
        // a remote server's directory is not used, and is not this machine's
        s.directory = s.host.empty() ? Resolve(baseDir, node.GetString("directory", ".")) : node.GetString("directory");
        s.program = node.GetString("program");
        s.control = node.GetString("control");
        s.service = node.GetString("service");
        s.start = node.GetString("start");
        s.stop = node.GetString("stop");
        if (!StringList(node.Find("args"), s.args) || !StringList(node.Find("after"), s.after)) {
            error = s.name + ": \"args\" and \"after\" are lists of strings";
            return std::nullopt;
        }
        profile.servers.push_back(std::move(s));
    }
    error = profile.Validate();
    if (!error.empty())
        return std::nullopt;
    return profile;
}

std::optional<Profile> LoadProfile(const std::string& path, std::string& error)
{
    std::ifstream file(Utf8Path(path), std::ios::binary);
    if (!file) {
        error = path + ": cannot be read";
        return std::nullopt;
    }
    std::ostringstream text;
    text << file.rdbuf();
    const std::string base = PathUtf8(std::filesystem::absolute(Utf8Path(path)).parent_path());
    auto profile = ParseProfile(text.str(), base, error);
    if (!profile)
        error = path + ": " + error;
    return profile;
}

namespace {

// A path under `baseDir` as relative to it (./x is written x); others as they are.
std::string Relative(const std::string& baseDir, const std::string& path)
{
    if (baseDir.empty() || path.empty())
        return path;
    const std::filesystem::path p = Utf8Path(path);
    if (!p.is_absolute())
        return path;
    const std::filesystem::path relative = p.lexically_normal().lexically_relative(Utf8Path(baseDir).lexically_normal());
    if (relative.empty() || *relative.begin() == "..")
        return path;
    return PathUtf8(relative);
}

class JsonWriter {
public:
    std::string Text() && { return std::move(m_out) + "\n"; }

    void Open(char bracket) { m_out += bracket; m_first.push_back(true); }
    void Close(char bracket)
    {
        const bool empty = m_first.back();
        m_first.pop_back();
        if (!empty)
            NewLine();
        m_out += bracket;
    }
    void Key(std::string_view key)
    {
        Item();
        x2::platform::AppendJsonString(m_out, key);
        m_out += ": ";
    }
    void Item()
    {
        if (!m_first.back())
            m_out += ',';
        m_first.back() = false;
        NewLine();
    }
    void String(std::string_view key, const std::string& value, std::string_view fallback = {})
    {
        if (value == fallback)
            return;
        Key(key);
        x2::platform::AppendJsonString(m_out, value);
    }
    // a list on one line
    void List(std::string_view key, const std::vector<std::string>& values)
    {
        if (values.empty())
            return;
        Key(key);
        m_out += '[';
        for (std::size_t i = 0; i < values.size(); ++i) {
            if (i)
                m_out += ", ";
            x2::platform::AppendJsonString(m_out, values[i]);
        }
        m_out += ']';
    }
    void Integer(std::string_view key, long long value)
    {
        Key(key);
        m_out += std::to_string(value);
    }

private:
    void NewLine()
    {
        m_out += '\n';
        m_out.append(m_first.size() * 2, ' ');
    }

    std::string m_out;
    std::vector<bool> m_first;
};

}  // namespace

std::string ProfileToJson(const Profile& profile, const std::string& baseDir)
{
    JsonWriter w;
    w.Open('{');
    w.String("x2ctl", Utf8Path(profile.x2ctl).has_parent_path() ? Relative(baseDir, profile.x2ctl) : profile.x2ctl, "x2ctl");
    w.String("controlDir", Relative(baseDir, profile.controlDir));
    if (!profile.hosts.empty()) {
        w.Key("hosts");
        w.Open('[');
        for (const HostSpec& h : profile.hosts) {
            w.Item();
            w.Open('{');
            w.String("name", h.name);
            w.String("destination", h.destination);
            if (h.port != 0)
                w.Integer("port", h.port);
            w.String("shell", h.shell == HostSpec::Shell::Cmd ? "cmd" : "sh", "sh");
            w.String("x2ctl", h.x2ctl, "x2ctl");
            w.String("controlDir", h.controlDir);
            w.String("identityFile", h.identityFile);
            w.List("ssh", h.ssh);
            w.String("start", h.start);
            w.String("stop", h.stop);
            w.Close('}');
        }
        w.Close(']');
    }
    w.Key("servers");
    w.Open('[');
    for (const ServerSpec& s : profile.servers) {
        w.Item();
        w.Open('{');
        w.String("name", s.name);
        w.String("host", s.host);
        // a local server's directory was resolved when read; "." is the default
        const std::string directory = s.host.empty() ? Relative(baseDir, s.directory) : s.directory;
        w.String("directory", directory == "." && s.host.empty() ? std::string() : directory);
        w.String("program", s.program);
        w.List("args", s.args);
        w.List("after", s.after);
        w.String("control", s.control);
        w.String("service", s.service);
        w.String("start", s.start);
        w.String("stop", s.stop);
        w.Close('}');
    }
    w.Close(']');
    w.Close('}');
    return std::move(w).Text();
}

bool SaveProfile(const Profile& profile, const std::string& path, std::string& error)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    const fs::path file = fs::absolute(Utf8Path(path), ec);
    const fs::path temp = fs::path(file).concat(".tmp");
    {
        std::ofstream out(temp, std::ios::binary | std::ios::trunc);
        out << ProfileToJson(profile, PathUtf8(file.parent_path()));
        out.close();
        if (!out) {
            error = path + ": cannot be written";
            fs::remove(temp, ec);
            return false;
        }
    }
    fs::rename(temp, file, ec);     // replaces the old file, also on Windows
    if (ec) {
        error = path + ": " + ec.message();
        fs::remove(temp, ec);
        return false;
    }
    return true;
}

Profile DefaultProfile(const std::string& distDir, const std::string& config, const std::string& group)
{
    Profile profile;
    profile.x2ctl = Resolve(distDir, "x2ctl");
    auto add = [&](const char* name, bool hasGroup, std::vector<std::string> after) {
        ServerSpec s;
        s.name = name;
        s.directory = Resolve(distDir, name);
        s.program = std::string("./") + name;
        s.args = {config};
        if (hasGroup)
            s.args.push_back(group);
        s.after = std::move(after);
        profile.servers.push_back(std::move(s));
    };
    add("EPublisherServer", false, {});
    add("LoginServer", true, {"EPublisherServer"});
    add("CenterServer", true, {"EPublisherServer"});
    add("GlobalServer", true, {"EPublisherServer"});
    add("GameServer", true, {"LoginServer", "CenterServer", "GlobalServer"});
    add("ChannelServer", true, {"GameServer"});
    return profile;
}

}  // namespace x2::servermanager
