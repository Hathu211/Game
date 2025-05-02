#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "const.h"

class Background {
	SDL_Texture* newTexture; 
	SDL_Renderer* renderer; 
	SDL_Rect camera; 
	int backgroundWidth, backgroundHeight; 
public:
	Background(SDL_Renderer* renderer, int width, int height, const char* imagePath = "assets/nen.png"); 
	void updateCamera(const SDL_Rect& playerRect); 
	SDL_Rect getCamera() const {
		return camera; 
	}
	int getWidth() const {
		return backgroundWidth;
	} 
	int getHeight() const {
		return backgroundHeight;
	}
	void render(); 
	~Background(); 
};

