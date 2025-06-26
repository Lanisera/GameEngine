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

    /////////////////////////////////////////////////////////////////
    /// 加载Entity
    /////////////////////////////////////////////////////////////////
    i = 0;
    sol::optional<sol::table> hasEntities = level["entities"];
    if (hasEntities == sol::nullopt) {
        sol::error error = result;
        std::string message = error.what();
        Logger::Error("lua dont own entities: " + message);
        return ;
    }
    sol::table entities = level["entities"];
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            Logger::Debug("Load entity Count: " + std::to_string(i));
            break;
        }
        sol::table entity = entities[i];

        Entity newEntity = registry->CreateEntity();

        // tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidbodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }
            
            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }
            
            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<Uint64>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * Game::ns,
                    static_cast<Uint64>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * Game::ns,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlledComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }
        }
        i++;
    }
}