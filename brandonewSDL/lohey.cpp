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
#define TARGET_FPS 144
#define TARGET_FRAME_TIME (1000/ TARGET_FPS)


static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
typedef enum { STATE_FIRST_IMAGE, STATE_SECOND_IMAGE } AppState1;
static AppState1 current_state = STATE_FIRST_IMAGE;
static Map mapInstance;


Uint64 last_tick = 0;
Uint64 current_tick = 0;
float delta_time;

void update() {
	last_tick = current_tick;
	current_tick = SDL_GetTicks();
	delta_time = (current_tick - last_tick) / 1000.0f;

	if (is_in_battle()) {
		update_battle(delta_time);
		return;
	}

	for (int i = 0; i < entities_count; i++) {
		if (entities[i].update != nullptr ) {
			entities[i].update(delta_time);
		}
	}
}


void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderClear(renderer);

	if (is_in_battle()) {
		render_battle(renderer);
	}

	for (int i = 0; i < entities_count; i++) {
		if (entities[i].render != nullptr && entities[i].render != (void(*)(SDL_Renderer*))0xcccccccccccccccc) {
			entities[i].render(renderer);
		}
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

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("notitle", 1280, 720, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}





	mapInstance.init_map(renderer);
	player.setMap(&mapInstance);
	init_player(renderer);
	if (!player.texture) {
		SDL_Log("Couldn't load player texture: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	init_monster1(renderer);
	SDL_SetRenderLogicalPresentation(renderer, 400 , 255, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT) {
		SDL_Log("Event quit");
		return SDL_APP_SUCCESS;
	}



		

	return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* appstate)
{

	update();
	render();
	app_wait_for_next_frame();
 
	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	for (int i = 0; i < entities_count; i++) {
		if (entities[i].cleanup != nullptr && entities[i].cleanup != (void(*)())0xcccccccccccccccc) {
			entities[i].cleanup();
		}
	}
	if (player.texture) SDL_DestroyTexture(player.texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_EVENT_QUIT;
}