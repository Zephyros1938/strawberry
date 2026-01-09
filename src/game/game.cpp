#include "game/game.hpp"
#include "assets/assetManager.hpp"
#include "engine/ecs.hpp"
#include "game/components/renderable.hpp"
#include "game/systems/camera_system.hpp"
#include "platform/input/mouseHandler.hpp"
#include <iostream>

Game::Game(int width, int height, const std::string &title)
    : window(width, height, title), mouseHandler(window.getGLFWwindow()),
      cameraSystem(&camera), inputSystem(&mouseHandler) {}

void Game::run() {
  std::cout << "running\n";
  while (!window.shouldClose()) {
    float currentFrame = window.getTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    window.pollEvents();
    processInput();

    inputSystem.update(inputComponents, deltaTime);
    cameraSystem.update(cameras, transforms);
    renderSystem.update(transforms, renderables);

    window.swapBuffers();
  }
}

void Game::setupScene() {
  window.setWindowUserPointer(this);
  window.setFramebufferSizeCallback(framebufferSizeCallback);
  window.setKeyCallback(keyCallback);

  window.setCursorPosCallback([](GLFWwindow *w, double x, double y) {
    auto *game = static_cast<Game *>(glfwGetWindowUserPointer(w));
    if (game)
      game->mouseHandler.handleMouse(x, y);
  });

  window.setScrollCallback([](GLFWwindow *w, double x, double y) {
    auto *game = static_cast<Game *>(glfwGetWindowUserPointer(w));
    if (game)
      // game->mouseHandler.handleScroll(y);
      std::cout << y << std::endl;
  });

  Mesh m = AssetManager::loadMesh("test", "assets/models/cheetah/cheetah.obj");
  Entity e = world.createEntity();
  Renderable r = Renderable{m.VAO, m.indexCount, 0x0004, m.textures};
  renderables.add(e, r);
}

void Game::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(game->window.getGLFWwindow(), true);
}

void Game::processInput() {};

Game::~Game() {}
