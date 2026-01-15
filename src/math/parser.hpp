#pragma once

#include <glm/glm.hpp>
#include <sstream>
#include <string>

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
