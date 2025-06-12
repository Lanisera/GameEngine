#pragma once

#include <SDL3/SDL.h>

#include "../EventBus/Event.h"


class KeyPressedEvent : public Event
{
public:
    SDL_Keycode keySymbol;
    
    KeyPressedEvent(SDL_Keycode keySymbol)
        : keySymbol(keySymbol)
    {

    }
};