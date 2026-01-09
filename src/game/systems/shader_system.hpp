#include "platform/rendering/camera.hpp"
#include <engine/ecs.hpp>
#include <game/components/renderable.hpp>
#include <platform/rendering/texture.hpp>

class ShaderSystem {
public:
  explicit ShaderSystem(Camera3D *cam) : cam(cam) {}

  void update(ComponentStore<Renderable> &renderables) {
    for (auto &[entity, renderable] : renderables.all()) {
      renderable.shader->setMat4("uCameraProjection",
                                 cam->getProjectionMatrix());
      renderable.shader->setMat4("uCameraView", cam->getViewMatrix());
    }
  }

private:
  Camera3D *cam;
};
