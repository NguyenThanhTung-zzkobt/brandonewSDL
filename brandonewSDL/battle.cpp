#include "battle.h"
#include "ui.h"



BattleState current_battle_state = BATTLE_NONE;
Entity* current_enemy = nullptr;

void start_battle(Entity* enemy) {
    current_battle_state = BATTLE_ACTIVE;
    current_enemy = enemy;

    // Optional: pause other systems (like map movement)
    SDL_Log("Battle started!");
}

void update_battle(float delta_time) {
    if (current_battle_state != BATTLE_ACTIVE) return;

    // Turn-based logic, inputs, animations
    // For now, just log for debugging
}

void render_battle(SDL_Renderer* renderer) {
    if (current_battle_state != BATTLE_ACTIVE) return;

    // Render basic UI: enemy/player HP, background, etc.
    draw_battle_ui(renderer, current_enemy); // custom UI drawing
}

void end_battle() {
    current_battle_state = BATTLE_NONE;
    current_enemy = nullptr;
}


bool is_in_battle() {
    return false; 
}