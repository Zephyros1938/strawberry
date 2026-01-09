// game/systems/input_system.hpp
#pragma once
#include "engine/ecs.hpp"
#include "game/components/input_component.hpp"
#include "platform/input/inputHandler.hpp"

class InputSystem {
public:
  explicit InputSystem(MouseHandler *mouse) : mouseHandler(mouse) {}

  void update(ComponentStore<InputComponent> &inputs, float deltaTime) {};

private:
  MouseHandler *mouseHandler;
};
