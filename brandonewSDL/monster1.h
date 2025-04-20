#pragma once
#include "entity.h"
#include "player.h"
#include "camera.h"
#include "battle.h"
#include <vector>
#include <string>
#include <cstdlib>   // For rand() and srand()
#include <ctime>     // For time() to seed the random number generator




class Monster1 {
public:
	SDL_FPoint position;
	float detection_radius;
	bool triggered;
	SDL_Texture* texture;
	std::string texture_path;

	Entity entity;

	float current_hp;
	float max_hp;
	int attack_power;
	std::string weakness;

	void render(SDL_Renderer* renderer);
	void update();
	void cleanup();
	void reload_monster_texture(SDL_Renderer* renderer);
};

enum BattleAction {
	///bomberplant
	NORMAL_ATTACK,
	SPIKE_ATTACK,
	POISON_INFECTION,
	//phantom
	SOUL_STEAL,
	CURSED

};

BattleAction choose_action(Entity* current_enemy);

void init_monster1(SDL_Renderer* renderer);
extern Monster1 Monster;
extern Monster1 phantom1;
extern std::vector<Monster1> monsters;