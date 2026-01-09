#pragma once

#include "engine/ecs.hpp"
#include "game/components/camera_component.hpp"
#include "game/components/transform.hpp"
#include "platform/rendering/camera.hpp"

class CameraSystem {
public:
  explicit CameraSystem(Camera3D *camera) : camera(camera) {}

  void update(ComponentStore<CameraComponent> &cameras,
              ComponentStore<Transform> &transforms);

private:
  Camera3D *camera; // Platform-level camera
};
