#include "assetManager.hpp"
#include "platform/rendering/shader.hpp"
#include "platform/rendering/texture.hpp"
#include "util/logger.hpp"
#include <filesystem>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Texture &AssetManager::loadTexture(const std::string &name, const char *path) {
  auto it = textures.find(name);
  Logger::Info("Loading texture \"%s\" (%s)", name.c_str(), path);

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
  Logger::Info("Loading shader \"%s\"", name.c_str());

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

Mesh &AssetManager::loadMesh(const std::string &name, const char *path) {
  Logger::Info("Loading mesh \"%s\" (%s)", name.c_str(), path);
  if (meshes.find(name) != meshes.end())
    return meshes[name];

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  std::string objDir = std::filesystem::path(path).parent_path().string() + "/";
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path,
                        objDir.c_str())) {
    Logger::Error("Failed to load OBJ:\n%s", err.c_str());
    exit(-1);
  }

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
      vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
      vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

      if (!attrib.normals.empty()) {
        vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
        vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
        vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
      } else {
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
      }

      if (!attrib.texcoords.empty()) {
        vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
        vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
      } else {
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
      }

      indices.push_back(indices.size());
    }
  }

  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // vertex positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // vertex normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texcoords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  std::vector<Texture *> meshTextures;
  for (const auto &mat : materials) {
    std::cout << "\tFound material \"" << mat.name << "\", diffuse texname: \""
              << mat.diffuse_texname << "\"\n";
    Logger::Info("\tFound material \"%s\"\n\t\tDiff: \"%s\"\n\t\tNorm: "
                 "\"%s\"\n\t\tSpec: \"%s\"",
                 name.c_str(), mat.diffuse_texname.c_str(),
                 mat.normal_texname.c_str(), mat.specular_texname.c_str());
    for (auto texture :
         {mat.diffuse_texname, mat.normal_texname, mat.specular_texname}) {
      if (!texture.empty()) {
        std::string objDir =
            std::filesystem::path(path).parent_path().string() + "/";
        std::string texPath = objDir + texture;
        Texture *tex = &AssetManager::loadTexture(mat.name, texPath.c_str());
        meshTextures.push_back(tex);
      }
    };
  }

  Mesh mesh{VAO, VBO, EBO, static_cast<unsigned int>(indices.size()),
            meshTextures};
  meshes[name] = mesh;
  return meshes[name];
}

Mesh &AssetManager::getMesh(const std::string &name) { return meshes.at(name); }

std::map<std::string, Texture> AssetManager::textures;
std::map<std::string, Shader> AssetManager::shaders;
std::map<std::string, Mesh> AssetManager::meshes;
