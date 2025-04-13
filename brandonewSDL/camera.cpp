#include "camera.h"
#include "player.h"

SDL_FRect camera = { 0, 0, 320, 180 };

void set_camera_position(SDL_FPoint target) {
    camera.x = target.x - camera.w / 2;
    camera.y = target.y - camera.h / 2;
}
