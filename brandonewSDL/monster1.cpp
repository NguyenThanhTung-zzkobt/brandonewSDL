#include "monster1.h"


static SDL_FRect sprite_portion = { 17, -2 , 15 ,18 };

std::vector<Monster1> monsters;

Monster1 phantom1;
Monster1 Monster; // Changed 'monster' to 'Monster'


void Monster1::update() {
    float dx = position.x - PLAYER.position.x; 
    float dy = position.y - PLAYER.position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    //SDL_Log("Monster Position: %.2f, %.2f", position.x, position.y);
    //SDL_Log("Player Position: %.2f, %.2f", PLAYER.position.x, PLAYER.position.y);
    //SDL_Log("Distance: %.2f, Triggered: %d", distance, triggered);

    if (distance < detection_radius && !triggered) { 
        triggered = true;
        start_battle(this);
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
    dst.x = position.x - camera.x; 
    dst.y = position.y - camera.y;
    dst.w = sprite_portion.w * 2.0f; // Scale if needed
    dst.h = sprite_portion.h * 2.0f;


    SDL_RenderTexture(renderer, texture, &sprite_portion, &dst); 
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR); 
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
}
void init_monster1(SDL_Renderer* renderer) {
    ///bomberplant
    Monster.texture = IMG_LoadTexture(renderer, "assets/Enemies_Sprites/Bomberplant_Sprites/bomberplant_spritesheet.png");
    if (!Monster.texture) {
        SDL_Log("Couldn't load monster texture: %s", SDL_GetError());
        return;
    }
    ///bomberplant
    Monster.position = { 180, 190 };
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
    //SDL_Log("monster current hp: %d", Monster.current_hp);
    //SDL_Log("monster displayed hp: %.2f", Monster.entity.displayed_hp);
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
    monsters.push_back(Monster);
    create_entity(monsters.back().entity);

    //phantom

    phantom1.texture = IMG_LoadTexture(renderer, "assets/Enemies_Sprites/Phantom_Sprites/phantom_spritesheet.png");
    if (!phantom1.texture) {
        SDL_Log("Couldn't load monster texture: %s", SDL_GetError());
        return;
    }

   
    phantom1.position = { 400, 200 };
    phantom1.detection_radius = 40.0f;
    phantom1.triggered = false;
    phantom1.max_hp = 50;
    phantom1.current_hp = phantom1.max_hp;
    phantom1.attack_power = 13;
    SDL_SetTextureScaleMode(phantom1.texture, SDL_SCALEMODE_NEAREST);
    strncpy_s(phantom1.entity.name, "phantom", MAX_NAME_LENGTH - 1);
    phantom1.entity.position = phantom1.position;
    phantom1.entity.texture = phantom1.texture;
    phantom1.entity.max_hp = phantom1.max_hp;
    phantom1.entity.current_hp = phantom1.current_hp;
    phantom1.entity.displayed_hp = (float)phantom1.current_hp;
    SDL_Log("phantom1 current hp: %d", phantom1.current_hp);
    SDL_Log("phantom1 displayed hp: %.2f", phantom1.entity.displayed_hp);
    phantom1.entity.attack_power = phantom1.attack_power;


    phantom1.entity.update = [](float dt) {
        phantom1.update();
        };
    phantom1.entity.render = [](SDL_Renderer* renderer) {
        phantom1.render(renderer);
        };
    phantom1.entity.cleanup = []() {
        phantom1.cleanup();
        };
    monsters.push_back(phantom1);
    create_entity(monsters.back().entity);
}