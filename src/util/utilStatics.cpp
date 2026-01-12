#include "util/logger.hpp"

char Logger::s_logs[LOG_LENGTH][MAX_MSG_LEN] = {};
int Logger::s_currentCount = 0;
bool Logger::s_logConsole = true;
LogLevel Logger::s_logLevel = LogLevel::DEBUG;
