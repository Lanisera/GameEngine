#pragma once

#include <SDL3/SDL.h>

#include "../ECS/ECS.h"

struct SpriteComponent
    : public Component<SpriteComponent>
{
    float width;
    float height;
    std::string imageId;
    SDL_FRect srcRect;

    SpriteComponent(float width = 0, float height = 0, std::string imageId = "", float srcRectX = 0, float srcRectY = 0)
        : width(width), height(height), imageId(imageId)
    {
        srcRect = {
            srcRectX, srcRectY,
            width, height
        };
    }
};