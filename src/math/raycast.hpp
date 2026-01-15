#pragma once

#include <glm/glm.hpp>

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;

  // A helper to get a point at a specific distance
  glm::vec3 at(float distance) const { return origin + (direction * distance); }
};

inline Ray getRayVP(const glm::mat4 &viewMatrix, const glm::mat4 &projMatrix,
                    const glm::vec3 &pos) {
  glm::mat4 invVP = glm::inverse(projMatrix * viewMatrix);
  glm::vec4 screenPos = glm::vec4(0.0, 0.0, -1.0f, 1.0f);
  glm::vec4 worldPosHomo = invVP * screenPos;

  // 3. Create Ray
  Ray ray;
  ray.origin = pos;
  ray.direction =
      glm::normalize(glm::vec3(worldPosHomo / worldPosHomo.w) - pos);

  return ray;
}
