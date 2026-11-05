#include "x2/servermanager/log_store.h"

#include <algorithm>

namespace x2::servermanager {

namespace {

char Lower(char c)
{
    return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c;
}

// `needle` must already be lowercase.
bool Contains(const std::string& haystack, const std::string& needle)
{
    if (needle.empty())
        return true;
    return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                       [](char a, char b) { return Lower(a) == b; }) != haystack.end();
}

std::string ToLower(std::string text)
{
    for (char& c : text)
        c = Lower(c);
    return text;
}

}  // namespace

bool LogFilter::Matches(const LogEntry& entry) const
{
    if (!(levels & LevelBit(entry.log.level)) || !(kinds & KindBit(entry.kind)))
        return false;
    const LogMessage& m = entry.log;
    if (!func.empty() && !Contains(m.func, ToLower(func)))
        return false;
    if (search.empty())
        return true;
    const std::string needle = ToLower(search);
    if (Contains(m.message, needle) || Contains(m.func, needle) || Contains(m.file, needle))
        return true;
    for (const LogField& f : m.fields)
        if (Contains(f.name, needle) || Contains(f.value, needle))
            return true;
    for (const std::string& t : m.text)
        if (Contains(t, needle))
            return true;
    return false;
}

LogStore::LogStore(std::size_t capacity) : m_capacity(capacity > 0 ? capacity : 1) {}

std::uint64_t LogStore::Add(LogEntry entry)
{
    entry.seq = m_nextSeq++;
    ++m_counts[static_cast<std::size_t>(entry.log.level)];
    m_entries.push_back(std::move(entry));
    while (m_entries.size() > m_capacity)
        m_entries.pop_front();
    return m_entries.back().seq;
}

void LogStore::Clear()
{
    m_entries.clear();
    ++m_generation;
}

const LogEntry* LogStore::Find(std::uint64_t seq) const
{
    if (seq < FirstSeq() || seq > LastSeq())
        return nullptr;
    return &m_entries[static_cast<std::size_t>(seq - FirstSeq())];
}

bool LogView::Refresh(const LogStore& store, const LogFilter& filter)
{
    bool changed = false;
    if (!m_valid || !(filter == m_filter) || store.Generation() != m_generation) {
        m_seqs.clear();
        m_filter = filter;
        m_generation = store.Generation();
        m_scanned = store.FirstSeq() - 1;
        m_valid = true;
        changed = true;
    }

    // drop what the store no longer has
    const std::uint64_t first = store.FirstSeq();
    const auto kept = std::lower_bound(m_seqs.begin(), m_seqs.end(), first);
    if (kept != m_seqs.begin()) {
        m_seqs.erase(m_seqs.begin(), kept);
        changed = true;
    }

    for (std::uint64_t seq = std::max(m_scanned + 1, first); seq <= store.LastSeq(); ++seq) {
        if (m_filter.Matches(*store.Find(seq))) {
            m_seqs.push_back(seq);
            changed = true;
        }
    }
    m_scanned = store.LastSeq();
    return changed;
}

}  // namespace x2::servermanager
