#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "const.h"
#include <vector>
#include <string>

class Background {
	SDL_Texture* newTexture; 
	SDL_Renderer* renderer; 
	SDL_Rect camera; 
	int backgroundWidth, backgroundHeight; 
	std::vector<SDL_Texture*> overLays; 
public:
	Background(SDL_Renderer* renderer, int width, int height, const char* imagePath = "assets/nen.png"); 
	void updateCamera(const SDL_Rect& playerRect); 
	SDL_Rect getCamera() const {return camera;} 
	int getWidth() const {return backgroundWidth;} 
	int getHeight() const {return backgroundHeight;}
	void render(); 
		//level
	void loadBackLevel(const std::vector<std::string>& paths); 
	void renderBackLevel (int level); 
	SDL_Texture* getoverLays(int level) const; 
	~Background(); 
};

