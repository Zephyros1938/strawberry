#pragma once
#include <engine/ecs2.hpp>
#include <game/components/renderable.hpp>
#include <game/components/transform.hpp>
#include <platform/rendering/shader.hpp>
#include <platform/rendering/texture.hpp>

class RenderSystem {
public:
  explicit RenderSystem() {}

  void update(ECS2 &ecs) {
    // Standard GL Setup
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Only get entities that have BOTH components
    auto entities = ecs.query<Renderable, Transform>();

    for (auto entity : entities) {
      // Get references (use & to avoid copying large structs every frame)
      auto &renderable = ecs.getComponent<Renderable>(entity);
      auto &transform = ecs.getComponent<Transform>(entity);

      if (renderable.shader)
        renderable.shader->use();

      // 1. Compute model matrix using the transform we know exists
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, transform.position);
      model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
      model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
      model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
      model = glm::scale(model, transform.scale);

      renderable.shader->setMat4("uModel", model);

      // 2. Bind textures
      for (size_t i = 0; i < renderable.textures.size(); ++i) {
        renderable.textures[i]->bind(GL_TEXTURE0 + static_cast<int>(i));
      }

      // 3. Draw
      glBindVertexArray(renderable.vao);
      glDrawElements(renderable.drawMode, renderable.indexCount,
                     GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      // 4. Unbind
      for (size_t i = 0; i < renderable.textures.size(); ++i) {
        renderable.textures[i]->unbind(GL_TEXTURE0 + static_cast<int>(i));
      }
    }
  }

private:
  float clearColor[4] = {0.5, 0.5, 0.5, 1.0};
};
