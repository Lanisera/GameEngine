#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

#include "../Component/LabelTextComponent.h"

class RenderTextSystem : public System 
{
public:
    RenderTextSystem() {
        RequireComponent<LabelTextComponent>();
    }

    void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore) {
        for (auto entity : GetSystemEntities()) {
            const auto& labelText = entity.GetComponent<LabelTextComponent>();

            SDL_Surface* surface = TTF_RenderText_Blended(
                assetStore->GetFont(labelText.assetId), 
                labelText.text.c_str(), 
                labelText.text.size(), 
                labelText.color
            );
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            float width;
            float height;
            SDL_GetTextureSize(texture, &width, &height);

            SDL_FRect dstRect = {
                labelText.position.x,
                labelText.position.y,
                width,
                height
            };

            SDL_RenderTexture(renderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }
};