#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

constexpr float MAX_ZOOM = 145.0f;
constexpr float MIN_ZOOM = 0.01f;

class Camera3D {
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw;
  float pitch;
  float mouseSensitivity;
  float moveSpeed;
  float zoom; // FOV in degrees
  bool constrainPitch;
  float aspectRatio;
  float nearPlane;
  float farPlane;

  Camera3D(const glm::vec3 &pos = glm::vec3(0),
           const glm::vec3 &world_up = glm::vec3(0, 1, 0),
           float yaw_angle = 0.0, float pitch_angle = 0.0,
           float sensitivity = 0.05, float speed = 1.0, float fov = 75.0,
           bool constrain_pitch = true, float aspect = 0.75,
           float near_p = 0.01, float far_p = 100.0)
      : position(pos), worldUp(world_up), yaw(yaw_angle), pitch(pitch_angle),
        mouseSensitivity(sensitivity), moveSpeed(speed), zoom(fov),
        constrainPitch(constrain_pitch), aspectRatio(aspect), nearPlane(near_p),
        farPlane(far_p) {
    updateVectors();
  }

  glm::mat4 getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
  }

  glm::mat4 getProjectionMatrix() const {
    return glm::perspective(glm::radians(zoom), aspectRatio, nearPlane,
                            farPlane);
  }

  void processMouse(float xoffset, float yoffset) {
    yaw += xoffset * mouseSensitivity;
    pitch += yoffset * mouseSensitivity;

    if (constrainPitch) {
      pitch = std::clamp(pitch, -89.0f, 89.0f);
    }

    updateVectors();
  }

  void processScroll(float yoffset) {
    zoom = std::clamp(zoom - yoffset, MIN_ZOOM, MAX_ZOOM);
  }

  void setAspectRatio(float width, float height) {
    aspectRatio = width / height;
  }

private:
  void updateVectors() {
    front = calculateFront(yaw, pitch);

    right = glm::normalize(glm::cross(front, worldUp));

    // Fallback if front and worldUp are colinear
    if (glm::length(right) < 1e-8f) {
      right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 0.0f, -1.0f)));
    }

    up = glm::normalize(glm::cross(right, front));
  }

  static glm::vec3 calculateFront(float yaw_deg, float pitch_deg) {
    float yaw_rad = glm::radians(yaw_deg);
    float pitch_rad = glm::radians(pitch_deg);

    glm::vec3 f;
    f.x = std::sin(yaw_rad) * std::cos(pitch_rad);
    f.y = std::sin(pitch_rad);
    f.z = -std::cos(yaw_rad) * std::cos(pitch_rad);

    return glm::normalize(f);
  }
};
