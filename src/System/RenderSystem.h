#pragma once

#include <SDL3/SDL.h>

#include "../ECS/ECS.h"
#include "../Component/SpriteComponent.h"
#include "../Component/TransformComponent.h"

class RenderSystem : public System
{
public:
    RenderSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Renderer* renderer ) {
        for (auto entity : GetSystemEntities()) {
            const auto& spriteComponent = entity.GetComponent<SpriteComponent>();
            const auto& transformComponent = entity.GetComponent<TransformComponent>();

            SDL_FRect rect = {
                transformComponent.position.x,
                transformComponent.position.y,
                spriteComponent.width,
                spriteComponent.height
            };

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);

        }
    }
};