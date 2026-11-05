// Opt-in: X2_ASSETS_CLIENT_DATA=<the client's data folder> imports every .kom there with tools/assets/layout.txt (and
// readable Lua: every script decompiled, Item.kim converted) and packs the result back, comparing every archive byte
// for byte. Needs the client's decoded size plus the kept blobs in
// free space (~6 GB for _client/data) in X2_ASSETS_WORK_DIR, or the system temporary folder. The client's assets are
// not in the repository.
#include "helpers.h"

#include "x2/assets/client_data.h"
#include "x2/kom/mount.h"

#include <cstdlib>

using namespace x2::assets;
using namespace assets_test;
namespace fs = std::filesystem;

TEST_CASE("assets: the client's .kom files import and pack back unchanged (X2_ASSETS_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_ASSETS_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0')
    {
        MESSAGE("X2_ASSETS_CLIENT_DATA not set: skipped");
        return;
    }
    const char* work = std::getenv("X2_ASSETS_WORK_DIR");
    TempDir dir(work && *work ? fs::path(work) : fs::temp_directory_path());

    std::string error;
    ImportOptions options;
    options.source = PathFromUtf8(folder);
    options.destination = dir / "client_data";
#ifdef X2_ASSETS_LAYOUT
    REQUIRE_MESSAGE(options.layout.Load(X2_ASSETS_LAYOUT, error), error);
#endif
    std::vector<std::string> notes;
    options.log = [&](const std::string& line) {
        if (line.find("kept") != std::string::npos || line.find("not available") != std::string::npos)
            notes.push_back(line);
    };
    ImportStats imported;
    REQUIRE_MESSAGE(Import(options, imported, error), error);
    MESSAGE(imported.archives << " archives, " << imported.files << " files, " << imported.bytes << " bytes, "
                              << imported.keptBlobs << " original blobs kept, " << imported.stored << " stored; Lua: "
                              << imported.luaSource << " decompiled, " << imported.luaKept << " kept, "
                              << imported.kim << " .kim converted");
    for (const std::string& note : notes)
        MESSAGE(note);
    CHECK(imported.luaSource > 0);
    CHECK(imported.luaKept == 0);  // every script of the client decompiles (2026-11-05: 4,553)
    CHECK(imported.kim == 1);

    Status status;
    REQUIRE_MESSAGE(GetStatus(options.destination, 0, status, error), error);
    CHECK(status.changed.empty());
    CHECK(status.missing.empty());
    CHECK(status.added.empty());

    PackOptions pack;
    pack.clientData = options.destination;
    pack.destination = dir / "out";
    PackStats packed;
    REQUIRE_MESSAGE(Pack(pack, packed, error), error);
    CHECK(packed.archives == imported.archives);
    CHECK(packed.identicalHeaders == imported.archives);
    CHECK(packed.notReproduced == 0);

    std::vector<fs::path> archives;
    REQUIRE_MESSAGE(x2::kom::ArchivesInLoadOrder(options.source, archives, error), error);
    std::size_t identical = 0;
    for (const fs::path& original : archives)
    {
        CAPTURE(original.filename().string());
        const bool same = ReadFile(pack.destination / original.filename()) == ReadFile(original);
        CHECK(same);
        identical += same ? 1 : 0;
    }
    MESSAGE(identical << " of " << archives.size() << " archives identical");
}
