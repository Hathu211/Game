#include "BossFish.h"
#include <SDL_image.h>
#include <cmath>
#include <iostream>

BossFish::BossFish(int x, int y, int w, int h, float speed, SDL_Renderer* renderer, const char* imagePath) {
	this->renderer = renderer; 
	this->speed = speed; 
	rect = { x,y,w,h };
	SDL_Surface* surface = IMG_Load(imagePath); 
	if (!surface) {
		std::cerr << "Failed to load" << IMG_GetError() << std::endl;
		texture = nullptr;
	} 
	else {
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	} 
}
void BossFish::updateBoss(int px, int py) {
	float dx = px - (rect.x + rect.w / 2); 
	float dy = py - (rect.y + rect.h / 2); 
	float dis = std::sqrt(dx * dx + dy * dy); 
	if (dis < 1) return; 
	rect.x += static_cast<int>(dx / dis * speed);
	rect.y += static_cast<int>(dy / dis * speed); 
}
void BossFish::render (const SDL_Rect& cam) {
	SDL_Rect dst = { rect.x - cam.x, rect.y - cam.y, rect.w, rect.h }; 
	SDL_RenderCopy(renderer, texture, nullptr, &dst); 
}

BossFish::~BossFish() {
	if (texture) SDL_DestroyTexture(texture);
}