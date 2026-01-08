#include "engine/core/Logger.h"

namespace {
std::vector<LogEntry> g_entries;
constexpr size_t kMaxEntries = 200;

void Push(LogLevel level, const std::string& text) {
    if (g_entries.size() >= kMaxEntries) {
        g_entries.erase(g_entries.begin(), g_entries.begin() + 1);
    }
    g_entries.push_back({level, text});
}
} // namespace

void Logger::Info(const std::string& text) { Push(LogLevel::Info, text); }

void Logger::Warn(const std::string& text) { Push(LogLevel::Warn, text); }

void Logger::Error(const std::string& text) { Push(LogLevel::Error, text); }

const std::vector<LogEntry>& Logger::Entries() { return g_entries; }

void Logger::Clear() { g_entries.clear(); }
