#include "ui.h"



static BattleUI battle_ui;
static std::vector<std::string> options = { "Attack", "Defend" , "Run" };
int selected_option = 0;




void draw_health_bar(SDL_Renderer* renderer, int x, int y, int width, int height, float percentage) {
    SDL_FRect bg = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_FRect fg = { x, y, (int)(width * percentage), height };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}

void draw_battle_ui(SDL_Renderer* renderer, const Entity* PLAYER, const Entity* Monster) {

    draw_health_bar(renderer, 50, 50, 50, 20, PLAYER->displayed_hp / PLAYER->max_hp);



    draw_health_bar(renderer, 300, 50, 50, 20, Monster->displayed_hp / Monster->max_hp);
}



void init_battle_ui(SDL_Renderer* renderer) {
    battle_ui.selected_option = 0;
    battle_ui.options = { "Attack", "Skill", "Item", "Flee" };
    battle_ui.text_color = { 255, 255, 255, 255 };


    battle_ui.font = TTF_OpenFont("C:/Users/Admin/Downloads/Open_Sans/static/OpenSans-Regular.ttf", 20);
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
    int y = 160; 

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

    if (event->type == SDL_EVENT_KEY_DOWN) {
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
            if (battle_ui.options[battle_ui.selected_option] == "Attack"){
                Monster.entity.current_hp -= PLAYER.entity.attack_power;
                PLAYER.entity.current_hp -= Monster.entity.attack_power;
                SDL_Log("player hp: %d / %d", PLAYER.current_hp, PLAYER.max_hp);
                SDL_Log("monster hp: %d / %d", Monster.current_hp, Monster.max_hp);

            }

            if (battle_ui.options[battle_ui.selected_option] == "Flee" || PLAYER.entity.current_hp <=0) {
                end_battle_lost();  // Custom function you'll create to end battle
                PLAYER.set_position(18, 107); // Reset to safe zone
            }

            if (Monster.entity.current_hp <= 0) {
                end_battle_won();

            }

            break;
        default:
            break;
        }
    }
}

void cleanup_battle_ui() {
    if (battle_ui.font) {
        TTF_CloseFont(battle_ui.font);
        battle_ui.font = nullptr;
    }
    SDL_DestroyTexture(Monster.texture);
}