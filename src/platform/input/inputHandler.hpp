#pragma once

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>

class MouseHandler {
public:
  MouseHandler(GLFWwindow *win)
      : window(win), lastX(0), lastY(0), scrollY(0), firstMouse(true) {}

  glm::vec2 handleMouse(double xpos, double ypos) {
    if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }
    float dx = xpos - lastX;
    float dy = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    delta = glm::vec2(dx, dy);
    return delta;
  }

  glm::vec2 getMouseDelta() const { return delta; }
  float getScroll() const { return scrollY; }
  void resetDelta() {
    delta = glm::vec2(0.0f);
    scrollY = 0.0f;
  }

  bool isKeyPressed(int key) const {
    return glfwGetKey(window, key) == GLFW_PRESS;
  }

private:
  GLFWwindow *window;
  float lastX, lastY;
  float scrollY;
  bool firstMouse;
  glm::vec2 delta;
};
