// game/components/camera_component.h
#pragma once
#include <glm/glm.hpp>

struct CameraComponent {
  float fov = 75.0f; // Field of view in degrees
  float aspectRatio = 0.75f;
  float nearPlane = 0.01f;
  float farPlane = 100.0f;

  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::vec3 front{0.0f, 0.0f, -1.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

  float yaw = 0.0f;
  float pitch = 0.0f;
};
