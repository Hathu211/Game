#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "const.h"

class Fish {
private:
    SDL_Renderer* renderer; 
    SDL_Rect rect;
    int speed;
    SDL_Texture* textureFish;
    int targetX, targetY;
    double angle;
    int worldW, worldH;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    int offsetX, offsetY; 
    

public:
    Fish(int x, int y, int w, int h, int speed, SDL_Renderer* renderer, const char* imagePath = "assets/fish.png",int worldWidth = SCREEN_WIDTH,int worldHeight = SCREEN_HEIGHT);
    void setTarget(int x, int y);
    void move(bool isKick);
    SDL_Texture* getTexture() const { return textureFish; }
    double       getAngle()   const { return angle; }
    SDL_RendererFlip getFlip() const { return flip;} 
    const SDL_Rect& getRect() const { return rect; }
    void grow(float bigSize);
    SDL_Rect getHeadRect() const;

    ~Fish();
};
