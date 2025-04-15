#include "menu.h"

firstState current_game_state = STATE_MAIN_MENU;

menuUI menu_ui;


static SDL_Texture* background_texture = nullptr;
static std::vector<std::string> selecting_options = { "NEW GAME" ,"CONTINUE" , "OPTIONS" , "CREDITS", "EXIT" };

int menu_select = 0;

TTF_Font* g_menu_font = nullptr;

SDL_Color g_color_normal = { 200, 200, 200, 255 }; 
SDL_Color g_color_selected = { 255, 255, 0, 255 };   



void init_menu_ui(SDL_Renderer* renderer) {
    menu_ui.menu_select = 0;
    menu_ui.selecting_options = { "NEW GAME" ,"CONTINUE" , "OPTIONS" , "CREDITS", "EXIT" };
    menu_ui.text_color = { 255, 255, 255, 255 };


    menu_ui.font = TTF_OpenFont("C:/Users/Admin/Downloads/Open_Sans/static/OpenSans-Regular.ttf", 20);
    if (!menu_ui.font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return;

    }

    SDL_Surface* background_surface = IMG_Load("assets/menu.png");
    if (!background_surface) {
        SDL_Log("Failed to load background image %s", SDL_GetError());
    }
    else {
        background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);
        if (!background_texture) {
            SDL_Log("Failed to create texture from background: %s", SDL_GetError());
        }
        SDL_Log(" created texture from background");
        SDL_DestroySurface(background_surface);
    }

    current_game_state = STATE_MAIN_MENU; 
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Menu UI Initialized.");
}


void render_menu_ui(SDL_Renderer* renderer) {
    if (!menu_ui.font) {
        SDL_Log("No font loaded. Cannot render battle UI.");
        return;
    }
    if (background_texture) {
        SDL_FRect dest;
        SDL_GetCurrentRenderOutputSize(renderer, (int*)&dest.w, (int*)&dest.h);
        dest.x = 0;
        dest.y = 0;
        SDL_RenderTexture(renderer, background_texture, NULL, &dest);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect panel = { 10, 150, 200, 150 };
    SDL_RenderFillRect(renderer, &panel);
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
    if (event == nullptr) { //  Check if event is NULL
        return; //  IMPORTANT:  Return immediately if event is NULL
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
                current_game_state = STATE_INIT_GAME;

            }
            if (menu_ui.selecting_options[menu_ui.menu_select] == "EXIT") {
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
}