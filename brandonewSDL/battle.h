#pragma once
#include "player.h"
#include "global.h"
class Monster1;

enum BattleState {
	BATTLE_NONE,
	BATTLE_ACTIVE,
	BATTLE_WON,
	BATTLE_LOST,
};



enum Turn {
	PLAYER_TURN,
	MONSTER_TURN
};

void start_battle(Monster1* enemy);
void update_battle(float delta_time);
void render_battle(SDL_Renderer* renderer);
bool is_in_battle();
void end_battle_lost();
void end_battle_won();
void exit_battle();
void execute_poison_infection(Entity* attacker, Entity* target);


extern bool is_player_turn;
extern Turn current_turn;
extern BattleState current_battle_state;
extern Entity* current_enemy;