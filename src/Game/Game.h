#pragma once
#include <SDL3/SDL.h>
#include <memory>

#include <sol/sol.hpp>

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../AssetStore/AssetStore.h"

class Game {
public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void LoadLevel(int level);
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destory();

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;
    static const Uint64 ns = 1000000000;
private:

    Uint64 FPS = 60;
    Uint64 frameDelay = 0;
    Uint64 nsPreviousFrame = 0;

    bool isDebug;
    bool isRunning;
    SDL_Window *window;
    SDL_FRect cameraRect;
    SDL_Renderer *renderer;
    
    sol::state luaState;
    std::unique_ptr<Registry> registry;
    std::unique_ptr<EventBus> eventBus;
    std::unique_ptr<AssetStore> assetStore;
};
