#pragma once
#include "entity.h"
#include "player.h"
#include "camera.h"
#include "battle.h"
#include <vector>

class Monster1 {
public:
	SDL_FPoint position;
	float detection_radius;
	bool triggered;
	SDL_Texture* texture;
	Entity entity;

	int current_hp;
	int max_hp;
	int attack_power;

	void render(SDL_Renderer* renderer);
	void update();
	void cleanup();
};

void init_monster1(SDL_Renderer* renderer);
extern Monster1 Monster;
extern Monster1 phantom1;
extern std::vector<Monster1> monsters;