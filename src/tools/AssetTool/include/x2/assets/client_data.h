// client_data: the client's .kom archives as an organized folder of ordinary files plus a manifest (decision T35,
// docs/tools/client-data.md). Import extracts the archives; pack writes them back, byte for byte when nothing changed.
#pragma once

#include "x2/assets/layout.h"
#include "x2/assets/manifest.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace x2::assets {

// Progress and warnings, one line each. Called from worker threads, one at a time.
using LogSink = std::function<void(const std::string&)>;

struct ImportOptions
{
    std::filesystem::path source;       // the client's data folder (its .kom, not recursive)
    std::filesystem::path destination;  // client_data: must not exist or be empty
    Layout layout;                      // no rules: everything goes to _unsorted/<archive>
    unsigned jobs = 0;                  // 0: one per hardware thread
    bool readableLua = true;            // decompile Lua bytecode and Item.kim (docs/tools/client-data.md#lua)
    LogSink log;
};

struct ImportStats
{
    std::size_t archives = 0;
    std::size_t files = 0;
    std::size_t keptBlobs = 0;  // Encoding::Blob
    std::size_t stored = 0;     // Encoding::Stored
    std::size_t luaSource = 0;  // Form::Lua: bytecode decompiled to source
    std::size_t luaKept = 0;    // bytecode the decompiler (or LuaJIT, reading its output) could not handle
    std::size_t kim = 0;        // Form::Kim
    std::uint64_t bytes = 0;    // written to client_data (contents)
};

// Fails before writing anything if the archives cannot be read or a name is in more than one archive (the manifest
// is keyed by name). A failure halfway leaves a partial destination behind. With readableLua, Lua bytecode becomes
// source and Item.kim becomes Item.lua + SetItem.lua (with the enum names of the decompiled Enum.lua); what cannot
// be converted stays as it was, logged.
bool Import(const ImportOptions& options, ImportStats& stats, std::string& error);

// The files of client_data by name (ASCII case-insensitive), skipping .kom/. Fails if a name is in two folders.
bool IndexFiles(const std::filesystem::path& clientData, std::vector<std::filesystem::path>& files,
                std::string& error);

struct Status
{
    std::vector<std::string> changed;  // content differs from the manifest
    std::vector<std::string> missing;  // in the manifest, not in client_data
    std::vector<std::filesystem::path> added;  // in client_data, not in the manifest
};

bool GetStatus(const std::filesystem::path& clientData, unsigned jobs, Status& status, std::string& error);

struct PackOptions
{
    std::filesystem::path clientData;
    std::filesystem::path destination;  // folder for the .kom (created if needed)
    unsigned jobs = 0;
    LogSink log;
};

struct PackStats
{
    std::size_t archives = 0;
    std::size_t identicalHeaders = 0;  // archives written with their original header
    std::size_t files = 0;
    std::size_t changed = 0;           // re-encoded from edited content
    std::size_t luaSource = 0;         // of them, edited decompiled scripts, packed as Lua source
    std::size_t kimCompiled = 0;       // of them, Item.kim compiled from edited scripts
    std::size_t missing = 0;           // dropped from their archive
    std::size_t added = 0;             // not packed: no archive is chosen for new files yet
    std::size_t notReproduced = 0;     // unchanged content whose re-encoding differs (another zlib)
};

// Writes every archive of the manifest to `destination`. Unchanged content is encoded as it was imported, so an
// unchanged client_data packs back into the original archives byte for byte (with zlib 1.3.1, the version the
// project pins). Edited files are re-encoded with their entry's algorithm; missing files are dropped; files not in
// the manifest are reported and left out. Edited decompiled scripts are packed as Lua source (UTF-8 with BOM, what
// the ported client loads, T21) after LuaJIT checks their syntax; edited item scripts are compiled into Item.kim.
bool Pack(const PackOptions& options, PackStats& stats, std::string& error);

} // namespace x2::assets
