#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "vector"
#include "string"

enum firstState {
	STATE_MAIN_MENU,
	STATE_INIT_GAME,
	STATE_INGAME
};

struct menuUI {
	int select;
	std::vector<std::string> selecting_options;
	TTF_Font* font;
	SDL_Color text_color;
};

void init_menu_ui(SDL_Renderer* renderer);
void render_menu_ui(SDL_Renderer* renderer);
void update_menu_ui(const SDL_Event* event);
void cleanup_menu_ui();

extern int selecting_option;

extern firstState current_game_state;