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
    if (SDL_Init(SDL_INIT_AUDIO) < 0) { //Initialize the audio subsystem
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
        return; // Important:  Return on error.  Don't continue.
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
    if (SDL_Init(SDL_INIT_AUDIO) < 0) { //Initialize the audio subsystem
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
        return; // Important:  Return on error.  Don't continue.
    }
    audiospec.format = MIX_DEFAULT_FORMAT;
    audiospec.channels = MIX_DEFAULT_CHANNELS;
    audiospec.freq = MIX_DEFAULT_FREQUENCY;

    if (Mix_OpenAudio(0, &audiospec) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error opening audio: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(0);
        return; // Important: Return on error
    }
    volume_level = 100;
    Mix_VolumeMusic(volume_level);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Audio Initialized.");
}

void cleanup_music() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    Mix_CloseAudio(); // Close the SDL_Mixer
    SDL_QuitSubSystem(SDL_INIT_AUDIO); // Quit the audio subsystem.
}

void play_music(const char* filename) {
    if (music) {
        Mix_FreeMusic(music); //Free the current music
        music = nullptr;
    }

    music = Mix_LoadMUS(filename);
    if (!music) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s\n", SDL_GetError());
        return; // Important:  Return on error.
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to play music: %s\n", SDL_GetError());
    }
}


