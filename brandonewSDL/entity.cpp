
#include "entity.h"

Entity entities[MAX_ENTITIES];
int entities_count = 0;
extern bool is_in_first_map = true;

void create_entity(Entity entity) {

	if (entities_count < MAX_ENTITIES) {
		entities[entities_count++] = entity;
	}
	else {
		SDL_Log("Maxium number of entities reached\n");
	}
	SDL_Log("Entity list before rendering:");
	for (auto& e : entities) {
		SDL_Log("Entity type: %s, pos x=%.2f y=%.2f", e.name, e.position.x, e.position.y);
	}
}

void destroy_entity(int index) {
	if (index < 0 || index >= entities_count) {
		SDL_Log("Invalid index\n");
		return;
	}

	entities[index].cleanup();


	for (int i = index; i < entities_count - 1; i++) {
		entities[i] = entities[i + 1];	
	}
	entities[entities_count - 1] = Entity{};
	entities_count--;
}


void swap_entities(int index1, int index2) {
	if (index1 < 0 || index1 >= MAX_ENTITIES || index2 < 0 || index2 >= MAX_ENTITIES) {
		SDL_Log("Invalid index %d and %d\n", index1, index2);
		return;
	}
	Entity temp = entities[index1];
	entities[index1] = entities[index2];
	entities[index2] = temp;
}

int find_entity(const char* name) {
	for (int i = 0; i < entities_count; i++) {
		if (strcmp(entities[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}