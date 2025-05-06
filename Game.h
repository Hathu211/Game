#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Background.h"
#include "const.h"
#include "Fish.h"
#include "FishAI.h"
#include "Bubble.h"
#include <SDL_ttf.h>
#include "Score.h"
#include "BossFish.h"

class Game {
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool running;
	bool isKick = false;
	Background* background; 
	Fish* fish; 

	std::vector<FishAI*> fishAI; //chua cac con ca AI khac sinh ra 
	SDL_Texture* fishAITex;

	std::vector<Bubble*> bubbles;
	Uint32 lastSpawnTime = 0; 
	Uint32 spawnInterval = 2000; 
	SDL_RendererFlip flip = SDL_FLIP_NONE;
		//bossFish
	SDL_Texture* bossFishTex;
	std::vector<BossFish*> bossFish; 
	bool bossSpawned; 
		//score 
	int currentLevel; 
	int playerScore; 
	TTF_Font* scoreFont;
		//level
	bool overlayActive; 
	Uint32 overlayStart; 
	Uint32 overlayLife; 

	Game();
	~Game();
	void run(); 
	void handleEvents(SDL_Event& evenet); 
	void update(); 
	void render(); 
		//level
	void nextLevel(); 
};
