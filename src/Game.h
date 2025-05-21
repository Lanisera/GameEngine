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
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
};
