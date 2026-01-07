#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <fstream>

char *loadFileToCstr(const char *path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file)
    return nullptr;
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  char *buf = new char[size + 1];
  if (!file.read(buf, size)) {
    delete[] buf;
    return nullptr;
  }
  buf[size] = '\0';
  return buf;
}

#endif
