#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

struct LabelTextComponent
{
    glm::vec2 position;
    std::string text;
    std::string assetId;
    SDL_Color color;

    LabelTextComponent(glm::vec2 position = glm::vec2(0), std::string text = "", std::string assetId = "", const SDL_Color color = {0, 0, 0, 255})
        : position(position), text(text), assetId(assetId), color(color)
    {

    }
};
