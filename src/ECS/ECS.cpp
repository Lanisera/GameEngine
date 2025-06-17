#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
}

void Entity::Kill() const {
    registry->KillEntity(*this);
}

void Entity::Tag(const std::string& tag) {
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const {
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) {
    registry->GroupEntity(*this, group);
}

bool Entity::BelongToGroup(const std::string& group) const {
    return registry->EntityBelongToGroup(*this, group);
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

    // Logger::Debug("Create Entity Id " + std::to_string(entityId));

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

void Registry::TagEntity(Entity entity, const std::string& tag) {
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) const {
    if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end())
        return false;
    return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntityByTag(const std::string& tag) {
    return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
    if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end())
        return ;

    const std::string tag = tagPerEntity.at(entity.GetId());
    tagPerEntity.erase(entity.GetId());
    entityPerTag.erase(tag);
}

void Registry::GroupEntity(Entity entity, const std::string& group) {
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongToGroup(Entity entity, const std::string& group) const {
    if (groupPerEntity.find(entity.GetId()) == groupPerEntity.end())
        return false;
    
    const auto& entities = entitiesPerGroup.find(group)->second;
    return entities.find(entity) != entities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) {
    const auto& entities = entitiesPerGroup.find(group)->second;
    return std::vector<Entity>(entities.begin(), entities.end());
}

void Registry::RemoveEntityGroup(Entity entity) {
    if (groupPerEntity.find(entity.GetId()) == groupPerEntity.end())
        return ;
    
    const std::string& group = groupPerEntity.at(entity.GetId());
    groupPerEntity.erase(entity.GetId());

    if (entitiesPerGroup.find(group) == entitiesPerGroup.end() ||
        entitiesPerGroup[group].find(entity) == entitiesPerGroup[group].end())
        return ;
    entitiesPerGroup[group].erase(entity);
}

void Registry::Update() {
    for (auto entity : entityToBeCreate) {
        AddEntityToSystems(entity);
    }
    entityToBeCreate.clear();

    for (auto entity : entityToBeRemove) {
        RemoveEntityFromSystems(entity);
        RemoveEntityGroup(entity);
        RemoveEntityTag(entity);
        
        const int entityId = entity.GetId();
        entityComponentSignatures[entityId].reset();
        freeIds.push_back(entityId);
    }
    entityToBeRemove.clear();
}