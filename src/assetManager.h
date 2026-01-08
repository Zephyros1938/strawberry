#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "shader.h"
#include "texture.h"
#include <map>
#include <string>
#include <unordered_map>
class AssetManager {
public:
  static Texture &loadTexture(const std::string &name, const char *path);
  static Texture &getTexture(const std::string &name);

  // Shaders
  static Shader &loadShader(const std::string &name,
                            const std::string &vertPath,
                            const std::string &fragPath);
  static Shader &getShader(const std::string &name);

private:
  static std::map<std::string, Texture> textures;
  static std::map<std::string, Shader> shaders;
};

#endif
