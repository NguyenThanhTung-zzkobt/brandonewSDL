#include "monster1.h"


static SDL_FRect sprite_portion = { 17, -2 , 15 ,18 };


Monster1 Monster; // Changed 'monster' to 'Monster'


void Monster1::update() {
    float dx = Monster.position.x - PLAYER.position.x; 
    float dy = Monster.position.y - PLAYER.position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance < Monster.detection_radius && !Monster.triggered) { 
        Monster.triggered = true;
        start_battle(&entity); // Note: You might want to use 'Monster.entity' here
    }
}

void Monster1::cleanup() {
    if (Monster.texture) { 
        SDL_Log("Cleaning up monster texture");
        SDL_DestroyTexture(Monster.texture); 
        Monster.texture = NULL; 
    }
}
void Monster1::render(SDL_Renderer* renderer) {
    SDL_FRect dst;
    dst.x = Monster.position.x - camera.x; 
    dst.y = Monster.position.y - camera.y;
    dst.w = sprite_portion.w * 2.0f; // Scale if needed
    dst.h = sprite_portion.h * 2.0f;


    SDL_RenderTexture(renderer, Monster.texture, &sprite_portion, &dst); 
    SDL_SetTextureScaleMode(Monster.texture, SDL_SCALEMODE_LINEAR); 
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
}
void init_monster1(SDL_Renderer* renderer) {
    Monster.texture = IMG_LoadTexture(renderer, "assets/Enemies_Sprites/Bomberplant_Sprites/bomberplant_spritesheet.png"); 
    if (!Monster.texture) { 
        SDL_Log("Couldn't load monster texture: %s", SDL_GetError());
        return;
    }
    Monster.position = { 185, 200 }; 
    Monster.detection_radius = 40.0f; 
    Monster.triggered = false; 
    Monster.max_hp = 100;
    Monster.current_hp = Monster.max_hp;
    Monster.attack_power = 8;
    SDL_SetTextureScaleMode(Monster.texture, SDL_SCALEMODE_NEAREST); 
    strncpy_s(Monster.entity.name, "bomberplant", MAX_NAME_LENGTH - 1); 
    Monster.entity.position = Monster.position; 
    Monster.entity.texture = Monster.texture; 
    Monster.entity.max_hp = Monster.max_hp;
    Monster.entity.current_hp = Monster.current_hp;
    Monster.entity.displayed_hp = (float)Monster.current_hp;
    SDL_Log("monster current hp: %d", Monster.current_hp);
    SDL_Log("monster displayed hp: %.2f", Monster.entity.displayed_hp);
    Monster.entity.attack_power = Monster.attack_power;


    Monster.entity.update = [](float dt) { 
        Monster.update();
        };
    Monster.entity.render = [](SDL_Renderer* renderer) { 
        Monster.render(renderer);
        };
    Monster.entity.cleanup = []() { 
        Monster.cleanup();
        };

    create_entity(Monster.entity); 
}