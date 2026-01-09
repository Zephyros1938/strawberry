#pragma once
#include <engine/ecs.hpp>
#include <game/components/renderable.hpp>
#include <game/components/transform.hpp>
#include <platform/rendering/shader.hpp>
#include <platform/rendering/texture.hpp>

class RenderSystem {
public:
  explicit RenderSystem(Shader *defaultShader = nullptr)
      : shader(defaultShader) {}

  void update(ComponentStore<Transform> &transforms,
              ComponentStore<Renderable> &renderables);

private:
  Shader *shader; // default, overidden by renderables
};
