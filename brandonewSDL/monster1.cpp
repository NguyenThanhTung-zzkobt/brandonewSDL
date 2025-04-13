#include "monster1.h"


static SDL_FRect sprite_portion = { 17, -2 , 15 ,18 };


Monster1 Monster; // Changed 'monster' to 'Monster'


void Monster1::update() {
    float dx = Monster.position.x - player.position.x; // Use 'Monster'
    float dy = Monster.position.y - player.position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance < Monster.detection_radius && !Monster.triggered) { // Use 'Monster'
        Monster.triggered = true;
        start_battle(&entity); // Note: You might want to use 'Monster.entity' here
    }
}

void Monster1::cleanup() {
    if (Monster.texture) { // Use 'Monster'
        SDL_Log("Cleaning up monster texture");
        SDL_DestroyTexture(Monster.texture); // Use 'Monster'
        Monster.texture = NULL; // Use 'Monster'
    }
}
void Monster1::render(SDL_Renderer* renderer) {
    SDL_FRect dst;
    dst.x = Monster.position.x - camera.x; // Use 'Monster'
    dst.y = Monster.position.y - camera.y;
    dst.w = sprite_portion.w * 2.0f; // Scale if needed
    dst.h = sprite_portion.h * 2.0f;


    SDL_RenderTexture(renderer, Monster.texture, &sprite_portion, &dst); // Use 'Monster'
    SDL_SetTextureScaleMode(Monster.texture, SDL_SCALEMODE_LINEAR); // Use 'Monster'
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
}
void init_monster1(SDL_Renderer* renderer) {
    Monster.texture = IMG_LoadTexture(renderer, "assets/Enemies_Sprites/Bomberplant_Sprites/bomberplant_spritesheet.png"); // Use 'Monster'
    if (!Monster.texture) { // Use 'Monster'
        SDL_Log("Couldn't load monster texture: %s", SDL_GetError());
        return;
    }
    Monster.position = { 185, 200 }; // Use 'Monster'
    Monster.detection_radius = 40.0f; // Use 'Monster'
    Monster.triggered = false; // Use 'Monster'

    SDL_SetTextureScaleMode(Monster.texture, SDL_SCALEMODE_NEAREST); // Use 'Monster'
    strncpy_s(Monster.entity.name, "bomberplant", MAX_NAME_LENGTH - 1); // Use 'Monster'
    Monster.entity.position = Monster.position; // Use 'Monster'
    Monster.entity.texture = Monster.texture; // Use 'Monster'

    Monster.entity.update = [](float dt) { // Use 'Monster'
        Monster.update();
        };
    Monster.entity.render = [](SDL_Renderer* renderer) { // Use 'Monster'
        Monster.render(renderer);
        };
    Monster.entity.cleanup = []() { // Use 'Monster'
        Monster.cleanup();
        };

    create_entity(Monster.entity); // Use 'Monster'
}