#pragma once

#include "../ECS/ECS.h"

struct SpriteComponent
    : public Component<SpriteComponent>
{
    float width;
    float height;

    SpriteComponent(float width = 0, float height = 0)
        : width(width), height(height)
    {

    }
};