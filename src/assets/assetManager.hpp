#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "platform/rendering/shader.hpp"
#include "platform/rendering/texture.hpp"
#include "tiny_obj_loader.h"
#include <map>
#include <string>

struct Mesh {
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
  unsigned int indexCount;
  std::vector<Texture *> textures;
};

class AssetManager {
public:
  static Texture &loadTexture(const std::string &name, const char *path);
  static Texture &getTexture(const std::string &name);

  // Shaders
  static Shader &loadShader(const std::string &name,
                            const std::string &vertPath,
                            const std::string &fragPath);
  static Shader &getShader(const std::string &name);

  // Meshes
  static Mesh &loadMesh(const std::string &name, const char *path);
  static Mesh &loadMesh(const std::string &name, const float vertices[],
                        const unsigned int indices[],
                        const std::vector<Texture *> textures);
  static Mesh &getMesh(const std::string &name);

  static std::map<std::string, Texture> textures;
  static std::map<std::string, Shader> shaders;
  static std::map<std::string, Mesh> meshes;
};

#endif
