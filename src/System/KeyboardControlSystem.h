#pragma once

#include "../ECS/ECS.h"

#include "../Logger/Logger.h"

#include "../Event/KeyPressedEvent.h"

#include "../EventBus/EventBus.h"

#include "../Component/SpriteComponent.h"
#include "../Component/RigidbodyComponent.h"
#include "../Component/KeyboardControlledComponent.h"

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<KeyboardControlledComponent>();
    }

    void SubscribeEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        for (auto entity : GetSystemEntities()) {
            const auto& keyboardControlledComponent = entity.GetComponent<KeyboardControlledComponent>();
            auto& spriteComponent = entity.GetComponent<SpriteComponent>();
            auto& rigidbodyComponent = entity.GetComponent<RigidbodyComponent>();

            switch (event.keySymbol)
            {
            case SDLK_UP:
                rigidbodyComponent.velocity = keyboardControlledComponent.upVelocity;
                spriteComponent.srcRect.y = spriteComponent.height * 0;
                break;
            case SDLK_RIGHT:
                rigidbodyComponent.velocity = keyboardControlledComponent.rightVelocity;
                spriteComponent.srcRect.y = spriteComponent.height * 1;
                break;
            case SDLK_DOWN:
                rigidbodyComponent.velocity = keyboardControlledComponent.downVelocity;
                spriteComponent.srcRect.y = spriteComponent.height * 2;
                break;
            case SDLK_LEFT:
                rigidbodyComponent.velocity = keyboardControlledComponent.leftVelocity;
                spriteComponent.srcRect.y = spriteComponent.height * 3;
                break;
            default:
                break;
            }
        }
    }

    void Update() {

    }
};