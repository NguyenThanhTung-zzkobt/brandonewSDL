#include "pause.h"



std::vector<std::string> PauseMenuButton = { "CONTINUE", "SAVE GAME", "MAIN MENU" };

pauseUI pause_ui;


static SDL_Surface* pause_surface;
static SDL_Texture* pause_menu_texture = nullptr;



SDL_Color p_color_normal = { 200, 200, 200, 255 };
SDL_Color p_color_selected = { 255, 255, 0, 255 };




void init_pause_menu_ui(SDL_Renderer* renderer) {
    SDL_Log("Initializing pause menu UI...");
    pause_ui.pause_select = 0;
    pause_ui.selecting_options = { "CONTINUE", "SAVE GAME", "MAIN MENU" };
    pause_ui.pause_text_color = { 255, 255, 255, 255 };

    pause_ui.pause_font = TTF_OpenFont("external/Open_Sans/static/OpenSans-Regular.ttf", 20);
    if (!pause_ui.pause_font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return;

    }

     pause_menu_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 715, 510);
    if (!pause_menu_texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error: %s", SDL_GetError());
        return;
    }
     SDL_Log(" created texture pause");

    // Set the texture as the rendering target.  This is needed before SDL_FillSurfaceRect.
    SDL_SetRenderTarget(renderer, pause_menu_texture);

    // Set the blend mode for the texture.  This is essential for alpha to work.
    SDL_SetTextureBlendMode(pause_menu_texture, SDL_BLENDMODE_BLEND);

    // Create a semi-transparent black color.
    Uint32 color = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32),NULL, 0, 0, 0, 128);

    // Create a surface.
    SDL_Surface* pause_surface = SDL_CreateSurface(715, 510, SDL_PIXELFORMAT_RGBA32);
      if (!pause_surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateSurface Error: %s", SDL_GetError());
        return;
    }
     SDL_Log(" surface loaded");
    SDL_FillSurfaceRect(pause_surface, NULL, color);

     SDL_BlitSurface(pause_surface, NULL, pause_surface, NULL);
     SDL_Texture* temp_texture = SDL_CreateTextureFromSurface(renderer, pause_surface);
      if (!temp_texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }
    SDL_RenderTexture(renderer, temp_texture, NULL, NULL);

    SDL_DestroySurface(pause_surface);
    SDL_DestroyTexture(temp_texture);
    SDL_SetRenderTarget(renderer, NULL); // VERY IMPORTANT:  Reset the render target!

    SDL_Log("Pause menu UI initialized.");
}


void render_pause_menu_ui(SDL_Renderer* renderer) {
    if (!pause_ui.pause_font) {
        SDL_Log("No font loaded. Cannot render pause UI.");
        return;
    }

    
    if (pause_menu_texture) {
        SDL_FRect dest;
        SDL_GetCurrentRenderOutputSize(renderer, (int*)&dest.w, (int*)&dest.h);
        dest.x = 0;
        dest.y = 0;
        SDL_SetTextureAlphaMod(pause_menu_texture, 128);
        SDL_RenderTexture(renderer, pause_menu_texture, NULL, &dest);
        SDL_SetTextureAlphaMod(pause_menu_texture, 255);
    }

    int x = 150;
    int y = 60;
    for (int i = 0; i < pause_ui.selecting_options.size(); ++i) {
        const std::string& option = pause_ui.selecting_options[i];

        SDL_Surface* surface = TTF_RenderText_Solid(pause_ui.pause_font, option.c_str(), 0, pause_ui.pause_text_color);
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

        if (i == pause_ui.pause_select) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150); // yellow highlight
            SDL_FRect highlight = { dest.x - 5, dest.y - 2, dest.w + 10, dest.h + 4 };
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_RenderTexture(renderer, texture, NULL, &dest);

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

}







void update_pause_menu_ui(SDL_Event* event) {

    if (event == nullptr) {
        return; 
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_UP:
            pause_ui.pause_select = (pause_ui.pause_select - 1 + pause_ui.selecting_options.size()) % pause_ui.selecting_options.size();
            break;
        case SDLK_DOWN:
            pause_ui.pause_select = (pause_ui.pause_select + 1) % pause_ui.selecting_options.size();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            SDL_Log("You selected: %s", pause_ui.selecting_options[pause_ui.pause_select].c_str());
            if (pause_ui.selecting_options[pause_ui.pause_select] == "CONTINUE") {
                current_game_state = STATE_INGAME;
                cleanup_pause_menu_ui();
                reset_delta_time();

            }
            else if (pause_ui.selecting_options[pause_ui.pause_select] == "SAVE GAME") {
                save_game(filename);
                reset_delta_time();
                
            }

            else if (pause_ui.selecting_options[pause_ui.pause_select] == "MAIN MENU") {
                // Reset player data
                PLAYER.position.x = 100;  // Or your initial position
                PLAYER.position.y = 100;
                PLAYER.current_hp = PLAYER.max_hp;
                PLAYER.inventory.clear();
                play_music("assets/OCTOPATH TRAVELER - opening menu-.mp3");
                // Destroy specific monster entities
                for (int i = 0; i < entities_count; ++i) {
                    if (i == find_entity("bomberplant") || i == find_entity("phantom")) {
                        destroy_entity(i);
                        i--; // Important: Adjust index after destroying
                    }
                }

                current_game_state = STATE_MAIN_MENU;
                cleanup_pause_menu_ui();
            }


            break;
        default:
            break;
        }
    }
}

void cleanup_pause_menu_ui() {
    SDL_Log("Cleaning up pause menu UI");
    if (pause_menu_texture) {
        SDL_DestroyTexture(pause_menu_texture);
        pause_menu_texture = NULL;
    }
    if (pause_ui.pause_font) {
        TTF_CloseFont(pause_ui.pause_font);
        pause_ui.pause_font = NULL;
    }
    SDL_Log("Pause menu UI cleaned up");
}