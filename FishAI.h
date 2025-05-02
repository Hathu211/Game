#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <cstdlib>

class FishAI {
	SDL_Renderer* renderer; 
	SDL_Rect rect; 
	SDL_Texture* textureAI; 
	int worldW, worldH; 
	float velX, velY; 
	SDL_RendererFlip flip; 
	bool autonomous; 
public: 
	FishAI(int x, int y, int w, int h, SDL_Renderer* renderer, int worldW, int worldH, const char* imagePath = "assets/50.png"); //ca goc
	FishAI(int w, int h, SDL_Renderer* renderer, int worldW, int worlH, float velX, float velY, const char* imagePath = "assets/50.png" ); //ca sinh ra
	void move(int targetX, int targetY); 
	void updateAuto(); 
	SDL_Texture* getTexture() const { return textureAI; }
	const SDL_Rect& getRect() const { return rect; } 
	SDL_RendererFlip getFlip() const { return flip; } 
	~FishAI(); 
};
