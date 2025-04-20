
#pragma once
#include <cstring>
#include <vector>
#include <string>
#include <SDL3/SDL.h>

#define MAX_ENTITIES 100
#define MAX_NAME_LENGTH 64

struct StatusEffect {
    enum Type {
        NONE,
        POISON

    };

    Type type = NONE;
    int duration = 0;
    int damagePerTurn = 0;
};

class Entity { // Changed from struct to class
public:
    char name[MAX_NAME_LENGTH];
    SDL_FPoint position;
    float detection_radius;
    bool triggered;
    int MY_LEVEL;

    float max_hp;
    float current_hp;
    int attack_power;
    StatusEffect active_status;
    std::string weakness;
    std::string selected_weapon;
    std::vector<int> inventory;

    float displayed_hp;

    virtual ~Entity() {};

    void (*cleanup)(void) = nullptr; // Initialize function pointers to nullptr
    void (*handle_events)(SDL_Event*) = nullptr;
    void (*update)(float) = nullptr;
    void (*render)(SDL_Renderer*) = nullptr;
    void (*set_position)(int x, int y) = nullptr;
    SDL_Texture* texture = nullptr;
};

extern Entity entities[MAX_ENTITIES];
extern int entities_count;

void create_entity(Entity entity);
void destroy_entity(int index);
void swap_entities(int index1, int index2);
int find_entity(const char* name);