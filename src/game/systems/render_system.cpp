#include "game/systems/render_system.hpp"
#include "engine/ecs.hpp"
#include <glad/glad.h>

void RenderSystem::update(ComponentStore<Transform> &transforms,
                          ComponentStore<Renderable> &renderables) {
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  for (auto &[entity, renderable] : renderables.all()) {
    if (renderable.shader)
      renderable.shader->use();

    // Compute model matrix
    glm::mat4 model = glm::mat4(1.0f);
    if (transforms.has(entity)) {
      Transform transform = transforms.get(entity);
      model = glm::translate(model, transform.position);
      model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
      model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
      model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
      model = glm::scale(model, transform.scale);
    }

    renderable.shader->setMat4("uModel", model);

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
      renderable.textures[i]->unbind(GL_TEXTURE0 + i);
    }
  }
}
