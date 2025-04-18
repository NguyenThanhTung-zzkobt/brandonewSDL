#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include "camera.h"
#include "time_manager.h"
#include "battle.h"
#include "vector"

class Map;



class Player  {
public:
	SDL_FPoint position;
	float velocity_x;
	float velocity_y;
	SDL_Texture* texture;
	Entity entity;
	Map* map_ref = nullptr;

	float max_hp;
	float current_hp;
	int attack_power;
	StatusEffect active_status;

	std::vector<int> inventory;


	Player();

	void update(float dt);
	void render(SDL_Renderer* r);
	void setMap(Map* map);
	bool checkCollision(float x ,float y);
	void cleanup();
	void set_position(int x, int y);

	void addItem(int itemID);
	bool removeItem(int itemID);
	int getInventorySize();
	int getItemCount(int itemID);
	void displayInventory();
	bool hasItem(int itemID);
	bool canAddItem();
	void clearInventory();
	bool removeItemByIndex(int index);

private:
	static const int MAX_INVENTORY_SIZE = 20;
};





typedef struct {
	float w, h;
} SpriteSize;



void reload_player_texture(SDL_Renderer* renderer);
void init_player(SDL_Renderer* renderer);
extern Player PLAYER;


