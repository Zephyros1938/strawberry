#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct LightComponent {
  vec3 color = {1.0f, 1.0f, 1.0f};
  float intensity = 1.0f;
  float range = 10.0f;
  int type = 0; // 0: Directional, 1: Point, 2: Spot
};

// Only added to entities that need it
struct SpotLightComponent {
  float innerAngle = 0.9f; // Pre-calculated cos(angle)
  float outerAngle = 0.8f;
};
