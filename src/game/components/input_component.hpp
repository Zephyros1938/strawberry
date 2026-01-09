// game/components/input_component.hpp
#pragma once
#include <glm/vec2.hpp>

struct InputComponent {
  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool jump = false;

  glm::vec2 mouseDelta{0.0f, 0.0f};
  float scroll = 0.0f;
};
