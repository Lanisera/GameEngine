#pragma once

#include <SDL3/SDL.h>

#include <algorithm>

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
        struct renderableEnity {
            SpriteComponent spriteComponent;
            TransformComponent transformComponent;
        };

        std::vector<renderableEnity> renderableEnities;
        for (const auto& entity : GetSystemEntities()) {
            renderableEnities.push_back({entity.GetComponent<SpriteComponent>(), entity.GetComponent<TransformComponent>()});
        }

        std::sort(renderableEnities.begin(), renderableEnities.end(), [](const renderableEnity& a, const renderableEnity& b) {
            return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
        });
        
        for (auto entity :renderableEnities) {
            const auto& spriteComponent = entity.spriteComponent;
            const auto& transformComponent = entity.transformComponent;

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