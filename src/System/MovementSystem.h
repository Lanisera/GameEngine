#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Event/CollisionEvent.h"
#include "../Component/TransformComponent.h"
#include "../Component/RigidbodyComponent.h"

class MovementSystem : public System
{
public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidbodyComponent>();
    }

    void SubscribeEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
    }

    void OnCollision(CollisionEvent& event) {
        Entity a = event.a;
        Entity b = event.b;

        if (a.BelongToGroup("enemy") && b.BelongToGroup("obstacle")) {
            OnEnemyHitObstacle(a, b);
        }

        if (b.BelongToGroup("enemy") && a.BelongToGroup("obstacle")) {
            OnEnemyHitObstacle(b, a);
        }
    }

    void OnEnemyHitObstacle(Entity enemy, Entity obstacle) {
        if (enemy.HasComponent<RigidbodyComponent>()) {
            auto& rigidbody = enemy.GetComponent<RigidbodyComponent>();
            if (rigidbody.velocity.x != 0)
                rigidbody.velocity.x *= -1;

            if (rigidbody.velocity.y != 0)
                rigidbody.velocity.y *= -1;
        }
    }

    void Update(double deltaTime) {
        // Logger::Debug("MovementSystem");
        for (auto entity : GetSystemEntities()) {
            auto& transformComponent = entity.GetComponent<TransformComponent>();
            const auto& rigidbodyComponent = entity.GetComponent<RigidbodyComponent>();
            const auto& sprite = entity.GetComponent<SpriteComponent>();
            
            transformComponent.position.x += rigidbodyComponent.velocity.x * deltaTime;
            transformComponent.position.y += rigidbodyComponent.velocity.y * deltaTime;

            // Prevent the main player from moving outside the map boundaries
            if (entity.HasTag("player")) {
                int paddingLeft = 10;
                int paddingTop = 10;
                int paddingRight = 50;
                int paddingBottom = 50;
                transformComponent.position.x = transformComponent.position.x < paddingLeft ? paddingLeft : transformComponent.position.x;
                transformComponent.position.x = transformComponent.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transformComponent.position.x;
                transformComponent.position.y = transformComponent.position.y < paddingTop ? paddingTop : transformComponent.position.y;
                transformComponent.position.y = transformComponent.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : transformComponent.position.y;
            }

            bool isOutsideMap = (
                transformComponent.position.x < 0 ||
                transformComponent.position.y < 0 ||
                transformComponent.position.x + (transformComponent.scale.x * sprite.width) > Game::mapWidth ||
                transformComponent.position.y + (transformComponent.scale.y * sprite.height) > Game::mapHeight
            );

            if (isOutsideMap && !entity.HasTag("player")) {
                entity.Kill();
            }

            // Logger::Info(std::to_string(entity.GetId()) + ": " + std::to_string(transformComponent.position.x) + ", " + std::to_string(transformComponent.position.y));
        }
    }
};