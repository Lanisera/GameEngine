#include "Game.h"
#include "../ECS/ECS.h"

#include "../System/DamageSystem.h"
#include "../System/RenderSystem.h"
#include "../System/MovementSystem.h"
#include "../System/CollisionSystem.h"
#include "../System/AnimationSystem.h"
#include "../System/RenderColliderSystem.h"
#include "../System/CameraMovementSystem.h"
#include "../System/ProjectileEmitSystem.h"
#include "../System/KeyboardControlSystem.h"
#include "../System/ProjectileLifecycleSystem.h"

#include "../Event/KeyPressedEvent.h"

#include "../Component/ProjectileEmitterComponent.h"
#include "../Component/TransformComponent.h"

#include <fstream>
#include <iostream>
#include <SDL3/SDL.h>

int Game::windowWidth = 0;
int Game::windowHeight = 0;
int Game::mapWidth = 0;
int Game::mapHeight = 0;

Game::Game() {
}

Game::~Game() {
}

void Game::Initialize() {
    isDebug = false;
    isRunning = true;
    frameDelay = 1000000000 / FPS;

    windowWidth = 1080;
    windowHeight = 720;

    cameraRect.w = static_cast<float>(windowWidth);
    cameraRect.h = static_cast<float>(windowHeight);
    
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("GameEngine", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        std::cout << "Error creating window: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
    }

    registry = std::make_unique<Registry>();
    eventBus = std::make_unique<EventBus>();
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
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<ProjectileLifecycleSystem>();

    assetStore->AddTexture(renderer, "image-tank", "../assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "image-chopper", "../assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "image-jungle", "../assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "image-bullet", "../assets/images/bullet.png");

    int tilemapRow = 20;
    int tilemapCol = 25;
    float tileSize = 32.0;
    float tileScale = 2.0;

    mapWidth = static_cast<int>(tilemapCol * tileSize * tileScale);
    mapHeight = static_cast<int>(tilemapRow * tileSize * tileScale);

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
            
            tile.Group("tile");
            tile.AddComponent<TransformComponent>(glm::vec2(x * tileSize * tileScale, y * tileSize * tileScale), glm::vec2(tileScale, tileScale));
            tile.AddComponent<SpriteComponent>(tileSize, tileSize, "image-jungle", 0, indexX * tileSize, indexY * tileSize);
        }
    }
    
    tilemapFile.close();

    Entity tank = registry->CreateEntity();
    tank.Group("Enemy");
    tank.AddComponent<TransformComponent>(glm::vec2(0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    tank.AddComponent<SpriteComponent>(32.0, 32.0, "image-tank", 1);
    tank.AddComponent<BoxColliderComponent>(32.0, 32.0);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), Game::ns * 2, Game::ns * 5, 10, false);

    Entity tank2 = registry->CreateEntity();
    tank2.Group("Enemy");
    tank2.AddComponent<TransformComponent>(glm::vec2(100.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank2.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    tank2.AddComponent<SpriteComponent>(32.0, 32.0, "image-tank", 1);
    tank2.AddComponent<BoxColliderComponent>(32.0, 32.0);
    tank2.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), Game::ns * 2, Game::ns * 5, 10, false);

    Entity chopper = registry->CreateEntity();
    chopper.Tag("Player");
    chopper.AddComponent<TransformComponent>(glm::vec2(100, 100), glm::vec2(1.0, 1.0));
    chopper.AddComponent<SpriteComponent>(32.0, 32.0, "image-chopper", 1);
    chopper.AddComponent<BoxColliderComponent>(32.0, 32.0);
    chopper.AddComponent<AnimationComponent>(2, 5);
    chopper.AddComponent<RigidbodyComponent>(glm::vec2(0));
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0.0, -100.0), glm::vec2(100.0, 0.0), glm::vec2(0.0, 100.0), glm::vec2(-100.0, 0.0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, Game::ns * 5, 10, true);

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
                if (sdlEvent.key.key == SDLK_D)
                    isDebug = !isDebug;
                eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.key);
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

    eventBus->Reset();
    registry->GetSystem<DamageSystem>().SubscribeEvents(eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeEvents(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SubscribeEvents(eventBus);

    registry->Update();

    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<CameraMovementSystem>().Update(cameraRect);
    registry->GetSystem<ProjectileEmitSystem>().Update(registry);
    registry->GetSystem<ProjectileLifecycleSystem>().Update(registry);
}


void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore, cameraRect);
    if (isDebug) {
        registry->GetSystem<RenderColliderSystem>().Update(renderer, cameraRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::Destory() {

}