#pragma once
#include "platform/rendering/shader.hpp"
#include "platform/rendering/texture.hpp"
#include <vector>

struct Renderable {
  unsigned int vao = 0;
  unsigned int indexCount = 0;
  unsigned int drawMode = 0x0004; // GL_TRIANGLES
  std::vector<Texture *> textures;
  Shader *shader = nullptr;
};
