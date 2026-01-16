#pragma once

#include "engine/ecs2.hpp"
#include "platform/rendering/uniform_buffer_management.hpp"
#include <game/components/light.hpp>
#include <game/components/transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>

constexpr int MAX_LIGHTS = 256;

struct GPULight {
  alignas(16) glm::vec3 color; // RGB intensity
  float intensity;             // Multiplier for color

  alignas(16) glm::vec3 position; // World space position (Point/Spot)
  float range;                    // How far the light reaches (attenuation)

  alignas(16) glm::vec3 direction; // Normalized vector (Directional/Spot)
  float spotAngle;                 // Inner/Outer cone ratio (Spot only)

  int type;         // 0: Directional, 1: Point, 2: Spot
  float shadowBias; // To prevent shadow acne
  int castShadows;  // Boolean flag (0 or 1)
  float padding;    // Keep the struct size a multiple of 16
};

struct LightSceneData {
  GPULight lights[MAX_LIGHTS];
  int numLights;
};

class LightingSystem {
  UniformBufferManager lightUBO;

public:
  LightingSystem(unsigned int binding)
      : lightUBO(sizeof(LightSceneData), binding) {
    lightUBO.registerUniform("LightData", sizeof(LightSceneData), 16);
  }

  void Update(ECS2 &ecs) {
    LightSceneData sceneData{};
    sceneData.numLights = 0;

    auto lights = ecs.query<Transform, LightComponent>();

    for (auto entity : lights) {
      if (sceneData.numLights >= MAX_LIGHTS)
        break;
      auto &transform = ecs.getComponent<Transform>(entity);
      auto &light = ecs.getComponent<LightComponent>(entity);

      GPULight &data = sceneData.lights[sceneData.numLights];
      data.position = transform.position;
      data.direction = glm::normalize(transform.position);
      data.color = light.color;
      data.intensity = light.intensity;
      data.type = light.type;
      data.range = light.range;
      if (light.type == 2 && ecs.hasComponent<SpotLightComponent>(entity)) {
        auto &spot = ecs.getComponent<SpotLightComponent>(entity);
        data.spotAngle = spot.outerAngle;
      }

      sceneData.numLights++;
    }

    lightUBO.setData("LightData", &sceneData);
  }
};
