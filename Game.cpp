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
    std::cout << "worldW = " << worldW << ", worldH = " << worldH << std::endl; //in ra terminal kiem tra worldW and worldH co loi khong, = 0; loi 
    fish = new Fish(180, 260, 85, 82, 100, renderer, "assets/fish.png", worldW, worldH);

	FishAI::InitScreenSize(worldW, worldH);
	fishAITex = IMG_LoadTexture(renderer, "assets/30.png");
	fishAI.push_back(new FishAI( 0, 100, 80, 82, renderer, "assets/30.png"));

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
    for (auto bubble : bubbles) {
        bubble->renderBubble(cam); 
    }
    SDL_RenderPresent(renderer); 
}

void Game::update() {
    Uint32 now = SDL_GetTicks();
    if (now - lastSpawnTime >= spawnInterval) {
        lastSpawnTime = now;
        int w = 80, h = 82;

        int worldH = background->getHeight();
        int worldW = background->getWidth();

        const int topLimit = worldW - 550;
        const int bottomEdge = worldH;

        if (worldW <= w || worldH <= h) {
            std::cerr << "Invalid world size: worldW = " << worldW << ", worldH" << worldH << std::endl;
            return;
        }

        int edge = rand() % 4;
        int x, y;
        float directionAngle;

        if (edge == 0) {
            x = 0;
            y = topLimit + rand() % (bottomEdge - topLimit + 1);
            directionAngle = 0.0f;
        }
        else {
            x = worldW - w;
            y = topLimit + rand() % (bottomEdge - topLimit + 1);
            directionAngle = M_PI;
        }
        std::cout << "Spawning FishAI at (" << x << ", " << y << ")" << std::endl;
        FishAI* newFishAI = new FishAI(x, y, w, h, renderer, "assets/30.png");
        if (newFishAI) {
            fishAI.push_back(newFishAI);
            std::cout << "FishAI spawned, total: " << fishAI.size() << std::endl;
        }
        else {
            std::cerr << "Error: Failed to create FishAI" << std::endl;
        }
    }
    fish->move(isKick);
    background->updateCamera(fish->getRect());
    //lay Rect o headFish
    SDL_Rect playerRect = fish->getRect();
    for (auto ai = fishAI.begin(); ai != fishAI.end(); ) {
        if (!(*ai)->update()) {
            delete* ai;
            ai = fishAI.erase(ai);
        }
        else {
            SDL_Rect aiRect = (*ai)->getRect();
            SDL_Rect pRect = fish->getRect(); 
            float pAngle = fish->getAngle(); 

            int headW = pRect.w / 3; 
            int headH = pRect.h / 2; 
            SDL_Rect headRect; 

            if (fabs(pAngle - 0.0f) < 0.001f) {
                headRect = { pRect.x + pRect.w - headW, pRect.y + (pRect.h - headH) / 2, headW, headH }; 
            }
            else {
                headRect = { pRect.x, pRect.y + (pRect.h - headH) / 2, headW, headH };
            }
            
            SDL_Rect aiRect = (*ai)->getRect(); 
            SDL_Rect intersection; 
            if (SDL_IntersectRect(&headRect, &aiRect, &intersection)) {
                int spawnX = intersection.x + intersection.w / 2; 
                int spawnY = intersection.y + intersection.h / 2;

                int numBubbles = 5 + rand() % 5;
                for (int i = 0; i < numBubbles; i++) {
                    int offsetX = rand() % 20 - 10;
                    int offsetY = rand() % 20 - 10;

                    float randAngle = (rand() % 360) * M_PI / 180.0;
                    float speed = 2.0f + (rand() % 3);
                    float initialVx = cosf(randAngle) * speed;
                    float initialVy = -sinf(randAngle) * speed;
                    Bubble* bubble = new Bubble(renderer, spawnX + offsetX, spawnY + offsetY, initialVx, initialVy, "assets/bubble.png", 50);
                    bubbles.push_back(bubble);
                }
                delete* ai;
                ai = fishAI.erase(ai);
            }
            else {
                ++ai;
            }
        }
    }             
    // Cập nhật và dọn dẹp bong bóng
    for (auto it = bubbles.begin(); it != bubbles.end(); ) {
        if (!(*it)->updateBubble(now)) {
            delete* it;
            it = bubbles.erase(it);
        }
        else {
            ++it;
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