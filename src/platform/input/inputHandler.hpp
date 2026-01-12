#pragma once

#include "iostream"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

enum KeyState {
  RELEASE = 0,
  PRESS = 1,
  HOLD = 2,
};

class InputHandler {
public:
  InputHandler(bool m_Debug = false, bool k_Debug = false)
      : m_Last(0), m_Scroll(0), m_FirstMouse(true), m_Debug(m_Debug),
        k_Debug(k_Debug) {}

  glm::vec2 handleMouse(double xpos, double ypos) {
    if (m_Debug) {
      std::cout << "Mouse Move: (" << xpos << "," << ypos << ")" << std::endl;
    }
    if (m_FirstMouse) {
      m_Last.x = xpos;
      m_Last.y = ypos;
      m_FirstMouse = false;
    }
    float dx = xpos - m_Last.x;
    float dy = m_Last.y - ypos;
    m_Last.x = xpos;
    m_Last.y = ypos;
    m_Delta = glm::vec2(dx, dy);
    return m_Delta;
  }

  void handleScroll(double xpos, double ypos) {
    m_Scroll = glm::vec2(xpos, ypos);
  }

  glm::vec2 getMouseDelta() const { return m_Delta; }
  glm::vec2 getScroll() const { return m_Scroll; }

  void resetDelta() {
    m_Delta = glm::vec2(0.0f);
    m_Scroll = glm::vec2(0);
  }

  void setMouseLocked(bool v, GLFWwindow *window) {
    glfwSetInputMode(window, GLFW_CURSOR,
                     v ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    m_Locked = v;
    m_FirstMouse = v;
  }

  bool getMouseLocked() { return m_Locked; }

  void setMouseSensitiviy(float v) { m_Sensitivity = v; }

  float getMouseSensitivity() { return m_Sensitivity; }

  void handleKeyboard(int key, int scancode, int action, int mods) {
    if (k_Debug) {
      std::cout << "Key: " << key << ":" << scancode << ":" << action << ":"
                << mods << std::endl;
    }

    if (action == GLFW_PRESS) {
      k_KeyStates[key] = KeyState::PRESS;
    } else if (action == GLFW_RELEASE) {
      k_KeyStates[key] = KeyState::RELEASE;
    }
  }

  bool isKeyDown(int k) {
    return k_KeyStates[k] == KeyState::HOLD ||
           k_KeyStates[k] == KeyState::PRESS;
  }

  bool isKeyPressed(int k) { return k_KeyStates[k] == KeyState::PRESS; }

  bool isKeyHeld(int k) { return k_KeyStates[k] == KeyState::HOLD; }

  bool isKeyUp(int k) { return k_KeyStates[k] == KeyState::RELEASE; }

  KeyState getKey(int k) { return k_KeyStates[k]; }

  void updateKeyboard() {
    for (int i = 0; i < 348; ++i) {
      if (k_KeyStates[i] == KeyState::PRESS) {
        k_KeyStates[i] = KeyState::HOLD;
      }
    }
  }

private:
  glm::dvec2 m_Last;
  glm::vec2 m_Scroll;
  glm::vec2 m_Delta;
  bool m_FirstMouse;
  bool m_Debug = false;
  bool m_Locked = false;
  float m_Sensitivity = 1.0;

  KeyState k_KeyStates[348];
  bool k_Debug = false;
};
