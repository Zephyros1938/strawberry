#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "camera.h"
#include "ecs.h"
#include "texture.h"
#include <GLFW/glfw3.h>

class RenderSystem {
public:
  void render(World &world, Camera3D &camera) {
    for (auto &pair : world.renderDataMap()) {
      Entity e = pair.first;
      if (!world.hasFrame(e))
        continue;

      auto &rd = pair.second;
      auto &frame = world.getFrame(e);

      for (size_t i = 0; i < rd.textures.size(); i++) {
        Texture *t = rd.textures[i];
        t->bind(GL_TEXTURE0 + i);
      }

      rd.shader.use();
      rd.shader.setMat4("uModel", frame.getMat4());
      rd.shader.setMat4("uCameraView", camera.getViewMatrix());
      rd.shader.setMat4("uCameraProjection", camera.getProjectionMatrix());

      glBindVertexArray(rd.vaoID);
      glDrawElements(rd.drawType, rd.indiceCount, GL_UNSIGNED_INT, 0);
    }
  }
};

class InputSystem {
public:
  bool isPressed(int k) { return KEY_STATE[k]; }

  void handleInput(int key, int scancode, int action, int mods) {
    KEY_STATE[key] = action == GLFW_PRESS;
  }

private:
  int KEY_STATE[GLFW_KEY_LAST];
};

#endif
