#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "camera.h"
#include "ecs.h"
#include "external/imgui.h"
#include "external/imgui_impl_glfw.h"
#include "external/imgui_impl_opengl3.h"
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

enum KeyState {
  UNPRESSED = 0,
  PRESSED = 1,
  HELD = 2,
};

class InputSystem {
public:
  bool isDown(int k) {
    KeyState ks = getKeyState(k);
    return (ks == KeyState::PRESSED) || (ks == KeyState::HELD);
  }

  bool isReleased(int k) { return getKeyState(k) == KeyState::UNPRESSED; }

  bool isHeld(int k) { return getKeyState(k) == KeyState::HELD; }

  bool isPressed(int k) { return getKeyState(k) == KeyState::PRESSED; }

  KeyState getKeyState(int k) { return KEY_STATE[k]; }

  void handleInput(int key, int scancode, int action, int mods) {
    KEY_STATE[key] =
        (action == GLFW_PRESS || action == GLFW_REPEAT)
            ? (KEY_STATE[key] == KeyState::PRESSED ? KeyState::HELD
                                                   : KeyState::PRESSED)
            : KeyState::UNPRESSED;
  }

private:
  KeyState KEY_STATE[GLFW_KEY_LAST];
};

class GuiSystem {
public:
  GuiSystem() {}

  void init(GLFWwindow *w) {
    std::cout << "Initializing GUI" << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init();
    std::cout << "GUI initialized" << std::endl;
  }

  void setMouseEnabled(bool enabled) {
    ImGuiIO &io = ImGui::GetIO();
    if (enabled) {
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    } else {
      io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
  }
};

#endif
