// Cross-check against tools/kom/kom.py, the format's reference until a C++ CLI replaces it (decision T35).
#include "helpers.h"

#include "x2/kom/archive.h"
#include "x2/kom/writer.h"

#include <cstdlib>
#include <sstream>

#if defined(X2_KOM_PYTHON)

using namespace x2::kom;
using namespace kom_test;

namespace {

struct Spec
{
    const char* name;
    std::uint32_t algorithm;
    std::size_t size;
};

// Keep in sync with test_entries() in kompy_check.py.
const Spec kSpecs[] = {{"Alpha.lua", kXorCrc, 37}, {"beta_TEXTURE.dds", kZlib, 1037}, {"Gamma.lua", kXorCrc3, 2037},
                       {"empty.txt", kZlib, 0}};

std::string Quote(const std::filesystem::path& p)
{
    return "\"" + p.string() + "\"";
}

bool RunKomPy(const std::string& args)
{
    const std::string command = std::string("\"") + X2_KOM_PYTHON + "\" -I " + Quote(X2_KOM_PY_CHECK) + " " +
                                Quote(X2_KOM_PY_DIR) + " " + args;
#if defined(_WIN32)
    return std::system(("\"" + command + "\"").c_str()) == 0;  // cmd.exe strips the outer quotes
#else
    return std::system(command.c_str()) == 0;
#endif
}

std::string Hex(const Bytes& data)
{
    static const char digits[] = "0123456789abcdef";
    std::string out;
    for (const std::uint8_t b : data)
    {
        out += digits[b >> 4];
        out += digits[b & 15];
    }
    return out;
}

void BuildSpecs(const std::filesystem::path& path)
{
    ArchiveBuilder b;
    std::string error;
    unsigned i = 0;
    for (const Spec& s : kSpecs)
        REQUIRE(b.Add(s.name, s.algorithm, Pattern(s.size, i++), error));
    REQUIRE_MESSAGE(b.Write(path, error), error);
}

} // namespace

TEST_CASE("kom: kom.py reads what x2::kom writes")
{
    TempDir dir;
    BuildSpecs(dir / "ours.kom");
    REQUIRE(RunKomPy("read " + Quote(dir / "ours.kom") + " " + Quote(dir / "ours.txt")));
    const Bytes text = ReadFile(dir / "ours.txt");
    std::istringstream lines(std::string(text.begin(), text.end()));
    std::string line;
    unsigned i = 0;
    for (const Spec& s : kSpecs)
    {
        REQUIRE(std::getline(lines, line));
        CHECK(line == std::string(s.name) + "\t" + std::to_string(s.algorithm) + "\t" + std::to_string(s.size) + "\t" +
                          Hex(Pattern(s.size, i++)));
    }
}

TEST_CASE("kom: x2::kom reads what kom.py writes, and writes the same bytes")
{
    TempDir dir;
    REQUIRE(RunKomPy("write " + Quote(dir / "theirs.kom")));
    std::string error;
    const auto a = Archive::Open(dir / "theirs.kom", error);
    REQUIRE_MESSAGE(a, error);
    REQUIRE(a->Entries().size() == std::size(kSpecs));
    unsigned i = 0;
    for (const Spec& s : kSpecs)
    {
        const Entry& e = a->Entries()[i];
        CHECK(e.name == s.name);
        CHECK(e.algorithm == s.algorithm);
        Bytes out;
        REQUIRE_MESSAGE(a->Read(e, out, error), error);
        CHECK(out == Pattern(s.size, i));
        ++i;
    }

    // Same zlib level, header layout and checksum-as-time: identical files.
    BuildSpecs(dir / "ours.kom");
    CHECK(ReadFile(dir / "ours.kom") == ReadFile(dir / "theirs.kom"));
}

#endif
