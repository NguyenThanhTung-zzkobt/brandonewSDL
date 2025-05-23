#include "music.h"

static Mix_Music* music = nullptr;

bool is_starting_new_game = false;
int volume_level = 100;
SDL_Rect volume_bar_rect;
int volume_bar_width = 200;
int volume_bar_height = 20;
int volume_bar_x_offset = 175;
int volume_bar_y_offset = 8;
bool in_volume_sub_menu = false;
SDL_AudioSpec audiospec = {};
int menu_select = 0;


#ifdef DISABLED

void init_music() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) { 
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
        return; 
    }
    audiospec.format = MIX_DEFAULT_FORMAT;
    audiospec.channels = MIX_DEFAULT_CHANNELS;
    audiospec.freq = MIX_DEFAULT_FREQUENCY;

    if (!Mix_OpenAudio(0, &audiospec)) {
        SDL_Log("Error opening audio: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(0);
        return;
    }
    else {
        if (current_game_state == STATE_MAIN_MENU) {
        music = Mix_LoadMUS("assets/OCTOPATH TRAVELER - opening menu-.mp3");
        }
        else if(current_game_state == STATE_INGAME) {
            if (!is_in_battle) {
                music = Mix_LoadMUS("assets/map1.mp3");
            }
            else {
                music = Mix_LoadMUS("assets/battle.mp3");
            }
        }

    }

    if (!music) {
        SDL_Log("Failed to load music: %s\n", SDL_GetError());
    }
    else {
        if (Mix_PlayMusic(music, -1) == -1) {
            SDL_Log("Failed to play music: %s\n", SDL_GetError());
        }
    }


    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Menu UI Initialized.");
}

void cleanup_music() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    SDL_CloseAudioDevice(0);
}
#endif

void init_music() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) { 
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
        return; 
    }
    audiospec.format = MIX_DEFAULT_FORMAT;
    audiospec.channels = MIX_DEFAULT_CHANNELS;
    audiospec.freq = MIX_DEFAULT_FREQUENCY;

    if (Mix_OpenAudio(0, &audiospec) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error opening audio: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(0);
        return; 
    }
    volume_level = 100;
    Mix_VolumeMusic(volume_level);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Audio Initialized.");

    switch (current_game_state) {
    case STATE_MAIN_MENU:
        render_menu_ui(renderer);
        break;
    case STATE_OPTIONS_MENU:
        render_options_ui(renderer);
        break;
    case STATE_CREDITS:
        render_credits_ui(renderer);
        break;
    case STATE_INIT_GAME:
        
        break;
    case STATE_INGAME:
        if (is_in_battle()) {
            render_battle(renderer);
        }
        else if (in_game_state == InGameState::INVENTORY) {
            inventoryEntity.render(renderer);
            if (inventoryEntity.showSubScreen) {
                inventoryEntity.renderSubScreen(renderer);
            }
        }
        else {
            for (int i = 0; i < entities_count; i++) {
                if (entities[i].render != nullptr && entities[i].render != (void(*)(SDL_Renderer*))0xcccccccccccccccc) {
                    entities[i].render(renderer);
                }
            }
        }
        break;
    case STATE_PAUSE_MENU:

        break;
    case END_GAME_WON:
        render_end_game(renderer);
    default:
        break;
    }
    play_music("assets/OCTOPATH TRAVELER - opening menu-.mp3");
}

void cleanup_music() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    Mix_CloseAudio(); 
    SDL_QuitSubSystem(SDL_INIT_AUDIO); 
}

void play_music(const char* filename) {
    if (music) {
        Mix_FreeMusic(music); 
        music = nullptr;
    }

    music = Mix_LoadMUS(filename);
    if (!music) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s\n", SDL_GetError());
        return; 
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to play music: %s\n", SDL_GetError());
    }
}


