#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_iostream.h"
#include "music.h" 
#include "player.h"
#include "monster1.h"
#include "camera.h"
#include "map.h"
#include <fstream>  
#include <cstring>
#include <vector>


struct SaveData {
    int player_x;
    int player_y;
    float player_hp;
    int player_inventory_size;


    std::vector<int> player_inventory;
    int game_state;
    int current_map_id;
    float camera_x;
    float camera_y;
};
extern std::string filename;

void save_game(const std::string& filename);
void load_game(const std::string& filename, SDL_Renderer* renderer);