#pragma once
#include "player.h"
#include "monster1.h"


enum BattleState {
	BATTLE_NONE,
	BATTLE_ACTIVE,
	BATTLE_WON,
	BATTLE_LOST,
};


void start_battle(Entity* enemy);
void update_battle(float delta_time);
void render_battle(SDL_Renderer* renderer);
bool is_in_battle();
void end_battle_lost();
void end_battle_won();
void exit_battle();
extern BattleState current_battle_state;