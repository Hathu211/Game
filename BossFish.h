#pragma once
#include <SDL.h>
#include <SDL_image.h>

class BossFish {
private: 
	SDL_Renderer* renderer; 
	SDL_Texture* texture; 
	SDL_Rect rect; 
	float speed; 
public: 
	BossFish(int x, int y, int w, int h, float speed, SDL_Renderer* renderer, const char* imagePath = "assets/100.png"); 
	~BossFish(); 
	void updateBoss(int playerCenterX, int playerCenterY); 
	void render(const SDL_Rect& cam); 
	SDL_Rect getRectBoss() const { return rect; }
};
