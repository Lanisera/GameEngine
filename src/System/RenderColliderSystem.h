#pragma once

#include <SDL3/SDL.h>

#include "../ECS/ECS.h"

#include "../Component/TransformComponent.h"
#include "../Component/BoxColliderComponent.h"

class RenderColliderSystem : public System
{
public:
    RenderColliderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(SDL_Renderer* renderer, const SDL_FRect& cameraRect) {
        for (auto entity : GetSystemEntities()) {
            const auto& transformComponent = entity.GetComponent<TransformComponent>();
            const auto& boxColliderComponent = entity.GetComponent<BoxColliderComponent>();
            
            SDL_FRect boxRect = {
                transformComponent.position.x + boxColliderComponent.offset.x - (boxColliderComponent.isFixed ? 0 : cameraRect.x),
                transformComponent.position.y + boxColliderComponent.offset.y - (boxColliderComponent.isFixed ? 0 : cameraRect.y),
                boxColliderComponent.width,
                boxColliderComponent.height
            };

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderRect(renderer, &boxRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
    }
};