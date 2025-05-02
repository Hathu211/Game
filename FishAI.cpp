#include "FishAI.h"
#include <iostream>
#include <ctime>

FishAI::FishAI(int x, int y, int w, int h, SDL_Renderer* renderer, int worldW, int worldH,  const char* imagePath) {
	this->renderer = renderer; 
	this->worldW = worldW;
	this->worldH = worldH;
	float velX = 0.0; 
	float velY = 0.0;
	flip = SDL_FLIP_NONE; 
	autonomous = false;
	rect = { x,y,w,h }; 

	SDL_Surface* surface = IMG_Load(imagePath); 
	if (!surface) {
		SDL_Log("Failed to load assets/50.png: %s", IMG_GetError()); 
	}
	textureAI = SDL_CreateTextureFromSurface(renderer, surface); 
	if (!textureAI) {
		SDL_Log("Failed to load assets/50.png: %s", IMG_GetError());
	}
	SDL_FreeSurface(surface); 
}

FishAI::FishAI(int w, int h, SDL_Renderer* renderer,int worldW, int worldH, float vX, float vY, const char* imagePath) {
	this->renderer = renderer; 
	this->worldW = worldW; 
	this->worldH = worldH; 
	this->velX = vX; 
	this->velY = vY;
	flip = (vX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	autonomous = true; 

	int spawnX = rand() % (worldW - w); 
	int spawnY = rand() % (worldH - h); 
	rect = { spawnX, spawnY, w, h }; 

	SDL_Surface* surface = IMG_Load(imagePath); 
	if (!surface) {
		std::cerr << "Failed to load fish image: " << IMG_GetError() << std::endl;
		exit(1);
	}
	textureAI = SDL_CreateTextureFromSurface(renderer, surface);
	if (!textureAI) {
		std::cerr << "Failed to create fish texture: " << SDL_GetError() << std::endl;
		exit(1);
	}
	SDL_FreeSurface(surface);
}

void FishAI::move(int targetX, int targetY) {
	if (autonomous) return; 

	float dx = targetX - (rect.x + rect.w / 2);
	float dy = targetY - (rect.y + rect.h / 2);
	float dist = std::sqrt(dx * dx + dy * dy);
	if (dist < 1.0f) return;

	flip = (dx < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	rect.x += static_cast<int>((dx / dist) * 2); 
	rect.y += static_cast<int>((dy / dist) * 2);

	if (rect.x < 0) rect.x = 0;
	if (rect.x + rect.w > worldW) rect.x = worldW - rect.w;
	const int topLimit = worldH - 550; //day tren ca window 550 pixel
	const int bottomEdge = worldH; //day duoi co th cham day
	if (rect.y < topLimit)
		rect.y = topLimit;
	if (rect.y + rect.h > bottomEdge)
		rect.y = bottomEdge - rect.h; 
}

void FishAI::updateAuto() {
	if (!autonomous) return; 

	rect.x += static_cast<int>(velX); 
	rect.y += static_cast<int>(velY); 

	if (rect.x <= 0 || rect.x + rect.w >= worldW) {
		velX = -velX;
		flip = (velX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	}
	if (rect.y <= 0 || rect.y + rect.h >= worldH) {
		velY = -velY;
	}

	if (rect.x < 0) rect.x = 0;
	if (rect.x + rect.w > worldW) rect.x = worldW - rect.w;
	const int topLimit = worldH - 550; //day tren ca window 550 pixel
	const int bottomEdge = worldH; //day duoi co th cham day
	if (rect.y < topLimit)
		rect.y = topLimit;
	if (rect.y + rect.h > bottomEdge)
		rect.y = bottomEdge - rect.h;
}

FishAI::~FishAI() {
	SDL_DestroyTexture(textureAI);
}