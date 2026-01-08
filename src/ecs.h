#ifndef ECS_H
#define ECS_H

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include <cstdint>
#include <glm/common.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <unordered_map>

using Entity = uint32_t;

class EntityRenderData {
public:
  EntityRenderData(Shader &s) : shader(s) {}

  Shader &shader;
  unsigned int vaoID = 0;
  unsigned int indiceCount = 0;
  GLint drawType = GL_TRIANGLES;
  std::vector<Texture *> textures = {};
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
  Entity nextEntity = 0;

  Entity createEntity() { return nextEntity++; }
  void destroyEntity(Entity e) {
    frames.erase(e);
    renderData.erase(e);
  }

  bool hasFrame(Entity e) const { return frames.contains(e); }
  const CFrame &getFrame(Entity e) { return frames.at(e); }
  void addFrame(Entity e, const CFrame &frame) { frames.emplace(e, frame); }

  bool hasRenderData(Entity e) const { return renderData.contains(e); }
  const EntityRenderData &getRenderData(Entity e) { return renderData.at(e); }
  void addRenderData(Entity e, const EntityRenderData &r) {
    renderData.emplace(e, r);
  }
  const std::unordered_map<Entity, EntityRenderData> &renderDataMap() {
    return renderData;
  }

private:
  std::unordered_map<Entity, EntityRenderData> renderData = {};
  std::unordered_map<Entity, CFrame> frames = {};
};

#endif
