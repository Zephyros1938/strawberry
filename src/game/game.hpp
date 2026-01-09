#pragma once

#include "engine/ecs.hpp"
#include "game/components/camera_component.hpp"
#include "game/components/renderable.hpp"
#include "game/components/transform.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/gui_system.hpp"
#include "game/systems/render_system.hpp"
#include "game/systems/shader_system.hpp"
#include "platform/input/inputHandler.hpp"
#include "platform/rendering/camera.hpp"
#include "platform/windowing/window.hpp"

#include <GLFW/glfw3.h>
#include <string>

class Game {
public:
  Game(int width, int height, const std::string &title);
  ~Game();

  void run();

private:
  void processInput();
  void setupScene();

  // Callbacks
  static void framebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void mouseMoveCallback(GLFWwindow *window, double x, double y);
  static void mouseScrollCallback(GLFWwindow *window, double x, double y);
  static void cursorPosCallback(GLFWwindow *w, double x, double y);
  static void scrollCallback(GLFWwindow *w, double x, double y);

  // Members
  Window window;

  Camera3D camera = Camera3D();
  InputHandler inputHandler;

  // ECS
  World world;

  // Components
  ComponentStore<Transform> transforms;
  ComponentStore<Renderable> renderables;
  ComponentStore<CameraComponent> cameras;
  // Systems
  RenderSystem renderSystem;
  CameraSystem cameraSystem;
  ShaderSystem shaderSystem;

  float deltaTime;
  float lastFrame;
};
