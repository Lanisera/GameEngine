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
    Logger::Debug("load succeed");

    sol::optional<sol::table> hasLevel = lua["Level"];
    if (hasLevel == sol::nullopt) {
        sol::error error = result;
        std::string message = error.what();
        Logger::Error("lua dont own Level: " + message);
        return ;
    }
    sol::table level = lua["Level"];

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
            Logger::Debug("Load Asset Count: " + std::to_string(i));
            break;
        }
        sol::table asset = assets[i];

        std::string assetId = asset["id"];
        std::string filePath = asset["file"];

        std::string type = asset["type"];
        if (type == "texture") {
            assetStore->AddTexture(renderer, assetId, filePath);
        }
        else if (type == "font") {
            int fontSize = asset["font_size"];
            assetStore->AddFont(assetId, filePath, asset["font_size"]);
        }
        i++;
    }

    /////////////////////////////////////////////////////////////////
    /// 加载地图
    /////////////////////////////////////////////////////////////////
    sol::optional<sol::table> hasMap = level["tilemap"];
    if (hasMap == sol::nullopt) {
        sol::error error = result;
        std::string message = error.what();
        Logger::Error("lua dont own tilemap: " + message);
        return ;
    }
    sol::table tilemap = level["tilemap"];

    int tilemapRow = tilemap["num_rows"];
    int tilemapCol = tilemap["num_cols"];
    float tileSize = tilemap["tile_size"];
    float tileScale = tilemap["scale"];
    
    Game::mapWidth = static_cast<int>(tilemapCol * tileSize * tileScale);
    Game::mapHeight = static_cast<int>(tilemapRow * tileSize * tileScale);

    std::string mapFile = tilemap["map_file"];
    std::string mapTextureAssetId = tilemap["texture_asset_id"];

    std::fstream tilemapFile;
    tilemapFile.open(mapFile);
    
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
            tile.AddComponent<TransformComponent>(glm::vec2(x * tileSize * tileScale, y * tileSize * tileScale), glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>(tileSize, tileSize, mapTextureAssetId, 0, indexX * tileSize, indexY * tileSize);
        }
    }
    
    tilemapFile.close();

}