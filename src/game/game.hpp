#pragma once

#include "engine/ecs.hpp"
#include "game/components/camera_component.hpp"
#include "game/components/renderable.hpp"
#include "game/components/transform.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/gui_system.hpp"
#include "game/systems/input_system.hpp"
#include "game/systems/render_system.hpp"
#include "platform/input/mouseHandler.hpp"
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

  // Members
  Window window;

  Camera3D camera;
  MouseHandler mouseHandler;

  // ECS
  World world;

  // Components
  ComponentStore<Transform> transforms;
  ComponentStore<Renderable> renderables;
  ComponentStore<CameraComponent> cameras;
  ComponentStore<InputComponent> inputComponents;
  // Systems
  RenderSystem renderSystem;
  InputSystem inputSystem;
  CameraSystem cameraSystem;

  float deltaTime;
  float lastFrame;
  float clearColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};
};
