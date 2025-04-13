#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include "camera.h"

class Map;


class Player {
public:
	SDL_FPoint position;
	float velocity_x;
	float velocity_y;
	SDL_Texture* texture;
	Map* map_ref = nullptr;

	int hp;
	int current_hp;
	int attack_power;

	Player();

	void update(float dt);
	void render(SDL_Renderer* r);
	void setMap(Map* map);
	bool checkCollision(float x ,float y);
	void cleanup();
public:
};


#ifdef DISABLED
typedef struct {
	float x, y;
} Position;
typedef struct {
	SDL_FRect position;
	float velocity_x;
	float velocity_y;
	SDL_Texture* texture;
} Player;



typedef struct {
	float w, h;
}SpriteSize;
#endif

typedef struct {
	float w, h;
} SpriteSize;


//extern Position player_position;

void init_player(SDL_Renderer* renderer);
extern Player player;


