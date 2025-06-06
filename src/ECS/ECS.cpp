#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
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
    int entityId = numEntities++;
    
    Entity newEntity = Entity{entityId};
    newEntity.registry = this;
    entityToBeCreate.insert(newEntity);

    if (entityId >= entityComponentSignatures.size()) {
        entityComponentSignatures.resize(entityId + 1);
    }

    Logger::Info("Create Entity Id " + std::to_string(entityId));

    return newEntity;
}

void Registry::AddEntityToSystems(Entity entity) {
    const int entityId = entity.GetId();
    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto system : systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (isInterested)
            system.second->AddEntityToSystem(entity);
    }
} 

void Registry::Update() {
    for (auto entity : entityToBeCreate) {
        AddEntityToSystems(entity);
    }
}