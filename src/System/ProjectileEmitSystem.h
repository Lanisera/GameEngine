#pragma once

#include "../ECS/ECS.h"

#include "../Component/SpriteComponent.h"
#include "../Component/TransformComponent.h"
#include "../Component/RigidbodyComponent.h"
#include "../Component/ProjectileComponent.h"
#include "../Component/BoxColliderComponent.h"
#include "../Component/ProjectileEmitterComponent.h"

class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem() {
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
                newEntity.AddComponent<SpriteComponent>(4, 4, "image-bullet", 4);
                newEntity.AddComponent<TransformComponent>(projectilePosition);
                newEntity.AddComponent<RigidbodyComponent>(projectileEmitterComponent.projectileVelocity);
                newEntity.AddComponent<BoxColliderComponent>(4.0, 4.0);
                newEntity.AddComponent<ProjectileComponent>(projectileEmitterComponent.projectileDuration, projectileEmitterComponent.projectileDamage, projectileEmitterComponent.isFriendly);

                projectileEmitterComponent.lastEmissionTime = SDL_GetTicksNS();
            }
        }
    }
};