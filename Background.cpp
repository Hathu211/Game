#include "Background.h" 
#include "const.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream> 
#include <vector>

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
void Background::loadBackLevel(const std::vector<std::string>& paths) {
	for (auto t : overLays) {
		if (t) SDL_DestroyTexture(t);
		overLays.clear(); //xoa back cu
	}
	for (auto& p : paths) { //tao back moi
		SDL_Surface* surface = IMG_Load(p.c_str());
		if (!surface) {
			std::cerr << "Failed to load backLevel" << IMG_GetError() << std::endl;
			overLays.push_back(nullptr);
		}
		else {
			SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			overLays.push_back(text);
		}
	}
}
void Background::renderBackLevel(int level) {
	int start = level - 1;
	if (start < 0 || start >= (int)overLays.size() || !overLays[start]) return;
	int w = SCREEN_WIDTH / 2, h = SCREEN_HEIGHT / 4;
	SDL_Rect dst = { (SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2, w,h };
	SDL_RenderCopy(renderer, overLays[start], nullptr, &dst);
}

SDL_Texture* Background::getoverLays(int level) const {
	int start = level - 1;
	if (start >= 0 && start < (int)overLays.size()) return overLays[start];
	return nullptr;
} 

Background::~Background() {
	SDL_DestroyTexture(newTexture);
	for (auto t : overLays) {
			if (t) SDL_DestroyTexture(t);
	}
}
