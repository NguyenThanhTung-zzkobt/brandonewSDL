#pragma once

#include <string>
#include <map>
#include <vector>
#include <SDl3/SDL.h>
#include <fstream>
#include <sstream>

struct AnimationFrame {
    SDL_Rect rect;
    float duration;
};

struct Animation {
    std::vector<AnimationFrame> frames;
    bool loop;
};

class Animated {
public:
    Animated();

    void setTexture(SDL_Texture* tex);
    void loadAnimations(const std::string& file);
    void setAnimation(const std::string& name);
    void update(float delta_time);
    void applyTo(SDL_FRect& target_rect);
    std::string getCurrentAnimationName() const;
    const SDL_Rect* getCurrentFrameRect() const;
    void cleanup();
    SDL_Texture* getTexture() const;


private:
    SDL_Texture* texture;
    std::map<std::string, Animation> animations;
    std::string current_animation;
    int current_frame;
    float time_accumulator;
};
