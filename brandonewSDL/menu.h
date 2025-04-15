#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDl_image.h"
#include "vector"
#include "string"

enum firstState {
	STATE_MAIN_MENU,
	STATE_INIT_GAME,
	STATE_INGAME,
	STATE_QUIT
};

struct menuUI {
	int menu_select;
	std::vector<std::string> selecting_options;
	TTF_Font* font;
	SDL_Color text_color;
};

void init_menu_ui(SDL_Renderer* renderer);
void render_menu_ui(SDL_Renderer* renderer);
void update_menu_ui(const SDL_Event* event);
void cleanup_menu_ui();
void close_game_from_menu();

extern int menu_select;
extern firstState current_game_state;
extern menuUI menu_ui;
extern TTF_Font* g_menu_font;
extern SDL_Color g_color_normal;
extern SDL_Color g_color_selected;