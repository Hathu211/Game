#include <SDL.h>
#include <iostream>
#include "Game.h"
#include "const.h"
#include "Background.h"
#include "Fish.h"
#include "FishAI.h"
#include <SDL_image.h>
#include <ctime>
#include <cstdlib>


using namespace std;

static float randFloat(float a, float b) {
	return a + static_cast<float>(rand()) / RAND_MAX * (b - a);
}

Game::Game() {
	srand(static_cast<unsigned int>(time(nullptr))); 
	running = true;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << " invalid SDL " << SDL_GetError() << endl;
		running = false;
	}
	window = SDL_CreateWindow("Feeding Frenzy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) {
		cerr << "invalid window " << SDL_GetError() << endl;
		running = false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		cerr << "invalid renderer " << SDL_GetError() << endl;
		running = false;
	}
	
	background = new Background(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, "assets/nen.png");
	
	int worldW = background->getWidth(); 
	int worldH = background->getHeight(); 
	fish = new Fish( 180,260,80,82,150,renderer,"assets/fishRight.png", worldW, worldH);
	lastSpawnTime = SDL_GetTicks();
	spawnInterval = 2000; 
	
	running = true;
}

void Game::spawnFishAI() {
	float angle = randFloat(0.0f, 2.0f * M_PI); 
	float speed = 1.5f + randFloat(0.0f, 1.5f); 
	float vx = std::cos(angle) * speed; 
	float vy = std::sin(angle) * speed;

	FishAI* newFishAI = new FishAI(60, 60, renderer, background->getWidth(), background->getHeight(), vx, vy, "assets/50.png");
	fishAI.push_back(newFishAI);
}

void Game::update() {
	Uint32 now = SDL_GetTicks(); 
	if (now - lastSpawnTime > spawnInterval) {
		spawnFishAI();
		lastSpawnTime = now;
	}
	for (auto f : fishAI) f->updateAuto(); 
	background->updateCamera(fish->getRect()); 
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	background->render(); 
	SDL_Rect cam = background->getCamera(); 
	SDL_Rect wr = fish->getRect();
	SDL_Rect dr = { wr.x - cam.x, wr.y - cam.y, wr.w, wr.h };
	SDL_RenderCopyEx(renderer, fish->getTexture(), nullptr, &dr, fish->getAngle(), nullptr, SDL_FLIP_NONE);

	for (auto f : fishAI) {
		SDL_Rect nr = f->getRect(); 
		SDL_Rect nd = { nr.x - cam.x, nr.y - cam.y, nr.w, nr.h };
		SDL_RenderCopyEx(renderer, f->getTexture(), nullptr, &nd, 0, nullptr, f->getFlip());
	}
	SDL_RenderPresent(renderer);
}

void Game::handleEvents(SDL_Event& e) {
	SDL_Rect cam = background->getCamera();

	if (e.type == SDL_QUIT) {
		running = false;
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
		int worldX = e.button.x + cam.x;
		int worldY = e.button.y + cam.y;
		SDL_Rect r = fish->getRect();
		if (worldX >= r.x && worldX <= r.x + r.w && worldY >= r.y && worldY <= r.y + r.h) { 
			isKick = true;
		}
	}
	else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
		isKick = false;
	}
	else if (e.type == SDL_MOUSEMOTION && isKick) {
		int worldX = e.motion.x + cam.x;
		int worldY = e.motion.y + cam.y;
		fish->setTarget(worldX, worldY);
	}
}

void Game::run() {
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			handleEvents(event);
		} 
		fish->move(isKick);
		background->updateCamera(fish->getRect());
		render();
		SDL_Delay(15);
	}
}

	Game::~Game() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		IMG_Quit();
		SDL_Quit();
	};
