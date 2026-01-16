#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord; // Added UVs

out vec3 vNormal;
out vec3 vWorldPos;
out vec2 vTexCoord; // Pass to frag

uniform mat4 uModel;

layout(std140, binding = 0) uniform uniformManager {
    mat4 uCameraView;
    mat4 uCameraProjection;
    float uDeltatime;
    float uTime;
};

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoord = aTexCoord;

    gl_Position = uCameraProjection * uCameraView * worldPos;
}
