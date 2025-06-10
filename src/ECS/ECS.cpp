#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
}

void Entity::Kill() const {
    registry->KillEntity(*this);
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity.GetId() == other.GetId();
    }), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}

Entity Registry::CreateEntity() {

    int entityId = 0;
    if (freeIds.empty()) {
        entityId = numEntities++;
        if (entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    }
    else {
        entityId = freeIds.front();
        freeIds.pop_front();
    }
    
    Entity newEntity = Entity{entityId};
    newEntity.registry = this;
    entityToBeCreate.insert(newEntity);

    // Logger::Info("Create Entity Id " + std::to_string(entityId));

    return newEntity;
}

void Registry::KillEntity(Entity entity) {
    entityToBeRemove.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity) {
    const int entityId = entity.GetId();
    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    for (const auto& system : systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (isInterested)
            system.second->AddEntityToSystem(entity);
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto& system : systems) {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::Update() {
    for (auto entity : entityToBeCreate) {
        AddEntityToSystems(entity);
    }
    entityToBeCreate.clear();

    for (auto entity : entityToBeRemove) {
        RemoveEntityFromSystems(entity);
        
        const int entityId = entity.GetId();
        entityComponentSignatures[entityId].reset();
        freeIds.push_back(entityId);
    }
}