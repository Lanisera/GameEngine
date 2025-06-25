#pragma once

#include <sol/sol.hpp>

class LevelLoader 
{
public:
    LevelLoader() = default;
    ~LevelLoader() = default;
    void LoadLevel(sol::state& lua, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int level);
};