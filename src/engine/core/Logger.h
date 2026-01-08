#pragma once

#include <string>
#include <vector>

enum class LogLevel {
    Info,
    Warn,
    Error,
};

struct LogEntry {
    LogLevel level;
    std::string text;
};

class Logger {
  public:
    static void Info(const std::string& text);
    static void Warn(const std::string& text);
    static void Error(const std::string& text);

    static const std::vector<LogEntry>& Entries();
    static void Clear();
};
