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

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
        for (auto entity : GetSystemEntities()) {
            const auto& spriteComponent = entity.GetComponent<SpriteComponent>();
            const auto& transformComponent = entity.GetComponent<TransformComponent>();

            SDL_FRect dstRect = {
                transformComponent.position.x,
                transformComponent.position.y,
                spriteComponent.width * transformComponent.scale.x,
                spriteComponent.height * transformComponent.scale.y
            };

            SDL_RenderTextureRotated(
                renderer, 
                assetStore->GetTexture(spriteComponent.imageId), 
                &spriteComponent.srcRect,
                &dstRect,
                transformComponent.rotation, 
                NULL,
                SDL_FLIP_NONE
            );

        }
    }
};