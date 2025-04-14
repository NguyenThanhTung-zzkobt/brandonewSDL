#include "menu.h"

firstState current_game_state = STATE_MAIN_MENU;

static std::vector<std::string> selecting_option = { "NEW GAME" ,"CONTINUE" , "OPTIONS" , "CREDITS", "EXIT" };