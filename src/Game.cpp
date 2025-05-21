#include "Game.h"

#include <iostream>
#include <SDL3/SDL.h>

Game::Game() {
    std::cout << "contor " << std::endl;
}

Game::~Game() {
    std::cout << "end" << std::endl;
}

void Game::Initialize() {
    isRunning = true;
    
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
}

void Game::Run() {
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
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

}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Game::Destory() {

}