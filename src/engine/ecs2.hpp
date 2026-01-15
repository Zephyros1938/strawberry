#pragma once

#include <bitset>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

using Entity = uint32_t;
const uint32_t MAX_COMPONENTS = 64;
using Signature = std::bitset<MAX_COMPONENTS>;

// -- Config --

class ComponentTypeManager {
public:
  template <typename T> static uint8_t getId() {
    static uint8_t typeId = nextId++;
    return typeId;
  }

private:
  static inline uint8_t nextId = 0;
};

class IComponentStore {
public:
  virtual ~IComponentStore() = default;
  virtual void removeEntity(Entity e) = 0;
};

template <typename T> class ComponentStore : public IComponentStore {
public:
  void add(Entity e, T component) { data[e] = component; }
  void removeEntity(Entity e) override { data.erase(e); }
  T &get(Entity e) { return data.at(e); }
  bool has(Entity e) const { return data.contains(e); }

private:
  std::unordered_map<Entity, T> data;
};

class ECS2 {
public:
  Entity createEntity() {
    Entity id = nextEntity++;
    signatures[id] = Signature(0);
    return id;
  }

  template <typename T> void addComponent(Entity e, T component) {
    getStore<T>()->add(e, component);
    signatures[e].set(ComponentTypeManager::getId<T>(), true);
  }

  template <typename T> T &getComponent(Entity e) {
    return getStore<T>()->get(e);
  }

  template <typename T> bool hasComponent(Entity e) {
    return signatures[e].test(ComponentTypeManager::getId<T>());
  }

  template <typename... Components> std::vector<Entity> query() {
    Signature requirement;
    // Fold expression to set bits for all requested types
    ((requirement.set(ComponentTypeManager::getId<Components>())), ...);

    std::vector<Entity> results;
    for (auto const &[entity, sig] : signatures) {
      if ((sig & requirement) == requirement) {
        results.push_back(entity);
      }
    }
    return results;
  }

private:
  Entity nextEntity = 1;
  std::unordered_map<Entity, Signature> signatures;
  std::unordered_map<std::type_index, std::shared_ptr<IComponentStore>> stores;

  // Helper to get or create a store for a specific type
  template <typename T> std::shared_ptr<ComponentStore<T>> getStore() {
    auto type = std::type_index(typeid(T));
    if (!stores.contains(type)) {
      stores[type] = std::make_shared<ComponentStore<T>>();
    }
    return std::static_pointer_cast<ComponentStore<T>>(stores[type]);
  }
};
