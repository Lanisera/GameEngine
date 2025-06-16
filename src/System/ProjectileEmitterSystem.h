#pragma once

#include "../ECS/ECS.h"

#include "../Component/SpriteComponent.h"
#include "../Component/TransformComponent.h"
#include "../Component/RigidbodyComponent.h"
#include "../Component/BoxColliderComponent.h"
#include "../Component/ProjectileEmitterComponent.h"

class ProjectileEmitterSystem : public System
{
    ProjectileEmitterSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<ProjectileEmitterComponent>();
    }

    void Update(std::unique_ptr<Registry>& registry) {
        for (auto entity : GetSystemEntities()) {
            const auto& transformComponent = entity.GetComponent<TransformComponent>();
            auto& projectileEmitterComponent = entity.GetComponent<ProjectileEmitterComponent>();

            if (SDL_GetTicksNS() - projectileEmitterComponent.lastEmissionTime > projectileEmitterComponent.repeatFrequency) {
                glm::vec2 projectilePosition = transformComponent.position;

                if (entity.HasComponent<SpriteComponent>()) {
                    const auto& spriteComponent = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += spriteComponent.width / 2;
                    projectilePosition.y += spriteComponent.height / 2;
                }
                
                Entity newEntity = registry->CreateEntity();
                newEntity.AddComponent<SpriteComponent>("image-bullet", 4, 4);
                newEntity.AddComponent<TransformComponent>(projectilePosition);
                newEntity.AddComponent<RigidbodyComponent>(glm::vec2(100.0, 0.0));
                newEntity.AddComponent<BoxColliderComponent>(4.0, 4.0);

                projectileEmitterComponent.lastEmissionTime = SDL_GetTicksNS();
            }
        }
    }
};