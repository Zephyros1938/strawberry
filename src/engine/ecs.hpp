#ifndef ECS_HPP
#define ECS_HPP

#include <cstdint>
#include <unordered_map>

using Entity = uint32_t;

class World {
public:
  Entity createEntity() { return nextEntity++; }

private:
  Entity nextEntity = 1;
};

template <typename Component> class ComponentStore {
public:
  void add(Entity e, Component &c) { data[e] = c; }
  bool has(Entity e) const { return data.contains(e); }
  Component &get(Entity e) { return data.at(e); }
  void remove(Entity e) const { data.erase(e); }

  auto &all() { return data; }

private:
  std::unordered_map<Entity, Component> data;
};

#endif
