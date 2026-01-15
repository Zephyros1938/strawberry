#pragma once

#include <glm/glm.hpp>
#include <sstream>
#include <string>

glm::vec3 parseVec3(const std::string &input) {
  std::stringstream ss(input);
  std::string segment;
  float components[3] = {0.0f, 0.0f, 0.0f};

  for (int i = 0; i < 3; ++i) {
    if (std::getline(ss, segment, ',')) {
      components[i] = std::stof(segment);
    }
  }

  return glm::vec3(components[0], components[1], components[2]);
}

template <typename T, int S> T parseVec(const std::string &input) {
  std::stringstream ss(input);
  std::string segment;
  float components[S];

  for (int i = 0; i < S; ++i) {
    if (std::getline(ss, segment, ',')) {
      components[i] = std::stof(segment);
    }
  }

  return T(S);
}
