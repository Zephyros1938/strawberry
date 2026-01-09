#include "game/systems/render_system.hpp"
#include "engine/ecs.hpp"
#include <glad/glad.h>

void RenderSystem::update(ComponentStore<Transform> &transforms,
                          ComponentStore<Renderable> &renderables) {
  if (!shader)
    return;
  shader->use();

  for (auto &[entity, transform] : transforms.all()) {
    if (!renderables.has(entity))
      continue;
    auto &renderable = renderables.get(entity);

    // Compute model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, transform.scale);

    shader->setMat4("model", model);

    // Bind textures if any
    for (size_t i = 0; i < renderable.textures.size(); ++i) {
      renderable.textures[i]->bind(GL_TEXTURE0 + i);
    }

    // Draw
    glBindVertexArray(renderable.vao);
    glDrawElements(renderable.drawMode, renderable.indexCount, GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);

    // Unbind textures
    for (size_t i = 0; i < renderable.textures.size(); ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}
