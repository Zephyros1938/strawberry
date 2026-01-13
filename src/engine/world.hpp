#pragma once

#include "assets/assetManager.hpp"
#include "platform/rendering/shader.hpp"

#include <map>
class World {
  std::map<std::string, Shader> shader = {};
  std::map<std::string, Mesh> meshes = {};
};
