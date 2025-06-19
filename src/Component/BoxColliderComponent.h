#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent 
{
    float width;
    float height;
    glm::vec2 offset;
    bool isFixed;
    
    BoxColliderComponent(float width = 0, float height = 0, glm::vec2 offset = glm::vec2(0), bool isFixed = false)
        : width(width), height(height), offset(offset), isFixed(isFixed)
    {

    }
};
