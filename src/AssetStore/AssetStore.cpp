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
        Logger::Error("Dont load image");
    }

    imageAssets.emplace(assetId, texture);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
    return imageAssets[assetId];
}

void AssetStore::ClearAssets() {
    for (auto image : imageAssets) {
        SDL_DestroyTexture(image.second);
    }
    imageAssets.clear();
}