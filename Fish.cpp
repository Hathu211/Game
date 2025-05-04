#include "Fish.h"
#include <cmath>
#include <iostream>

Fish::Fish(int x, int y, int w, int h, int speed, SDL_Renderer* renderer, const char* imageFish, int worldW, int worldH) {
    this->renderer = renderer;
    this->worldW = worldW;
    this->worldH = worldH;
    flip = SDL_FLIP_NONE;
    this->speed = speed;
    rect = { x,y,w,h };
    targetX = x + w / 2;
    targetY = y + h / 2;

    SDL_Surface* surface = IMG_Load(imageFish);
    if (!surface) {
        std::cerr << "Failed to load fish image: " << IMG_GetError() << std::endl;
        exit(1);
    }
    else {
        textureFish = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!textureFish) {
            std::cerr << "Failed to create fish texture: " << SDL_GetError() << std::endl;
            exit(1);
        }
    }
}

void Fish::setTarget(int x, int y) {
    targetX = x;
    targetY = y;
}

void Fish::move(bool isKick) {
    if (!isKick) return;
    float dx = targetX - (rect.x + rect.w / 2);
    float dy = targetY - (rect.y + rect.h / 2);
    flip = (dy < 0) ? SDL_FLIP_VERTICAL : SDL_FLIP_HORIZONTAL; //cap nhat lat theo Oy

    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0f) return;

    double targetAngle = std::atan2(dy, dx) * 180.0 / M_PI;
    double diff = targetAngle - angle;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    double maxA = 4.0;
    if (diff > maxA) diff = maxA;
    else if (diff < -maxA) diff = -maxA;
    angle += diff;

    float vel = speed / 55.0f;
    rect.x += static_cast<int>((dx / dist) * vel);
    rect.y += static_cast<int>((dy / dist) * vel);

    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > worldW) rect.x = worldW  - rect.w;
    const int topLimit = worldH - 550; //day tren ca window 550 pixel
    const int bottomEdge = worldH; //day duoi co th cham day
    if (rect.y < topLimit)              
        rect.y = topLimit;           
    if (rect.y + rect.h > bottomEdge)   
        rect.y = bottomEdge - rect.h;
}

Fish::~Fish() {
    SDL_DestroyTexture(textureFish);
}
