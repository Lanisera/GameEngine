#pragma once

#include <SDL3/SDL.h>

struct ProjectileComponent
{
    Uint64 duration;
    int damage;
    bool isFriendly;
    Uint64 startTime;

    ProjectileComponent(Uint64 duration = 0, int damage = 0, bool isFriendly = false)
        : duration(duration), damage(damage), isFriendly(isFriendly)
    {
        startTime = SDL_GetTicksNS();
    }
};
