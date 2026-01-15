#pragma once

#include "engine/ecs2.hpp"
#include "game/components/camera_component.hpp"
#include "platform/rendering/camera.hpp"

class CameraSystem {
public:
  // This pointer represents your platform's actual Camera3D (e.g., Raylib,
  // OpenGL)
  Camera3D *target;

  void update(ECS2 &ecs) {
    if (!target)
      return;

    // Query for any entity with a CameraComponent
    auto cameras = ecs.query<CameraComponent>();

    for (auto e : cameras) {
      auto &cam = ecs.getComponent<CameraComponent>(e);

      // Math: Calculate orientation
      glm::vec3 front;
      front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
      front.y = sin(glm::radians(cam.pitch));
      front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
      cam.front = glm::normalize(front);

      glm::vec3 right = glm::normalize(glm::cross(cam.front, cam.worldUp));
      cam.up = glm::normalize(glm::cross(right, cam.front));

      // Sync with platform
      target->position = cam.position;
      target->front = cam.front;
      target->up = cam.up;
      target->worldUp = cam.worldUp;
      target->zoom = cam.fov;
      target->nearPlane = cam.nearPlane;
      target->farPlane = cam.farPlane;

      break; // Typically only update one main camera per frame
    }
  }
};
