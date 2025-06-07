#pragma once

#include <SDL3/SDL.h>

#include "../ECS/ECS.h"

struct SpriteComponent
    : public Component<SpriteComponent>
{
    float width;
    float height;
    std::string imageId;
    SDL_FRect rect;

    SpriteComponent(float width = 0, float height = 0, std::string imageId = "", float rectX = 0, float rectY = 0)
        : width(width), height(height), imageId(imageId)
    {
        rect = {
            rectX, rectY,
            width, height
        };
    }
};