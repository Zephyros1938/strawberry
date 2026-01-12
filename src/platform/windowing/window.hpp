#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <string>

class Window {
public:
  Window(int width, int height, const std::string &title)
      : width(width), height(height), title(title) {
    std::cout << "Init GLFW\n";
    initGLFW();
    std::cout << "Create Window\n";
    createWindow();
    std::cout << "Init GLAD\n";
    initGLAD();
    std::cout << "Set up OpenGL State\n";
    setupOpenGLState();
  }

  ~Window() {
    std::cout << "Destroy Window";
    if (window)
      glfwDestroyWindow(window);
    glfwTerminate();
  }

  GLFWwindow *getGLFWwindow() const { return window; }

  bool shouldClose() const { return glfwWindowShouldClose(window); }

  void swapBuffers() { glfwSwapBuffers(window); }

  void pollEvents() { glfwPollEvents(); }

  void setFramebufferSizeCallback(GLFWframebuffersizefun cb) {
    glfwSetFramebufferSizeCallback(window, cb);
  }

  void setKeyCallback(GLFWkeyfun cb) {
    std::cout << "Set key cb" << std::endl;
    glfwSetKeyCallback(window, cb);
  }

  void setWindowUserPointer(void *p) { glfwSetWindowUserPointer(window, p); }

  void setCursorPosCallback(GLFWcursorposfun cb) {
    glfwSetCursorPosCallback(window, cb);
  }

  void setMouseButtonCallback(GLFWmousebuttonfun cb) {
    glfwSetMouseButtonCallback(window, cb);
  }

  void setScrollCallback(GLFWscrollfun cb) {
    glfwSetScrollCallback(window, cb);
  }

  float getTime() { return glfwGetTime(); }

private:
  int width;
  int height;
  std::string title;
  GLFWwindow *window = nullptr;

  void initGLFW() {
    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");
    // Optional: configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  }

  void createWindow() {
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
      throw std::runtime_error("Failed to create GLFW window");
    glfwMakeContextCurrent(window);
  }

  void initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      throw std::runtime_error("Failed to initialize GLAD");
  }

  void setupOpenGLState() {
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
  }
};
