#pragma once

#include "../ECS/ECS.h"

#include "../Logger/Logger.h"

#include "../Event/KeyPressedEvent.h"

#include "../EventBus/EventBus.h"

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem() {

    }

    void SubscribeEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        std::string keycode = std::to_string(event.keySymbol);
        std::string keySymbol = std::string(1, event.keySymbol);

        Logger::Info("Pressed Key " + keycode + ": " + keySymbol);
    }
};