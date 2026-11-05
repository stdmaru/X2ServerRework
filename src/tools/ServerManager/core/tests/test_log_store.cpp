#include "x2/servermanager/log_store.h"

#include <doctest/doctest.h>

using namespace x2::servermanager;

namespace {

LogEntry Entry(EntryKind kind, LogLevel level, std::string message, std::string func = {})
{
    LogEntry e;
    e.kind = kind;
    e.log.level = level;
    e.log.message = std::move(message);
    e.log.func = std::move(func);
    return e;
}

}  // namespace

TEST_CASE("log store: sequence numbers and capacity")
{
    LogStore store(3);
    CHECK(store.LastSeq() == 0);
    CHECK(store.Find(1) == nullptr);
    for (int i = 1; i <= 5; ++i)
        CHECK(store.Add(Entry(EntryKind::Log, LogLevel::Info, std::to_string(i))) == static_cast<std::uint64_t>(i));
    CHECK(store.Size() == 3);
    CHECK(store.FirstSeq() == 3);
    CHECK(store.LastSeq() == 5);
    CHECK(store.Dropped() == 2);
    CHECK(store.Find(2) == nullptr);
    REQUIRE(store.Find(4));
    CHECK(store.Find(4)->log.message == "4");
    CHECK(store.Oldest(0).log.message == "3");
    CHECK(store.Count(LogLevel::Info) == 5);

    store.Clear();
    CHECK(store.Empty());
    CHECK(store.Find(5) == nullptr);
    CHECK(store.Add(Entry(EntryKind::Log, LogLevel::Info, "6")) == 6);
    CHECK(store.FirstSeq() == 6);
}

TEST_CASE("log filter: levels, kinds, search and function")
{
    LogEntry e = Entry(EntryKind::Log, LogLevel::Warn, "Proxy pointer not found", "KProxyManager::SendTo");
    e.log.fields.push_back({"iServerUID", "77"});
    e.log.text.push_back("서버 연결 실패");
    e.log.file = "/src/ProxyManager.cpp";

    LogFilter f;
    CHECK(f.Matches(e));
    f.levels = LevelBit(LogLevel::Error);
    CHECK_FALSE(f.Matches(e));
    f.levels = LevelBit(LogLevel::Error) | LevelBit(LogLevel::Warn);
    CHECK(f.Matches(e));
    f.kinds = KindBit(EntryKind::Command);
    CHECK_FALSE(f.Matches(e));
    f.kinds = kAllKinds;

    for (const char* hit : {"POINTER", "proxymanager::", "iserveruid", "77", "연결", "proxymanager.cpp"}) {
        f.search = hit;
        CHECK_MESSAGE(f.Matches(e), hit);
    }
    f.search = "nothing";
    CHECK_FALSE(f.Matches(e));
    f.search.clear();
    f.func = "sendto";
    CHECK(f.Matches(e));
    f.func = "Other";
    CHECK_FALSE(f.Matches(e));
}

TEST_CASE("log view: incremental, refiltered, follows drops and clears")
{
    LogStore store(4);
    LogView view;
    LogFilter errors;
    errors.levels = LevelBit(LogLevel::Error);

    store.Add(Entry(EntryKind::Log, LogLevel::Error, "e1"));
    store.Add(Entry(EntryKind::Log, LogLevel::Info, "i2"));
    CHECK(view.Refresh(store, errors));
    CHECK(view.Seqs() == std::vector<std::uint64_t>{1});
    CHECK_FALSE(view.Refresh(store, errors));   // nothing new

    store.Add(Entry(EntryKind::Log, LogLevel::Error, "e3"));
    store.Add(Entry(EntryKind::Log, LogLevel::Info, "i4"));
    CHECK(view.Refresh(store, errors));
    CHECK(view.Seqs() == std::vector<std::uint64_t>{1, 3});

    CHECK(view.Refresh(store, LogFilter{}));    // filter changed: everything
    CHECK(view.Seqs() == std::vector<std::uint64_t>{1, 2, 3, 4});

    store.Add(Entry(EntryKind::Log, LogLevel::Error, "e5"));    // drops 1
    CHECK(view.Refresh(store, LogFilter{}));
    CHECK(view.Seqs() == std::vector<std::uint64_t>{2, 3, 4, 5});

    store.Clear();
    store.Add(Entry(EntryKind::Log, LogLevel::Error, "e6"));
    CHECK(view.Refresh(store, LogFilter{}));
    CHECK(view.Seqs() == std::vector<std::uint64_t>{6});
}
