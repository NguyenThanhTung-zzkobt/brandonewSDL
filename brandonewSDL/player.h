#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include "camera.h"
#include "time_manager.h"
#include "battle.h"
#include "vector"
#include "string"
#include "map"
#include "animated.h"


class Item;

class Map;

class Player {
public:
    SDL_FPoint position;
    float velocity_x;
    float velocity_y;
    SDL_Texture* texture;
    Entity entity;
    Map* map_ref = nullptr;
    std::string selected_weapon;

    float max_hp;
    float current_hp;
    int attack_power;
    StatusEffect active_status;
    int MY_LEVEL;

    std::map<int, Item> item_data; // Ensure Item is defined
    std::vector<std::string> weapons;
    std::vector<int> inventory;

    Player();
    std::vector<std::string>& getWeapons() {
        return weapons;
    }



    void update(float dt);
    void render(SDL_Renderer* r);
    void setMap(Map* map);
    bool checkCollision(float x, float y);
    void cleanup();
    void set_position(int x, int y);

    void addItem(int itemID, const std::map<int, Item>& itemMap);
    bool removeItem(int itemID, const std::map<int, Item>& itemMap);
    int getInventorySize();
    int getItemCount(int itemID);
    void displayInventory(const std::map<int, Item>& itemMap);
    bool hasItem(int itemID);
    bool canAddItem();
    void clearInventory();
    bool removeItemByIndex(int index, const std::map<int, Item>& itemMap);
    int getItemID(const std::string& itemName, const std::map<int, Item>& itemMap);
    Animated animator;

private:
    static const int MAX_INVENTORY_SIZE = 20;
};

typedef struct {
    float w, h;
} SpriteSize;

void reload_player_texture(SDL_Renderer* renderer);
void init_player(SDL_Renderer* renderer);
extern Player PLAYER;

struct TriggerArea {
    float x;       // X position of the area's top-left corner (map coordinates)
    float y;       // Y position of the area's top-left corner (map coordinates)
    float width;   // Width of the area in pixels
    float height;  // Height of the area in pixels
    bool triggered; // Flag to indicate if the trigger has already occurred
};

void update_game_state(float delta_time);