#pragma once

#include <set>
#include <vector>
#include <bitset>
#include <memory>
#include <typeindex>
#include <algorithm>
#include <unordered_map>

#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;
using Signature = std::bitset<MAX_COMPONENTS>;

class Entity {
    friend class Registry;
public:
    Entity(int id) : id(id) {}
    int GetId() const;

    bool operator==(const Entity& other) const { return this->id == other.id; }
    bool operator!=(const Entity& other) const { return this->id != other.id; }
    bool operator>(const Entity& other) const { return this->id > other.id; }
    bool operator<(const Entity& other) const { return this->id < other.id; }

    template<typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
    template<typename TComponent> void RemoveComponent();
    template<typename TComponent> bool HasComponent() const;
    template<typename TComponent> TComponent& GetComponent() const;
    
private:
    int id;

    class Registry* registry;
};

struct IComponent {
protected:
    static int nextId;
};

template<typename T>
struct Component : public IComponent {
public:
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

class System {
public:
    System() = default;
    ~System() = default;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature& GetComponentSignature() const;

    template<typename TComponent> void RequireComponent(); 

private:
    std::vector<Entity> entities;
    Signature componentSignature;
};

class IPool {
protected:
    virtual ~IPool() = default;
};

template<typename T>
class Pool : public IPool {
public:
    Pool(int size = 100) {
        Resize(size);
    }
    
    virtual ~Pool() = default;

    void Resize(int size) { data.resize(size); }

    bool isEmpty() const { return data.empty(); }

    int GetSize() const { return data.size(); }

    void Clear() { data.clear(); }

    void Add(T object) { data.push_back(object); }

    // TODO: 错误检查
    T& Get(int index) { return static_cast<T&>(data[index]); }

    // TODO: 错误检查
    void Set(int index, T object) { data[index] = object; }

    // TODO: 错误检查
    T& operator[](int index) { return data[index]; }
private: 
    std::vector<T> data;
};

class Registry {
public:
    Registry() = default;

    Entity CreateEntity();

    template<typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
    template<typename TComponent> void RemoveComponent(Entity entity);
    template<typename TComponent> bool HasComponent(Entity entity) const; 
    template<typename TComponent> TComponent& GetComponent(Entity entity) const;

    template<typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
    template<typename TSystem> void RemoveSystem();
    template<typename TSystem> bool HasSystem() const;
    template<typename TSystem> TSystem& GetSystem() const;

    void AddEntityToSystems(Entity entity);

    void Update();

private:
    int numEntities{0};

    std::vector<std::shared_ptr<IPool>> componentPools;

    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    std::set<Entity> entityToBeCreate;
    std::set<Entity> entityToBeRemove;

};

template<typename TComponent, typename ...TArgs> 
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template<typename TComponent> 
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template<typename TComponent> 
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template<typename TComponent> 
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

template<typename TComponent>
void System::RequireComponent() {
    const int componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template<typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const int entityId = entity.GetId();
    const int componentId = Component<TComponent>::GetId();

    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        componentPools[componentId] = std::make_shared<Pool<TComponent>>();
    }

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    Logger::Info("Add Component Id " + std::to_string(componentId) + " to Entity Id " + std::to_string(entityId));
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    const int entityId = entity.GetId();
    const int componentId = Component<TComponent>::GetId();

    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Info("Remove Component Id " + std::to_string(componentId) + " to Entity Id " + std::to_string(entityId));
}

template<typename TComponent>
bool Registry::HasComponent(Entity entity) const {
    const int entityId = entity.GetId();
    const int componentId = Component<TComponent>::GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const {
    const int entityId = entity.GetId();
    const int componentId = Component<TComponent>::GetId();

    return std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId])->Get(entityId);
}

template<typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem =  
            std::make_shared<TSystem>(std::forward<TArgs>(args)...);

    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
void Registry::RemoveSystem() {
    auto systemItem = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(systemItem);
}

template<typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem>
TSystem& Registry::GetSystem() const {
    return *std::static_pointer_cast<TSystem>(systems.find(std::type_index(typeid(TSystem)))->second);
}