#pragma once
#include <SDL.h>
#include <SDL_image.h>

class Bubble {
private: 
	SDL_Renderer* renderer; 
	SDL_Texture* texture; 
	SDL_Rect rect; 
	float vx, vy; 
	Uint8 alpha; 
	Uint32 lifeTime; 
	Uint32 startTime;
	float wobble; // hieu ung bubble troi noi 
public: 
	Bubble(SDL_Renderer* renderer, int x, int y, float initialVx, float initiaVy, const char* imagePath = "assets/bubble.png", int size = 50); 
	~Bubble(); 
	bool updateBubble (Uint32 currentTime); 
	void renderBubble (const SDL_Rect& camera); 
};
