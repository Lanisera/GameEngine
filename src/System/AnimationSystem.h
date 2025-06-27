#pragma once

#include "../ECS/ECS.h"

#include "../Component/SpriteComponent.h"
#include "../Component/AnimationComponent.h"

class AnimationSystem : public System
{
public:
    AnimationSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<AnimationComponent>();
    }

    void Update() {
        Logger::Debug("AnimationSystem");
        for (auto& entity : GetSystemEntities()) {
            auto& spriteComponent = entity.GetComponent<SpriteComponent>();
            auto& animationComponent = entity.GetComponent<AnimationComponent>();

            int curFrame = ((SDL_GetTicksNS() - animationComponent.startTime) * animationComponent.framePerSeconds / 1000000000) % animationComponent.numFrames;
            spriteComponent.srcRect.x = spriteComponent.width * curFrame;
        }
    }

};