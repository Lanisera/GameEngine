#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "AssetStore.h"
#include "../Logger/Logger.h"

AssetStore::AssetStore() {
    Logger::Info("AssetStore ctor called!");
}

AssetStore::~AssetStore() {
    ClearAssets();
    Logger::Info("AssetStore destor called!");
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());

    if (texture == NULL) {
        Logger::Error("Dont load image " + assetId);
    }

    imageAssets.emplace(assetId, texture);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
    return imageAssets[assetId];
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filePath, float fontSize) {
    TTF_Font* font = TTF_OpenFont(filePath.c_str(), fontSize);
    if (font == NULL) {
        Logger::Error("Dont Load Font");
    }

    fontAssets.emplace(assetId, font);
}

TTF_Font* AssetStore::GetFont(const std::string& assetId) {
    return fontAssets[assetId];
}

void AssetStore::ClearAssets() {
    for (auto image : imageAssets) {
        SDL_DestroyTexture(image.second);
    }
    imageAssets.clear();

    for (auto font : fontAssets) {
        TTF_CloseFont(font.second);
    }
    fontAssets.clear();
}