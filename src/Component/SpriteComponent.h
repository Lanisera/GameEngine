#pragma once

#include <SDL3/SDL.h>

#include "../ECS/ECS.h"

struct SpriteComponent
    : public Component<SpriteComponent>
{
    float width;
    float height;
    int zIndex;
    std::string imageId;
    SDL_FRect srcRect;

    SpriteComponent(float width = 0, float height = 0, std::string imageId = "", int zIndex = 0, float srcRectX = 0, float srcRectY = 0)
        : width(width), height(height), imageId(imageId), zIndex(zIndex)
    {
        srcRect = {
            srcRectX, srcRectY,
            width, height
        };
    }
};