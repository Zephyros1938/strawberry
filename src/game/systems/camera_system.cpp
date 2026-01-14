#include "camera_system.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void CameraSystem::update(ComponentStore<CameraComponent> &cameras,
                          ComponentStore<Transform> &transforms) {
  if (!camera)
    return;

  // For simplicity, use the first camera entity

  for (auto &[entity, camComp] : cameras.all()) {
    glm::vec3 pos = camComp.position;

    // If entity has a Transform, apply it
    if (transforms.has(entity)) {
      auto &t = transforms.get(entity);
      pos = t.position;
    }

    // Calculate front vector from yaw/pitch
    float yawRad = glm::radians(camComp.yaw);
    float pitchRad = glm::radians(camComp.pitch);
    glm::vec3 front;
    front.x = sin(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = -cos(yawRad) * cos(pitchRad);
    front = glm::normalize(front);

    // Calculate other vectors
    glm::vec3 right = glm::normalize(glm::cross(front, camComp.worldUp));
    if (glm::length(right) < 1e-8f) {
      right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 0.0f, -1.0f)));
    }

    glm::vec3 up = glm::normalize(glm::cross(right, front));
    camComp.front = front;
    camComp.up = up;

    // Update platform Camera3D
    camera->position = pos;
    camera->front = front;
    camera->up = up;
    camera->worldUp = camComp.worldUp;

    camera->zoom = camComp.fov;
    // camera->aspectRatio = camComp.aspectRatio;
    camera->nearPlane = camComp.nearPlane;
    camera->farPlane = camComp.farPlane;
    // camera->yaw = camComp.yaw;
    // camera->pitch = camComp.pitch;

    break; // use first camera only
  }
}
