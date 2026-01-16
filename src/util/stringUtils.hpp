#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace stringUtils {

std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

// Trim from start (left)
std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\n\r\f\v");
  return (start == std::string::npos) ? "" : s.substr(start);
}

// Trim from end (right)
std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(" \t\n\r\f\v");
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

// Trim from both ends
std::string trim(const std::string &s) { return rtrim(ltrim(s)); }
} // namespace stringUtils
