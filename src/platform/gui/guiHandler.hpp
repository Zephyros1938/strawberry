#pragma once

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <utility>

class GuiHandler {
public:
  GuiHandler(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 460");
  }

  void NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void Finalize() { ImGui::Render(); }

  void Begin(const char *header) { ImGui::Begin(header); }

  template <typename... Args> void Text(const char *fmt, Args &&...args) {
    ImGui::Text(fmt, std::forward(args)...);
  }

  void End() { ImGui::End(); }

  void Render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
};
