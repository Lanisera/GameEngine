#pragma once

#include <memory>

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"

#include "../Event/CollisionEvent.h"

#include "../Component/BoxColliderComponent.h"

class DamageSystem : public System
{
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
    }
     
    void OnCollision(CollisionEvent& event) {
        // Logger::Info("Collision happened !!!");
        // event.a.Kill();
        // event.b.Kill();
    }
};