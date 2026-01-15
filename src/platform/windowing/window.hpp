#pragma once
#include "glad/glad.h"
#include "util/logger.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>

class Window {
public:
  Window(int width, int height, const std::string &title)
      : width(width), height(height), title(title) {
    Logger::Debug("Init GLFW");
    initGLFW();
    Logger::Debug("Create window");
    createWindow();
    Logger::Debug("Init OpenGL");
    initGLAD();
    Logger::Debug("Finalize OpenGL state");
    setupOpenGLState();
  }

  ~Window() {
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

  void setKeyCallback(GLFWkeyfun cb) { glfwSetKeyCallback(window, cb); }

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

  void setSize(int width, int height) {
    this->width = width;
    this->height = height;
  }
  int getWidth() { return width; }
  int getHeight() { return height; }

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
    // glCullFace(GL_FRONT_AND_BACK);
    // glFrontFace(GL_CW);
  }
};
