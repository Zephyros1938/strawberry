#pragma once
#include <engine/ecs.hpp>
#include <game/components/renderable.hpp>
#include <game/components/transform.hpp>
#include <platform/rendering/shader.hpp>
#include <platform/rendering/texture.hpp>

class RenderSystem {
public:
  explicit RenderSystem() {}

  void update(ComponentStore<Transform> &transforms,
              ComponentStore<Renderable> &renderables);

private:
  float clearColor[4] = {0.5, 0.5, 0.5, 1.0};
};
