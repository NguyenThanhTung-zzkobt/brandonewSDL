#define SDL_MAIN_USE_CALLBACKS 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "entity.h"
#include "player.h"
#include "map.h"
#include "camera.h"
#include "monster1.h"
#include "battle.h"
#include "ui.h"
#include "menu.h"

#define TARGET_FPS 144
#define TARGET_FRAME_TIME (1000/ TARGET_FPS)
#define MIX_FLAGS MIX_INIT_OGG

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static Map mapInstance;

Uint64 last_tick = 0;
Uint64 current_tick = 0;
float delta_time;

void update() {
	last_tick = current_tick;
	current_tick = SDL_GetTicks();
	delta_time = (current_tick - last_tick) / 1000.0f;

	switch (current_game_state) {
	case STATE_MAIN_MENU:
		break;
	case STATE_INIT_GAME:
		mapInstance.init_map(renderer);
		PLAYER.setMap(&mapInstance);
		init_player(renderer);
		init_monster1(renderer);
		cleanup_menu_ui();
		current_game_state = STATE_INGAME;
		break;
	case STATE_INGAME:
		if (is_in_battle()) {
			update_battle(delta_time);
		}
		else {
			for (int i = 0; i < entities_count; i++) {
				if (entities[i].update != nullptr) {
					entities[i].update(delta_time);
				}
			}
		}
		break;
	default:
		break;
	}
}

void render() {
	if (current_game_state != STATE_MAIN_MENU) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Default background for other states
		SDL_RenderClear(renderer);
	}

	switch (current_game_state) {
	case STATE_MAIN_MENU:
		render_menu_ui(renderer);
		break;
	case STATE_OPTIONS_MENU:
		render_options_ui(renderer);
		break;
	case STATE_CREDITS:
		render_credits_ui(renderer); 
		break;
	case STATE_INIT_GAME:
		// Render initial game scene.  This might be a loading screen, or the initial map.
		break;
	case STATE_INGAME:
		if (is_in_battle()) {
			render_battle(renderer);
		}
		else {
			for (int i = 0; i < entities_count; i++) {
				if (entities[i].render != nullptr && entities[i].render != (void(*)(SDL_Renderer*))0xcccccccccccccccc) {
					entities[i].render(renderer);
				}
			}
		}
		break;
	default:
		break;
	}
	SDL_RenderPresent(renderer);
}

void app_wait_for_next_frame() {
	Uint64 frame_time = SDL_GetTicks() - current_tick;
	if (frame_time < TARGET_FRAME_TIME) SDL_Delay(TARGET_FRAME_TIME - frame_time);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION,SDL_LOG_PRIORITY_VERBOSE);

	SDL_Log("App started");

	if (TTF_Init() == -1) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("notitle", 1280, 720, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}





	init_menu_ui(renderer);
	SDL_SetRenderLogicalPresentation(renderer, 400 , 255, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{


	if (event->type == SDL_EVENT_QUIT) {
		SDL_Log("Event quit");
		return SDL_APP_SUCCESS;
	}

	switch (current_game_state) {
	case STATE_MAIN_MENU:
		update_menu_ui(event); // Pass the event to the menu update function
		break;
	case STATE_OPTIONS_MENU:
		update_options_ui(renderer,window,event); // Pass the event to the menu update function
		break;
	case STATE_CREDITS:
		update_credits_ui(event);
		break;
	case STATE_INIT_GAME:
		// Handle events specific to the initial game state (if any)
		break;
	case STATE_INGAME:
		if (is_in_battle()) {
			update_battle_ui(event);
		}
		else {
			//  handle_events(event);  //  Original game event handling
		}
		break;
	case STATE_QUIT:
		return SDL_APP_SUCCESS;
	default:
		break;
	}
	return SDL_APP_CONTINUE;
}


		


SDL_AppResult SDL_AppIterate(void* appstate)
{

	if (current_game_state != STATE_QUIT) {
		update();
		render();
		app_wait_for_next_frame();
		return SDL_APP_CONTINUE;
	}
	return SDL_APP_SUCCESS;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	for (int i = 0; i < entities_count; i++) {
		if (entities[i].cleanup != nullptr && entities[i].cleanup != (void(*)())0xcccccccccccccccc) {
			entities[i].cleanup();
		}
	}
	if (PLAYER.texture) SDL_DestroyTexture(PLAYER.texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_EVENT_QUIT;
}
