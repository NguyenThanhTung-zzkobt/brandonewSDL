#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDl_image.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3/SDL_surface.h"
#include "vector"
#include "string"
#include "map"


#include "time_manager.h"
#include "music.h"
#include "save.h"


class Item;

enum GameState {
	STATE_MAIN_MENU,
	STATE_INIT_GAME,
	STATE_INGAME,
	STATE_OPTIONS_MENU,
	STATE_CREDITS,
	STATE_PAUSE_MENU,
	STATE_QUIT,
	END_GAME_WON,
	END_GAME_LOST
};

enum OptionsOrigin {
	OPTIONS_FROM_MAIN_MENU,
	OPTIONS_FROM_PAUSE_MENU
};

enum InGameState {
	NORMAL,
	INVENTORY
};
extern InGameState in_game_state;


extern OptionsOrigin options_origin;








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
void init_options_ui(SDL_Renderer* renderer);
void render_options_ui(SDL_Renderer* renderer);
void update_options_ui(SDL_Renderer* renderer, SDL_Window* window, const SDL_Event* event);
void render_credits_ui(SDL_Renderer* renderer);
void update_credits_ui(const SDL_Event* event);



extern int menu_select;
extern GameState current_game_state;
extern menuUI menu_ui;
extern TTF_Font* g_menu_font;
extern SDL_Color g_color_normal;
extern SDL_Color g_color_selected;
extern bool is_starting_new_game;
extern int get_current_game_state_id();
extern void switch_game_state(GameState new_state, SDL_Renderer* renderer);


extern bool menu_ui_initialized;
extern bool option_menu_ui_initialized;




class InventoryEntity {
public:
	SDL_Texture* texture;  
	SDL_FRect frect;           
	TTF_Font* font;          
	int selectedItemIndex;
	bool showSubScreen = false;
	int selectedItemID = -1;


	InventoryEntity();
	~InventoryEntity();
	void init(SDL_Renderer* renderer, std::map<int, Item>& itemMap);
	void render(SDL_Renderer* renderer);
	void cleanup();
	void update(float dt); 
	void handle_events(SDL_Event* event); 
	void renderSubScreen(SDL_Renderer* renderer);

};

void render_end_game(SDL_Renderer* renderer);
void render_end_game_lost(SDL_Renderer* renderer);
extern InventoryEntity inventoryEntity;