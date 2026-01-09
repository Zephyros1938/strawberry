#include "camera_system.hpp"
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

    // Update platform Camera3D
    camera->position = pos;
    camera->front = front;
    camera->up = camComp.up;
    camera->worldUp = camComp.worldUp;

    camera->zoom = camComp.fov;
    camera->aspectRatio = camComp.aspectRatio;
    camera->nearPlane = camComp.nearPlane;
    camera->farPlane = camComp.farPlane;

    break; // use first camera only
  }
}
