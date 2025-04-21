#include "menu.h"

GameState current_game_state = STATE_MAIN_MENU;
OptionsOrigin options_origin;
menuUI menu_ui;
InGameState in_game_state;

static SDL_Surface* background_surface;
static SDL_Texture* background_texture = nullptr;


InventoryEntity inventoryEntity;



static std::vector<std::string> selecting_options = { "NEW GAME" ,"CONTINUE" , "OPTIONS" , "CREDITS", "EXIT" };
static std::vector<std::string> selecting_options_option = { "FULLSCREEN", "CHANGE VOLUME" };

bool fullscreen_enabled = false;
int options_select = 0;




int options_sub_select = 0; 
bool in_options_sub_menu = false;
SDL_Rect fullscreen_checkbox_rect; 



TTF_Font* g_menu_font = nullptr;



SDL_Color g_color_normal = { 200, 200, 200, 255 }; 
SDL_Color g_color_selected = { 255, 255, 0, 255 };   



void init_menu_ui(SDL_Renderer* renderer) {
    
    if (menu_ui.font != nullptr) return;
    menu_ui.menu_select = 0;
    menu_ui.selecting_options = { "NEW GAME" ,"CONTINUE" , "OPTIONS" , "CREDITS", "EXIT" };
    menu_ui.text_color = { 255, 255, 255, 255 };


    menu_ui.font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 20);
    if (!menu_ui.font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return;

    }

    background_surface = IMG_Load("assets/menu1.png");
    if (!background_surface) {
        SDL_Log("Failed to load background image %s", SDL_GetError());
    }
    else {
        SDL_Log("Background surface loaded: width=%d, height=%d, format=%s, pitch=%d",
            background_surface->w, background_surface->h,
            SDL_GetPixelFormatName(background_surface->format),
            background_surface->pitch);
        background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);
        if (!background_texture) {
            SDL_Log("Failed to create texture from background: %s", SDL_GetError());
        }
        SDL_Log(" created texture from background");
        SDL_DestroySurface(background_surface);
    }


}


void render_menu_ui(SDL_Renderer* renderer) {
    if (!menu_ui.font) {
        SDL_Log("No font loaded. Cannot render menu UI.");
        return;
    }
    if (background_texture) {
        SDL_FRect dest;
        SDL_GetCurrentRenderOutputSize(renderer, (int*)&dest.w, (int*)&dest.h);
        dest.x = 0;
        dest.y = 0;
        SDL_RenderTexture(renderer, background_texture, NULL, NULL);
        
    }
    if (background_texture) {
        SDL_FRect dest;
        SDL_GetCurrentRenderOutputSize(renderer, (int*)&dest.w, (int*)&dest.h);
        dest.x = 0;
        dest.y = 0;
        SDL_RenderTexture(renderer, background_texture, NULL, NULL);
    }
    TTF_Font* name_font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 20);
    if (!name_font) {
        SDL_Log("No font loaded. Cannot render credits.");
        return;
    }
    if (name_font) {
        TTF_SetFontStyle(name_font, TTF_STYLE_BOLD);
    }
    // Store the original text color
    SDL_Color original_text_color = menu_ui.text_color;

    std::vector<std::string> name_text = {
        "KNIGHT TO THE RESCUE",

    };

    int name_x = 70;
    int name_y = 40;
    for (const auto& line : name_text) {


        menu_ui.text_color = { 255, 255, 255, 255 }; 

        SDL_Surface* surface = TTF_RenderText_Solid(name_font, line.c_str(), 0, menu_ui.text_color);
        if (!surface) continue;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) { SDL_DestroySurface(surface); continue; }
        SDL_FRect dest = { (float)name_x, (float)name_y, (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        name_y += 30; 
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
    
    menu_ui.text_color = original_text_color;
    TTF_CloseFont(name_font); 


 



    int x = 150;
    int y = 100;
    for (int i = 0; i < menu_ui.selecting_options.size(); ++i) {
        const std::string& option = menu_ui.selecting_options[i];

        SDL_Surface* surface = TTF_RenderText_Solid(menu_ui.font, option.c_str(), 0, menu_ui.text_color);
        if (!surface) {
            SDL_Log("Failed to render text: %s", SDL_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_FRect dest = { (float)x, (float)(y + i * 30), (float)surface->w, (float)surface->h };

        if (i == menu_ui.menu_select) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150); 
            SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_RenderTexture(renderer, texture, NULL, &dest);

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
}



void update_menu_ui(const SDL_Event* event){
    if (event == nullptr) { 
        return; 
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_UP:
            menu_ui.menu_select = (menu_ui.menu_select - 1 + menu_ui.selecting_options.size()) % menu_ui.selecting_options.size();
            break;
        case SDLK_DOWN:
            menu_ui.menu_select = (menu_ui.menu_select + 1) % menu_ui.selecting_options.size();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            SDL_Log("You selected: %s", menu_ui.selecting_options[menu_ui.menu_select].c_str());
            if (menu_ui.selecting_options[menu_ui.menu_select] == "NEW GAME") {
                is_starting_new_game = true;
                map_transition_triggered = false;
                PLAYER.position.x = 100;  
                PLAYER.position.y = 100;
                PLAYER.current_hp = PLAYER.max_hp; 
                PLAYER.inventory.clear();           
                init_monster1(renderer);
                mapInstance.loadMap(0, renderer); 
                play_music("assets/map1.mp3");
                switch_game_state(STATE_INIT_GAME, renderer);
                reset_delta_time();
                cleanup_menu_ui();
                for (int i = 0; i < entities_count; ++i) {
                    if ( i == find_entity("phantom")) {
                        destroy_entity(i);
                        
                        i--; 
                    }
                }

            }
            else if (menu_ui.selecting_options[menu_ui.menu_select] == "CONTINUE") {
                map_transition_triggered = false;
                load_game(filename, renderer);
                current_game_state = STATE_INGAME;
                play_music("assets/map1.mp3");
                reset_delta_time();
                cleanup_menu_ui();
                if (current_map_id != 1) {
                    for (int i = 0; i < entities_count; ++i) {
                        if (i == find_entity("phantom") || i == find_entity("bomberplant")){
                            
                            destroy_entity(i);
                            Monster.cleanup();
                            i--; 
                        }
                    }
                }
                else if (current_map_id != 0) {
                    for (int i = 0; i < entities_count; ++i) {
                        if (i == find_entity("bomberplant")) {
                            destroy_entity(i);
                            i--; 
                        }
                    }
                }


            }
            else if (menu_ui.selecting_options[menu_ui.menu_select] == "OPTIONS") {
                options_origin = OPTIONS_FROM_MAIN_MENU;
                current_game_state = STATE_OPTIONS_MENU;
                reset_delta_time();
            }
            else if (menu_ui.selecting_options[menu_ui.menu_select] == "CREDITS") {
                current_game_state = STATE_CREDITS;
                reset_delta_time();
            }

            else if(menu_ui.selecting_options[menu_ui.menu_select] == "EXIT") {
                cleanup_menu_ui();
                current_game_state = STATE_QUIT;
            }

            break;
        default:
            break;
        }
    }

}





void cleanup_menu_ui() {
    if (menu_ui.font) {
        TTF_CloseFont(menu_ui.font);
        menu_ui.font = nullptr;
    }
    if (background_texture) {
        SDL_DestroyTexture(background_texture);
        background_texture = nullptr;
    }

    Mix_Quit();
}


void init_options_ui(SDL_Renderer* renderer) {
    SDL_Log("Initializing options UI...");

    if (!menu_ui.font) {
        menu_ui.font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 20);
        if (!menu_ui.font) {
            SDL_Log("Failed to load options menu font: %s", SDL_GetError());
        }
        else {
            SDL_Log("Options menu font loaded");
        }
    }

    menu_ui.text_color = { 255, 255, 255, 255 };
}

void render_options_ui(SDL_Renderer* renderer) {



    if (background_texture) {
        SDL_FRect dest;
        SDL_GetCurrentRenderOutputSize(renderer, (int*)&dest.w, (int*)&dest.h);
        dest.x = 0;
        dest.y = 0;
        SDL_RenderTexture(renderer, background_texture, NULL, NULL);
    }

    int x = 20;
    int y = 100;

    for (int i = 0; i < selecting_options_option.size(); ++i) {
        const std::string& option = selecting_options_option[i];
        SDL_Surface* surface = TTF_RenderText_Solid(menu_ui.font, option.c_str(), 0, menu_ui.text_color);
        if (!surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) { SDL_DestroySurface(surface); continue; }

        SDL_FRect dest = { (float)x, (float)(y + i * 30), (float)surface->w, (float)surface->h };

        if (i == options_select) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
            SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_RenderTexture(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

    // FULLSCREEN CHECKBOX
    if (options_select == 0) {
        int box_x = x + 200;
        int box_y = y;
        int box_size = 20;
        SDL_FRect fullscreen_checkbox_rect = { (float)box_x, (float)box_y, (float)box_size, (float)box_size };

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderRect(renderer, &fullscreen_checkbox_rect);

        if (fullscreen_enabled) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_FRect fill_rect = { fullscreen_checkbox_rect.x + 5, fullscreen_checkbox_rect.y + 5, fullscreen_checkbox_rect.w - 10, fullscreen_checkbox_rect.h - 10 };
            SDL_RenderFillRect(renderer, &fill_rect);
        }

        if (in_options_sub_menu && options_sub_select == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderRect(renderer, &fullscreen_checkbox_rect);
        }
    }

   
    if (options_select == 1) {
        int bar_x = x + volume_bar_x_offset;
        int bar_y = y + (options_select * 30) + volume_bar_y_offset;
        SDL_FRect volume_bar_rect = { (float)bar_x, (float)bar_y, (float)volume_bar_width, (float)volume_bar_height };

       
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &volume_bar_rect);

        
        int fill_width = (int)((float)volume_level / 100.0f * volume_bar_width);
        SDL_FRect fill_rect = { (float)bar_x, (float)bar_y, (float)fill_width, (float)volume_bar_height };
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        SDL_RenderFillRect(renderer, &fill_rect);

        
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderRect(renderer, &volume_bar_rect);

        
        if (in_volume_sub_menu) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderRect(renderer, &volume_bar_rect);
        }
    }
}

void update_options_ui(SDL_Renderer* renderer, SDL_Window* window,const SDL_Event* event) {
    if (event == nullptr) {
        return;
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_UP:
            options_select = (options_select - 1 + selecting_options_option.size()) % selecting_options_option.size();
            in_options_sub_menu = false; 
            options_sub_select = 0;
            break;
        case SDLK_DOWN:
            options_select = (options_select + 1) % selecting_options_option.size();
            in_options_sub_menu = false; 
            options_sub_select = 0;
            break;
        case SDLK_RIGHT:
            if (options_select == 0 && !in_options_sub_menu) {
                in_options_sub_menu = true;
                options_sub_select = 0;
            }
            else if (options_select == 1) {
                in_volume_sub_menu = true;
                volume_level = SDL_min(100, volume_level + 5); 
                Mix_VolumeMusic(volume_level * MIX_MAX_VOLUME / 100);
            }
            break;
        case SDLK_LEFT:
            if (options_select == 0) {
                in_options_sub_menu = false;
                options_sub_select = 0;
                
            }
            else if (options_select == 1) {
                in_volume_sub_menu = true;
                volume_level = SDL_max(0, volume_level - 5); 
                Mix_VolumeMusic(volume_level * MIX_MAX_VOLUME / 100);
            }
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (options_select == 0 && in_options_sub_menu && options_sub_select == 0) {
                fullscreen_enabled = !fullscreen_enabled;
                SDL_SetWindowFullscreen(window, fullscreen_enabled ? true : 0);
            }
            else if (options_select == 1) {
                in_volume_sub_menu = true;
            }
            break;
        case SDLK_ESCAPE:
            if (options_origin == OPTIONS_FROM_MAIN_MENU) {
                current_game_state = STATE_MAIN_MENU;
            }
            else if (options_origin == OPTIONS_FROM_PAUSE_MENU) {
                current_game_state = STATE_PAUSE_MENU;
            }
            break;
        default:
            break;
        }
    }
}



void render_credits_ui(SDL_Renderer* renderer) {
    if (background_texture) {
        SDL_FRect dest;
        SDL_GetCurrentRenderOutputSize(renderer, (int*)&dest.w, (int*)&dest.h);
        dest.x = 0;
        dest.y = 0;
        SDL_RenderTexture(renderer, background_texture, NULL, NULL);
    }
    TTF_Font* credits_font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 12);
    if (!credits_font) {
        SDL_Log("No font loaded. Cannot render credits.");
        return;
    }
    if (credits_font) {
        TTF_SetFontStyle(credits_font, TTF_STYLE_BOLD);
    }
    // Store the original text color
    SDL_Color original_text_color = menu_ui.text_color;

    std::vector<std::string> credits_text = {
        "CREDITS",
        "",
        "Programming: ",
        "NGUYEN THANH TUNG , DAM VAN TEU, DUONG CONG TRUC",
        "",
        "Art:",
        "DUONG CONG TRUC",
        "",
        "Music:",
        "credit to Yasunori Nishiki for music",
        "",
        "Press ESC to return"
    };

    int x = 10;
    int y = 20;
    for (const auto& line : credits_text) {

        if (line == "CREDITS") {
            menu_ui.text_color = { 255, 255, 255, 255 };
        }
        else if (line.find("Programming") != std::string::npos) {
            menu_ui.text_color = { 255, 255, 255, 255 };
        }
        else {
            menu_ui.text_color = { 255, 255, 255, 255 }; 
        }

        SDL_Surface* surface = TTF_RenderText_Solid(credits_font, line.c_str(), 0,menu_ui.text_color);
        if (!surface) continue;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) { SDL_DestroySurface(surface); continue; }
        SDL_FRect dest = { (float)x, (float)y, (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        y += 30; 
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

    menu_ui.text_color = original_text_color;
    TTF_CloseFont(credits_font); 
}

void update_credits_ui(const SDL_Event* event) {
    if (event == nullptr) {
        return;
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) {
            current_game_state = STATE_MAIN_MENU;
        }
    }
}


int get_current_game_state_id() {
    return current_game_state;
}


void switch_game_state(GameState new_state, SDL_Renderer* renderer) {

    current_game_state = new_state;
    SDL_Log("Switched to game state %d\n", new_state);

    switch (new_state) {
    case STATE_MAIN_MENU:
        
        break;
    case STATE_INIT_GAME:
        
        break;
    case STATE_INGAME:
        
        break;
    default:
        SDL_Log("Unknown game state: %d\n", new_state);
        break;
    }
}


InventoryEntity::InventoryEntity() : texture(nullptr), font(nullptr), selectedItemIndex(-1) {
    frect = { 0, 0, 0, 0 }; 
}

// Destructor
InventoryEntity::~InventoryEntity() {
    cleanup(); 
}

// Initialization
void InventoryEntity::init(SDL_Renderer* renderer, std::map<int, Item>& itemMap) {

    initializeItems(itemMap);
    PLAYER.item_data = itemMap;
    
    
    frect.x = 50;  
    frect.y = 50; 
    frect.w = 300; 
    frect.h = 200; 

    
    font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 10);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s\n", SDL_GetError());
     
    }
    selectedItemIndex = 0;
}

// Rendering
void InventoryEntity::render(SDL_Renderer* renderer) {
   
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &frect);

    if (font) {
        SDL_Color textColor = { 255, 255, 255, 255 };
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Inventory:", 0, textColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
            if (titleTexture) {
                SDL_FRect titleFRect = { frect.x + 10, frect.y + 10, titleSurface->w, titleSurface->h };
                SDL_RenderTexture(renderer, titleTexture, NULL, &titleFRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_DestroySurface(titleSurface);
        }

        int yOffset = frect.y + 40;

        for (size_t i = 0; i < PLAYER.inventory.size(); ++i) {
            int itemID = PLAYER.inventory[i];
            std::string itemName = "Invalid Item";
            if (PLAYER.item_data.find(itemID) != PLAYER.item_data.end()) { 
                itemName = "[" + std::to_string(i) + "] " + PLAYER.item_data[itemID].name; 
            }
            std::string itemText = itemName;
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, itemText.c_str(), 0, textColor);
            if (!textSurface) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_RenderText_Solid: %s\n", SDL_GetError());
                continue;
            }
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (!textTexture) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
                SDL_DestroySurface(textSurface);
                continue;
            }

            SDL_FRect textFRect = { frect.x + 10, yOffset, textSurface->w, textSurface->h };
            if (i == selectedItemIndex) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
                SDL_FRect highlightRect = textFRect;
                highlightRect.x -= 5;
                highlightRect.w += 10;
                SDL_RenderFillRect(renderer, &highlightRect);
            }
            SDL_RenderTexture(renderer, textTexture, NULL, &textFRect);

            SDL_DestroySurface(textSurface);
            SDL_DestroyTexture(textTexture);
            yOffset += 30;
        }
    }
    if (showSubScreen) {
        renderSubScreen(renderer);
    }
}

// Cleanup
void InventoryEntity::cleanup() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}



void InventoryEntity::handle_events(SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_DOWN:
            selectedItemIndex++;
            if (selectedItemIndex >= PLAYER.inventory.size()) {
                selectedItemIndex = PLAYER.inventory.size() - 1;
            }
            break;
        case SDLK_UP:
            selectedItemIndex--;
            if (selectedItemIndex < 0) {
                selectedItemIndex = 0;
            }
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (selectedItemIndex >= 0 && selectedItemIndex < PLAYER.inventory.size()) {
                selectedItemID = PLAYER.inventory[selectedItemIndex];
                SDL_Log("Use Item ID: %d ", selectedItemID);
                if (PLAYER.hasItem(selectedItemID)) {
                    itemUsage(selectedItemID, itemMap);
                    

                }
            }
            break;
        case SDLK_E: 
            if (selectedItemIndex >= 0 && selectedItemIndex < PLAYER.inventory.size()) {
                showSubScreen = true;
                selectedItemID = PLAYER.inventory[selectedItemIndex];
                SDL_Log("Show Subscreen for Item ID: %d", selectedItemID);
            }
            break;
        case SDLK_ESCAPE:
            showSubScreen = false;
            break;
        }
    }
}

void InventoryEntity::renderSubScreen(SDL_Renderer* renderer) {
   
    SDL_FRect subScreenRect;
    subScreenRect.x = frect.x + frect.w / 2 - 40; 
    subScreenRect.y = frect.y + frect.h / 2 - 75; 
    subScreenRect.w = 200;
    subScreenRect.h = 150;

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(renderer, &subScreenRect);

    
    if (font && selectedItemIndex >= 0 && selectedItemIndex < PLAYER.inventory.size()) {
        int itemID = PLAYER.inventory[selectedItemIndex];

        
        auto it = PLAYER.item_data.find(itemID);  
        if (it != PLAYER.item_data.end()) {
            const Item& item = it->second;

            SDL_Color textColor = { 255, 255, 255, 255 };

            SDL_Surface* descriptionSurface = TTF_RenderText_Solid_Wrapped(
                font, item.description.c_str(),0, textColor, subScreenRect.w - 20
            );

            if (descriptionSurface) {
                SDL_Texture* descriptionTexture = SDL_CreateTextureFromSurface(renderer, descriptionSurface);
                if (descriptionTexture) {
                    SDL_FRect descriptionRect = {
                        subScreenRect.x + 10,
                        subScreenRect.y + 10,
                        (float)descriptionSurface->w,
                        (float)descriptionSurface->h
                    };
                    SDL_RenderTexture(renderer, descriptionTexture, NULL, &descriptionRect);
                    SDL_DestroyTexture(descriptionTexture);
                }
                SDL_DestroySurface(descriptionSurface);
            }

            // Render "Return" and "Use" options
            SDL_Surface* returnSurface = TTF_RenderText_Solid(font, "Return",0, textColor);
            SDL_Surface* useSurface = TTF_RenderText_Solid(font, "Use",0, textColor);
            if (returnSurface && useSurface) {
                SDL_Texture* returnTexture = SDL_CreateTextureFromSurface(renderer, returnSurface);
                SDL_Texture* useTexture = SDL_CreateTextureFromSurface(renderer, useSurface);
                if (returnTexture && useTexture) {
                    SDL_FRect returnRect = {
                        subScreenRect.x + 10,
                        subScreenRect.y + subScreenRect.h - 40,
                        (float)returnSurface->w,
                        (float)returnSurface->h
                    };
                    SDL_FRect useRect = {
                        subScreenRect.x + subScreenRect.w - useSurface->w - 10,
                        subScreenRect.y + subScreenRect.h - 40,
                        (float)useSurface->w,
                        (float)useSurface->h
                    };

                    SDL_RenderTexture(renderer, returnTexture, NULL, &returnRect);
                    SDL_RenderTexture(renderer, useTexture, NULL, &useRect);

                    SDL_DestroyTexture(returnTexture);
                    SDL_DestroyTexture(useTexture);
                }
                SDL_DestroySurface(returnSurface);
                SDL_DestroySurface(useSurface);
            }
        }
        else {
            SDL_Log("Item ID %d not found in item map!", itemID);
        }
    }
}


void render_end_game(SDL_Renderer* renderer) {
   
    SDL_Texture* bg = IMG_LoadTexture(renderer, "assets/menu.png");
    if (!bg) {
        SDL_Log("Failed to load end screen background: %s", SDL_GetError());
        return;
    }
    SDL_RenderTexture(renderer, bg, NULL, NULL);
    SDL_DestroyTexture(bg);
    TTF_Font* credits_font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 12);
    if (!credits_font) {
        SDL_Log("No font loaded. Cannot render credits.");
        return;
    }

    SDL_Color original_text_color = menu_ui.text_color;

    std::vector<std::string> credits_text = {
        "ONLY ENDING???",
        "",
        "IT SEEMS LIKE THERE IS NO PRINCESS HERE ",
        "HE(?) THOUGHT EVERYBODY SAID THIS IS",
        "",
        "THE PLACE",
        "BUT EVERYTHING REMAINS A MYSTERY",
        "",
        "",
        "",
        "",
        "Press ALT+F4 to ehe"
    };

    int x = 50;
    int y = 20;
    for (const auto& line : credits_text) {


        menu_ui.text_color = { 0, 0, 0, 255 }; 


        SDL_Surface* surface = TTF_RenderText_Solid(credits_font, line.c_str(), 0, menu_ui.text_color);
        if (!surface) continue;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) { SDL_DestroySurface(surface); continue; }
        SDL_FRect dest = { (float)x, (float)y, (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        y += 30; 
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
    
    menu_ui.text_color = original_text_color;
    TTF_CloseFont(credits_font);
}

void render_end_game_lost(SDL_Renderer* renderer) {
    SDL_Texture* bg = IMG_LoadTexture(renderer, "assets/menu.png");
    if (!bg) {
        SDL_Log("Failed to load end screen background: %s", SDL_GetError());
        return;
    }
    SDL_RenderTexture(renderer, bg, NULL, NULL);
    SDL_DestroyTexture(bg);

    TTF_Font* credits_font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 12);
    if (!credits_font) {
        SDL_Log("No font loaded. Cannot render credits.");
        return;
    }
    
    SDL_Color original_text_color = menu_ui.text_color;

    std::vector<std::string> credits_text = {
        "",
        "",
        " ",
        "",
        "",
        "IT SEEM LIKE YOU ARE NOT WORTHY",
        "",
        "",
        "",
        "",
        "",
        "Press ALT+F4 to ehe"
    };

    int x = 50;
    int y = 20;
    for (const auto& line : credits_text) {


        menu_ui.text_color = { 0, 0, 0, 255 }; 


        SDL_Surface* surface = TTF_RenderText_Solid(credits_font, line.c_str(), 0, menu_ui.text_color);
        if (!surface) continue;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) { SDL_DestroySurface(surface); continue; }
        SDL_FRect dest = { (float)x, (float)y, (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        y += 30; 
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
   
    menu_ui.text_color = original_text_color;
    TTF_CloseFont(credits_font);
}