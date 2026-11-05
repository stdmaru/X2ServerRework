#pragma once

// What ServerManager shows for one server: its logs, loose output, the commands typed in its prompt with their results,
// and connection events, in arrival order, with a bounded history and filtered views for the log window.

#include "x2/servermanager/protocol.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace x2::servermanager {

enum class EntryKind {
    Log,        // `log` message
    Output,     // `output` message, or a line that is not a protocol message
    Command,    // Lua typed in ServerManager's prompt
    Result,     // its result (`output` and `error` in the message)
    Event,      // connection and status changes, noted by ServerManager
};

struct LogEntry {
    std::uint64_t seq = 0;      // assigned by LogStore, increasing from 1
    EntryKind kind = EntryKind::Log;
    LogMessage log;             // for kinds other than Log: time, level and message (the text) only
};

// Bit masks for LogFilter.
constexpr unsigned LevelBit(LogLevel level) { return 1u << static_cast<unsigned>(level); }
constexpr unsigned KindBit(EntryKind kind) { return 1u << static_cast<unsigned>(kind); }
inline constexpr unsigned kAllLevels = 0xFu;
inline constexpr unsigned kAllKinds = 0x1Fu;

struct LogFilter {
    unsigned levels = kAllLevels;
    unsigned kinds = kAllKinds;
    std::string search;     // substring of the message, function, file, fields or text; ASCII case-insensitive
    std::string func;       // substring of the function name; ASCII case-insensitive

    bool Matches(const LogEntry& entry) const;
    bool operator==(const LogFilter&) const = default;
};

class LogStore {
public:
    explicit LogStore(std::size_t capacity = 100000);

    // Appends `entry` (its seq is overwritten) and drops the oldest entries beyond the capacity.
    std::uint64_t Add(LogEntry entry);
    void Clear();

    std::size_t Size() const { return m_entries.size(); }
    std::size_t Capacity() const { return m_capacity; }
    bool Empty() const { return m_entries.empty(); }
    std::uint64_t FirstSeq() const { return m_nextSeq - m_entries.size(); }     // oldest kept
    std::uint64_t LastSeq() const { return m_nextSeq - 1; }                      // 0 when nothing was ever added
    std::uint64_t Dropped() const { return FirstSeq() - 1; }                    // dropped or cleared
    unsigned Generation() const { return m_generation; }                        // changes on Clear

    const LogEntry* Find(std::uint64_t seq) const;      // nullptr if dropped or never added
    const LogEntry& Oldest(std::size_t index) const { return m_entries[index]; }

    // Entries added per level since the start (not reduced by drops or Clear), for counters in the UI.
    std::uint64_t Count(LogLevel level) const { return m_counts[static_cast<std::size_t>(level)]; }

private:
    std::size_t m_capacity;
    std::deque<LogEntry> m_entries;
    std::uint64_t m_nextSeq = 1;
    unsigned m_generation = 0;
    std::uint64_t m_counts[4] = {};
};

// The entries of a LogStore that pass a filter, updated incrementally: only new entries are tested, unless the filter
// changed or the store was cleared.
class LogView {
public:
    // Returns true when the visible list changed.
    bool Refresh(const LogStore& store, const LogFilter& filter);
    const std::vector<std::uint64_t>& Seqs() const { return m_seqs; }

private:
    std::vector<std::uint64_t> m_seqs;
    LogFilter m_filter;
    std::uint64_t m_scanned = 0;    // last seq tested
    unsigned m_generation = 0;
    bool m_valid = false;
};

}  // namespace x2::servermanager
