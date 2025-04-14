#include "player.h"
#include "map.h"
#define PLAYER_SPEED 200.0f 
static SDL_Texture* player_texture = NULL;
static SDL_FRect sprite_portion = { 17, 14 , 15 ,18 };

static SpriteSize sprite_size{ 15, 15 };


Player::Player() {
	position.x = 18;
	position.y = 107;
	velocity_x = 0;
	velocity_y = 0;
	max_hp = 100;
	
	current_hp = 100;
	attack_power = 20;
	texture = nullptr;
}
Player PLAYER;



static int movement_speed = 30;

void Player::setMap(Map* map) {
	map_ref = map;
	if (map_ref) {
		SDL_Log("Map reference successfully set in Player");
		if (!map_ref->getCollisionData(0, 0) && map_ref->getWidth() == 0) {
			SDL_Log("WARNING: Collision data might not be initialized yet!");
		}
	}
	else {
		SDL_Log("Map reference is null");
	}
}



bool Player::checkCollision(float x, float y) {

	if (!map_ref) {
		SDL_Log("Map reference is null");
		return false;
	}

	float w = sprite_size.w;
	float h = sprite_size.h;

	int left = static_cast<int>((x) / TILE_WIDTH);
	int right = static_cast<int>((x + w - 1) / TILE_WIDTH);
	int top = static_cast<int>((y) / TILE_HEIGHT);
	int bottom = static_cast<int>((y + h - 1) / TILE_HEIGHT);

	for (int ty = top; ty <= bottom; ++ty) {
		for (int tx = left; tx <= right; ++tx) {
			// Bounds check
			if (tx < 0 || ty < 0 || tx >= map_ref->getWidth() || ty >= map_ref->getHeight()) {
				return true; // Treat out-of-bounds as solid
			}

			if (map_ref->getCollisionData(tx, ty) != 0) {
				return true;
			}
		}
	}

	return false;
}

void Player::cleanup() {
	if (player_texture) {
		SDL_Log("Cleaning up player texture");
		SDL_DestroyTexture(player_texture);
		player_texture = NULL;
	}
}

void Player::update(float delta_time) {
	const Uint8* keyboard_state = (const Uint8*)SDL_GetKeyboardState(NULL);

	// Vertical movement
	if (keyboard_state[SDL_SCANCODE_W]) {
		float new_y = PLAYER.position.y - movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(PLAYER.position.x, new_y)) {
			PLAYER.position.y = new_y;
			sprite_portion = { 17, 14 , 15 ,18 };
		}
	}
	if (keyboard_state[SDL_SCANCODE_S]) {
		float new_y = PLAYER.position.y + movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(PLAYER.position.x, new_y)) {
			PLAYER.position.y = new_y;
			sprite_portion = { 17, 14 , 15 ,18 };
		}
	}

	// Horizontal movement
	if (keyboard_state[SDL_SCANCODE_A]) {
		float new_x = PLAYER.position.x - movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(new_x, PLAYER.position.y)) {
			PLAYER.position.x = new_x;
			sprite_portion = { 17, 14 , 15 ,18 };
		}
	}
	if (keyboard_state[SDL_SCANCODE_D]) {
		float new_x = PLAYER.position.x + movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(new_x, PLAYER.position.y)) {
			PLAYER.position.x = new_x;
			sprite_portion = { 17, 14 , 15 ,18 };
		}
	}
	camera.x = PLAYER.position.x - camera.w / 2;
	camera.y = PLAYER.position.y - camera.h / 2;

	if (camera.x < 0) camera.x = 0;
	if (camera.y < 0) camera.y = 0;

	if (camera.x + camera.w >= 420) camera.x = 420 - camera.w;
	if (camera.y + camera.h >= 240) camera.y = 240 - camera.h;
}
void Player::render(SDL_Renderer* renderer) {
	float final_x = camera.w / 2 - sprite_size.w / 2;
	float final_y = camera.h / 2 - sprite_size.h / 2;

	if (camera.x <= 0) final_x =PLAYER.position.x - sprite_size.w / 2;
	if (camera.y <= 0) final_y = PLAYER.position.y - sprite_size.h / 2;

	if (camera.x + camera.w >= 420) final_x = PLAYER.position.x - (420 - camera.w) - sprite_size.w / 2;
	if (camera.y + camera.h >= 240) final_y = PLAYER.position.y - (240 - camera.h) - sprite_size.h / 2;

	SDL_FRect PLAYER_rect = { 
		PLAYER.position.x - camera.x
		, PLAYER.position.y - camera.y 
		, sprite_size.w
		, sprite_size.h 
	};
	SDL_RenderTexture(renderer, PLAYER.texture, &sprite_portion, &PLAYER_rect);
	SDL_SetTextureScaleMode(PLAYER.texture, SDL_SCALEMODE_LINEAR);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	float left = (float)(PLAYER.position.x / TILE_WIDTH);
	float right = (float)((PLAYER.position.x + sprite_size.w - 1) / TILE_WIDTH);
	float top = (float)(PLAYER.position.y / TILE_HEIGHT);
	float bottom = (float)((PLAYER.position.y + sprite_size.h - 1) / TILE_HEIGHT);

	for (int y = top; y <= bottom; ++y) {
		for (int x = left; x <= right; ++x) {
			SDL_FRect tileBox = {
				x * TILE_WIDTH - camera.x,
				y * TILE_HEIGHT - camera.y,
				(float)TILE_WIDTH,
				(float)TILE_HEIGHT
			};

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100); // semi-transparent green
			SDL_RenderRect(renderer, &tileBox);
		}
	}

}

void init_player(SDL_Renderer* renderer) {

	PLAYER.texture = IMG_LoadTexture(renderer, "assets/Char_Sprites/char_spritesheet.png");
	if (!PLAYER.texture) {
		SDL_Log("Couldn't load player texture: %s", SDL_GetError());
		return;
	}
	SDL_SetTextureScaleMode(PLAYER.texture, SDL_SCALEMODE_NEAREST);

	PLAYER.entity.position = PLAYER.position;
	PLAYER.entity.max_hp = PLAYER.max_hp;
	
	PLAYER.entity.current_hp = PLAYER.current_hp;
	PLAYER.entity.displayed_hp = (float)PLAYER.current_hp;
	SDL_Log("Init: current_hp = %d, displayed_hp = %.2f", PLAYER.entity.current_hp, PLAYER.entity.displayed_hp);
	PLAYER.entity.attack_power = PLAYER.attack_power;

	strncpy_s(PLAYER.entity.name , "PLAYER", MAX_NAME_LENGTH -1);
	PLAYER.entity.update = [](float dt) {
		PLAYER.update(dt);
		};

	PLAYER.entity.render = [](SDL_Renderer* r) {
		PLAYER.render(r);
		};

	PLAYER.entity.cleanup = []() {
		PLAYER.cleanup();
		};




	create_entity(PLAYER.entity);
}


void Player::set_position(int x, int y) {
	this->position.x = x;
	this->position.y = y;
}