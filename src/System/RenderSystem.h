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

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_FRect& camera) {
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

            bool isOutsideCameraView = (
                    entity.transformComponent.position.x + (entity.transformComponent.scale.x * entity.spriteComponent.width) < camera.x ||
                    entity.transformComponent.position.x > camera.x + camera.w ||
                    entity.transformComponent.position.y + (entity.transformComponent.scale.y * entity.spriteComponent.height) < camera.y ||
                    entity.transformComponent.position.y > camera.y + camera.h
                );
            
            if (isOutsideCameraView) continue;

            SDL_FRect dstRect = {
                transformComponent.position.x - camera.x,
                transformComponent.position.y - camera.y,
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