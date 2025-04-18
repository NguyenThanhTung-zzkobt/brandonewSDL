#pragma once


#include <SDL3/SDL.h>

extern Uint64 last_tick;
extern Uint64 current_tick;
extern float delta_time;
extern int frame_count;


void reset_delta_time();
