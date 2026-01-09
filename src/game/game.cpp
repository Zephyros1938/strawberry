#include "game/game.hpp"
#include "assets/assetManager.hpp"
#include "engine/ecs.hpp"
#include "game/components/camera_component.hpp"
#include "game/components/renderable.hpp"
#include "game/components/transform.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/render_system.hpp"
#include "platform/input/inputHandler.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/ext/quaternion_geometric.hpp>
#include <iostream>

Game::Game(int width, int height, const std::string &title)
    : window(width, height, title),
      inputHandler(window.getGLFWwindow(), false, false), cameraSystem(&camera),
      shaderSystem(&camera) {}

void Game::run() {
  setupScene();
  std::cout << "running\n";
  while (!window.shouldClose()) {
    float currentFrame = window.getTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    window.pollEvents();
    processInput();

    // inputSystem.update(inputComponents, deltaTime);
    cameraSystem.update(cameras, transforms);
    shaderSystem.update(renderables);
    renderSystem.update(transforms, renderables);

    window.swapBuffers();
  }
}

void Game::setupScene() {
  window.setWindowUserPointer(this);
  window.setFramebufferSizeCallback(framebufferSizeCallback);
  window.setKeyCallback(keyCallback);
  window.setCursorPosCallback(cursorPosCallback);

  window.setScrollCallback(scrollCallback);

  renderSystem = RenderSystem(&AssetManager::loadShader(
      "default", "assets/shaders/default.vert", "assets/shaders/default.frag"));

  Mesh m = AssetManager::loadMesh("test", "assets/models/cheetah/cheetah.obj");
  Entity e = world.createEntity();
  Renderable r = Renderable{m.VAO, m.indexCount, 0x0004, m.textures,
                            &AssetManager::getShader("default")};
  Transform et = Transform{};
  transforms.add(e, et);
  renderables.add(e, r);

  Entity ce = world.createEntity();
  CameraComponent cc = CameraComponent();
  cameras.add(ce, cc);

  inputHandler.setMouseSensitiviy(0.5);

  std::cout << e << " " << ce << std::endl;
}

void Game::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;
  glViewport(0, 0, width, height);
  game->camera.setAspectRatio(width, height);
}

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;

  game->inputHandler.handleKeyboard(key, scancode, action, mods);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(game->window.getGLFWwindow(), true);
}

void Game::cursorPosCallback(GLFWwindow *window, double x, double y) {
  auto *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (game) {
    if (!game->inputHandler.getMouseLocked())
      return;
    glm::vec2 d = game->inputHandler.handleMouse(x, y) *
                  game->inputHandler.getMouseSensitivity();
    for (auto &[entity, camComp] : game->cameras.all()) {
      camComp.pitch += d.y;
      camComp.yaw += d.x;
      break;
    }
  }
}

void Game::scrollCallback(GLFWwindow *window, double x, double y) {
  auto *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (game) {
    if (!game->inputHandler.getMouseLocked())
      return;

    game->inputHandler.handleScroll(x, y);
    for (auto &[entity, camComp] : game->cameras.all()) {
      camComp.fov = std::clamp(camComp.fov - game->inputHandler.getScroll().y,
                               1.0f, 160.0f);
    }
  }
}

void Game::processInput() {
  for (auto &[entity, camComp] : cameras.all()) {
    if (inputHandler.isKeyDown('W')) {
      camComp.position += camComp.front * deltaTime;
    }
    if (inputHandler.isKeyDown('S')) {
      camComp.position -= camComp.front * deltaTime;
    }
    if (inputHandler.isKeyDown('A')) {
      camComp.position -=
          glm::normalize(glm::cross(camComp.front, camComp.up)) * deltaTime;
    }
    if (inputHandler.isKeyDown('D')) {
      camComp.position +=
          glm::normalize(glm::cross(camComp.front, camComp.up)) * deltaTime;
    }
    if (inputHandler.isKeyDown(GLFW_KEY_LEFT_ALT)) {
      inputHandler.setMouseLocked(!inputHandler.getMouseLocked(),
                                  window.getGLFWwindow());
    }
    // std::cout << camComp.position.x << " " << camComp.position.y << " "
    //           << camComp.position.z << std::endl;
    break;
  }
};

Game::~Game() {}
