#include "item.h"


std::map<int, Item> itemMap;
std::vector<Item> items;




std::vector<Item> initializeItems(std::map<int, Item>& itemMap) {
	


	Item healing_potion("Healing potion", 1, "A handful of medicine created by the Greatest Magest ever known to be existed in man history PUCCI");
	items.push_back(healing_potion);
	itemMap[healing_potion.id] = healing_potion;

	Item cure_status_potion("Cure status potion", 2, "A status healing concoction developed by current era mages to fight back bad status");
	items.push_back(cure_status_potion);
	itemMap[cure_status_potion.id] = cure_status_potion;

	Item miracle_fusion("Miracle fusion", 3, "A heroic mixture of bravery and strength to restore anything!! \"ONLY A TRUE HERO KNOW WHAT THIS IS\"");
	items.push_back(miracle_fusion);
	itemMap[miracle_fusion.id] = miracle_fusion;


	Item smol_mixture("Smol mixture ???", 4, "A unknown thing , maybe it have a certain usage");
	items.push_back(smol_mixture);
	itemMap[smol_mixture.id] = smol_mixture;

	return items;
}


std::vector<Item> allItems = initializeItems(itemMap);


void itemUsage(int itemId, std::map<int, Item> itemMap) {
	auto it = itemMap.find(itemId);
	if (it != itemMap.end()) {
		const Item& item = it->second;
		SDL_Log("Item: %s\n", item.name.c_str()); // Corrected
		//  Add specific usage logic based on item ID.
		switch (itemId) {
		case 1: // Healing potion
			SDL_Log("  You drink the healing potion.  You feel better!"); // Corrected
			if (PLAYER.entity.current_hp + 50 >= PLAYER.entity.max_hp) {
				PLAYER.entity.current_hp = PLAYER.entity.max_hp;
				SDL_Log("  HP increased  to %.2f", PLAYER.entity.current_hp);
				PLAYER.removeItem(1, itemMap);
				break;
			}
			else {
				PLAYER.entity.current_hp += 50;
				SDL_Log("  HP increased  to %.2f", PLAYER.entity.current_hp);
				PLAYER.removeItem(1, itemMap);
				break;
			}
			
		case 2: // Cure status potion
			SDL_Log("  You drink the cure status potion.  Your status effects are gone!"); 
			PLAYER.entity.active_status.type = StatusEffect::Type::NONE;
			PLAYER.removeItem(2, itemMap);
			
			break;
		case 3: // Miracle fusion
			SDL_Log(" You feel a surge of power!"); 
			PLAYER.entity.current_hp = PLAYER.entity.max_hp;
			SDL_Log("  HP increased  to %.2f", PLAYER.entity.current_hp);
			PLAYER.entity.active_status.type = StatusEffect::Type::NONE;
			PLAYER.removeItem(3, itemMap);
			break;
		case 4: // Smol mixture
			SDL_Log(" Something happened but you did not know what it is."); // Corrected
			PLAYER.removeItem(4, itemMap);
			break;
		default:
			SDL_Log("  No special action for this item."); // Corrected
			break;
		}
	}
	else {
		SDL_Log("Item ID %d not found!", itemId); // Corrected
	}
}

