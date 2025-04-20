#include "player.h"
#include "map.h"
static SDL_Texture* player_texture = NULL;
static SDL_FRect sprite_portion = { 17, 14 , 15 ,18 };

static SpriteSize sprite_size{ 15, 15 };


float trigger_x_min = 384.0f;
float trigger_x_max = 402.0f;
float trigger_y_min = 120.0f;
float trigger_y_max = 145.0f;

bool map_transition_triggered = false;


Player::Player() {
	position.x = 18;
	position.y = 107;
	velocity_x = 0;
	velocity_y = 0;
	MY_LEVEL = 1;
	max_hp = 100.0 + MY_LEVEL * 5;
	active_status.type = StatusEffect::Type::NONE;
	active_status.duration = 0;
	active_status.damagePerTurn = 0;
	current_hp = 100.0 +MY_LEVEL;
	attack_power = 15 + MY_LEVEL;
	
	texture = nullptr;
	weapons = { "Sword", "Axe", "Bow"};
	selected_weapon = "Sword";
	inventory = {1};

	std::vector<std::string>& getWeapons(); 
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
	if (texture) {
		SDL_Log("Cleaning up player texture");
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
}

void Player::update(float delta_time) {
	const Uint8* keyboard_state = (const Uint8*)SDL_GetKeyboardState(NULL);
	bool moved = false; // <<--- Thêm cờ kiểm tra di chuyển
	// Vertical movement
	if (keyboard_state[SDL_SCANCODE_W]) {
		float new_y = PLAYER.position.y - movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(PLAYER.position.x, new_y)) {
			//SDL_Log("Player Update: >>> Trying to set animation: walk_up");
			PLAYER.position.y = new_y;
			//sprite_portion = { 17, 14 , 15 ,18 };
			PLAYER.animator.setAnimation("walk_up"); // <<--- THÊM DÒNG NÀY
			moved = true; // <<--- Đánh dấu đã di chuyển
			SDL_Log("Player Position: x=%.2f, y=%.2f", PLAYER.position.x, PLAYER.position.y);
		}
	}
	if (keyboard_state[SDL_SCANCODE_S]) {
		float new_y = PLAYER.position.y + movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(PLAYER.position.x, new_y)) {
			PLAYER.position.y = new_y;
			//sprite_portion = { 17, 14 , 15 ,18 };
			PLAYER.animator.setAnimation("walk_down"); // <<--- THÊM DÒNG NÀY
			moved = true; // <<--- Đánh dấu đã di chuyển
			SDL_Log("Player Position: x=%.2f, y=%.2f", PLAYER.position.x, PLAYER.position.y);
		}
	}

	// Horizontal movement
	if (keyboard_state[SDL_SCANCODE_A]) {
		float new_x = PLAYER.position.x - movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(new_x, PLAYER.position.y)) {
			PLAYER.position.x = new_x;
			//sprite_portion = { 17, 14 , 15 ,18 };
			PLAYER.animator.setAnimation("walk_left"); // <<--- THÊM DÒNG NÀY
			moved = true; // <<--- Đánh dấu đã di chuyển
			SDL_Log("Player Position: x=%.2f, y=%.2f", PLAYER.position.x, PLAYER.position.y);
		}
	}
	if (keyboard_state[SDL_SCANCODE_D]) {
		float new_x = PLAYER.position.x + movement_speed * delta_time;
		// Check collision on both axes (horizontal and vertical)
		if (!PLAYER.checkCollision(new_x, PLAYER.position.y)) {
			PLAYER.position.x = new_x;
			//sprite_portion = { 17, 14 , 15 ,18 };
			PLAYER.animator.setAnimation("walk_right"); // <<--- THÊM DÒNG NÀY
			moved = true; // <<--- Đánh dấu đã di chuyển
			//SDL_Log("Address of PLAYER in movement code: %p", (void*)&PLAYER);
			SDL_Log("Player Position: x=%.2f, y=%.2f", PLAYER.position.x, PLAYER.position.y);
		}
	}
	if (!moved) {
		std::string current = PLAYER.animator.getCurrentAnimationName();
		if (current == "walk_down") PLAYER.animator.setAnimation("idle_down");
		else if (current == "walk_up") PLAYER.animator.setAnimation("idle_up");
		else if (current == "walk_left") PLAYER.animator.setAnimation("idle_left");
		else if (current == "walk_right") PLAYER.animator.setAnimation("idle_right");
	}

	animator.update(delta_time);


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

	PLAYER.animator.applyTo(sprite_portion);
	//SDL_Log("Player Render - sprite_portion: y=%.1f, h=%.1f (Anim: %s)",sprite_portion.y, sprite_portion.h, PLAYER.animator.getCurrentAnimationName().c_str());
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

	PLAYER.animator.setTexture(PLAYER.texture);
	PLAYER.animator.loadAnimations("assets/Char_Sprites/player_animations.txt");
	PLAYER.animator.setAnimation("walk_down"); // animation mặc định

	PLAYER.entity.position = PLAYER.position;
	PLAYER.entity.MY_LEVEL = PLAYER.MY_LEVEL;
	PLAYER.entity.max_hp = PLAYER.max_hp;
	
	PLAYER.entity.current_hp = PLAYER.current_hp;
	PLAYER.entity.displayed_hp = (float)PLAYER.current_hp;
	PLAYER.entity.active_status.type = PLAYER.active_status.type;
	PLAYER.entity.active_status.duration = PLAYER.active_status.duration;
	PLAYER.entity.active_status.damagePerTurn = PLAYER.active_status.damagePerTurn;
	PLAYER.entity.attack_power = PLAYER.attack_power;
	PLAYER.entity.selected_weapon = PLAYER.selected_weapon;

	strncpy_s(PLAYER.entity.name, "PLAYER", MAX_NAME_LENGTH - 1);
	PLAYER.entity.update = [](float dt) {
		PLAYER.update(dt);
		};

	PLAYER.entity.render = [](SDL_Renderer* r) {
		PLAYER.render(r);
		};

	PLAYER.entity.cleanup = []() {
		PLAYER.cleanup();
		};
	PLAYER.entity.set_position = [](int x , int y) {
		PLAYER.set_position(x, y);
		};



	create_entity(PLAYER.entity);
}


void reload_player_texture(SDL_Renderer* renderer) {
	if (PLAYER.texture) {
		SDL_DestroyTexture(PLAYER.texture);
		PLAYER.texture = nullptr;
	}
	PLAYER.texture = IMG_LoadTexture(renderer, "assets/Char_Sprites/char_spritesheet.png");
	SDL_Log("PLAYER.texture = %p", PLAYER.texture);
	if (!PLAYER.texture) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load player texture: %s", SDL_GetError());
	}
	else {
		// Quan trọng: Cập nhật lại texture cho animator sau khi reload
		PLAYER.animator.setTexture(PLAYER.texture);
		SDL_SetTextureScaleMode(PLAYER.texture, SDL_SCALEMODE_NEAREST); // Đặt lại scale mode
	}
}



void Player::set_position(int x, int y) {
	PLAYER.position.x = static_cast<float>(x);
	PLAYER.position.y = static_cast<float>(y);
}



void Player::addItem(int itemID, const std::map<int, Item>& itemMap) {
	if (inventory.size() < MAX_INVENTORY_SIZE) {
		if (itemMap.find(itemID) != itemMap.end()) {
			inventory.push_back(itemID);
			SDL_Log("Added item: %s", itemMap.at(itemID).name.c_str());
		}
		else {
			SDL_Log("Item ID %d not found in itemMap.", itemID);
		}
	}
	else {
		SDL_Log("Inventory full. Cannot add item: %d", itemID);
	}
}

bool Player::removeItem(int itemID, const std::map<int, Item>& itemMap) {
	for (auto it = inventory.begin(); it != inventory.end(); ++it) {
		if (*it == itemID) {
			inventory.erase(it);
			SDL_Log("Removed item: %s", itemMap.at(itemID).name.c_str());
			return true;
		}
	}
	SDL_Log("Item ID %d not found in inventory.", itemID);
	return false;
}


bool Player::removeItemByIndex(int index, const std::map<int, Item>& itemMap) {
	if (index >= 0 && index < inventory.size()) {
		int itemID = inventory[index];
		SDL_Log("Removed item at index %d: %s", index, itemMap.at(itemID).name.c_str());
		inventory.erase(inventory.begin() + index);
		return true;
	}
	SDL_Log("Invalid index: %d", index);
	return false;
}




void useItem(int itemID, std::map<int, Item>& itemMap) {
	auto it = itemMap.find(itemID);
	if (it != itemMap.end()) {
		const Item& item = it->second;
		SDL_Log("Using item: %s", item.name.c_str());

		if (item.id == 1) { // Healing potion
			PLAYER.entity.current_hp += 20; // or however much it heals
		}
		else if (item.id == 2) {
			PLAYER.entity.active_status = {}; // Remove status effect
		}
		// ... other effects
	}
	else {
		SDL_Log("Cannot use unknown item ID: %d", itemID);
	}
}


void Player::displayInventory(const std::map<int, Item>& itemMap) {
	if (inventory.empty()) {
		SDL_Log("Inventory is empty.");
		return;
	}

	SDL_Log("Inventory:");
	for (size_t i = 0; i < inventory.size(); ++i) {
		int itemID = inventory[i];
		auto it = itemMap.find(itemID);
		if (it != itemMap.end()) {
			const Item& item = it->second;
			SDL_Log("[%zu] %s - %s", i, item.name.c_str(), item.description.c_str());
		}
		else {
			SDL_Log("[%zu] Unknown Item ID: %d", i, itemID);
		}
	}
}

bool Player::hasItem(int itemID) {
	return std::find(inventory.begin(), inventory.end(), itemID) != inventory.end();
}

bool Player::canAddItem() {
	return inventory.size() < MAX_INVENTORY_SIZE;
}

void Player::clearInventory() {
	inventory.clear();
	SDL_Log("Inventory cleared.");
}

int Player::getInventorySize() {
	return static_cast<int>(inventory.size());
}

int Player::getItemCount(int itemID) {
	return std::count(inventory.begin(), inventory.end(), itemID);
}


int Player::getItemID(const std::string & itemName, const std::map<int, Item>&itemMap) {
	for (const auto& pair : itemMap) {
		if (pair.second.name == itemName) {
			return pair.first;
		}
	}
	return -1; // Return -1 to indicate not found
}


void update_game_state(float delta_time) {
	//SDL_Log("Address of PLAYER in update_game_state: %p", (void*)&PLAYER);
	float px = PLAYER.position.x;
	float py = PLAYER.position.y;

	if (!map_transition_triggered &&
		px >= trigger_x_min && px <= trigger_x_max &&
		py >= trigger_y_min && py <= trigger_y_max) {

		map_transition_triggered = true;
		mapInstance.loadMap(1, renderer);

	}



}