#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "const.h"
#include <string>

class FishAI {
private:
    SDL_Renderer* renderer;
    SDL_Rect rect;
    SDL_Rect collisionRect; //thu nho vung va cham
    SDL_Texture* textureAI;
    float vx, vy; 
    float angle; 
    float speed; 
    SDL_RendererFlip flip; 
    static int backW, backH; //gioi han background de AI khong tran
    int pointValue;
    std::string imagePathAI; 

public:
    FishAI(int x, int y, int w, int h, SDL_Renderer* renderer, const char* imagePathAI = "assets/cavang20.png", float directionAngle = 0.0f, float speed = 3.5f, int pointValue = 20);
    ~FishAI();
    bool update(); //dung bool de check neu cham bien -> xoa AI
    void render(const SDL_Rect& camera);
    void setSpeed(float s); 
    static void InitScreenSize(int w, int h); 
    const SDL_Rect& getRect() const { return rect; } 
    float getAngle() const { return angle; } 
    int getPointValue() const { return pointValue; }
    std::string getImagePath() const { return imagePathAI; }
    const SDL_Rect& getCollisionRect() const { return collisionRect; }
};

