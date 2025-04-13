#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include "external/cute_tiled.h"
#include <direct.h> 
#include <windows.h>
#include "camera.h"
#include <vector>
#define TILE_WIDTH 16
#define TILE_HEIGHT 16


typedef struct Texture {
    SDL_Texture* texture;
    int firstgid;
    int tilecount;
    int tileset_width;
    int tileset_height;
    struct Texture* next;
} Texture;

class Map {
public:
    void init_map(SDL_Renderer* renderer);
    void processCollisionLayer(cute_tiled_layer_t* layer);
    int getWidth() const;
    int getHeight() const;
    int getCollisionData(int x, int y) const;

private:
    std::vector<std::vector<int>> collisionData;
};