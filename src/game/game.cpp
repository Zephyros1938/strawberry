#include "game/game.hpp"
#include "assets/assetManager.hpp"
#include "engine/ecs.hpp"
#include "game/components/camera_component.hpp"
#include "game/components/renderable.hpp"
#include "game/components/transform.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/render_system.hpp"
#include "game/utils/worldLoader.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "math/parser.hpp"
#include "platform/gui/guiHandler.hpp"
#include "platform/input/inputHandler.hpp"
#include "platform/rendering/uniform_buffer_management.hpp"
#include "util/logger.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math/raycast.hpp>
#include <string>

Game::Game(int width, int height, const std::string &title)
    : window(width, height, title), inputHandler(false, false),
      uniformBufferManager(256, 0), guiHandler(window.getGLFWwindow()),
      cameraSystem(&camera), totalTime(0.0) {}

void Game::run() {
  setupScene();
  loadScene("assets/worlds/test.swld");
  Logger::Info("Running");
  ImGuiIO &io = ImGui::GetIO();

  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

  totalTime = 0.0;
  static Entity selectedEID = 0;

  while (!window.shouldClose()) {
    float currentFrame = window.getTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (deltaTime > 0 && deltaTime < 1.0f) {
      totalTime += deltaTime;
    }

    window.pollEvents();
    processInput();
    inputHandler.updateKeyboard();
    inputHandler.updateMouseButton();

    cameraSystem.update(cameras, transforms);

    glm::mat4 cameraProjectionMatrix = camera.getProjectionMatrix();
    glm::mat4 cameraViewMatrix = camera.getViewMatrix();
    uniformBufferManager.setData("uCameraView", &cameraViewMatrix);
    uniformBufferManager.setData("uCameraProjection", &cameraProjectionMatrix);
    uniformBufferManager.setData("uDeltatime", &deltaTime);
    uniformBufferManager.setData("uTime", &totalTime);

    guiHandler.NewFrame();
    ImGui::Begin("Debugging");
    ImGui::SeparatorText("Window");
    ImGui::Text("FPS: %.2f", 1.0f / deltaTime);
    ImGui::Text("Time: %.2f", totalTime);
    ImGui::Text("GLFW Time: %.2f", window.getTime());
    ImGui::Text("Difference GLFW-Time: %.02f", window.getTime() - totalTime);
    int wx, wy;
    glfwGetWindowSize(window.getGLFWwindow(), &wx, &wy);
    ImGui::Text("Size: %ix%i", wx, wy);
    ImGui::SeparatorText("Camera System");
    ImGui::Text("POSIT: x%.2f y%.2f z%.2f", camera.position.x,
                camera.position.y, camera.position.z);
    ImGui::Text("ROTAT: x%.2f y%.2f z%.2f", camera.front.x, camera.front.y,
                camera.front.z);
    ImGui::Text("Zoom: %.2f", camera.zoom);
    ImGui::SeparatorText("Logs");
    if (ImGui::Button("Clear Logs")) {
      Logger::Clear();
    }
    ImGui::BeginChild("LogRegion", ImVec2(0, 150), true,
                      ImGuiWindowFlags_HorizontalScrollbar);
    auto allLogs = Logger::GetLogs();
    int count = Logger::GetLogCount();
    for (int i = 0; i < count; i++) {
      ImGui::TextUnformatted(allLogs[i]);
    }

    // Auto-scroll to bottom if new logs are added
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    ImGui::End();
    ImGui::Begin("Editor");

    // NO LOOP HERE
    if (ImGui::BeginCombo(
            "Select Entity",
            selectedEID == 0 ? "None" : std::to_string(selectedEID).c_str())) {
      // THE LOOP GOES INSIDE: This populates the list items
      for (unsigned int e = 1; e <= world.getNextEntityId(); e++) {
        if (!transforms.has(e))
          continue;

        bool isSelected = (selectedEID == e);
        if (ImGui::Selectable(std::to_string(e).c_str(), isSelected)) {
          selectedEID = e;
        }

        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Separator();

    // Now show the sliders for the one entity we picked
    if (selectedEID != 0 && transforms.has(selectedEID)) {
      Transform &t = transforms.get(selectedEID);

      names.has(selectedEID)
          ? ImGui::Text("Editing Entity \"%s\" (EID: %i)",
                        names.get(selectedEID).c_str(), selectedEID)
          : ImGui::Text("Editing Entity <noname> (EID: %i)", selectedEID);
      ImGui::DragFloat3("Position", &t.position.x, 0.1f);
      ImGui::DragFloat3("Rotation", &t.rotation.x, 0.5f);
      ImGui::DragFloat3("Scale", &t.scale.x, 0.05f);

    } else {
      ImGui::Text("Please select an entity from the dropdown.");
    }

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

  renderSystem = RenderSystem();

  Entity ce = world.createEntity();
  CameraComponent cc = CameraComponent();
  cameras.add(ce, cc);

  inputHandler.setMouseSensitiviy(0.5);

  uniformBufferManager.registerUniform("uCameraView", sizeof(glm::mat4), 16);
  uniformBufferManager.registerUniform("uCameraProjection", sizeof(glm::mat4),
                                       16);
  uniformBufferManager.registerUniform("uDeltatime", sizeof(float), 4);
  uniformBufferManager.registerUniform("uTime", sizeof(float), 4);
}

void Game::loadScene(std::string fp = "assets/worlds/test.swld") {
  WorldLoader l(fp);
  for (auto [i, x] : l.shaderObjects.all()) {
    AssetManager::loadShader(i, x[0], x[1]);
  }
  for (auto [i, x] : l.meshObjects.all()) {
    AssetManager::loadMesh(i, x.c_str());
  }
  for (auto i : l.entityBlueprints) {
    Entity e = world.createEntity();
    names.add(e, i.name);
    if (i.data.count("POS")) {
      Transform t;
      t.position = parseVec<glm::vec3, 3>(i.data.at("POS"));
      if (i.data.count("ROT")) {
        t.rotation = parseVec<glm::vec3, 3>(i.data.at("ROT"));
      }
      if (i.data.count("SCALE")) {
        t.scale = parseVec<glm::vec3, 3>(i.data.at("SCALE"));
      }
      transforms.add(e, t);
    }
    if (i.data.count("MESH")) {
      Mesh m = AssetManager::getMesh(i.data.at("MESH").c_str());
      Renderable r =
          Renderable{m.VAO, m.indexCount, m.suggestedDrawMode, m.textures,
                     i.data.count("SHADER")
                         ? &AssetManager::getShader(i.data.at("SHADER").c_str())
                         : &AssetManager::getShader("default")};
      renderables.add(e, r);
    }
  }
}

void Game::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;
  glViewport(0, 0, width, height);
  game->camera.setAspectRatio(width, height);
  game->window.setSize(width, height);
}

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (!game)
    return;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(game->window.getGLFWwindow(), true);
  if (game->guiHandler.GetWantMouseCapture())
    return;

  game->inputHandler.handleKeyboard(key, scancode, action, mods);
}

void Game::cursorPosCallback(GLFWwindow *window, double x, double y) {

  auto *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (!game)
    return;
  if (!game->inputHandler.getMouseLocked())
    return;
  if (game->guiHandler.GetWantMouseCapture())
    return;

  glm::vec2 d = game->inputHandler.handleMouseMove(x, y) *
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
  if (game->guiHandler.GetWantMouseCapture())
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
  if (game->guiHandler.GetWantMouseCapture())
    return;

  game->inputHandler.handleMouseButton(button, action, mods);
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
    break;
  }
};

Game::~Game() {}
