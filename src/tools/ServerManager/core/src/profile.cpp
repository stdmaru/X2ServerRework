#include "x2/servermanager/profile.h"

#include "x2/platform/json.h"

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

int Profile::Find(std::string_view name) const
{
    for (std::size_t i = 0; i < servers.size(); ++i)
        if (servers[i].name == name)
            return static_cast<int>(i);
    return -1;
}

std::string Profile::Validate() const
{
    std::set<std::string> names;
    for (const ServerSpec& s : servers) {
        if (s.name.empty())
            return "a server has no name";
        if (s.program.empty())
            return s.name + ": no program";
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
        s.directory = Resolve(baseDir, node.GetString("directory", "."));
        s.program = node.GetString("program");
        s.control = node.GetString("control");
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
