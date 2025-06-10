#include "Game.h"
#include "../ECS/ECS.h"

#include "../System/RenderSystem.h"
#include "../System/MovementSystem.h"
#include "../System/AnimationSystem.h"

#include "../Component/TransformComponent.h"

#include <fstream>
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
    assetStore = std::make_unique<AssetStore>();
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::LoadLevel(int level) {
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();

    assetStore->AddTexture(renderer, "image-tank", "../assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "image-chopper", "../assets/images/chopper.png");
    assetStore->AddTexture(renderer, "image-jungle", "../assets/tilemaps/jungle.png");

    int tilemapRow = 20;
    int tilemapCol = 25;
    float tileSize = 32.0;
    float tileScale = 2.0;

    std::fstream tilemapFile;
    tilemapFile.open("../assets/tilemaps/jungle.map");
    
    for (int y = 0; y < tilemapRow; y ++) {
        for (int x = 0; x < tilemapCol; x ++) {
            Entity tile = registry->CreateEntity();
            
            char ch;
            tilemapFile.get(ch);
            int indexY = ch - '0';
            tilemapFile.get(ch);
            int indexX = ch - '0';
            tilemapFile.ignore();

            tile.AddComponent<TransformComponent>(glm::vec2(x * tileSize * tileScale, y * tileSize * tileScale), glm::vec2(tileScale, tileScale));
            tile.AddComponent<SpriteComponent>(tileSize, tileSize, "image-jungle", 0, indexX * tileSize, indexY * tileSize);
        }
    }
    
    tilemapFile.close();

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(10.0, 10.0));
    tank.AddComponent<SpriteComponent>(32.0, 32.0, "image-tank", 1);

    Entity chopper = registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(100, 100), glm::vec2(1.0, 1.0));
    chopper.AddComponent<SpriteComponent>(32.0, 32.0, "image-chopper", 1);
    chopper.AddComponent<AnimationComponent>(2, 5);

}

void Game::Setup() {
    LoadLevel(0);
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
    registry->GetSystem<AnimationSystem>().Update();
    registry->Update();
}


void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

    SDL_RenderPresent(renderer);
}

void Game::Destory() {

}