#pragma once

#include <SDL3/SDL.h>

struct AnimationComponent
{
    int numFrames;
    int framePerSeconds;
    Uint64 startTime;
    bool isLoop;

    AnimationComponent(int numFrames = 1, int framePerSeconds = 1, bool isLoop = true)
        : numFrames(numFrames), framePerSeconds(framePerSeconds), isLoop(isLoop)
    {
        startTime = SDL_GetTicksNS();
    }
};