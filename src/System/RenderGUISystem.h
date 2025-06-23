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

    void Update(SDL_Renderer* renderer, std::unique_ptr<Registry>& registry, const SDL_FRect& camera) {
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn Enemies")) {
            static float enemyPosX = 0;
            static float enemyPosY = 0;
            static float scaleX = 1;
            static float scaleY = 1;
            static float rotation = 0;

            static float velocityX = 0;
            static float velocityY = 0;

            const char* enemyImage[] = {"image-tank", "image-chopper"};
            static int image_item = 0;

            static float projAngle = 0;
            static float projSpeed = 0;
            static int projRepeat = 3;
            static int projDuration = 5;

            static int health = 100;

            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("enemyImage", &image_item, enemyImage, IM_ARRAYSIZE(enemyImage));
            }
            ImGui::Spacing();

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputFloat("posX", &enemyPosX);
                ImGui::InputFloat("posY", &enemyPosY);
                ImGui::SliderFloat("scaleX", &scaleX, 1.0, 10.0);
                ImGui::SliderFloat("scaleY", &scaleX, 1.0, 10.0);
                ImGui::SliderAngle("rotation (deg)", &rotation, 0, 360);
            }
            ImGui::Spacing();

            if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputFloat("velocityX", &velocityX);
                ImGui::InputFloat("velocityY", &velocityY);
            }
            ImGui::Spacing();

            // Section to input enemy projectile emitter values
            if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
                ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
                ImGui::InputInt("repeat (sec)", &projRepeat);
                ImGui::InputInt("duration (sec)", &projDuration);
            }
            ImGui::Spacing();

            // Section to input enemy health values
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("%", &health, 0, 100);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Spawn Enemy")) {
                Entity enemy = registry->CreateEntity();
                enemy.Group("Enemy");
                enemy.AddComponent<TransformComponent>(glm::vec2(enemyPosX, enemyPosY), glm::vec2(scaleX, scaleY), glm::degrees(rotation));
                enemy.AddComponent<RigidbodyComponent>(glm::vec2(velocityX, velocityY));
                enemy.AddComponent<SpriteComponent>(32.0, 32.0, enemyImage[image_item], 2);
                enemy.AddComponent<BoxColliderComponent>(32.0, 32.0);
                double projVelocityX = cos(projAngle) * projSpeed;
                double projVelocityY = sin(projAngle) * projSpeed;
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projVelocityX, projVelocityY), Game::ns * projRepeat, Game::ns * projDuration, 10, false);
                enemy.AddComponent<HealthComponent>(health);

                enemyPosX = enemyPosY = rotation = projAngle = 0;
                scaleX = scaleY = 1;
                projRepeat = projDuration = 10;
                projSpeed = 100;
                health = 100;
            }

            // Display a small overlay window to display the map position using the mouse
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
            ImGui::SetNextWindowBgAlpha(0.9f);
            if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
                ImGui::Text(
                    "Map coordinates (x=%.1f, y=%.1f)",
                    ImGui::GetIO().MousePos.x + camera.x,
                    ImGui::GetIO().MousePos.y + camera.y
                );
            }
            ImGui::End();
        }
        ImGui::End();

        // ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};