#ifndef ECS_H
#define ECS_H

#include "shader.h"
#include <glm/common.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <unordered_map>

enum EntityType { Renderable = 0 };

class EntityRenderData {
public:
  Shader *shader = nullptr;
  unsigned int vaoID = 0;
  unsigned int indiceCount = 0;
  GLint drawType = GL_TRIANGLES;
};

class CFrame {
public:
  glm::vec3 position{0};
  glm::vec3 rotation{0};
  glm::vec3 scale{1};

  glm::mat4 getMat4() const {
    glm::mat4 mat = glm::mat4(1.0);
    mat = glm::translate(mat, position);
    mat = glm::rotate(mat, rotation.x, glm::vec3(1, 0, 0));
    mat = glm::rotate(mat, rotation.y, glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, rotation.z, glm::vec3(0, 0, 1));
    mat = glm::scale(mat, scale);
    return mat;
  }
};

class World {
public:
  unsigned int currentEntity = 0;

  std::unordered_map<unsigned int, EntityType> entityTypes = {};
  std::unordered_map<unsigned int, EntityRenderData> renderData = {};
  std::unordered_map<unsigned int, CFrame> frames = {};

  unsigned int nextEntity() { return currentEntity++; }
};

#endif
