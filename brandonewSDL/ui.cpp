#include "ui.h"


bool is_player_turn = true;
Turn current_turn = PLAYER_TURN;
int turn_count = 1;

static BattleUI battle_ui;

int selected_option = 0;


SDL_Texture* render_text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(),0, color);
    if (!surface) {
        SDL_Log("Failed to render text: %s", SDL_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

void draw_health_bar(SDL_Renderer* renderer, int x, int y, int width, int height, float percentage) {
    SDL_FRect bg = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_FRect fg = { x, y, (int)(width * percentage), height };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}

void draw_battle_ui(SDL_Renderer* renderer, const Entity* PLAYER, const Entity* Monster) {
    TTF_Font* font = battle_ui.font; // Use the battle UI font
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Texture* player_name_texture = nullptr;
    SDL_Texture* monster_name_texture = nullptr;
    SDL_FRect player_name_rect;
    SDL_FRect monster_name_rect;
    float w, h;

    if (font) {
        player_name_texture = render_text(renderer, font, PLAYER->name, textColor);
        monster_name_texture = render_text(renderer, font, Monster->name, textColor);

        if (player_name_texture) {
            SDL_GetTextureSize(player_name_texture, &w, &h);
            player_name_rect.w = w;
            player_name_rect.h = h;
            player_name_rect.x = 50;
            player_name_rect.y = 20;
            SDL_RenderTexture(renderer, player_name_texture, NULL, &player_name_rect);
            SDL_DestroyTexture(player_name_texture);
        }

        if (monster_name_texture) {
            SDL_GetTextureSize(monster_name_texture, &w, &h);
            monster_name_rect.w = w;
            monster_name_rect.h = h;
            monster_name_rect.x = 250;
            monster_name_rect.y = 20;
            SDL_RenderTexture(renderer, monster_name_texture, NULL, &monster_name_rect);
            SDL_DestroyTexture(monster_name_texture);
        }
    }

    draw_health_bar(renderer, 50, 50, 50, 20, (float)PLAYER->current_hp / PLAYER->max_hp);
    draw_health_bar(renderer, 250, 50, 50, 20, (float)Monster->displayed_hp / Monster->max_hp);
}



void init_battle_ui(SDL_Renderer* renderer) {
    battle_ui.selected_option = 0;
    battle_ui.options = { "Attack", "Weapon" , "Skill", "Item", "Flee" };
    battle_ui.text_color = { 255, 255, 255, 255 };


    battle_ui.font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 20);
    if (!battle_ui.font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());

    }
}

void render_battle_ui(SDL_Renderer* renderer) {
    if (!battle_ui.font) {
        SDL_Log("No font loaded. Cannot render battle UI.");
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); 
    SDL_FRect panel = { 10, 150, 200, 150 }; 
    SDL_RenderFillRect(renderer, &panel);
    int x = 20;
    int y = 100; 

    for (int i = 0; i < battle_ui.options.size(); ++i) {
        const std::string& option = battle_ui.options[i];

        SDL_Surface* surface = TTF_RenderText_Solid(battle_ui.font, option.c_str(),0, battle_ui.text_color);
        if (!surface) {
            SDL_Log("Failed to render text: %s", SDL_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_FRect dest = { (float)x, (float)(y + i * 30), (float)surface->w, (float)surface->h };

        if (i == battle_ui.selected_option) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150); // yellow highlight
            SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_RenderTexture(renderer, texture, NULL, &dest);

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
}

void update_battle_ui(const SDL_Event* event) {
    if (!is_in_battle() || current_enemy == nullptr || PLAYER.entity.current_hp <= 0) {
        return;
    }

    if (current_turn == PLAYER_TURN) {
        if (event->type == SDL_EVENT_KEY_DOWN) {
            if (PLAYER.entity.active_status.type == StatusEffect::POISON && PLAYER.entity.active_status.duration > 0) {
                PLAYER.entity.current_hp -= PLAYER.entity.active_status.damagePerTurn;
                SDL_Log("Player takes %d poison damage!", PLAYER.entity.active_status.damagePerTurn);
                PLAYER.entity.active_status.duration--; 
                if (PLAYER.entity.active_status.duration == 0) {
                    PLAYER.entity.active_status.type = StatusEffect::NONE; 
                    SDL_Log("Poison effect on player has ended.");
                }
            }
            switch (event->key.key) {
            case SDLK_UP:
                battle_ui.selected_option = (battle_ui.selected_option - 1 + battle_ui.options.size()) % battle_ui.options.size();
                break;
            case SDLK_DOWN:
                battle_ui.selected_option = (battle_ui.selected_option + 1) % battle_ui.options.size();
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                SDL_Log("You selected: %s", battle_ui.options[battle_ui.selected_option].c_str());
                if (battle_ui.options[battle_ui.selected_option] == "Attack") {
                    current_enemy->current_hp -= PLAYER.entity.attack_power;
                   
                    

                }

                if (battle_ui.options[battle_ui.selected_option] == "Flee" || PLAYER.entity.current_hp <= 0) {
                    end_battle_lost();  // Custom function you'll create to end battle
                    PLAYER.entity.set_position(18, 107); // Reset to safe zone
                    break;
                }

                if (current_enemy->current_hp <= 0) {
                    end_battle_won();
                    break;

                }
                current_turn = MONSTER_TURN;
                turn_count++;
                break;
            default:
                break;
            }
        }

    }
    else if (current_turn == MONSTER_TURN && current_enemy != nullptr) {
        BattleAction monster_action = choose_action();
        switch (monster_action) {
        case NORMAL_ATTACK:
            if (current_enemy != nullptr) { // Add null check here

                PLAYER.entity.current_hp -= current_enemy->attack_power;
                SDL_Log("Monster used NORMAL_ATTACK!");
            }
            else {
                SDL_Log("Error: Cannot attack player, current_enemy is null!");
            }
            break;
        case SPIKE_ATTACK:
            if (current_enemy != nullptr) { // Add null check here
                PLAYER.entity.current_hp -= current_enemy->attack_power + 5;
                SDL_Log("Monster used SPIKE_ATTACK!");
            }
            else {
                SDL_Log("Error: Cannot attack player, current_enemy is null!");
            }
            break;
        case POISON_INFECTION:
            if (current_enemy != nullptr) { // Add null check here
                execute_poison_infection(current_enemy, &(PLAYER.entity));
                SDL_Log("Monster used POISON_INFECTION!");
            }
            else {
                SDL_Log("Error: Cannot use poison, current_enemy is null!");
            }
            break;
        default:
            break;
        }
        current_turn = PLAYER_TURN;

        turn_count++;
    }

}

void cleanup_battle_ui() {
    if (battle_ui.font) {
        TTF_CloseFont(battle_ui.font);
        battle_ui.font = nullptr;
    }
}