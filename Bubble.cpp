#include "Bubble.h"
#include <cstdlib>
#include <iostream>

Bubble::Bubble(SDL_Renderer* renderer, int x, int y, float initialVx, float initialVy, const char* imagePath, int size) {
	this->renderer = renderer;
	rect = { x,y, 100, 100 }; 
	vx = initialVx + (rand() % 21 - 10) / 10.0f; //huong di chuyen cua bubble
	vy = initialVy -(rand() % 3 + 1); 
	wobble = (rand() % 10) / 10.0f; //lac lu bubble
	alpha = 255; 
	lifeTime = 1000 + (rand() % 1000); 
	startTime = SDL_GetTicks(); 
	
	SDL_Surface* surface = IMG_Load(imagePath); 
	if (!surface) {
		SDL_Log("Failed to load bubble image: %s", IMG_GetError());
		texture = nullptr;
	} 
	else {
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		if (!texture) {
			SDL_Log("Failed to create bubble texture : % s", SDL_GetError());
		}
	}
}

bool Bubble::updateBubble(Uint32 currentTime) {
	rect.x += static_cast<int> (vx); 
	rect.y += static_cast<int>(vy); 

	Uint32 elasped = currentTime - startTime; //tinh so s ma bubble duoc sinh ra
	if (elasped >= lifeTime) { //neu thoi gian spawn > lifeTime -> false
		return false;
	} 
	alpha = 255 * (lifeTime - elasped) / lifeTime; //mo dan 
	SDL_SetTextureAlphaMod(texture, alpha); 
	return true; 
}

void Bubble::renderBubble(const SDL_Rect& camera) {
	if (!texture) return; 
	SDL_Rect dst = { rect.x - camera.x, rect.y - camera.y, rect.w, rect.h }; 
	SDL_RenderCopy(renderer, texture, nullptr, &dst); 
}

Bubble::~Bubble() {
	if (texture) {
		SDL_DestroyTexture(texture); 
	}
}