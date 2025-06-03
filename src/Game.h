#pragma once
#include <SDL3/SDL.h>

class Game {
public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void ProcessInput();
    void Update();
    void Render();
    void Destory();
private:

    Uint64 FPS = 60;
    Uint64 frameDelay = 0;
    Uint64 nsPreviousFrame = 0;

    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
};
