#pragma once

#include "util/logger.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

const std::string WL_SHADER_BEGIN = "BEGINSHADER";
const std::string WL_SHADER_VERT = "SHADERVERT";
const std::string WL_SHADER_FRAG = "SHADERFRAG";
const std::string WL_SHADER_END = "ENDSHADER";

const std::string WL_MESH_BEGIN = "BEGINMESH";
const std::string WL_MESH_PATH = "MESH";
const std::string WL_MESH_END = "ENDMESH";

const std::string WL_TEX_BEGIN = "BEGINTEX";
const std::string WL_TEX_PATH = "TEX";
const std::string WL_TEX_END = "ENDTEX";

const std::string WL_ENTITY_BEGIN = "BEGINENTITY";
const std::string WL_ENTITY_POSITION = "POS";
const std::string WL_ENTITY_ROTATION = "ROT";
const std::string WL_ENTITY_SCALE = "SCALE";
const std::string WL_ENTITY_END = "ENDENTITY";

template <typename K, typename V> class WorldLoaderObject {
  std::map<K, V> data;

public:
  V &operator[](const K &index) { return data[index]; }
  bool contains(const K &index) { return data.find(index) != data.end(); }
  std::map<K, V> all() { return data; }
};

enum WorldLoaderReadPhase { FIND = 0, SHADER, MESH, TEXTURE, ENTITY };

struct EntityBlueprint {
  std::string name;
  std::map<std::string, std::string> data;
  /*
   * ex: ("POS", 10,10,10)
   */
};

class WorldLoader {
public:
  WorldLoaderObject<std::string, std::vector<std::string>> shaderObjects;
  WorldLoaderObject<std::string, std::string> meshObjects;
  WorldLoaderObject<std::string, std::string> textureObjects;
  std::vector<EntityBlueprint> entityBlueprints;

  WorldLoader(const std::string filePath) {
    Logger::Debug("Loading world from file \"%s\"", filePath.c_str());
    std::ifstream file(filePath);

    if (!file) {
      throw std::runtime_error("Could not open world file: " + filePath);
    }

    std::string version;
    std::getline(file, version);
    Logger::Debug("\tVERSION: %s", version.substr(1).c_str());

    if (version == "V0.1.0") {
      std::string line;
      std::string currentObjectName;

      while (std::getline(file, line)) {

        if (line.empty())
          continue;
        if (line.substr(0, 1) == "#")
          continue;

        switch (currentReadPhase) {
        case FIND:
          if (line.substr(0, WL_SHADER_BEGIN.size()) == WL_SHADER_BEGIN) {
            currentObjectName = line.substr(WL_SHADER_BEGIN.size());
            currentReadPhase = SHADER;
          } else if (line.substr(0, WL_MESH_BEGIN.size()) == WL_MESH_BEGIN) {
            currentObjectName = line.substr(WL_MESH_BEGIN.size());
            currentReadPhase = MESH;
          } else if (line.substr(0, WL_TEX_BEGIN.size()) == WL_TEX_BEGIN) {
            currentObjectName = line.substr(WL_TEX_BEGIN.size());
            currentReadPhase = TEXTURE;
          } else if (line.substr(0, WL_ENTITY_BEGIN.size()) ==
                     WL_ENTITY_BEGIN) {
            currentObjectName = line.substr(WL_ENTITY_BEGIN.size());
            currentReadPhase = ENTITY;
            entityBlueprints.push_back({currentObjectName, {}});
          }
          break;

        case SHADER:
          if (line == WL_SHADER_END) {
            currentReadPhase = FIND;
          } else if (line.substr(0, WL_SHADER_VERT.size()) == WL_SHADER_VERT) {
            shaderObjects[currentObjectName].push_back(
                line.substr(WL_SHADER_VERT.size()));
          } else if (line.substr(0, WL_SHADER_FRAG.size()) == WL_SHADER_FRAG) {
            shaderObjects[currentObjectName].push_back(
                line.substr(WL_SHADER_FRAG.size()));
          }
          break;

        case MESH:
          if (line == WL_MESH_END) {
            currentReadPhase = FIND;
          } else if (line.substr(0, WL_MESH_PATH.size()) == WL_MESH_PATH) {
            meshObjects[currentObjectName] = line.substr(WL_MESH_PATH.size());
          }
          break;

        case TEXTURE:
          if (line == WL_TEX_END) {
            currentReadPhase = FIND;
          } else if (line.substr(0, WL_TEX_PATH.size()) == WL_TEX_PATH) {
            textureObjects[currentObjectName] = line.substr(WL_TEX_PATH.size());
          }
          break;

        case ENTITY:
          if (line == WL_ENTITY_END) {
            currentReadPhase = FIND;
          } else {
            size_t delim = line.find(": ");
            if (delim != std::string::npos) {
              std::string key = line.substr(0, delim);
              std::string value = line.substr(delim + 2);
              entityBlueprints.back().data[key] = value;
            }
          }
          break;
        }
      };

      file.close();
    } else {
      file.close();
      throw std::runtime_error("Unsupported WorldLoader version: " + version);
    }
  }

private:
  WorldLoaderReadPhase currentReadPhase = FIND;
};
