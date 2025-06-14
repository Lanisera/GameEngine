#pragma once

#include <SDL3/SDL.h>

#include "../Game/Game.h"

#include "../ECS/ECS.h"

#include "../Component/TransformComponent.h"
#include "../Component/CameraFollowComponent.h"

class CameraMovementSystem : public System
{
public:
    CameraMovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<CameraFollowComponent>();
    }

    void Update(SDL_FRect& cameraRect) {
        for (auto entity : GetSystemEntities()) {
            const auto& transformComponent = entity.GetComponent<TransformComponent>();
            
            float cameraPosX = transformComponent.position.x - Game::windowWidth / 2;
            float cameraPosY = transformComponent.position.y - Game::windowHeight / 2;

            if (cameraPosX <= 0) 
                cameraPosX = 0;
            else if (cameraPosX + Game::windowWidth >= Game::mapWidth) {
                cameraPosX = Game::mapWidth - Game::windowWidth;
            }

            if (cameraPosY <= 0) 
                cameraPosY = 0;
            else if (cameraPosY + Game::windowHeight >= Game::mapHeight) {
                cameraPosY = Game::mapHeight - Game::windowHeight;
            }

            cameraRect.x = cameraPosX;
            cameraRect.y = cameraPosY;
        }
    }
};