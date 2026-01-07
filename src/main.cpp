#include "camera.h"
#include "ecs.h"
#include "glad/glad.h"
#include "input/mouseHandler.h"
#include "shader.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include <cstring>
#include <glm/common.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <iostream>

static int SIZE_X = 800;
static int SIZE_Y = 600;
static int KEY_STATE[GLFW_KEY_LAST];
static float DELTA_TIME = 0.0f;
static float LAST_FRAME = 0.0f;

static MouseHandler MOUSE_HANDLER = MouseHandler(0, 0);

static float CLEAR_COLOR[4] = {0.2f, 0.3f, 0.3f, 1.0f};

const float vertices[] = {
    0.5,  0.5,  0.0, 1.0, 0.0, 0.0, 1.0, 1.0, // 1
    0.5,  -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, // 2
    -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, // 3
    -0.5, 0.5,  0.0, 1.0, 1.0, 0.0, 0.0, 1.0, // 4
};
const unsigned int indices[] = {0, 1, 3, 1, 2, 3};

static World game = World{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  SIZE_X = width;
  SIZE_Y = height;
  glViewport(0, 0, SIZE_X, SIZE_Y);
  std::cout << "Window resized to " << SIZE_X << "x" << SIZE_Y << std::endl;

  auto *camera = static_cast<Camera3D *>(glfwGetWindowUserPointer(window));

  if (camera) {
    camera->setAspectRatio(width, height);
  }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  KEY_STATE[key] = action == GLFW_PRESS;
  std::cout << key << ":" << action << std::endl;

  if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
    MOUSE_HANDLER.locked = !MOUSE_HANDLER.locked;
    if (!MOUSE_HANDLER.locked)
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

void mouse_move_callback(GLFWwindow *window, double x, double y) {
  auto *camera = static_cast<Camera3D *>(glfwGetWindowUserPointer(window));

  glm::vec2 d = MOUSE_HANDLER.handleMouse(x, y);

  if (camera) {
    if (MOUSE_HANDLER.locked) {

      camera->processMouse(d.x, d.y);
    }
  }
}

void mouse_scroll_callback(GLFWwindow *window, double x, double y) {
  auto *camera = static_cast<Camera3D *>(glfwGetWindowUserPointer(window));

  if (camera) {
    camera->processScroll(y);
  }
}

int main() {
  std::cout << "strawberry :smile:" << std::endl;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(SIZE_X, SIZE_Y, "strawberry", NULL, NULL);
  assert(window != NULL);
  std::cout << "made window successfully" << std::endl;
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, SIZE_X, SIZE_Y);

  /* Callbacks */

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);

  Camera3D camera = Camera3D();
  glfwSetWindowUserPointer(window, &camera);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glm::mat4 model = glm::identity<glm::mat4>();

  /* Funcs */

  auto processInput = [&]() {
    if (KEY_STATE[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(window, true);
    };
    if (KEY_STATE[GLFW_KEY_W]) {
      camera.position += camera.front * camera.moveSpeed * DELTA_TIME;
    }
    if (KEY_STATE[GLFW_KEY_S]) {
      camera.position -= camera.front * camera.moveSpeed * DELTA_TIME;
    }
    if (KEY_STATE[GLFW_KEY_A]) {
      camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) *
                         camera.moveSpeed * DELTA_TIME;
    }
    if (KEY_STATE[GLFW_KEY_D]) {
      camera.position += glm::normalize(glm::cross(camera.front, camera.up)) *
                         camera.moveSpeed * DELTA_TIME;
    }
  };

  /* shader shit */

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

  Shader p("assets/shaders/default.vert", "assets/shaders/default.frag");
  Texture t("assets/images/cheetahlogo.png");

  /* Main */

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    DELTA_TIME = currentFrame - LAST_FRAME;
    LAST_FRAME = currentFrame;
    glfwSwapBuffers(window);
    glfwPollEvents();

    processInput();

    glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2],
                 CLEAR_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    p.use();
    p.setMat4("uModel", model);
    p.setMat4("uCameraView", camera.getViewMatrix());
    p.setMat4("uCameraProjection", camera.getProjectionMatrix());
    t.bind();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  glfwTerminate();
  return 0;
}
