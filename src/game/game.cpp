#include "game/game.hpp"
#include "assets/assetManager.hpp"
#include "engine/ecs.hpp"
#include "game/components/camera_component.hpp"
#include "game/components/renderable.hpp"
#include "game/components/transform.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/render_system.hpp"
#include "imgui_impl_glfw.h"
#include "platform/gui/guiHandler.hpp"
#include "platform/input/inputHandler.hpp"
#include "platform/rendering/uniform_buffer_management.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

Game::Game(int width, int height, const std::string &title)
    : window(width, height, title), inputHandler(false, false),
      uniformBufferManager(256, 0), guiHandler(window.getGLFWwindow()),
      cameraSystem(&camera), shaderSystem(&camera) {}

void Game::run() {
  setupScene();
  std::cout << "running\n";
  ImGuiIO &io = ImGui::GetIO();

  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

  while (!window.shouldClose()) {
    float currentFrame = window.getTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    int winW, winH;
    int fbW, fbH;
    glfwGetWindowSize(window.getGLFWwindow(), &winW, &winH);
    glfwGetFramebufferSize(window.getGLFWwindow(), &fbW, &fbH);

    io.DisplaySize = ImVec2((float)winW, (float)winH);
    io.DisplayFramebufferScale = ImVec2(fbW / (float)winW, fbH / (float)winH);

    window.pollEvents();
    processInput();
    inputHandler.updateKeyboard();

    cameraSystem.update(cameras, transforms);

    glm::mat4 cameraProjectionMatrix = camera.getProjectionMatrix();
    glm::mat4 cameraViewMatrix = camera.getViewMatrix();
    uniformBufferManager.setData("uCameraView", &cameraViewMatrix);
    uniformBufferManager.setData("uCameraProjection", &cameraProjectionMatrix);

    guiHandler.NewFrame();
    ImGui::Begin("test");
    ImGui::Text("aaaa");
    ImGui::End();
    guiHandler.Finalize();
    renderSystem.update(transforms, renderables);
    guiHandler.Render();

    window.swapBuffers();
  }
}

void Game::setupScene() {
  window.setWindowUserPointer(this);
  window.setFramebufferSizeCallback(framebufferSizeCallback);
  window.setKeyCallback(keyCallback);
  window.setCursorPosCallback(cursorPosCallback);
  window.setMouseButtonCallback(mouseButtonCallback);

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

  uniformBufferManager.registerUniform("uCameraView", sizeof(glm::mat4), 16);
  uniformBufferManager.registerUniform("uCameraProjection", sizeof(glm::mat4),
                                       16);
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
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;

  game->inputHandler.handleKeyboard(key, scancode, action, mods);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(game->window.getGLFWwindow(), true);
}

void Game::cursorPosCallback(GLFWwindow *window, double x, double y) {

  auto *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (!game)
    return;
  if (!game->inputHandler.getMouseLocked())
    return;

  glm::vec2 d = game->inputHandler.handleMouse(x, y) *
                game->inputHandler.getMouseSensitivity();
  for (auto &[entity, camComp] : game->cameras.all()) {
    camComp.pitch += d.y;
    if (camComp.constrainPitch) {
      camComp.pitch = std::clamp(camComp.pitch, -89.0f, 89.0f);
    }
    camComp.yaw += d.x;
    break;
  }
}

void Game::scrollCallback(GLFWwindow *window, double x, double y) {
  ImGui_ImplGlfw_ScrollCallback(window, x, y);

  auto *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (!game)
    return;
  if (!game->inputHandler.getMouseLocked())
    return;

  game->inputHandler.handleScroll(x, y);
  for (auto &[entity, camComp] : game->cameras.all()) {
    camComp.fov = std::clamp(camComp.fov - game->inputHandler.getScroll().y,
                             1.0f, 160.0f);
  }
}

void Game::mouseButtonCallback(GLFWwindow *window, int button, int action,
                               int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;
}

void Game::processInput() {
  for (auto &[entity, camComp] : cameras.all()) {
    if (inputHandler.isKeyHeld('W')) {
      camComp.position += camComp.front * deltaTime;
    }
    if (inputHandler.isKeyHeld('S')) {
      camComp.position -= camComp.front * deltaTime;
    }
    if (inputHandler.isKeyHeld('A')) {
      camComp.position -=
          glm::normalize(glm::cross(camComp.front, camComp.up)) * deltaTime;
    }
    if (inputHandler.isKeyHeld('D')) {
      camComp.position +=
          glm::normalize(glm::cross(camComp.front, camComp.up)) * deltaTime;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_LEFT_ALT)) {
      inputHandler.setMouseLocked(!inputHandler.getMouseLocked(),
                                  window.getGLFWwindow());
    }
    // std::cout << camComp.position.x << " " << camComp.position.y << " "
    //           << camComp.position.z << std::endl;
    break;
  }
};

Game::~Game() {}
