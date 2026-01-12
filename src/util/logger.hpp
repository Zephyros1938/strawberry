#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstring>

#define LOG_LENGTH 100
#define MAX_MSG_LEN 256 // Max length of a single formatted message

enum LogLevel { ERR = 0, WARN, LOG, DEBUG };

class Logger {
public:
  static void Error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Write(LogLevel::ERR, fmt, args);
    va_end(args);
  }
  static void Warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Write(LogLevel::WARN, fmt, args);
    va_end(args);
  }
  static void Info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Write(LogLevel::LOG, fmt, args);
    va_end(args);
  }
  static void Debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Write(LogLevel::DEBUG, fmt, args);
    va_end(args);
  }

  static void SetLevel(LogLevel level) { s_logLevel = level; }

  static const char (*GetLogs()) [MAX_MSG_LEN] { return s_logs; }

  // Returns a specific log line by index
  static const char *GetLogAt(int index) {
    if (index < 0 || index >= s_currentCount)
      return nullptr;
    return s_logs[index];
  }

  static int GetLogCount() { return s_currentCount; }

  static void Clear() {
    s_currentCount = 0;
    // Optional: Zero out the memory so old logs are physically gone
    std::memset(s_logs, 0, sizeof(s_logs));
  }

private:
  static void Write(LogLevel level, const char *fmt, va_list args) {
    if (level <= s_logLevel) {
      // Determine which slot to fill
      int index =
          (s_currentCount < LOG_LENGTH) ? s_currentCount : (LOG_LENGTH - 1);

      // Format the string into our static buffer
      vsnprintf(s_logs[index], MAX_MSG_LEN, fmt, args);

      if (s_logConsole) {
        const char *labels[] = {"ERROR", "WARN", "INFO", "DEBUG"};
        printf("[%s]: %s\n", labels[level], s_logs[index]);
      }

      if (s_currentCount < LOG_LENGTH)
        s_currentCount++;
    }
  }

  static char s_logs[LOG_LENGTH][MAX_MSG_LEN]; // 2D array to store actual text
  static int s_currentCount;
  static bool s_logConsole;
  static LogLevel s_logLevel;
};
