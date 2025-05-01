#include "Background.h" 
#include "const.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

Background::Background(SDL_Renderer* renderer, int width, int height, const char* imagePath) {
	this->renderer = renderer;
	camera = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	SDL_Surface* surface = IMG_Load(imagePath);
	if (!surface) {
		SDL_Log("Failed to load background: %s", IMG_GetError());
		exit(1);
	}
	backgroundWidth = surface->w;
	backgroundHeight = surface->h;

	newTexture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!newTexture) {
		SDL_Log("Failed to create background texture: %s", SDL_GetError());
		exit(1);
	}
}

void Background::updateCamera(const SDL_Rect & playerRect) {
	camera.x = playerRect.x + playerRect.w / 2 - SCREEN_WIDTH / 2;
	camera.y = playerRect.y + playerRect.h / 2 - SCREEN_HEIGHT / 2;

	if (camera.x < 0) {
			camera.x = 0;
	}
	if (camera.y < 0) {
			camera.y = 0;
	}
	if (camera.x + camera.w > backgroundWidth) {
			camera.x = backgroundWidth - camera.w;
	}
	if (camera.y + camera.h > backgroundHeight) {
			camera.y = backgroundHeight - camera.h;
	}
}

void Background::render() {
	SDL_RenderCopy(renderer, newTexture, &camera, nullptr);
} 

Background::~Background() {
	SDL_DestroyTexture(newTexture);
}
