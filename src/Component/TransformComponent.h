#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"

struct TransformComponent 
{
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;

    TransformComponent(glm::vec2 position = glm::vec2(0.0, 0.0), 
                        glm::vec2 scale = glm::vec2(1.0, 1.0),
                        double rotation = 0.0)
        : position(position), scale(scale), rotation(rotation)
    {
        
    }
};