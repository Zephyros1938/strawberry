#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include <glm/ext/vector_float2.hpp>
class MouseHandler {
public:
  bool locked;
  MouseHandler(float x, float y)
      : lastX(x), lastY(y), firstMouse(true), locked(true), scrollY(0) {};
  glm::vec2 handleMouse(float xp, float yp) {
    if (firstMouse) {
      lastX = xp;
      lastY = yp;
      firstMouse = false;
    }
    float xo = xp - lastX;
    float yo = lastY - yp;
    lastX = xp;
    lastY = yp;
    return glm::vec2(xo, yo);
  }

private:
  float lastX, lastY, scrollY;
  bool firstMouse;
};

#endif
