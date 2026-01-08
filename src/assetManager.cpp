#include "assetManager.h"
#include "shader.h"
#include "texture.h"
#include <iostream>

Texture &AssetManager::loadTexture(const std::string &name, const char *path) {
  auto it = textures.find(name);
  std::cout << "Loading texture \"" << name << "\" (" << path << ")"
            << std::endl;

  if (it == textures.end()) {
    auto [newIt, success] = textures.emplace(name, Texture(path));
    return newIt->second;
  }

  return it->second;
}

Texture &AssetManager::getTexture(const std::string &name) {
  return textures.at(name); // throws if not found
}

Shader &AssetManager::loadShader(const std::string &name,
                                 const std::string &vertPath,
                                 const std::string &fragPath) {
  auto it = shaders.find(name);
  std::cout << "Loading shader \"" << name << "\"" << std::endl;

  if (it == shaders.end()) {
    auto [newIt, success] =
        shaders.emplace(name, Shader(vertPath.c_str(), fragPath.c_str()));
    return newIt->second;
  }
  return it->second;
}

Shader &AssetManager::getShader(const std::string &name) {
  return shaders.at(name);
}
std::map<std::string, Texture> AssetManager::textures;
std::map<std::string, Shader> AssetManager::shaders;
