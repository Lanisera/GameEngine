#include "Game.h"
#include "../ECS/ECS.h"

#include "../System/DamageSystem.h"
#include "../System/RenderSystem.h"
#include "../System/MovementSystem.h"
#include "../System/CollisionSystem.h"
#include "../System/AnimationSystem.h"
#include "../System/RenderGUISystem.h"
#include "../System/RenderTextSystem.h"
#include "../System/RenderColliderSystem.h"
#include "../System/CameraMovementSystem.h"
#include "../System/ProjectileEmitSystem.h"
#include "../System/KeyboardControlSystem.h"
#include "../System/RenderHealthBarSystem.h"
#include "../System/ProjectileLifecycleSystem.h"

#include "../Event/KeyPressedEvent.h"

#include "../Component/ProjectileEmitterComponent.h"
#include "../Component/TransformComponent.h"

#include <fstream>
#include <iostream>
#include <SDL3/SDL.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

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
        Logger::Error("Error initializing SDL: " + std::string(SDL_GetError()));
        return;
    }

    if (!TTF_Init()) {
        Logger::Error("Error initializing TTF: " + std::string(SDL_GetError()));
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

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
    registry->AddSystem<RenderGUISystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<RenderHealthBarSystem>();
    registry->AddSystem<ProjectileLifecycleSystem>();

    assetStore->AddTexture(renderer, "image-tank", "../assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "image-tree", "../assets/images/tree.png");
    assetStore->AddTexture(renderer, "image-chopper", "../assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "image-jungle", "../assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "image-bullet", "../assets/images/bullet.png");

    assetStore->AddFont("font-arial-10", "../assets/fonts/pico8.ttf", 10);
    assetStore->AddFont("font-arial-5", "../assets/fonts/pico8.ttf", 5);

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
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(20.0, 0.0));
    tank.AddComponent<SpriteComponent>(32.0, 32.0, "image-tank", 1);
    tank.AddComponent<BoxColliderComponent>(32.0, 32.0);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), Game::ns * 2, Game::ns * 5, 10, false);
    tank.AddComponent<HealthComponent>(100);

    Entity tank2 = registry->CreateEntity();
    tank2.Group("Enemy");
    tank2.AddComponent<TransformComponent>(glm::vec2(120.0, 500.0), glm::vec2(1.0, 1.0), 0.0);
    tank2.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    tank2.AddComponent<SpriteComponent>(32.0, 32.0, "image-tank", 1);
    tank2.AddComponent<BoxColliderComponent>(32.0, 32.0);
    tank2.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), Game::ns * 2, Game::ns * 5, 10, false);
    tank2.AddComponent<HealthComponent>(100);

    Entity chopper = registry->CreateEntity();
    chopper.Tag("Player");
    chopper.AddComponent<TransformComponent>(glm::vec2(150, 100), glm::vec2(1.0, 1.0));
    chopper.AddComponent<SpriteComponent>(32.0, 32.0, "image-chopper", 1);
    chopper.AddComponent<BoxColliderComponent>(32.0, 32.0);
    chopper.AddComponent<AnimationComponent>(2, 5);
    chopper.AddComponent<RigidbodyComponent>(glm::vec2(0));
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0.0, -100.0), glm::vec2(100.0, 0.0), glm::vec2(0.0, 100.0), glm::vec2(-100.0, 0.0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, Game::ns * 5, 10, true);
    chopper.AddComponent<HealthComponent>(100);

    Entity treeA = registry->CreateEntity();
    treeA.Group("Obstacle");
    treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 495.0), glm::vec2(1.0, 1.0));
    treeA.AddComponent<SpriteComponent>(16.0, 32.0, "image-tree", 1);
    treeA.AddComponent<BoxColliderComponent>(16.0, 32.0);

    Entity treeB = registry->CreateEntity();
    treeB.Group("Obstacle");
    treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 495.0), glm::vec2(1.0, 1.0));
    treeB.AddComponent<SpriteComponent>(16.0, 32.0, "image-tree", 1);
    treeB.AddComponent<BoxColliderComponent>(16.0, 32.0);

    SDL_Color white = {255, 255, 255, 255};
    Entity testText = registry->CreateEntity();
    testText.AddComponent<LabelTextComponent>(glm::vec2(0), "Test Font", "font-arial-10", white);

}

void Game::Setup() {
    LoadLevel(0);
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        ImGui_ImplSDL3_ProcessEvent(&sdlEvent);
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
    registry->GetSystem<MovementSystem>().SubscribeEvents(eventBus);
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
    registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, cameraRect);
    registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore);
    if (isDebug) {
        registry->GetSystem<RenderColliderSystem>().Update(renderer, cameraRect);
        registry->GetSystem<RenderGUISystem>().Update(renderer, registry, cameraRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::Destory() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}