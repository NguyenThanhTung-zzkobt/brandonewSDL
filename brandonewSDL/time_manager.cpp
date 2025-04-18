#include "time_manager.h"

int frame_count = 0;
void reset_delta_time() {
    current_tick = SDL_GetTicks();
    last_tick = current_tick;
}