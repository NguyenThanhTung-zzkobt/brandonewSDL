#include "ui.h"

void draw_health_bar(SDL_Renderer* renderer, int x, int y, int width, int height, float percentage) {
    SDL_FRect bg = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_FRect fg = { x, y, (int)(width * percentage), height };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}

void draw_battle_ui(SDL_Renderer* renderer, Entity* enemy) {
    draw_health_bar(renderer, 50, 50, 200, 20, (float)player.current_hp / player.hp);
    draw_health_bar(renderer, 300, 50, 200, 20, (float)enemy->current_hp / enemy->max_hp);
}