#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

#include "../ECS/ECS.h"

#include "../Component/ProjectileEmitterComponent.h"

class RenderGUISystem : public System
{
public:
    RenderGUISystem() = default;

    void Update(SDL_Renderer* renderer, std::unique_ptr<Registry>& registry) {
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn Enemies")) {
            static float enemyPosX = 0;
            static float enemyPosY = 0;

            const char* enemyImage[] = {"image-tank", "image-chopper"};
            static int image_item = 0;

            ImGui::InputFloat("posX", &enemyPosX);
            ImGui::InputFloat("posY", &enemyPosY);
            ImGui::Combo("enemyImage", &image_item, enemyImage, IM_ARRAYSIZE(enemyImage));

            if (ImGui::Button("Spawn Enemy")) {
                Entity enemy = registry->CreateEntity();
                enemy.Group("Enemy");
                enemy.AddComponent<TransformComponent>(glm::vec2(enemyPosX, enemyPosY), glm::vec2(1.0, 1.0), 0.0);
                enemy.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
                enemy.AddComponent<SpriteComponent>(32.0, 32.0, enemyImage[image_item], 1);
                enemy.AddComponent<BoxColliderComponent>(32.0, 32.0);
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), Game::ns * 2, Game::ns * 5, 10, false);
                enemy.AddComponent<HealthComponent>(100);
            }
        }
        ImGui::End();

        // ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};