#include "save.h"



std::string filename = "my_save_game.dat";





void save_game(const std::string& filename) {
    SDL_Log("Saving game to %s", filename.c_str());
    
    SaveData data;
    data.player_x = PLAYER.position.x;
    data.player_y = PLAYER.position.y;
    data.player_hp = PLAYER.entity.current_hp;
    data.game_state = get_current_game_state_id();
    data.current_map_id = mapInstance.currentMap;
    data.camera_x = camera.x;
    data.camera_y = camera.y;
    data.player_inventory = PLAYER.inventory;
    data.player_level = PLAYER.entity.MY_LEVEL;


    
    SDL_IOStream* file = SDL_IOFromFile(filename.c_str(), "wb");
    if (!file) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file for writing: %s", SDL_GetError());
        return;
    }

    
    if (SDL_WriteIO(file, &data.player_x, sizeof(data.player_x)) != sizeof(data.player_x) ||
        SDL_WriteIO(file, &data.player_y, sizeof(data.player_y)) != sizeof(data.player_y) ||
        SDL_WriteIO(file, &data.player_hp, sizeof(data.player_hp)) != sizeof(data.player_hp) ||
        SDL_WriteIO(file, &data.game_state, sizeof(data.game_state)) != sizeof(data.game_state) ||
        SDL_WriteIO(file, &data.current_map_id, sizeof(data.current_map_id)) != sizeof(data.current_map_id) ||
        SDL_WriteIO(file, &data.camera_x, sizeof(data.camera_x)) != sizeof(data.camera_x) ||
        SDL_WriteIO(file, &data.camera_y, sizeof(data.camera_y)) != sizeof(data.camera_y)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to write fixed-size data: %s", SDL_GetError());
        SDL_CloseIO(file);
        return;
    }

    if (SDL_WriteIO(file, &data.player_level, sizeof(data.player_level)) != sizeof(data.player_level)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to write player level: %s", SDL_GetError());
        SDL_CloseIO(file);
        return;
    }

    SDL_Log("Saving HP = %f", data.player_hp);
    
    int inventory_size = static_cast<int>(data.player_inventory.size());
    if (SDL_WriteIO(file, &inventory_size, sizeof(inventory_size)) != sizeof(inventory_size)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to write inventory size: %s", SDL_GetError());
        SDL_CloseIO(file);
        return;
    }

    
    for (int item : data.player_inventory) {
        if (SDL_WriteIO(file, &item, sizeof(item)) != sizeof(item)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to write inventory item: %s", SDL_GetError());
            SDL_CloseIO(file);
            return;
        }
    }

    // Close the file
    if (!SDL_CloseIO(file)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to close file: %s", SDL_GetError());
        return;
    }

    SDL_Log("Game saved to %s", filename.c_str());
}

void load_game(const std::string& filename, SDL_Renderer* renderer) {
    SDL_IOStream* file = SDL_IOFromFile(filename.c_str(), "rb");
    if (!file) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_IOFromFile: %s", SDL_GetError());
        return;
    }
    int player_level;
    int player_x, player_y;
    float player_hp;
    int game_state, current_map_id;
    int camera_x, camera_y;
    int inventory_size;
    
    SDL_ReadIO(file, &player_x, sizeof(player_x));
    SDL_ReadIO(file, &player_y, sizeof(player_y));
    SDL_ReadIO(file, &player_hp, sizeof(player_hp));
    SDL_ReadIO(file, &game_state, sizeof(game_state));
    SDL_ReadIO(file, &current_map_id, sizeof(current_map_id));
    SDL_ReadIO(file, &camera_x, sizeof(camera_x));
    SDL_ReadIO(file, &camera_y, sizeof(camera_y));
    SDL_ReadIO(file, &player_level, sizeof(player_level));
    SDL_ReadIO(file, &inventory_size, sizeof(inventory_size));
    
    std::vector<int> loaded_inventory(inventory_size);
    for (int i = 0; i < inventory_size; ++i) {
        SDL_ReadIO(file, &loaded_inventory[i], sizeof(int));
    }
    
    SDL_CloseIO(file);




    switch_game_state((GameState)game_state, renderer);
    mapInstance.loadMap(current_map_id, renderer);
    reload_player_texture(renderer);
    camera.x = camera_x;
    camera.y = camera_y;

    init_monster1(renderer);

    PLAYER.position.x = player_x;
    PLAYER.position.y = player_y;
    PLAYER.entity.current_hp = player_hp;
    SDL_Log("Loaded HP = %f", player_hp); 
    PLAYER.entity.MY_LEVEL = player_level;
    PLAYER.inventory = std::move(loaded_inventory);

    SDL_Log("Game loaded from %s", filename.c_str());
}