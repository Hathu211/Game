#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "const.h"

class FishAI {
private:
    SDL_Renderer* renderer;
    SDL_Rect rect;
    SDL_Texture* textureAI;
    float vx, vy; 
    float angle; 
    float speed; 
    SDL_RendererFlip flip; 
    static int backW, backH; //gioi han background de AI khong tran
    int pointValue;

public:
    FishAI(int x, int y, int w, int h, SDL_Renderer* renderer, const char* imagePathAI = "assets/30.png", float directionAngle = 0.0f, float speed = 3.5f, int pointValue = 20);
    ~FishAI();
    bool update(); //dung bool de check neu cham bien -> xoa AI
    void render(const SDL_Rect& camera);
    void setSpeed(float s); 
    static void InitScreenSize(int w, int h); 
    const SDL_Rect& getRect() const { return rect; } 
    float getAngle() const { return angle; } 
    int getPointValue() const { return pointValue; }
};

