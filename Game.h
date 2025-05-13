#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include "Background.h"
#include "const.h"
#include "Fish.h"
#include "FishAI.h"
#include "Bubble.h"
#include "Score.h"
#include "BossFish.h"
#include "Menu.h"

class Game {
public:
	enum GameState { MENU , PLAYING};
	GameState gameState; 
	Menu* menu; 
	int selectedOption; 
	Mix_Chunk* clickSound; 

	SDL_Window* window;
	SDL_Renderer* renderer;
	bool running;
	bool isKick = false;
	Background* background; 
	Fish* fish;
		//fishAI
	std::vector<FishAI*> fishAI; 
	SDL_Texture* fishAITex;
	std::string imagePathAI;
		//bubble
	std::vector<Bubble*> bubbles;
	Uint32 lastSpawnTime = 0; 
	Uint32 spawnInterval = 1000; 
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
		//warning
	bool showWarning; 
	Uint32 warningStart; 
	Uint32 warningLife; 
		//shrk spawn delay
	bool pendingShark; 
	Uint32 sharkSpawnTime; 
	int pendingSharkX, pendingSharkY, pendingSharkW, pendingSharkH; 
	bool sharkGrow = false;
		//shark tam thoi
	float pendingSharkDirection; 
	int pendingSharkPointValue; 
	std::string pendingSharkImage;
		//mixer
	Mix_Music* backgroundMusic;
	Mix_Music* levelMusic;
	Mix_Chunk* eatFishSound;
	Mix_Chunk* levelUpSound;
	Mix_Chunk* gameOverSound;
	bool isMusicOn = true; 

	Game();
	~Game();
	void run(); 
	void handleEvents(SDL_Event& event); 
	void update(); 
	void render(); 
		//level
	void nextLevel(); 
};
