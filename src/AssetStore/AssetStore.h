#pragma once

#include <map>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class AssetStore
{
public:
    AssetStore();
    ~AssetStore();

    void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath);
    SDL_Texture* GetTexture(const std::string& assetId);

    void AddFont(const std::string& asssetId, const std::string& filePath, float fontSize);
    TTF_Font* GetFont(const std::string& assetId);

    void ClearAssets();

private:
    std::map<std::string, SDL_Texture*> imageAssets;
    std::map<std::string, TTF_Font*> fontAssets;
};