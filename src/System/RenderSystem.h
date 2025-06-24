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
            renderableEnity renderableEntity;
            renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
            renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();

            // Check if the entity sprite is outside the camera view
            bool isOutsideCameraView = (
                renderableEntity.transformComponent.position.x + (renderableEntity.transformComponent.scale.x * renderableEntity.spriteComponent.width) < camera.x ||
                renderableEntity.transformComponent.position.x > camera.x + camera.w ||
                renderableEntity.transformComponent.position.y + (renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.height) < camera.y ||
                renderableEntity.transformComponent.position.y > camera.y + camera.h
            );

            if (isOutsideCameraView) {
                continue;
            }

            renderableEnities.emplace_back(renderableEntity);
        }

        std::sort(renderableEnities.begin(), renderableEnities.end(), [](const renderableEnity& a, const renderableEnity& b) {
            return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
        });
        
        for (auto entity :renderableEnities) {
            const auto& spriteComponent = entity.spriteComponent;
            const auto& transformComponent = entity.transformComponent;

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