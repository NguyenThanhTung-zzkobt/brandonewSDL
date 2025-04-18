#pragma once

#include "SDL3/SDL.h"
#include "menu.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3/SDL_surface.h"
#include "vector"
#include "string"
#include "time_manager.h"
#include "save.h"


struct pauseUI {
	int pause_select;
	std::vector<std::string> selecting_options;
	std::vector<std::string> PauseMenuButton;
	TTF_Font* pause_font;
	SDL_Color pause_text_color;
};

extern std::vector<std::string> PauseMenuButton; 
extern int pause_select;
extern SDL_Color p_color_normal;
extern SDL_Color p_color_selected;




void init_pause_menu_ui(SDL_Renderer* renderer);
void update_pause_menu_ui(SDL_Event* event);
void render_pause_menu_ui(SDL_Renderer* renderer);
void cleanup_pause_menu_ui();