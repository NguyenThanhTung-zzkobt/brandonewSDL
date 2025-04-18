#pragma once
#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "menu.h"
#include "battle.h"

extern Mix_Music* music;

extern bool is_starting_new_game ;
extern int volume_level;
extern SDL_Rect volume_bar_rect;
extern int volume_bar_width ;
extern int volume_bar_height ;
extern int volume_bar_x_offset ;
extern int volume_bar_y_offset ;
extern bool in_volume_sub_menu ;
extern SDL_AudioSpec audiospec ;
extern int menu_select ;

void init_music();
void cleanup_music();
void play_music(const char* filename);