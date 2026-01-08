#include "game.h"
#include "assetManager.h"
#include "camera.h"
#include "ecs.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "input/mouseHandler.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

#define GAME_H

Game::Game(int w, int h, const std::string &title)
    : width(w), height(h), window(nullptr), camera(), mouseHandler(0, 0),
      deltaTime(0.0), lastFrame(0.0) {

  camera.zoom = 20.0;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  assert(window != nullptr);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init GLAD" << std::endl;
    exit(-1);
  }

  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);

  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mouseMoveCallback);
  glfwSetScrollCallback(window, mouseScrollCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  setupScene();
}

void Game::setupScene() {
  Mesh *mesh =
      &AssetManager::loadMesh("cheetah", "assets/models/cheetah/cheetah.obj");
  Shader *shader = &AssetManager::loadShader(
      "default", "assets/shaders/default.vert", "assets/shaders/default.frag");

  for (int y = -25; y < 25; y++) {
    for (int x = -25; x < 25; x++) {
      Entity e = world.createEntity();
      EntityRenderData rd(*shader, mesh->VAO, mesh->indexCount, mesh->textures);
      world.addFrame(e, CFrame{glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(1)});
      world.addRenderData(e, rd);
    }
  }

  guiSystem.init(window);

  std::cout << "Finished setting up." << std::endl;
}

void Game::run() {
  std::cout << "Running..." << std::endl;
  bool doWireframe;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwSwapBuffers(window);
    glfwPollEvents();

    processInput();

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int current_vao;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow();
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.3f", 1.0 / deltaTime);
    ImGui::Text("Entities (Renderable): %lu", world.renderDataMap().size());
    ImGui::SliderFloat4("Set background color", clearColor, 0.0, 1.0);
    ImGui::SeparatorText("Camera");
    ImGui::Text("Position: (%.3f,%.3f,%.3f)", camera.position.x,
                camera.position.y, camera.position.z);
    ImGui::Text("Front: (%.3f,%.3f,%.3f)", camera.front.x, camera.front.y,
                camera.front.z);
    ImGui::Text("Pitch: %.3f, Yaw: %.3f", camera.pitch, camera.yaw);
    ImGui::Text("Zoom: %.3f", camera.zoom);
    ImGui::SeparatorText("OpenGL");
    ImGui::Text("current_vao: %i", current_vao);

    if (ImGui::Button("Add entity at current position")) {
      Mesh *mesh = &AssetManager::getMesh("cheetah");
      Entity e = world.createEntity();
      EntityRenderData rd(AssetManager::getShader("default"), mesh->VAO,
                          mesh->indexCount, mesh->textures);
      world.addFrame(e, CFrame(camera.position, glm::vec3(0), glm::vec3(1)));
      world.addRenderData(e, rd);
    }

    ImGui::Checkbox("Wireframe Render", &doWireframe);

    if (doWireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ImGui::End();
    renderSystem.render(world, camera);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
  glfwTerminate();
}

void Game::processInput() {
  if (inputSystem.isPressed(GLFW_KEY_ESCAPE))
    glfwSetWindowShouldClose(window, true);

  if (inputSystem.isDown(GLFW_KEY_W))
    camera.position += camera.front * camera.moveSpeed * deltaTime;
  if (inputSystem.isDown(GLFW_KEY_S))
    camera.position -= camera.front * camera.moveSpeed * deltaTime;
  if (inputSystem.isDown(GLFW_KEY_A))
    camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) *
                       camera.moveSpeed * deltaTime;
  if (inputSystem.isDown(GLFW_KEY_D))
    camera.position += glm::normalize(glm::cross(camera.front, camera.up)) *
                       camera.moveSpeed * deltaTime;
}

void Game::framebufferSizeCallback(GLFWwindow *w, int width, int height) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(w));
  std::cout << "Window resized to " << width << "x" << height << std::endl;
  game->width = width;
  game->height = height;
  glViewport(0, 0, width, height);
  game->camera.setAspectRatio(width, height);
}

void Game::mouseMoveCallback(GLFWwindow *w, double x, double y) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(w));
  glm::vec2 d = game->mouseHandler.handleMouse(x, y);
  if (game->mouseHandler.locked) {
    game->camera.processMouse(d.x, d.y);
  }
};

void Game::mouseScrollCallback(GLFWwindow *w, double x, double y) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(w));

  ImGuiIO io = ImGui::GetIO();

  if (!io.WantCaptureMouse) {
    game->camera.processScroll(y);
  }
}

void Game::keyCallback(GLFWwindow *w, int key, int scancode, int action,
                       int mods) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(w));

  game->inputSystem.handleInput(key, scancode, action, mods);
  // std::cout << key << ":" << action << std::endl;

  if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
    game->mouseHandler.locked = !game->mouseHandler.locked;
  }

  if (!game->mouseHandler.locked) {
    game->guiSystem.setMouseEnabled(true);
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  else {
    game->guiSystem.setMouseEnabled(false);
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

Game::~Game() {}
