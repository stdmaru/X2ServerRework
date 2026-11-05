// Opt-in: X2_KOM_CLIENT_DATA=<the client's data folder> lists, decodes and round-trips every .kom there. Finds packer
// variants of the XML header that the hand-made tests do not cover. The client's assets are not in the repository.
#include "helpers.h"

#include "x2/kom/mount.h"
#include "x2/kom/writer.h"

#include <cstdio>
#include <cstdlib>

using namespace x2::kom;
using namespace kom_test;

TEST_CASE("kom: the client's .kom files (X2_KOM_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_KOM_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0')
    {
        MESSAGE("X2_KOM_CLIENT_DATA not set: skipped");
        return;
    }

    std::vector<std::string> warnings;
    Mount mount([&](const std::string& w) { warnings.push_back(w); });
    std::string error;
    REQUIRE_MESSAGE(mount.AddFolder(folder, error), error);
    REQUIRE_FALSE(mount.Archives().empty());

    TempDir dir;
    std::size_t entries = 0, opaque = 0;
    std::uint64_t decoded = 0;
    Bytes out;
    for (const auto& archive : mount.Archives())
    {
        CAPTURE(archive->Path().string());
        for (const Entry& e : archive->Entries())
        {
            ++entries;
            if (!IsDecodable(e.algorithm))
            {
                ++opaque;
                REQUIRE_MESSAGE(archive->ReadStored(e, out, error), error);
                continue;
            }
            REQUIRE_MESSAGE(archive->Read(e, out, error), error);
            decoded += out.size();
        }
        REQUIRE_MESSAGE(ArchiveBuilder::From(archive).Write(dir / "copy.kom", error), error);
        CHECK(ReadFile(dir / "copy.kom") == ReadFile(archive->Path()));
    }

    // One rewrite with a change, through the generated header.
    const auto& first = mount.Archives().front();
    ArchiveBuilder changed = ArchiveBuilder::From(first);
    REQUIRE(changed.Add("x2_kom_test.txt", kZlib, ToBytes("test"), error));
    REQUIRE(changed.Write(dir / "changed.kom", error));
    const auto c = Archive::Open(dir / "changed.kom", error);
    REQUIRE_MESSAGE(c, error);
    CHECK(c->Entries().size() == first->Entries().size() + 1);
    REQUIRE(c->Read(c->Entries().front(), out, error));

    MESSAGE(mount.Archives().size() << " archives, " << entries << " entries (" << opaque << " opaque), "
                                    << decoded << " bytes decoded, " << warnings.size() << " duplicate names");
    for (std::size_t i = 0; i < warnings.size() && i < 20; ++i)
        MESSAGE(warnings[i]);
}
