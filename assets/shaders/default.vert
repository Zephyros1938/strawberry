#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uCameraView;
uniform mat4 uCameraProjection;

out vec3 Color;
out vec2 TexCoord;

void main() {
  gl_Position = uModel * uCameraProjection * uCameraView * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  Color = aColor;
  TexCoord = aTexCoord;
}
