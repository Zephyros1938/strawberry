#include "platform/rendering/camera.hpp"
#include <engine/ecs.hpp>
#include <game/components/renderable.hpp>
#include <platform/rendering/texture.hpp>

class ShaderSystem {
public:
  explicit ShaderSystem(Camera3D *cam) : cam(cam) {}

  void update(ComponentStore<Renderable> &renderables) {}

private:
  Camera3D *cam;
};
