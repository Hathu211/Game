#include "FishAI.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <cstdlib>

int FishAI::backW = SCREEN_WIDTH; 
int FishAI::backH = SCREEN_HEIGHT; 

void FishAI::InitScreenSize(int w, int h) {
	backW = w; 
	backH = h;
}

FishAI::FishAI(int x, int y, int w, int h, SDL_Renderer* renderer, const char* imagePathAI, float directionAngle, float speed)
	: renderer(renderer),
	rect{ x, y, w, h },
	angle(directionAngle),
	speed(speed),
	flip(SDL_FLIP_NONE) {
	vx = cosf(angle) * speed; 
	vy = sinf(angle) * speed;

	SDL_Surface* surface = IMG_Load(imagePathAI);
	if (!surface) {
		SDL_Log("IMG_Load failed: %s", IMG_GetError());		
		textureAI = nullptr; 
	}
	else {
		textureAI = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		if (!textureAI) {
			SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
		}
	}	
}

// FishAI.cpp
bool FishAI::update() {
	// 1. Di chuyển
	rect.x += static_cast<int>(vx);
	rect.y += static_cast<int>(vy);

	// 2. Giới hạn ngang như Player
	if (rect.x < 0)
		rect.x = 0;
	else if (rect.x + rect.w > backW)
		rect.x = backW - rect.w;

	// 3. Giới hạn dọc như Player
	const int topLimit = backH - 550;
	const int bottomEdge = backH;
	if (rect.y < topLimit)
		rect.y = topLimit;
	else if (rect.y + rect.h > bottomEdge)
		rect.y = bottomEdge - rect.h;

	// 4. Nếu chạm biên (bất kỳ) -> xóa
	if (rect.x == 0
		|| rect.x + rect.w == backW
		|| rect.y == topLimit
		|| rect.y + rect.h == bottomEdge) {
		return false;
	}

	return true;
}


void FishAI::render(const SDL_Rect& camera) {
	SDL_Rect dst = {
			rect.x - camera.x,
			rect.y - camera.y,
			rect.w,
			rect.h
	};
	SDL_RenderCopyEx(renderer, textureAI, nullptr, &dst, 0.0, nullptr, flip);
}

void FishAI::setSpeed(float s) {
	speed = s; 
	vx = cosf(angle) * speed; 
	vy = sinf(angle) * speed; 
}

FishAI::~FishAI() {
	if (textureAI){
		SDL_DestroyTexture(textureAI);
		textureAI = nullptr;
	}
}
	