#pragma once

#include <memory>


#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"

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

        if (a.BelongToGroup("Bullet") && b.HasTag("Player")) {
            OnProjectileHitPlayer(a, b);
        }
        if (b.BelongToGroup("Bullet") && a.HasTag("Player")) {
            OnProjectileHitPlayer(b, a);
        }
        if (a.BelongToGroup("Bullet") && b.BelongToGroup("Enemy")) {
            OnProjectileHitEnemy(a, b);
        }
        if (b.BelongToGroup("Bullet") && a.BelongToGroup("Enemy")) {
            OnProjectileHitEnemy(b, a);
        }
    }

    void OnProjectileHitPlayer(Entity bullet, Entity player) {
        const auto& projectile = bullet.GetComponent<ProjectileComponent>();

        if (!projectile.isFriendly && player.HasComponent<HealthComponent>()) {
            auto& health = player.GetComponent<HealthComponent>();
            
            health.healthPercentage -= projectile.damage;

            if (health.healthPercentage <= 0)
                player.Kill();
        
            bullet.Kill();
        }
    }

    void OnProjectileHitEnemy(Entity bullet, Entity enemy) {
        const auto& projectile = bullet.GetComponent<ProjectileComponent>();

        if (projectile.isFriendly && enemy.HasComponent<HealthComponent>()) {
            auto& health = enemy.GetComponent<HealthComponent>();
            
            health.healthPercentage -= projectile.damage;

            if (health.healthPercentage <= 0)
                enemy.Kill();
        
            bullet.Kill();
        }
    }
};