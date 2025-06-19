#pragma once

#include <SDL3/SDL.h>

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

#include "../Component/HealthComponent.h"
#include "../Component/SpriteComponent.h"
#include "../Component/TransformComponent.h"

class RenderHealthBarSystem : public System
{
public:
    RenderHealthBarSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<HealthComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore, const SDL_FRect& camera) {
        for (auto entity : GetSystemEntities()) {
            const auto& sprite = entity.GetComponent<SpriteComponent>();
            const auto& health = entity.GetComponent<HealthComponent>();
            const auto& transform = entity.GetComponent<TransformComponent>();

            SDL_Color healthBarColor = {255, 255, 255, 255};

            if (health.healthPercentage >= 0 && health.healthPercentage < 40) {
                    // 0-40 = red
                    healthBarColor = {255, 0, 0, 255};
            }
            if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
                // 40-80 = yellow
                healthBarColor = {255, 255, 0, 255};
            }
            if (health.healthPercentage >= 80 && health.healthPercentage <= 100) {
                // 80-100 = green
                healthBarColor = {0, 255, 0, 255};
            }

            float healthWidth = 15;
            float healthHeight = 3;
            float healthPosX = transform.position.x + (sprite.width * transform.scale.x) - camera.x;
            float healthPosY = transform.position.y - camera.y;
            SDL_FRect healthRect = {
                healthPosX,
                healthPosY,
                healthWidth * (static_cast<float>(health.healthPercentage) / 100),
                healthHeight 
            };
            
            SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, healthBarColor.a);
            SDL_RenderFillRect(renderer, &healthRect);

            std::string healthText = std::to_string(health.healthPercentage);

            SDL_Surface* surface = TTF_RenderText_Blended(
                assetStore->GetFont("font-arial-5"), 
                healthText.c_str(), 
                healthText.size(), 
                healthBarColor
            );
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            float width;
            float height;
            SDL_GetTextureSize(texture, &width, &height);

            SDL_FRect dstRect = {
                healthPosX,
                healthPosY + 5,
                width,
                height
            };

            SDL_RenderTexture(renderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }
};