#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Background.h"
#include "const.h"
#include "Fish.h"
#include "FishAI.h"

class Game {
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool running;
	bool isKick = false;
	Background* background; 
	Fish* fish; 
	std::vector<FishAI*> fishAI; 
	SDL_Texture* fishAITex; 
	Uint32 lastSpawnTime = 0; 
	Uint32 spawnInterval = 2000; 
	int winW = 0, winH = 0; 
	int worldW = 0, worldH = 0;
	
	Game();
	~Game();
	void run(); 
	void spawnFish();
	void handleEvents(SDL_Event& evenet); 
	void update(); 
	void render();
};
