#include <SDL.h>
#include <iostream>
#include "Game.h"
#include "const.h"
#include "Background.h"
#include "Fish.h"
#include <SDL_image.h>
#include <ctime>
#include <cstdlib>

using namespace std;

Game::Game() {
    srand((unsigned)time(nullptr));
    running = true;
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    std::cout << "SDL base path: " << SDL_GetBasePath() << std::endl;

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
	FishAI::InitScreenSize(worldW, worldH);

    fish = new Fish(180, 260, 80, 82, 100, renderer, "assets/fishRight.png", worldW, worldH);
	fishAITex = IMG_LoadTexture(renderer, "assets/30.png");
	fishAI.push_back(new FishAI(worldW / 2 - 40, worldH / 2 - 40, 80, 82, renderer, "assets/30.png"));
	lastSpawnTime = SDL_GetTicks();
    running = true;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    background->render();
    SDL_Rect cam = background->getCamera();
    
    SDL_Rect wr = fish->getRect();
    SDL_Rect dr = { wr.x - cam.x, wr.y - cam.y, wr.w, wr.h };
    SDL_RenderCopyEx(renderer, fish->getTexture(), nullptr, &dr, fish->getAngle(), nullptr, SDL_FLIP_NONE);
	for (auto ai : fishAI) {
		ai->render(cam);
	}
    SDL_RenderPresent(renderer);
     
}

void Game::update() {
 
    Uint32 now = SDL_GetTicks();
    if (now - lastSpawnTime >= spawnInterval) {
        lastSpawnTime = now;
        int w = 80, h = 82;
        int x = rand() % (winW - w);
        int y = rand() % (winH - h);
        fishAI.push_back(
            new FishAI(x, y, w, h, renderer, "assets/30.png")
        );
    }

    fish->move(isKick);
    background->updateCamera(fish->getRect());

    for (auto ai = fishAI.begin(); ai != fishAI.end(); ) {
        if (!(*ai)->update()) {
            delete* ai;
            ai = fishAI.erase(ai);
        } 
        else { 
			++ai;
        }
    }
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
        update(); 
        render();
        SDL_Delay(15);
    }
}

Game::~Game() {
    for (auto ai : fishAI) delete ai;
    fishAI.clear();
    if (fishAITex) SDL_DestroyTexture(fishAITex);
    delete fish;
    delete background;
    IMG_Quit();
    SDL_Quit();
}