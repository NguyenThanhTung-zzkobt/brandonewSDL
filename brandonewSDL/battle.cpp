#include "battle.h"



BattleState current_battle_state = BATTLE_NONE;
Entity* current_enemy = nullptr;

void start_battle(Monster1* enemy) {
    if (PLAYER.entity.current_hp <= 0) {
        SDL_Log("PLAYER is no longer able to battle ");
        return;
    }
    current_battle_state = BATTLE_ACTIVE;
    current_enemy = &enemy->entity;


   
    if (renderer) {
        init_battle_ui(renderer);
    }
    else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, ". for battle UI initialization!");
    }



    // Optional: pause other systems (like map movement)
    SDL_Log("Battle started!");
}

void update_battle(float delta_time) {
    if (current_battle_state != BATTLE_ACTIVE) return;
    float smoothing = 10.0f;
    PLAYER.entity.displayed_hp += (PLAYER.entity.current_hp - PLAYER.entity.displayed_hp) * smoothing * delta_time;

    if (current_enemy) { 
        current_enemy->displayed_hp += (current_enemy->current_hp - current_enemy->displayed_hp) * smoothing * delta_time;
       
    }
    
    // Turn-based logic, inputs, animations
    // For now, just log for debugging
}

void render_battle(SDL_Renderer* renderer) {
    if (current_battle_state != BATTLE_ACTIVE) return;

    // Render basic UI: enemy/player HP, background, etc.
    //Entity* PLAYER_entity = &entities[find_entity("PLAYER")];
    draw_battle_ui(renderer, &PLAYER.entity, current_enemy); // custom UI drawing
    render_battle_ui(renderer);
    //trigger_weapon_ui(renderer);
}




void end_battle_lost() {
    cleanup_battle_ui();
    current_battle_state = BATTLE_NONE;

    // Reset the triggered flag of the enemy if it exists
    if (current_enemy != nullptr) {
        SDL_Log("current_enemy ptr = %p", (void*)current_enemy);
        for (auto& monster : monsters) {
            SDL_Log("Comparing to monster.entity = %p", (void*)&monster.entity);
            if (&monster.entity == current_enemy) {
                monster.triggered = false;
                if (monster.entity.texture != nullptr) {
                    SDL_DestroyTexture(monster.entity.texture);
                    monster.entity.texture = nullptr;
                    SDL_Log("Monster texture destroyed!");
                }
                break;
            }
        }
        current_enemy = nullptr;
    }
    SDL_Log("Player lost the battle!");
}
void end_battle_won() {

    PLAYER.addItem(1, itemMap);
    int temp = PLAYER.entity.MY_LEVEL;
    PLAYER.entity.MY_LEVEL++;
    if(temp < PLAYER.entity.MY_LEVEL) {
        PLAYER.entity.max_hp = PLAYER.entity.max_hp + 5;
        PLAYER.entity.current_hp = PLAYER.entity.current_hp + PLAYER.entity.MY_LEVEL;
        PLAYER.entity.attack_power = PLAYER.entity.attack_power + 1;
    }
    SDL_Log("Player Created:");
    SDL_Log("  Position: x = %f, y = %f", PLAYER.entity.position.x, PLAYER.entity.position.y);
    SDL_Log("  Level: %d", PLAYER.entity.MY_LEVEL);
    SDL_Log("  Max HP: %f", PLAYER.entity.max_hp);
    SDL_Log("  Current HP: %f", PLAYER.entity.current_hp);
    SDL_Log("  Attack Power: %d", PLAYER.entity.attack_power);
    SDL_Log("  Active Status: Type = %d, Duration = %d, Damage per Turn = %d",
        PLAYER.entity.active_status.type, PLAYER.entity.active_status.duration, PLAYER.entity.active_status.damagePerTurn);
    SDL_Log("  Selected Weapon: %s", PLAYER.entity.selected_weapon.c_str());
    SDL_Log("  Weapons: ");


    cleanup_battle_ui();
    current_battle_state = BATTLE_NONE;

    // Reset the triggered flag of the defeated enemy if it exists
    if (current_enemy != nullptr) {
        for (auto& monster : monsters) {
            if (&monster.entity == current_enemy) {
                monster.triggered = false;
                SDL_Log("Monster trigger reset after defeat!");
                if (monster.entity.texture != nullptr) {
                    SDL_DestroyTexture(monster.entity.texture);
                    monster.entity.texture = nullptr;
                    SDL_Log("Monster texture destroyed!");
                }
                break;
            }
        }
        current_enemy = nullptr;
    }
    SDL_Log("Monster defeated!");
}


bool is_in_battle() {
    return current_battle_state == BATTLE_ACTIVE; 
}


///stauts
void execute_poison_infection(Entity* attacker, Entity* target) {
    if (!target) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: execute_poison_infection received a null target pointer!");
        return;
    }

    target = &PLAYER.entity;
    if (target) {
        if (target->active_status.type == StatusEffect::POISON) {
            // If already poisoned, reset the duration
            target->active_status.duration = 3;
            SDL_Log("Player re-infected with POISON! Duration reset.");
        }
        else {
            // If not already poisoned, apply the effect
            target->active_status.type = StatusEffect::POISON;
            target->active_status.duration = 3;
            target->active_status.damagePerTurn = 15;
            SDL_Log("Player inflicted with POISON!");
        }
    }
    else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: execute_poison_infection target is not a Player!");
    }
}

void execute_damage_from_player(Entity* target) {
    if (!target) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: execute_damge_from_player received a null target pointer!");
        return;
    }


    if (PLAYER.entity.selected_weapon == target->weakness) {
        target->current_hp -= PLAYER.entity.attack_power + PLAYER.entity.attack_power * 0.2;
    }
    else  {
        target->current_hp -= PLAYER.entity.attack_power;
    }
}