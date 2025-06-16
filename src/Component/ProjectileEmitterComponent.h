#pragma once

#include <SDL3/SDL.h>

#include <glm/glm.hpp>

struct ProjectileEmitterComponent
{
    glm::vec2 projectileVelocity;
    Uint64 repeatFrequency;
    Uint64 projectileDuration;
    int projectileDamage;
    bool isFriendly;
    Uint64 lastEmissionTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), Uint64 repeatFrequency = 0, 
                               Uint64 projectileDuration = 0, int projectileDamage = 0, bool isFriendly = false)
        : projectileVelocity(projectileVelocity), repeatFrequency(repeatFrequency), 
          projectileDuration(projectileDuration), projectileDamage(projectileDamage), isFriendly(isFriendly)
    {
        lastEmissionTime = SDL_GetTicksNS();
    }
};
