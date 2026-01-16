#pragma once

#include "glad/glad.h"
#include "util/fileUtils.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <vector>

#include <iostream>

#include <string>

inline bool getShaderCompileSuccess(unsigned int shader) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILE_FAIL\n" << infoLog << std::endl;
    return false;
  }
  return true;
}

inline unsigned int compileShader(const unsigned int t, const char *path) {
  unsigned int s;
  s = glCreateShader(t);
  char *ssource = loadFileToCstr(path);
  glShaderSource(s, 1, &ssource, NULL);
  glCompileShader(s);
  if (!getShaderCompileSuccess(s))
    return 0;
  return s;
}

inline unsigned int compileProgram(std::vector<unsigned int> shaders) {
  unsigned int p;
  p = glCreateProgram();
  for (auto s : shaders) {
    glAttachShader(p, s);
  }
  glLinkProgram(p);

  int success;
  glGetProgramiv(p, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(p, 512, NULL, infoLog);
    std::cerr << "ERROR::PROGRAM::LINK_FAIL\n" << infoLog << std::endl;
  }

  for (auto s : shaders) {
    glDeleteShader(s);
  }

  return p;
};

class Shader {
public:
  unsigned int ID;

  Shader(const char *vertexPath, const char *fragmentPath) {
    unsigned int vShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    unsigned int fShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
    unsigned int program = compileProgram({vShader, fShader});
    ID = program;
  }
  void use() const { glUseProgram(ID); }
  void setBool(const std::string &name, bool value) {

    glUniform1i(getUniformName(name), (int)value);
  }
  void setInt(const std::string &name, int value) {
    glUniform1i(getUniformName(name), value);
  }
  void setFloat(const std::string &name, float value) {
    glUniform1f(getUniformName(name), value);
  }
  void setMat4(const std::string &name, const glm::mat4 value) {
    glUniformMatrix4fv(getUniformName(name), 1, GL_FALSE,
                       glm::value_ptr(value));
  }
  void setVec3(const std::string &name, const glm::vec3 value) {
    glUniform3f(getUniformName(name), value.x, value.y, value.z);
  }

private:
  std::unordered_map<std::string, GLint> uniformNames = {};

  GLint getUniformName(const std::string &name) {
    if (uniformNames.find(name) == uniformNames.end()) {
      uniformNames[name] = glGetUniformLocation(ID, name.c_str());
    }
    return uniformNames[name];
  }
};
