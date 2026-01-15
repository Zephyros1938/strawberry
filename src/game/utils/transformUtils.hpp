#pragma once

#include "game/components/transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

inline glm::mat4 transformToMat4(Transform &transform) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, transform.position);
  model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
  model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
  model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
  model = glm::scale(model, transform.scale);
  return model;
}
