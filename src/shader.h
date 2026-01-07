#ifndef SHADER
#define SHADER

#include "fileUtils.h"
#include "glad/glad.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <regex>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

char getShaderCompileSuccess(unsigned int shader) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILE_FAIL\n" << infoLog << std::endl;
    return -1;
  }
  return 0;
}

unsigned int compileShader(const unsigned int t, const char *path) {
  unsigned int s;
  s = glCreateShader(t);
  char *ssource = loadFileToCstr(path);
  glShaderSource(s, 1, &ssource, NULL);
  glCompileShader(s);
  if (getShaderCompileSuccess(s))
    return 0;
  return s;
}

unsigned int compileProgram(std::vector<unsigned int> shaders) {
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
  void use() { glUseProgram(ID); }
  void setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  void setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
  }
};

#endif // ! SHADER
