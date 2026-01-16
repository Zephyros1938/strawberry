#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;

layout(std140, binding = 0) uniform uniformManager {
  mat4 uCameraView;
  mat4 uCameraProjection;
  float uDeltatime;
  float uTime;
};

void main() {
  gl_Position = uCameraProjection * uCameraView * uModel * vec4(aPos, 1.0);
}
