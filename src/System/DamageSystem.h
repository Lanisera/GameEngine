#pragma once

#include <memory>

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"

#include "../Event/CollisionEvent.h"

#include "../Component/HealthComponent.h"
#include "../Component/ProjectileComponent.h"
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
        Entity a = event.a;
        Entity b = event.b;

        if (a.BelongToGroup("bullet") && b.HasTag("Player")) {
            OnProjectileHitPlayer(a, b);
        }
        if (b.BelongToGroup("bullet") && a.HasTag("Player")) {
            OnProjectileHitPlayer(b, a);
        }
        if (a.BelongToGroup("bullet") && b.BelongToGroup("enemy")) {

        }
        if (b.BelongToGroup("bullet") && a.BelongToGroup("enemy")) {

        }
    }

    void OnProjectileHitPlayer(Entity bullet, Entity player) {
        const auto& projectile = bullet.GetComponent<ProjectileComponent>();
        auto& health = player.GetComponent<HealthComponent>();

        if (!projectile.isFriendly) {
            health.healthPercentage -= projectile.damage;
        }

        if (health.healthPercentage <= 0)
            player.Kill();
        
        bullet.Kill();
    }
};