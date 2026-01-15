#pragma once

#include "util/logger.hpp"
#include <glad/glad.h>
#include <map>
#include <stdexcept>
#include <string>

class UniformBufferManager {
public:
  UniformBufferManager(GLsizeiptr totalSize, GLuint binding)
      : size(totalSize), binding(binding) {
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  ~UniformBufferManager() { glDeleteBuffers(1, &ubo); }

  void registerUniform(const std::string &name, GLsizeiptr dataSize,
                       GLsizeiptr alignment) {
    GLsizeiptr alignedOffset = align(nextOffset, alignment);

    if (alignedOffset + dataSize > size) {
      throw std::runtime_error("Uniform buffer overflow");
    }

    offsets.emplace(name, alignedOffset);
    sizes.emplace(name, dataSize);
    nextOffset = alignedOffset + dataSize;

    Logger::Debug("Uniform \"%s\" registered at 0x%X", name.c_str(),
                  alignedOffset);
  }

  void setData(const std::string &name, const void *data) {
    auto it = offsets.find(name);
    if (it == offsets.end()) {
      throw std::runtime_error("Uniform not registered: " + name);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, it->second, sizes.at(name), data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

private:
  static GLsizeiptr align(GLsizeiptr offset, GLsizeiptr alignment) {
    return (offset + alignment - 1) & ~(alignment - 1);
  }

private:
  GLuint ubo{};
  GLsizeiptr size{};
  GLuint binding{};

  GLsizeiptr nextOffset = 0;
  std::map<std::string, GLsizeiptr> offsets;
  std::map<std::string, GLsizeiptr> sizes;
};
