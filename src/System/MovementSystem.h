#pragma once

#include "../ECS/ECS.h"
#include "../Component/TransformComponent.h"
#include "../Component/RigidbodyComponent.h"

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidbodyComponent>();
    }

    void Update(double deltaTime)
    {
        for (auto entity : GetSystemEntities()) {
            auto& transformComponent = entity.GetComponent<TransformComponent>();
            const auto& rigidbodyComponent = entity.GetComponent<RigidbodyComponent>();
            
            transformComponent.position.x += rigidbodyComponent.velocity.x * deltaTime;
            transformComponent.position.y += rigidbodyComponent.velocity.y * deltaTime;
        }
    }
};