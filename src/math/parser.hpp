#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <sstream>
#include <string>

template <typename T>
inline void parseComponents(const std::string &input, T *out, int count) {
  std::stringstream ss(input);
  std::string segment;

  for (int i = 0; i < count; ++i) {
    if (!std::getline(ss, segment, ',')) {
      throw std::runtime_error("Not enough components in input string");
    }

    if constexpr (std::is_floating_point_v<T>) {
      out[i] = static_cast<T>(std::stof(segment));
    } else if constexpr (std::is_integral_v<T>) {
      out[i] = static_cast<T>(std::stoi(segment));
    } else {
      static_assert(sizeof(T) == 0, "Unsupported component type");
    }
  }
}

template <typename Vec> inline Vec parseVec(const std::string &input) {
  using Scalar = typename Vec::value_type;
  constexpr int N = Vec::length();

  Vec result(Scalar(0));
  parseComponents(input, &result[0], N);
  return result;
}

template <typename Mat> inline Mat parseMat(const std::string &input) {
  using Scalar = typename Mat::value_type;
  constexpr int N = Mat::length(); // columns

  Scalar values[N * N];
  parseComponents(input, values, N * N);

  Mat m(Scalar(1));
  int idx = 0;

  for (int col = 0; col < N; ++col) {
    for (int row = 0; row < N; ++row) {
      m[col][row] = values[idx++];
    }
  }

  return m;
}

template <typename Quat = glm::quat>
inline Quat parseQuatEulerDeg(const std::string &input) {
  glm::vec3 euler = parseVec<glm::vec3>(input);
  return glm::quat(glm::radians(euler));
}
