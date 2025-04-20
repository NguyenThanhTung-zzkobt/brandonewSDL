#pragma once
#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>
#include "entity.h"
#include "player.h"
#include "vector"
#include "string"
#include "SDL3_ttf/SDL_ttf.h"
#include "battle.h"
#include "monster1.h"

void draw_battle_ui(SDL_Renderer* renderer, const Entity* PLAYER, const Entity* Monster);

struct BattleUI {
    int selected_option;
    std::vector<std::string> options;
    std::vector<std::string> weapon_options;
    TTF_Font* font;
    SDL_Color text_color;
};

enum BattleUIMode {
    MAIN,
    WEAPON_SELECT,
    ITEM_SELECT
};

void init_battle_ui(SDL_Renderer* renderer);
void render_battle_ui(SDL_Renderer* renderer);
void update_battle_ui(const SDL_Event* event);
void cleanup_battle_ui();
void trigger_weapon_ui(SDL_Renderer* renderer);
void trigger_item_ui(SDL_Renderer* renderer);
void init_weapon_ui(SDL_Renderer* renderer);
void update_weapon_options_ui(SDL_Event* event);
void update_item_options_ui(SDL_Event* event);



extern int selected_option;
extern BattleUIMode current_ui_mode;