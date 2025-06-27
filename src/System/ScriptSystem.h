#pragma once

#include "../ECS/ECS.h"

#include "../Component/ScriptComponent.h"
#include "../Component/RigidbodyComponent.h"
#include "../Component/ProjectileEmitterComponent.h"

class ScriptSystem : public System
{
public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    void SetEntityPosition(Entity entity, double x, double y) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.position.x = x;
            transform.position.y = y;
        }
        else {
            Logger::Error("Entity has not TransformComponent");
        }
    }

    void SetEntityVelocity(Entity entity, double x, double y) {
        if (entity.HasComponent<RigidbodyComponent>()) {
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
            rigidbody.velocity.x = x;
            rigidbody.velocity.y = y;
        } else {
            Logger::Error("Trying to set the velocity of an entity that has no rigidbody component");
        }
    }

    void SetEntityRotation(Entity entity, double angle) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.rotation = angle;
        } else {
            Logger::Error("Trying to set the rotation of an entity that has no transform component");
        }
    }

    void SetProjectileVelocity(Entity entity, double x, double y) {
        if (entity.HasComponent<ProjectileEmitterComponent>()) {
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
            projectileEmitter.projectileVelocity.x = x;
            projectileEmitter.projectileVelocity.y = y;
        } else {
            Logger::Error("Trying to set the projectile velocity of an entity that has no projectile emitter component");
        }
    }

    void CreateLuaBindings(sol::state& lua) {
        lua.new_usertype<Entity>(
            "entity",
            "get_id", &Entity::GetId,
            "destroy", &Entity::Kill,
            "has_tag", &Entity::HasTag,
            "belong_to_group", &Entity::BelongToGroup
        );

        lua.set_function("set_position", &ScriptSystem::SetEntityPosition, this);
        lua.set_function("set_velocity", &ScriptSystem::SetEntityVelocity, this);
        lua.set_function("set_rotation", &ScriptSystem::SetEntityRotation, this);
        lua.set_function("set_projectile_velocity", &ScriptSystem::SetProjectileVelocity, this);
    }

    void Update(double deltaTime, Uint64 ellapsedTime) {
        // Logger::Debug("ScriptSystem");
        for (auto entity : GetSystemEntities()) {
            const auto& script = entity.GetComponent<ScriptComponent>();
            script.func(entity, deltaTime, ellapsedTime);
        }
    }
};