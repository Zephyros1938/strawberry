#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uCameraView;
uniform mat4 uCameraProjection;

out vec3 Normal;
out vec2 TexCoord;

void main() {
  gl_Position = uCameraProjection * uCameraView * uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  Normal = aNormal;
  TexCoord = aTexCoord;
}
