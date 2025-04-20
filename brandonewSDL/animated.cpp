#include "animated.h"
#include <fstream>
#include <sstream>

Animated::Animated() {
    texture = nullptr;
    current_frame = 0;
    time_accumulator = 0.0f;
}

void Animated::setTexture(SDL_Texture* tex) {
    texture = tex;
}

void Animated::loadAnimations(const std::string& file) {
    std::ifstream infile(file);
    if (!infile.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open animation file: %s", file.c_str());
        return;
    }
    SDL_Log("Opened animation file: %s", file.c_str());

    std::string line;
    Animation* currentAnim = nullptr;

    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;  // ✅ Bỏ qua dòng trống và dòng comment

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "animation") {
            std::string name;
            std::string loop_str;
            iss >> name >> loop_str;
            animations[name] = Animation();
            animations[name].loop = (loop_str == "true");
            currentAnim = &animations[name];

            SDL_Log("Loaded animation: %s (loop=%s)", name.c_str(), loop_str.c_str());  // ✅ Log để kiểm tra
        }
        else if (token == "frame" && currentAnim) {
            int x, y, w, h;
            float duration;
            iss >> x >> y >> w >> h >> duration;
            currentAnim->frames.push_back({ SDL_Rect{ x, y, w, h }, duration });
        }
    }

    if (animations.empty()) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "No animations were loaded from file: %s", file.c_str());
    }
}

void Animated::setAnimation(const std::string& name) {
    if (current_animation == name || animations.find(name) == animations.end()) return;
    current_animation = name;
    current_frame = 0;
    time_accumulator = 0.0f;
}

void Animated::update(float delta_time) {
    if (animations.find(current_animation) == animations.end()) return;
    const auto& anim = animations[current_animation];

    time_accumulator += delta_time;
    while (time_accumulator >= anim.frames[current_frame].duration) {
        time_accumulator -= anim.frames[current_frame].duration;
        current_frame++;

        if (current_frame >= anim.frames.size()) {
            if (anim.loop) {
                current_frame = 0;
            }
            else {
                current_frame = anim.frames.size() - 1;
                break;
            }
        }
    }
}

void Animated::applyTo(SDL_FRect& target_rect) {
    if (animations.find(current_animation) == animations.end()) return;
    SDL_Rect frame = animations[current_animation].frames[current_frame].rect;
    target_rect.w = (float)frame.w;
    target_rect.h = (float)frame.h;
    target_rect.x = (float)frame.x;
    target_rect.y = (float)frame.y;
}

std::string Animated::getCurrentAnimationName() const {
    return current_animation;
}

void Animated::cleanup() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

SDL_Texture* Animated::getTexture() const {
    return texture;
}

const SDL_Rect* Animated::getCurrentFrameRect() const {
    auto it = animations.find(current_animation);
    if (it == animations.end()) return nullptr;
    if (current_frame >= it->second.frames.size()) return nullptr;
    return &it->second.frames[current_frame].rect;
}
