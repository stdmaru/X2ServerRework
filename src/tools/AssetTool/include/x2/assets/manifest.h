// The manifest of a client_data folder (decision T35, docs/tools/client-data.md): what packs it back into the
// client's .kom archives. Keyed by entry name, never by path, so files can move inside client_data freely.
//
// client_data/.kom/manifest.tsv, UTF-8, one record per line, tab-separated:
//   # comment
//   x2-assets-manifest  2
//   archive  <name.kom>  <entry count>
//   file     <name> <archive> <index> <algorithm> <size> <stored size> <checksum> <file time> <encoding> <form>
//            <sha256>
// Checksum and file time are 8 hex digits, sha256 64 (of the file's content in client_data; for the kim form, of
// its two scripts, KimContentHash). Archives are listed in the client's load order; files in archive order, then
// by index.
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace x2::assets {

// How a file's content becomes the entry's stored blob.
enum class Encoding
{
    Zlib,    // "zlib": encoding the content with the entry's algorithm at KOG's zlib level gives the original blob
    Blob,    // "blob": it does not; the original blob is kept in .kom/blobs/<name> for as long as the content matches
    Stored,  // "stored": the content is the stored blob itself (an algorithm x2::kom does not decode)
};

std::string_view ToString(Encoding encoding);

// What the file in client_data is (docs/tools/client-data.md#lua).
enum class Form
{
    Raw,  // "raw": the entry, decoded
    Lua,  // "lua": KOG bytecode decompiled to Lua source; the bytecode is kept in .kom/originals/<name>
    Kim,  // "kim": Item.kim as two scripts, Item.lua and SetItem.lua; the original is kept in .kom/originals/<name>
};

std::string_view ToString(Form form);

struct FileRecord
{
    std::string name;
    std::string archive;
    std::uint32_t index = 0;
    std::uint32_t algorithm = 0;
    std::uint32_t size = 0;        // decoded size
    std::uint32_t storedSize = 0;
    std::uint32_t checksum = 0;    // adler32 of the original stored blob
    std::uint32_t fileTime = 0;
    Encoding encoding = Encoding::Zlib;  // of the original decoded entry
    Form form = Form::Raw;
    std::string sha256;            // lower-case hex of the content written to client_data
};

struct ArchiveRecord
{
    std::string name;
    std::uint32_t count = 0;
};

struct Manifest
{
    static constexpr int kVersion = 2;

    std::vector<ArchiveRecord> archives;
    std::vector<FileRecord> files;

    bool Read(const std::filesystem::path& path, std::string& error);
    bool Write(const std::filesystem::path& path, std::string& error) const;
};

// Where client_data keeps its metadata: <client_data>/.kom/...
inline constexpr std::string_view kMetaFolder = ".kom";
std::filesystem::path ManifestPath(const std::filesystem::path& clientData);
std::filesystem::path HeaderPath(const std::filesystem::path& clientData, std::string_view archive);
std::filesystem::path BlobPath(const std::filesystem::path& clientData, std::string_view name);
std::filesystem::path OriginalPath(const std::filesystem::path& clientData, std::string_view name);

// The files of client_data that hold a record: its name, or for the kim form <stem>.lua and Set<stem>.lua.
std::vector<std::string> ContentFiles(const FileRecord& record);
std::vector<std::string> KimScriptNames(std::string_view kimName);

// UTF-8 <-> std::filesystem::path.
std::filesystem::path PathFromUtf8(std::string_view s);
std::string PathToUtf8(const std::filesystem::path& path);

} // namespace x2::assets
