#ifndef GAME_H
#define GAME_H

#include "assetManager.h"
#include "camera.h"
#include "ecs.h"
#include "input/mouseHandler.h"
#include "systems.h"
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <stdio.h>

class Game {
public:
  Game(int width, int height, const std::string &title);
  ~Game();

  void run();

private:
  void initWindow();
  void initGL();
  void processInput();
  void setupScene();

  // Callbacks
  static void framebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void mouseMoveCallback(GLFWwindow *window, double x, double y);
  static void mouseScrollCallback(GLFWwindow *window, double x, double y);

  // Members
  int width, height;
  GLFWwindow *window;
  Camera3D camera;
  MouseHandler mouseHandler;
  World world;
  RenderSystem renderSystem;
  InputSystem inputSystem;
  AssetManager assetManager;

  float deltaTime;
  float lastFrame;
  float clearColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};
};

#endif
