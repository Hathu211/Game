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
        //khoi tao collision
    float collision = 0.8f; 
    collisionRect.w = static_cast<int>(w * collision);
    collisionRect.h = static_cast<int>(h * collision);
    /*collisionRect.x = x + (w - collisionRect.w) / 2;
    collisionRect.y = y + (h - collisionRect.h) / 2;*/

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

void Fish::grow(float bigSize) {
    int cx = rect.x + rect.w / 2; 
    int cy = rect.y + rect.h / 2; 
    rect.w = static_cast<int>(rect.w * bigSize); 
    rect.h = static_cast<int>(rect.h * bigSize); 
    rect.x = cx - rect.w / 2;
    rect.y = cy - rect.h / 2;
    float collision = 0.8f;
    collisionRect.w = static_cast<int>(rect.w * collision);
    collisionRect.h = static_cast<int>(rect.h * collision);
    /*collisionRect.x = rect.x + (rect.w - collisionRect.w) / 2;
    collisionRect.y = rect.y + (rect.h - collisionRect.h) / 2;*/
}

SDL_Rect Fish::getHeadRect() const {
    const SDL_Rect& p = this->getRect();
    int headW = p.w / 3;
    int headH = p.h / 2;
    SDL_Rect head;
    double pAngle = this->getAngle();
    if (fabs(pAngle - 0.0) < 0.001) {
        head = {p.x + p.w - headW, p.y + (p.h - headH) / 2,headW, headH };
    }
    else {
        head = { p.x, p.y + (p.h - headH) / 2, headW, headH }; 
    }
    return head;
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
    /*collisionRect.x = rect.x + (rect.w - collisionRect.w) / 2;
    collisionRect.y = rect.y + (rect.h - collisionRect.h) / 2;*/
}

Fish::~Fish() {
    SDL_DestroyTexture(textureFish);
}
