#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "Game.h"
#include "LevelLoader.h"

#include "../Logger/Logger.h"

#include "../Component/HealthComponent.h"
#include "../Component/SpriteComponent.h"
#include "../Component/LabelTextComponent.h"
#include "../Component/AnimationComponent.h"
#include "../Component/TransformComponent.h"
#include "../Component/RigidbodyComponent.h"
#include "../Component/ProjectileComponent.h"
#include "../Component/BoxColliderComponent.h"
#include "../Component/CameraFollowComponent.h"
#include "../Component/ProjectileEmitterComponent.h"
#include "../Component/KeyboardControlledComponent.h"

void LevelLoader::LoadLevel(sol::state& lua, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int levelNumber) {
    std::string filePath = "../assets/scripts/Level" + std::to_string(levelNumber) + ".lua";
    sol::load_result result = lua.load_file(filePath);
    if (!result.valid()) {
        sol::error error = result;
        std::string message = error.what();
        Logger::Error("lua syntax error: " + message);
    }

    lua.script_file(filePath);

    sol::optional<sol::table> hasLevel = lua["Level"];
    if (hasLevel == sol::nullopt) {
        sol::error error = result;
        std::string message = error.what();
        Logger::Error("lua dont own Level: " + message);
        return ;
    }
    sol::table level = lua["level"];

    /////////////////////////////////////////////////////////////////
    /// 加载外部资源(图片，字体，音频等)
    /////////////////////////////////////////////////////////////////
    int i = 0;
    sol::optional<sol::table> hasAssets = level["assets"];
    if (hasAssets == sol::nullopt) {
        sol::error error = result;
        std::string message = error.what();
        Logger::Error("lua dont own assets: " + message);
        return ;
    }
    sol::table assets = level["assets"];
    
    while (true) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        }

        
    }
}