#include "Game.h"
#include "../ECS/ECS.h"

#include "../System/RenderSystem.h"
#include "../System/MovementSystem.h"

#include "../Component/TransformComponent.h"

#include <iostream>
#include <SDL3/SDL.h>

Game::Game() {
}

Game::~Game() {
}

void Game::Initialize() {
    isRunning = true;
    frameDelay = 1000000000 / FPS;
    
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("GameEngine", 1080, 720, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        std::cout << "Error creating window: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
    }

    registry = std::make_unique<Registry>();
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Setup() {
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(10.0, 10.0));
    tank.AddComponent<SpriteComponent>(20.0, 20.0);
    // tank.RemoveComponent<TransformComponent>();
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type){
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (sdlEvent.key.key == SDLK_ESCAPE)
                    isRunning = false;
                break;
            default:
                break;
        }
    }
}

void Game::Update() {
    /// SDL3 使用ns稳定帧率
    Uint64 timeToWait = frameDelay - (SDL_GetTicksNS() - nsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= frameDelay) {
        SDL_DelayNS(timeToWait);
    }

    double deltaTime = (SDL_GetTicksNS() - nsPreviousFrame) / 1000000000.0;
    // std::cout << deltaTime << std::endl;

    nsPreviousFrame = SDL_GetTicksNS();

    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->Update();
}


void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer);

    SDL_RenderPresent(renderer);
}

void Game::Destory() {

}