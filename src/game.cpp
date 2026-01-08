#include "game.h"
#include "camera.h"
#include "input/mouseHandler.h"
#include "shader.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <string>

#define GAME_H

Game::Game(int w, int h, const std::string &title)
    : width(w), height(h), window(nullptr), camera(), mouseHandler(0, 0),
      deltaTime(0.0), lastFrame(0.0) {

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

  std::cout << "Finished pass 0" << std::endl;

  setupScene();
}

void Game::setupScene() {
  const float vertices[] = {
      0.5,  0.5,  0.0, 1.0, 0.0, 0.0, 1.0, 1.0, // 1
      0.5,  -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, // 2
      -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, // 3
      -0.5, 0.5,  0.0, 1.0, 1.0, 0.0, 0.0, 1.0, // 4
  };
  const unsigned int indices[] = {0, 1, 3, 1, 2, 3};
  // Vertex data, VAO, VBO, EBO setup
  unsigned int VBO, EBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  Entity e = world.createEntity();
  world.addFrame(e, CFrame{glm::vec3(0), glm::vec3(0), glm::vec3(1)});
  Shader *shader = &AssetManager::loadShader(
      "default", "assets/shaders/default.vert", "assets/shaders/default.frag");
  Texture *tex =
      &AssetManager::loadTexture("cheetah", "assets/images/cheetahlogo.png");

  EntityRenderData rd(*shader);
  rd.vaoID = VAO;
  rd.indiceCount = 6;
  world.addRenderData(e, rd);
  std::cout << "Finished pass 1" << std::endl;
}

void Game::run() {
  std::cout << "Running!" << std::endl;
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
