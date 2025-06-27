#pragma once

#include "../ECS/ECS.h"

#include "../EventBus/EventBus.h"

#include "../Event/KeyPressedEvent.h"

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

    void SubscribeEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        switch (event.keySymbol)
        {
        case SDLK_SPACE:
            for (auto entity : GetSystemEntities()) {
                if (entity.HasComponent<CameraFollowComponent>()) {
                    const auto& transformComponent = entity.GetComponent<TransformComponent>();
                    const auto& rigidbodyComponent = entity.GetComponent<RigidbodyComponent>();
                    const auto& projectileEmitterComponent = entity.GetComponent<ProjectileEmitterComponent>();

                    int directionX = 0;
                    int directionY = 0;
                    if (rigidbodyComponent.velocity.x < 0) directionX = -1;
                    if (rigidbodyComponent.velocity.x > 0) directionX = 1;
                    if (rigidbodyComponent.velocity.y < 0) directionY = -1;
                    if (rigidbodyComponent.velocity.y > 0) directionY = 1;

                    glm::vec2 projectileVelocity;
                    projectileVelocity.x = projectileEmitterComponent.projectileVelocity.x * directionX;
                    projectileVelocity.y = projectileEmitterComponent.projectileVelocity.y * directionY;

                    glm::vec2 projectilePosition = transformComponent.position;

                    if (entity.HasComponent<SpriteComponent>()) {
                        const auto& spriteComponent = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += spriteComponent.width / 2;
                        projectilePosition.y += spriteComponent.height / 2;
                    }

                    Entity newEntity = entity.registry->CreateEntity();
                    newEntity.Group("bullet");
                    newEntity.AddComponent<SpriteComponent>(4.0, 4.0, "bullet-texture", 4);
                    newEntity.AddComponent<TransformComponent>(projectilePosition);
                    newEntity.AddComponent<RigidbodyComponent>(projectileVelocity);
                    newEntity.AddComponent<BoxColliderComponent>(4.0, 4.0);
                    newEntity.AddComponent<ProjectileComponent>(projectileEmitterComponent.projectileDuration, projectileEmitterComponent.projectileDamage, projectileEmitterComponent.isFriendly);
                }
            }
            break;
        }
    }

    void Update(std::unique_ptr<Registry>& registry) {
        // Logger::Debug("ProjectileEmitSystem");
        for (auto entity : GetSystemEntities()) {
            const auto& transformComponent = entity.GetComponent<TransformComponent>();
            auto& projectileEmitterComponent = entity.GetComponent<ProjectileEmitterComponent>();

            if (projectileEmitterComponent.repeatFrequency == 0)
                continue;

            if (SDL_GetTicksNS() - projectileEmitterComponent.lastEmissionTime > projectileEmitterComponent.repeatFrequency) {
                glm::vec2 projectilePosition = transformComponent.position;

                if (entity.HasComponent<SpriteComponent>()) {
                    const auto& spriteComponent = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += spriteComponent.width / 2;
                    projectilePosition.y += spriteComponent.height / 2;
                }
                
                Entity newEntity = registry->CreateEntity();
                newEntity.Group("bullet");
                newEntity.AddComponent<SpriteComponent>(4, 4, "bullet-texture", 4);
                newEntity.AddComponent<TransformComponent>(projectilePosition);
                newEntity.AddComponent<RigidbodyComponent>(projectileEmitterComponent.projectileVelocity);
                newEntity.AddComponent<BoxColliderComponent>(4.0, 4.0);
                newEntity.AddComponent<ProjectileComponent>(projectileEmitterComponent.projectileDuration, projectileEmitterComponent.projectileDamage, projectileEmitterComponent.isFriendly);

                projectileEmitterComponent.lastEmissionTime = SDL_GetTicksNS();
            }
        }
    }
};