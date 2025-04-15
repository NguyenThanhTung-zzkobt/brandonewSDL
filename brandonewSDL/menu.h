#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDl_image.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "vector"
#include "string"

enum firstState {
	STATE_MAIN_MENU,
	STATE_INIT_GAME,
	STATE_INGAME,
	STATE_OPTIONS_MENU,
	STATE_OPTIONS_CREDITS,
	STATE_QUIT
};

struct menuUI {
	int menu_select;
	std::vector<std::string> selecting_options;
	std::vector<std::string> selecting_options_option;
	TTF_Font* font;
	SDL_Color text_color;
};

void init_menu_ui(SDL_Renderer* renderer);
void render_menu_ui(SDL_Renderer* renderer);
void update_menu_ui(const SDL_Event* event);
void cleanup_menu_ui();
void render_options_ui(SDL_Renderer* renderer);
void update_options_ui(SDL_Renderer* renderer, SDL_Window* window, const SDL_Event* event);

extern int menu_select;
extern firstState current_game_state;
extern menuUI menu_ui;
extern TTF_Font* g_menu_font;
extern SDL_Color g_color_normal;
extern SDL_Color g_color_selected;