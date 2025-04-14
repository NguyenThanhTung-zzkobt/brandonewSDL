#include "battle.h"
#include "ui.h"



BattleState current_battle_state = BATTLE_NONE;
Entity* current_enemy = nullptr;

void start_battle(Entity* enemy) {
    current_battle_state = BATTLE_ACTIVE;
    current_enemy = enemy;

    init_battle_ui(SDL_GetRenderer(SDL_GetWindowFromID(1)));


    // Optional: pause other systems (like map movement)
    SDL_Log("Battle started!");
}

void update_battle(float delta_time) {
    if (current_battle_state != BATTLE_ACTIVE) return;
    float smoothing = 10.0f;
    PLAYER.entity.displayed_hp += (PLAYER.entity.current_hp - PLAYER.entity.displayed_hp) * smoothing * delta_time;

    Monster.entity.displayed_hp += (Monster.entity.current_hp - Monster.entity.displayed_hp) * smoothing * delta_time;
  

    SDL_Log(" player: %.2f , mosnter %.2f", PLAYER.entity.displayed_hp, Monster.entity.displayed_hp);
    
    // Turn-based logic, inputs, animations
    // For now, just log for debugging
}

void render_battle(SDL_Renderer* renderer) {
    if (current_battle_state != BATTLE_ACTIVE) return;

    // Render basic UI: enemy/player HP, background, etc.
    Entity* PLAYER_entity = &entities[find_entity("PLAYER")];
    draw_battle_ui(renderer, &PLAYER.entity, current_enemy); // custom UI drawing
    render_battle_ui(renderer);
}

void end_battle_lost() {
    cleanup_battle_ui();
    current_battle_state = BATTLE_NONE;
    current_enemy = nullptr;
    SDL_Log("Player fled the battle!");
}
void end_battle_won() {
    cleanup_battle_ui();
    current_battle_state = BATTLE_NONE;
    current_enemy = nullptr;
    SDL_Log("Monster defeated!");
}


bool is_in_battle() {
    return current_battle_state == BATTLE_ACTIVE; 
}
