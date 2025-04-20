#pragma once



#include "SDL3/SDL.h"
#include "vector"
#include "string"
#include "map"
#include "player.h"


class Item {
public:
	std::string name;
	int id;
	std::string description;
	///std::vector<int> items;

	Item(const std::string& name, int id, const std::string& description)
		: name(name), id(id), description(description) {
	}
	Item() : name(""), id(0), description("") {}
	~Item() {

	}

	void display();
};




extern std::vector<Item> initializeItems(std::map<int, Item>& itemMap);
extern std::map<int, Item> itemMap;
extern std::vector<Item> allItems;


extern void itemUsage(int itemId, std::map<int, Item> itemMap);