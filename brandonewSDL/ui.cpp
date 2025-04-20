#include "ui.h"


bool is_player_turn = true;
Turn current_turn = PLAYER_TURN;
int turn_count = 1;
bool poison_applied_this_turn =  false;
bool cursed_applied_this_turn = false; 


static BattleUI battle_ui;
BattleUIMode current_ui_mode = BattleUIMode::MAIN;
int option_selects = 0;

static std::vector<std::string> weapon_options;
int weapon_options_index = 0;

static std::vector<std::string> item_options;
int item_options_index = 0;

std::string getItemName(int itemID, const std::map<int, Item>& itemMap) {
    auto it = itemMap.find(itemID);
    if (it != itemMap.end()) {
        return it->second.name;
    }
    return "Unknown Item"; 
}




bool in_weapon_options_sub_menu = false;
int weapon_options_sub_select = 0;

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

void draw_battle_ui(SDL_Renderer* renderer, const Entity* Player, const Entity* Monster) {
    TTF_Font* font = battle_ui.font; // Use the battle UI font
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Texture* player_name_texture = nullptr;
    SDL_Texture* monster_name_texture = nullptr;
    SDL_FRect player_name_rect;
    SDL_FRect monster_name_rect;
    float w, h;

    Player = &PLAYER.entity;

    if (font) {
        player_name_texture = render_text(renderer, font, Player->name, textColor);
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

    draw_health_bar(renderer, 50, 50, 50, 20, (float)Player->displayed_hp / Player->max_hp);
    draw_health_bar(renderer, 250, 50, 50, 20, (float)Monster->displayed_hp / Monster->max_hp);
}



void init_battle_ui(SDL_Renderer* renderer) {
    battle_ui.selected_option = 0;
    battle_ui.options = { "Attack", "Weapon", "Item", "Flee" };
    battle_ui.text_color = { 255, 255, 255, 255 };


    battle_ui.font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 10);
    if (!battle_ui.font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());

    }
}

void render_battle_ui(SDL_Renderer* renderer) {
    if (!battle_ui.font) {
        SDL_Log("No font loaded. Cannot render battle UI.");
        return;
    }

    int x = 20;
    int y = 100;
    int line_spacing = 30;

    // === MAIN MENU ===
    for (int i = 0; i < battle_ui.options.size(); ++i) {
        const std::string& option = battle_ui.options[i];
        SDL_Surface* surface = TTF_RenderText_Solid(battle_ui.font, option.c_str(), 0, battle_ui.text_color);
        if (!surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_FRect dest = { (float)x, (float)(y + i * line_spacing), (float)surface->w, (float)surface->h };

        if (i == battle_ui.selected_option) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150); // highlight
            SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_RenderTexture(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

    
    if (battle_ui.selected_option == 1 && current_ui_mode == BattleUIMode::WEAPON_SELECT) {
        int submenu_x = x + 150;  // Shift to the right
        int submenu_y = y + (0 * line_spacing);  // Align with "Attack"
        if (weapon_options.empty()) {
            weapon_options = PLAYER.getWeapons(); // Copy the weapons from the player
            if (weapon_options.empty())
            {
                SDL_Log("weapon_options is empty");
                return;
            }
        }
        for (int i = 0; i < weapon_options.size(); ++i) {
            const std::string& weapon = weapon_options[i];
            SDL_Surface* surface = TTF_RenderText_Solid(battle_ui.font, weapon.c_str(), 0, battle_ui.text_color);
            if (!surface) continue;

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                SDL_DestroySurface(surface);
                continue;
            }

            SDL_FRect dest = { (float)submenu_x, (float)(submenu_y + i * line_spacing), (float)surface->w, (float)surface->h };

            if (i == weapon_options_index) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
                SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
                SDL_RenderFillRect(renderer, &highlight);
            }

            SDL_RenderTexture(renderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
            SDL_DestroySurface(surface);
        }
    }


    if (battle_ui.selected_option == 2 && current_ui_mode == BattleUIMode::ITEM_SELECT) {
        item_options.clear();
        for (int itemID : PLAYER.inventory) {
            item_options.push_back(getItemName(itemID, itemMap));
        }
        if (item_options.empty()) {
            SDL_Log("item_options is empty");
            return; // or display a message
        }

        int x = 70;
        int y = 100;
        SDL_Log("item_options.size(): %zu", item_options.size());
        for (size_t i = 0; i < item_options.size(); ++i) {
            const std::string& option = item_options[i];
            SDL_Log("Rendering option: %s at index %zu", option.c_str(), i);

            SDL_Surface* surface = TTF_RenderText_Solid(battle_ui.font, option.c_str(),0, battle_ui.text_color);
            if (!surface) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_RenderText_Solid Error: %s", SDL_GetError());
                continue;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
                SDL_DestroySurface(surface);
                continue;
            }

            SDL_FRect dest = { (float)x, (float)(y + i * 30), (float)surface->w, (float)surface->h };
            SDL_Log("Rendering at: x=%f, y=%f, w=%f, h=%f", dest.x, dest.y, dest.w, dest.h);

            if (i == item_options_index) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
                SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
                SDL_RenderFillRect(renderer, &highlight);
                SDL_Log("Highlighting option: %s", option.c_str());
            }

            SDL_RenderTexture(renderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
            SDL_DestroySurface(surface);
        }
        SDL_Log("Items rendered");
    }
}


void update_battle_ui(const SDL_Event* event) {
    if (!is_in_battle() || current_enemy == nullptr || PLAYER.entity.current_hp <= 0) {
        return;
    }

    if (current_turn == PLAYER_TURN) {

       
        if (PLAYER.entity.active_status.type == StatusEffect::POISON && PLAYER.entity.active_status.duration > 0 && !poison_applied_this_turn) {
            poison_applied_this_turn = true;
            PLAYER.entity.current_hp -= PLAYER.entity.active_status.damagePerTurn;
            SDL_Log("Player takes %d poison damage!", PLAYER.entity.active_status.damagePerTurn);
            PLAYER.entity.active_status.duration--;
            if (PLAYER.entity.active_status.duration == 0) {
                PLAYER.entity.active_status.type = StatusEffect::NONE;
                SDL_Log("Poison effect on player has ended.");
            }
        }
        if (PLAYER.entity.active_status.type == StatusEffect::CURSED && PLAYER.entity.active_status.duration > 0 && !cursed_applied_this_turn) {
            cursed_applied_this_turn = true;
            PLAYER.entity.active_status.cursed_count_down++;
            PLAYER.entity.active_status.duration--;
            switch (PLAYER.entity.active_status.cursed_count_down) { 
            case 1:
                SDL_Log("YOU FEEL A CHILL DOWN THE SPINE...");
                break;
            case 2:
                SDL_Log("A BURNING FIRE IS STARTED WITHIN YOUR LUNGS...");
                break;
            case 3:
                PLAYER.entity.current_hp = 1;
                SDL_Log("YOU ARE BARELY ALIVE!!!");
                break;
            }
            if (PLAYER.entity.active_status.duration == 0) {
                PLAYER.entity.active_status.cursed_count_down = 0;
                PLAYER.entity.active_status.type = StatusEffect::NONE;
                SDL_Log("CURSED effect on player has ended.");
            }
        }


        if (event->type == SDL_EVENT_KEY_DOWN) {
            SDL_Log("current battle ui %d", current_ui_mode);
            if (current_ui_mode == BattleUIMode::MAIN) {
               

                switch (event->key.key) {
                case SDLK_UP:
                    battle_ui.selected_option = (battle_ui.selected_option - 1 + battle_ui.options.size()) % battle_ui.options.size();
                    break;
                case SDLK_DOWN:
                    battle_ui.selected_option = (battle_ui.selected_option + 1) % battle_ui.options.size();
                    break;
                case SDLK_RIGHT:
                    if (battle_ui.selected_option == 1) { // Weapon option
                        current_ui_mode = BattleUIMode::WEAPON_SELECT;
                        weapon_options_index = 0;

                    }
                    if (battle_ui.selected_option == 2) { // Weapon option
                        current_ui_mode = BattleUIMode::ITEM_SELECT;
                        
                        item_options_index = 0;
                        //trigger_item_ui(renderer);

                    }
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                   
                    SDL_Log("You selected: %s", battle_ui.options[battle_ui.selected_option].c_str());
                    if(battle_ui.options[battle_ui.selected_option] == "Attack") {
                        execute_damage_from_player(current_enemy);

                        if (current_enemy && current_enemy->current_hp <= 0) {
                            end_battle_won(current_enemy);
                            return; // Early exit to avoid using deleted enemy
                        }

                        current_turn = MONSTER_TURN;
                        turn_count++;
                        break;
                    }

                    if (battle_ui.options[battle_ui.selected_option] == "Weapon") {

                        //trigger_weapon_ui(renderer); 
                        break;
                    }

                    if (battle_ui.options[battle_ui.selected_option] == "Item") {
                        break;
                    }
                    if (battle_ui.options[battle_ui.selected_option] == "Flee" ) {
                        end_battle_lost(current_enemy);
                        ///LAYER.entity.set_position(18, 107);
                        break;
                    }



    
                    break;
                default:
                    break;
                }
            }
        }

    }

    
    if (current_turn == MONSTER_TURN && current_enemy != nullptr) {
        BattleAction monster_action = choose_action(current_enemy);
        SDL_Log("Chosen action: %d", monster_action);
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
        case SOUL_STEAL:
            if (current_enemy != nullptr) { // Add null check here
                PLAYER.entity.current_hp -= current_enemy->attack_power;
                current_enemy->current_hp += current_enemy->attack_power;
                SDL_Log("Monster used SOUL_STEAL!");
            }
            else {
                SDL_Log("Error: Cannot attack player, current_enemy is null!");
            }
            break;
        case CURSED:
            if (current_enemy != nullptr) { // Add null check here
                execute_cursed(current_enemy, &(PLAYER.entity));
                SDL_Log("Monster used CURSED!");
            }
            else {
                SDL_Log("Error: Cannot use poison, current_enemy is null!");
            }
            break;
        default:
            break;
        }
        current_turn = PLAYER_TURN;
        poison_applied_this_turn = false;
        cursed_applied_this_turn = false;
        turn_count++;
    }
    if (PLAYER.entity.current_hp <= 0) {
        end_battle_lost(current_enemy);
        return;
    }
}

void cleanup_battle_ui() {
    if (battle_ui.font) {
        TTF_CloseFont(battle_ui.font);
        battle_ui.font = nullptr;
    }
}



void trigger_weapon_ui(SDL_Renderer* renderer) {
    SDL_Log("trigger_weapon_ui called");
    // Initialize the static vector only once
    if (weapon_options.empty()) {
        weapon_options = PLAYER.getWeapons(); // Copy the weapons from the player
        if (weapon_options.empty())
        {
            SDL_Log("weapon_options is empty");
            return;
        }
    }

    int x = 70;
    int y = 100;
    SDL_Log("weapon_options.size(): %zu", weapon_options.size()); //check size
    for (size_t i = 0; i < weapon_options.size(); ++i) { // Use the static vector
        const std::string& option = weapon_options[i];
        SDL_Log("Rendering option: %s at index %zu", option.c_str(), i); // Log each option

        SDL_Surface* surface = TTF_RenderText_Solid(battle_ui.font, option.c_str(), 0, battle_ui.text_color);
        if (!surface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_RenderText_Solid Error: %s", SDL_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_FRect dest = { (float)x, (float)(y + i * 30), (float)surface->w, (float)surface->h };
        SDL_Log("Rendering at: x=%f, y=%f, w=%f, h=%f", dest.x, dest.y, dest.w, dest.h);

        if (i == weapon_options_index) { // Use the static index
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
            SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
            SDL_RenderFillRect(renderer, &highlight);
            SDL_Log("Highlighting option: %s", option.c_str());
        }

        SDL_RenderTexture(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
    SDL_Log("Weapons rendered");
    //SDL_Log("weapon_options_index: %d", weapon_options_index); //for debug
}   


void update_weapon_options_ui(SDL_Event* event) {
    if (!event || current_ui_mode != BattleUIMode::WEAPON_SELECT) return;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            SDL_Log("current battle ui %d", current_ui_mode);
        case SDLK_DOWN:
            weapon_options_index = (weapon_options_index + 1) % weapon_options.size();
            break;
        case SDLK_UP:
            weapon_options_index = (weapon_options_index - 1 + weapon_options.size()) % weapon_options.size();
            break;
        case SDLK_LEFT:
            if (battle_ui.selected_option == 1) {

                current_ui_mode = BattleUIMode::MAIN;
                //weapon_options.clear();
            }
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            PLAYER.selected_weapon = weapon_options[weapon_options_index];
            SDL_Log("Selected weapon: %s", PLAYER.selected_weapon.c_str());

            // Return to main menu after choosing
            current_ui_mode = BattleUIMode::MAIN;
            //weapon_options.clear();
            break;
        case SDLK_ESCAPE:
            current_ui_mode = BattleUIMode::MAIN;
            weapon_options.clear();
            break;
        }
    }
}

void update_item_options_ui(SDL_Event* event) {
    if (!event || current_ui_mode != BattleUIMode::ITEM_SELECT) return;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_DOWN:
            if (item_options.size() > 0) {
                item_options_index = (item_options_index + 1) % item_options.size();
            }
            break;
        case SDLK_UP:
            if (item_options.size() > 0) {
                item_options_index = (item_options_index - 1 + item_options.size()) % item_options.size();
            }
            break;
        case SDLK_LEFT:
            if (battle_ui.selected_option == 2) {
                current_ui_mode = BattleUIMode::MAIN;
            }
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (item_options.size() > 0) {
                //SDL_Log("Used item: %s", item_options[item_options_index]);
                int itemID = PLAYER.getItemID(item_options[item_options_index], itemMap);
                if (itemID != -1)
                {
                    SDL_Log("Used item: %s", item_options[item_options_index]);
                    itemUsage(itemID, itemMap);
                }
                else
                {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Item ID not found.");
                }
            }
            break;
        case SDLK_ESCAPE:
            current_ui_mode = BattleUIMode::MAIN;
            break;
        }
    }
}

