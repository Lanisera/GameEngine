#pragma once

#include "../Logger/Logger.h"

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"

#include "../Event/CollisionEvent.h"

#include "../Component/TransformComponent.h"
#include "../Component/BoxColliderComponent.h"

class CollisionSystem : public System
{
public:
    CollisionSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(std::unique_ptr<EventBus>& eventBus) {
        // Logger::Debug("CollisionSystem");
        const auto& entities = GetSystemEntities();
        for (auto i = entities.begin(); i != entities.end(); i++) {
            Entity a = *i;
            const auto& aTransformComponent = a.GetComponent<TransformComponent>();
            const auto& aBoxColliderComponent = a.GetComponent<BoxColliderComponent>();
            
            for (auto j = i; j != entities.end(); j++) {
                Entity b = *j;
                if (a == b) continue;
                
                auto& bTransformComponent = b.GetComponent<TransformComponent>();
                auto& bBoxColliderComponent = b.GetComponent<BoxColliderComponent>();

                bool isCollision = CheckAABBCollision(
                    aTransformComponent.position.x + aBoxColliderComponent.offset.x,
                    aTransformComponent.position.y + aBoxColliderComponent.offset.y,
                    aBoxColliderComponent.width,
                    aBoxColliderComponent.height,
                    bTransformComponent.position.x + bBoxColliderComponent.offset.x,
                    bTransformComponent.position.y + bBoxColliderComponent.offset.y,
                    bBoxColliderComponent.width,
                    bBoxColliderComponent.height
                );

                if (isCollision) {
                    eventBus->EmitEvent<CollisionEvent>(a, b);
                }
            }
        }
    }

private:
    bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
        return aX + aW >= bX &&
               bX + bW >= aX &&
               aY + aH >= bY &&
               bY + bH >= aY;
    }

};