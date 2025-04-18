#include "menu.h"

GameState current_game_state = STATE_MAIN_MENU;
OptionsOrigin options_origin;
menuUI menu_ui;

static SDL_Surface* background_surface;
static SDL_Texture* background_texture = nullptr;







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
    play_music("assets/OCTOPATH TRAVELER - opening menu-.mp3");
    if (menu_ui.font != nullptr) return;
    menu_ui.menu_select = 0;
    menu_ui.selecting_options = { "NEW GAME" ,"CONTINUE" , "OPTIONS" , "CREDITS", "EXIT" };
    menu_ui.text_color = { 255, 255, 255, 255 };


    menu_ui.font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 20);
    if (!menu_ui.font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return;

    }

    background_surface = IMG_Load("assets/menu.png");
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

    int x = 150;
    int y = 60;
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
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150); // yellow highlight
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

                PLAYER.position.x = 100;  
                PLAYER.position.y = 100;
                PLAYER.current_hp = PLAYER.max_hp; 
                PLAYER.inventory.clear();           

                mapInstance.loadMap(0, renderer); 

                switch_game_state(STATE_INIT_GAME, renderer);
                reset_delta_time();
                cleanup_menu_ui();

            }
            else if (menu_ui.selecting_options[menu_ui.menu_select] == "CONTINUE") {
                load_game(filename, renderer);
                current_game_state = STATE_INGAME;
                reset_delta_time();
                cleanup_menu_ui();


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

    // ✅ VOLUME BAR - Always render if option selected
    if (options_select == 1) {
        int bar_x = x + volume_bar_x_offset;
        int bar_y = y + (options_select * 30) + volume_bar_y_offset;
        SDL_FRect volume_bar_rect = { (float)bar_x, (float)bar_y, (float)volume_bar_width, (float)volume_bar_height };

        // Bar background
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &volume_bar_rect);

        // Volume fill
        int fill_width = (int)((float)volume_level / 100.0f * volume_bar_width);
        SDL_FRect fill_rect = { (float)bar_x, (float)bar_y, (float)fill_width, (float)volume_bar_height };
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        SDL_RenderFillRect(renderer, &fill_rect);

        // Border
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderRect(renderer, &volume_bar_rect);

        // Highlight if volume menu is active
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
            in_options_sub_menu = false; // Reset sub-menu when navigating options
            options_sub_select = 0;
            break;
        case SDLK_DOWN:
            options_select = (options_select + 1) % selecting_options_option.size();
            in_options_sub_menu = false; // Reset sub-menu when navigating options
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
    menu_ui.text_color = { 255 , 0 , 0 , 255 };

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

    int x = 50;
    int y = 20;
    for (const auto& line : credits_text) {

            if (line == "CREDITS") {
                menu_ui.text_color = { 255 , 0 , 0 , 255 };
            }
            else if (line.find("Programming") != std::string::npos) {
                menu_ui.text_color = { 255 , 0 , 0 , 255 };
            }
            else {
                menu_ui.text_color = { 255 , 0 , 0 , 255 };
            }



        SDL_Surface* surface = TTF_RenderText_Solid(credits_font, line.c_str(),0, menu_ui.text_color);
        if (!surface) continue;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) { SDL_DestroySurface(surface); continue; }
        SDL_FRect dest = { (float)x, (float)y, (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        y += 30; // Move down for the next line
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
}

void update_credits_ui(const SDL_Event* event) {
    if (event == nullptr) {
        return;
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) {
            current_game_state = STATE_MAIN_MENU; // Go back to the main menu
        }
    }
}


int get_current_game_state_id() {
    return current_game_state;
}


void switch_game_state(GameState new_state, SDL_Renderer* renderer) {
    // For now, just set the global state.  In a more complex game, you would
    //  deallocate the old state and allocate the new state.  You might also
    //  need to initialize the new state (e.g., load a level).
    current_game_state = new_state;
    SDL_Log("Switched to game state %d\n", new_state);
    // Here you would also call any necessary initialization for the new game state.
    // For example, if new_state is the ID of a level, you would load the level here.
    switch (new_state) {
    case STATE_MAIN_MENU:
        // Initialize first state
        break;
    case STATE_INIT_GAME:
        // Initialize second state
        break;
    case STATE_INGAME:
        // Initialize third state
        break;
    default:
        SDL_Log("Unknown game state: %d\n", new_state);
        break;
    }
}

