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
    TTF_Font* font;
    SDL_Color text_color;
};

void init_battle_ui(SDL_Renderer* renderer);
void render_battle_ui(SDL_Renderer* renderer);
void update_battle_ui(const SDL_Event* event);
void cleanup_battle_ui();

extern int selected_option;