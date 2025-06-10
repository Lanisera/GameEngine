#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent 
{
    float width;
    float height;
    glm::vec2 offset;
    
    BoxColliderComponent(float width = 0, float height = 0, glm::vec2 offset = glm::vec2(0))
        : width(width), height(height), offset(offset)
    {

    }
};
