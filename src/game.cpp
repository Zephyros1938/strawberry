#include "game.h"
#include "assetManager.h"
#include "camera.h"
#include "ecs.h"
#include "input/mouseHandler.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
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

  for (int i = -1; i < 2; i++) {
    Entity e = world.createEntity();
    world.addFrame(e, CFrame{glm::vec3(i, 0, 0), glm::vec3(0), glm::vec3(1)});

    Shader *shader =
        &AssetManager::loadShader("default", "assets/shaders/default.vert",
                                  "assets/shaders/default.frag");

    EntityRenderData rd(*shader);
    rd.vaoID = mesh->VAO;
    rd.indiceCount = mesh->indexCount;
    rd.textures = mesh->textures;
    world.addRenderData(e, rd);
  }

  std::cout << "Finished setting up." << std::endl;
}

void Game::run() {
  std::cout << "Running..." << std::endl;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwSwapBuffers(window);
    glfwPollEvents();

    processInput();

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    renderSystem.render(world, camera);
  }
  glfwTerminate();
}

void Game::processInput() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.position += camera.front * camera.moveSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.position -= camera.front * camera.moveSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) *
                       camera.moveSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
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

  game->camera.processScroll(y);
}

void Game::keyCallback(GLFWwindow *w, int key, int scancode, int action,
                       int mods) {
  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(w));

  game->inputSystem.handleInput(key, scancode, action, mods);

  if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
    game->mouseHandler.locked = !game->mouseHandler.locked;
  }

  if (!game->mouseHandler.locked)
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  else
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Game::~Game() {}
