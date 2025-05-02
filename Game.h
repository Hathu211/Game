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
	Uint32 lastSpawnTime; 
	Uint32 spawnInterval; 
	
	Game();
	void render();
	void handleEvents(SDL_Event& evenet); 
	void run();
	void update(); 
	void spawnFishAI(); 
	~Game();
};
