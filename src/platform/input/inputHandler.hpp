#pragma once

#include "util/logger.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/vec2.hpp>

enum ButtonState {
  RELEASE = 0,
  PRESS = 1,
  HOLD = 2,
};

class InputHandler {
public:
  InputHandler(bool m_Debug = false, bool k_Debug = false)
      : m_Last(0), m_Scroll(0), m_FirstMouse(true), m_Debug(m_Debug),
        k_Debug(k_Debug) {
    std::fill_n(m_PointerStates, 8, ButtonState::RELEASE);
    std::fill_n(k_KeyStates, GLFW_KEY_LAST, ButtonState::RELEASE);
  }

  glm::vec2 handleMouseMove(double xpos, double ypos) {
    if (m_Debug) {
      Logger::Debug("Mouse moved to %dx%d", xpos, ypos);
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

  void handleMouseButton(int button, int action, int mods) {
    if (m_Debug)
      Logger::Debug("Mouse button: %i:%i:%i", button, action, mods);

    if (action == GLFW_PRESS) {
      m_PointerStates[button] = ButtonState::PRESS;
    } else if (action == GLFW_RELEASE) {
      m_PointerStates[button] = ButtonState::RELEASE;
    }
  }

  void updateMouseButton() {
    for (int i = 0; i < 8; i++) {
      if (m_PointerStates[i] == ButtonState::PRESS)
        m_PointerStates[i] = ButtonState::HOLD;
    }
  }

  void handleScroll(double xpos, double ypos) {
    m_Scroll = glm::vec2(xpos, ypos);
  }

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
  glm::vec2 getMouseDelta() const { return m_Delta; }
  glm::vec2 getScroll() const { return m_Scroll; }
  ButtonState *getMousePointerState() { return m_PointerStates; }

  bool isMouseDown(int mb) { return isMouseHeld(mb) || isMousePressed(mb); }
  bool isMousePressed(int mb) {
    return m_PointerStates[mb] == ButtonState::PRESS;
  }
  bool isMouseHeld(int mb) { return m_PointerStates[mb] == ButtonState::HOLD; }
  bool isMouseUp(int mb) { return m_PointerStates[mb] == ButtonState::RELEASE; }

  glm::dvec2 getMousePos() { return m_Last; }

  /*                            */
  /*          KEYBOARD          */
  /*                            */

  void handleKeyboard(int key, int scancode, int action, int mods) {
    if (k_Debug) {
      Logger::Debug("Key pressed: %c:%i:%i:%i", key, scancode, action, mods);
    }

    if (action == GLFW_PRESS) {
      k_KeyStates[key] = ButtonState::PRESS;
    } else if (action == GLFW_RELEASE) {
      k_KeyStates[key] = ButtonState::RELEASE;
    }
  }
  void updateKeyboard() {
    for (int i = 0; i < 348; ++i) {
      if (k_KeyStates[i] == ButtonState::PRESS) {
        k_KeyStates[i] = ButtonState::HOLD;
      }
    }
  }

  bool isKeyDown(int k) {
    return k_KeyStates[k] == ButtonState::HOLD ||
           k_KeyStates[k] == ButtonState::PRESS;
  }

  bool isKeyPressed(int k) { return k_KeyStates[k] == ButtonState::PRESS; }
  bool isKeyHeld(int k) { return k_KeyStates[k] == ButtonState::HOLD; }
  bool isKeyUp(int k) { return k_KeyStates[k] == ButtonState::RELEASE; }
  ButtonState getKey(int k) { return k_KeyStates[k]; }
  ButtonState *getKeyStates() { return k_KeyStates; }

private:
  glm::dvec2 m_Last;
  glm::vec2 m_Scroll;
  glm::vec2 m_Delta;
  bool m_FirstMouse;
  bool m_Debug = false;
  bool m_Locked = false;
  float m_Sensitivity = 1.0;
  ButtonState m_PointerStates[8];

  ButtonState k_KeyStates[GLFW_KEY_LAST];
  bool k_Debug = false;
};
