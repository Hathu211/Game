#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Background.h"
#include "const.h"
#include "Fish.h"
#include "FishAI.h"
#include "Bubble.h"

class Game {
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool running;
	bool isKick = false;
	Background* background; 
	Fish* fish; 
	std::vector<FishAI*> fishAI; //chua cac con ca AI khac sinh ra
	std::vector<Bubble*> bubbles; 
	SDL_Texture* fishAITex; 
	Uint32 lastSpawnTime = 0; 
	Uint32 spawnInterval = 2000; 
	SDL_RendererFlip flip = SDL_FLIP_NONE; 

	Game();
	~Game();
	void run(); 
	void handleEvents(SDL_Event& evenet); 
	void update(); 
	void render();
};
