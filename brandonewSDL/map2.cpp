#ifdef f

#define _CRT_SECURE_NO_WARNINGS
#define CUTE_TILED_IMPLEMENTATION
#include "map2.h"

// Global variables

Texture2* texture2;
static Map2* activeMapInstance2 = nullptr;




static void cleanup_map() {
	if (!texture2) return;
	Texture2* current_texture = texture2;
	while (current_texture) {
		if (current_texture->texture2) {
			SDL_DestroyTexture(current_texture->texture2);
		}
		Texture2* next = current_texture->next;
		SDL_free(current_texture);
		current_texture = next;
	}
	texture2 = NULL;
	if (map) {
		cute_tiled_free_map(map);
		map = NULL;
	}
}

static void render(SDL_Renderer* renderer) {
	if (!map || !map->width || !map->height) {
		SDL_Log("Map data is invalid or dimensions are zero.");
		return;
	}

	cute_tiled_layer_t* temp_layer = layer;
	while (temp_layer) {
		if (!temp_layer->visible || !temp_layer->data) {
			temp_layer = temp_layer->next;
			continue;
		}

		// Draw the tiles normally
		for (int i = 0; i < map->height; i++) {
			for (int j = 0; j < map->width; j++) {
				int tile_index = i * map->width + j;
				int tile_id = temp_layer->data[tile_index];
				if (tile_id == 0) continue;

				Texture2* temp_texture = texture2;
				Texture2* texture_to_use = NULL;
				while (temp_texture) {
					if (tile_id >= temp_texture->firstgid &&
						tile_id <= temp_texture->firstgid + temp_texture->tilecount - 1) {
						texture_to_use = temp_texture;
						break;
					}
					temp_texture = temp_texture->next;
				}

				if (!texture_to_use || texture_to_use->tileset_width == 0) continue;

				int tileset_columns = texture_to_use->tileset_width / map->tilewidth;
				if (tileset_columns == 0) continue;

				SDL_FRect src = {
					(float)(((tile_id - texture_to_use->firstgid) % tileset_columns) * map->tilewidth),
					(float)(((tile_id - texture_to_use->firstgid) / tileset_columns) * map->tileheight),
					(float)map->tilewidth,
					(float)map->tileheight
				};

				SDL_FRect dst = {
					(float)(j * map->tilewidth - camera.x),
					(float)(i * map->tileheight - camera.y),
					(float)map->tilewidth,
					(float)map->tileheight
				};

				if (texture_to_use->texture2) {
					//SDL_SetTextureAlphaMod(texture_to_use->texture, 128);
					SDL_RenderTexture(renderer, texture_to_use->texture2, &src, &dst);
					//SDL_SetTextureAlphaMod(texture_to_use->texture, 255);
				}
			}
		}

		// Draw collision overlay if this is the collision layer
		if (strcmp(temp_layer->name.ptr, "Collision") == 0 && activeMapInstance2) {
			for (int y = 0; y < map->height; ++y) {
				for (int x = 0; x < map->width; ++x) {
					if (activeMapInstance2->getCollisionData(x, y) != 0) {
						SDL_FRect rect = {
							x * map->tilewidth - camera.x,
							y * map->tileheight - camera.y,
							(float)map->tilewidth,
							(float)map->tileheight
						};
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
						SDL_RenderFillRect(renderer, &rect);
					}
				}
			}
		}

		temp_layer = temp_layer->next;
	}


}

void Map2::init_map(SDL_Renderer* renderer) {
	while (true) {
		int map_index = find_entity("map");
		if (map_index == -1) break;
		destroy_entity(map_index);
	}

	activeMapInstance2 = this;
	const char map_path[] = "tiled/castle.json";
	map = cute_tiled_load_map_from_file(map_path, NULL);
	if (!map) {
		SDL_Log("Error loading the map from path: %s", map_path);
		return;
	}


	layer = map->layers;
	tileset = map->tilesets;
	if (!map->layers) {
		SDL_Log("Map layers are null!");
		return;
	}

	cute_tiled_layer_t* current_layer = map->layers;
	SDL_Log("Listing all layers in map:");
	for (cute_tiled_layer_t* l = map->layers; l; l = l->next) {
		SDL_Log("- Layer name: %s, width: %d, height: %d, visible: %d, has data: %d",
			l->name.ptr, l->width, l->height, l->visible, l->data != NULL);
	}
	while (current_layer) {
		SDL_Log("Found layer: %s", current_layer->name.ptr);

		if (current_layer->name.ptr && strcmp(current_layer->name.ptr, "Collision") == 0) {
			SDL_Log("Calling processCollisionLayer for layer: %s", current_layer->name.ptr);
			processCollisionLayer(current_layer);
			SDL_Log("Collision layer processed successfully");
		}

		if (!current_layer->visible) {
			SDL_Log("Layer %s is not visible, skipping", current_layer->name.ptr);
			current_layer = current_layer->next;
			continue;
		}

		current_layer = current_layer->next;
	}

	texture2 = (Texture2*)SDL_malloc(sizeof(Texture2));
	if (!texture2) {
		SDL_Log("Memory allocation failed for texture");
		return;
	}

	Texture2* current_texture = texture2;
	while (tileset) {
		SDL_Log("Attempting to load texture: %s", tileset->image.ptr);
		current_texture->texture2 = IMG_LoadTexture(renderer, tileset->image.ptr);
		if (!current_texture->texture2) {
			SDL_Log("Error loading texture for tileset");
			SDL_Log("SDL_image Error: %s", SDL_GetError());
			tileset = tileset->next;
			continue;
		}
		SDL_Log("Texture loaded successfully");
		SDL_SetTextureScaleMode(current_texture->texture2, SDL_SCALEMODE_NEAREST);

		current_texture->firstgid = tileset->firstgid;
		current_texture->tilecount = tileset->tilecount;
		current_texture->tileset_width = tileset->imagewidth;
		current_texture->tileset_height = tileset->imageheight;

		tileset = tileset->next;
		if (tileset) {
			current_texture->next = (Texture2*)SDL_malloc(sizeof(Texture));
			if (!current_texture->next) {
				SDL_Log("Memory allocation failed for new texture");
				break;
			}
			current_texture = current_texture->next;
		}
		else {
			current_texture->next = NULL;
		}
	}



	Entity map_e;
	strncpy_s(map_e.name, "map2", MAX_NAME_LENGTH - 1);
	map_e.render = render;
	map_e.cleanup = cleanup_map;
	map_e.handle_events = nullptr;
	map_e.update = nullptr;
	map_e.texture = nullptr;

	create_entity(map_e);
	currentMap = 0;
	int map_new_index = find_entity("map2");
	swap_entities(0, map_new_index);
}

void Map2::processCollisionLayer(cute_tiled_layer_t* layer) {
	if (!layer || !layer->data) {
		SDL_Log("ERROR: Invalid collision layer or data");
		return;
	}

	SDL_Log("Processing collision layer: %s", layer->name.ptr);
	SDL_Log("Layer width: %d, height: %d", layer->width, layer->height);
	if (!layer->data) {
		SDL_Log("ERROR: Layer data is NULL!");
		return;
	}

	collisionData.resize(layer->height);  // Rows (Y-axis)
	//SDL_Log("collisionData.resize to %zu rows", collisionData.size());
	for (int i = 0; i < layer->height; ++i) {
		collisionData[i].resize(layer->width);  // Columns (X-axis)
		//SDL_Log("Row %d resized to %zu columns", i, collisionData[i].size());

	}
	for (int y = 0; y < layer->height; ++y) {
		for (int x = 0; x < layer->width; ++x) {
			int data = layer->data[y * layer->width + x]; // Read data from the Tiled layer
			collisionData.at(y).at(x) = (data != 0) ? 1 : 0; // Assign it to our vector
			//SDL_Log("Tile (%d, %d) -> Collision Value: %d", x, y, data);
		}
	}

	//SDL_Log("Collision layer processed successfully, size: %d x %d", layer->width, layer->height);
	if (!collisionData.empty()) {
		SDL_Log("collisionData[0].size(): %zu", collisionData[0].size());
	}
	else {
		SDL_Log("collisionData[0] not accessible because collisionData is empty.");
	}
}

int Map2::getCollisionData(int x, int y) const {
	if (y < 0 || y >= static_cast<int>(collisionData.size())) {
		SDL_Log("ERROR: Y index out of bounds in collision data: y = %d", y);
		return 0;
	}

	if (x < 0 || x >= static_cast<int>(collisionData[y].size())) {
		SDL_Log("ERROR: X index out of bounds in collision data: x = %d", x);
		return 0;
	}

	int value = collisionData[y][x];
	return value;
}


int Map2::getWidth() const {
	return map ? map->width : 0;
}

int Map2::getHeight() const {
	return map ? map->height : 0;
}

int Map2::getCurrentMapId() const {
	return currentMap;
}



void Map2::loadMap(int map_id, SDL_Renderer* renderer) {
	//mapInstance.init_map(renderer);
	 //switch (map_id) {
	 //   case 0:
	mapInstance.init_map(renderer);
	//     break;
   //  case 1:
	//     mapInstance.init_map(renderer);
	//     break;
	// default:
	 //    SDL_Log("Unknown map ID: %d\n", map_id);
	//     break;
 //}
	mapInstance.currentMap = map_id;
}

#endif