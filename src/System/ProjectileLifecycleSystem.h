#pragma once

#include "../ECS/ECS.h"

#include "../Component/ProjectileComponent.h"

class ProjectileLifecycleSystem : public System 
{
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update(std::unique_ptr<Registry>& registry) {
        for (auto entity : GetSystemEntities()) {
            const auto& projectileComponent = entity.GetComponent<ProjectileComponent>();

            if (SDL_GetTicksNS() - projectileComponent.startTime > projectileComponent.duration) {
                registry->KillEntity(entity);
                Logger::Info("Remove Entity");
            }
        }
    }
};
