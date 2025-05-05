#include <SDL.h>
#include <iostream>
#include <string>
#include "Game.h"
#include "const.h"
#include "Background.h"
#include "Fish.h"
#include <SDL_image.h>
#include <ctime>
#include <cstdlib>
#include <SDL_ttf.h>
#include "Score.h"

using namespace std;

Game::Game() {
    //tao font
    if (TTF_Init() < 0) {
        cerr << "TTF_Init failed: " << TTF_GetError() << endl; 
        IMG_Quit(); 
        SDL_Quit(); 
        running = false; 
        return; 
    }
    scoreFont = TTF_OpenFont("assets/font.ttf", 20); 
    if (!scoreFont) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        running = false;
        return;
    }
    currentLevel = 1; 
    playerScore = 0; 
    bossSpawned = false; 
    //tao window
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
    fish = new Fish(180, 260, 85, 82, 150, renderer, "assets/fish.png", worldW, worldH);

	FishAI::InitScreenSize(worldW, worldH);
	fishAITex = IMG_LoadTexture(renderer, "assets/30.png");
	fishAI.push_back(new FishAI( 0, 100, 80, 82, renderer, "assets/30.png"));

	lastSpawnTime = SDL_GetTicks();
    bossSpawned = false; 
    running = true;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    background->render();
    SDL_Rect cam = background->getCamera(); //ve xong back

    //ve fishPlayer + score
    SDL_Rect wr = fish->getRect();
    SDL_Rect dr = { wr.x - cam.x, wr.y - cam.y, wr.w, wr.h };
    SDL_RenderCopyEx(renderer, fish->getTexture(), nullptr, &dr, fish->getAngle(), nullptr, SDL_FLIP_NONE);
    SDL_Color white = { 255,255,255,255 };
    string playerScoreText = to_string(playerScore); // ta can chuyen score thanh string, vi ttf can string 
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont, playerScoreText.c_str(), white);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    int textH, textW;
    SDL_QueryTexture(scoreTexture, nullptr, nullptr, &textW, &textH);
    SDL_Rect scorePlayer = { dr.x + dr.w / 2 - textW / 2, dr.y - textH - 5, textW, textH };
    SDL_FreeSurface(scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scorePlayer);
    SDL_DestroyTexture(scoreTexture);

    //ve scoreAI + AI
    for (auto ai : fishAI) {
        ai->render(cam);
        SDL_Rect aiWorld = ai->getRect();
        SDL_Rect aiScreen = { aiWorld.x - cam.x, aiWorld.y - cam.y, aiWorld.w, aiWorld.h };
        SDL_Color white = { 255,255,255,255 };
        string aiScoreText = to_string(score::getScoreLevel1(currentLevel));
        SDL_Surface* aiSurface = TTF_RenderText_Solid(scoreFont, aiScoreText.c_str(),white);
        SDL_Texture* aiTexture = SDL_CreateTextureFromSurface(renderer, aiSurface);
        int textW, textH;
        SDL_QueryTexture(aiTexture, nullptr, nullptr, &textW, &textH);
        SDL_Rect aiScoreAI = { aiScreen.x + aiScreen.w / 2 - textW / 2, aiScreen.y - textH - 5, textW, textH };
        SDL_FreeSurface(aiSurface);
        SDL_RenderCopy(renderer, aiTexture, nullptr, &aiScoreAI);
        SDL_DestroyTexture(aiTexture);
    } 
    //ve score bossFish
    for (auto b : bossFish) {
        b->render(background->getCamera());
        // hiển thị điểm 180 trên đầu boss
        SDL_Color blue = { 0, 255, 255, 255 };
        std::string text = "100";
        SDL_Surface* surf = TTF_RenderText_Solid(scoreFont, text.c_str(), blue);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        int w, h; SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        SDL_Rect r = b->getRectBoss();
        SDL_Rect dst = { r.x - cam.x + r.w / 2 - w / 2,
                         r.y - cam.y - h - 5, w, h };
        SDL_FreeSurface(surf);
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
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
            //gioi han phia tren
        const int topLimit = worldW - 550;
        const int bottomEdge = worldH;
        if (worldW <= w || worldH <= h) {
            std::cerr << "Invalid world size: worldW = " << worldW << ", worldH" << worldH << std::endl;
            return;
        }
            //random 4 goc sinh ra ca AI
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
        //xu ly va cham voi Boss
    int worldW = background->getWidth(); 
    int worldH = background->getHeight();
    const int topLimit = worldH - 550;
    const int bottomEdge = worldH;
    const int bw = 100, bh = 102;
    const float speed = 3.5f;
    if (!bossSpawned && playerScore > 40) { //khi fish duoc 40 diem thi boss xuat hien 
        int edge = rand() % 2; 
        int bx = (edge == 0) ? 0 : (worldW - bw); 
        int by = topLimit + rand() % (bottomEdge - topLimit + 1); 
        bossFish.push_back(new BossFish(bx, by, bw, bh, speed, renderer, "assets/100.png")); 
        bossSpawned = true; 
    }

    SDL_Rect playerRect = fish->getRect();
    SDL_Rect headRect = fish->getHeadRect();          // dùng trực tiếp headRect của Player
    int pcx = playerRect.x + playerRect.w / 2;
    int pcy = playerRect.y + playerRect.h / 2;

    for (auto it = bossFish.begin(); it != bossFish.end(); ) {
        BossFish* b = *it;
        b->updateBoss(pcx, pcy);

        SDL_Rect bRect = b->getRectBoss();
        SDL_Rect intersection;
        // Nếu head của Player chạm boss
        if (SDL_IntersectRect(&headRect, &bRect, &intersection)) {
            // Tạo bong bóng giống FishAI
            int spawnX = intersection.x + intersection.w / 2;
            int spawnY = intersection.y + intersection.h / 2;
            int numBubbles = 5 + rand() % 5;
            for (int i = 0; i < numBubbles; ++i) {
                int offsetX = rand() % 20 - 10;
                int offsetY = rand() % 20 - 10;
                float angle = (rand() % 360) * M_PI / 180.0f;
                float speed = 2.0f + (rand() % 3);
                bubbles.push_back(new Bubble(
                    renderer,
                    spawnX + offsetX,
                    spawnY + offsetY,
                    cosf(angle) * speed,
                    -sinf(angle) * speed,
                    "assets/bubble.png",
                    50
                ));
            }

            // Sau khi đã tạo bubble mới xét điểm
            if (playerScore >= 100) {
                fish->grow(1.3f);
                playerScore += 100;
                delete b;
                it = bossFish.erase(it);
            }
            else {
                running = false;  // boss mạnh hơn → game over
                return;
            }
        }
        else {
            ++it;
        }
    }
    //lay Rect o headFish va xu ly va cham voi fishAI
    for (auto ai = fishAI.begin(); ai != fishAI.end(); ) {
        if (!(*ai)->update()) {
            delete* ai;
            ai = fishAI.erase(ai);
        }
        else {
            SDL_Rect aiRect = (*ai)->getRect();
            SDL_Rect pRect = fish->getRect(); 
            SDL_Rect headRect = fish->getHeadRect();
            SDL_Rect intersection; 
            if (SDL_IntersectRect(&headRect, &aiRect, &intersection)) {
                //tao bubble
                int spawnX = intersection.x + intersection.w / 2; 
                int spawnY = intersection.y + intersection.h / 2;
                int numBubbles = 5 + rand() % 5;
                for (int i = 0; i < numBubbles; i++) {
                    int offsetX = rand() % 20 - 10;
                    int offsetY = rand() % 20 - 10;
                    int bx = spawnX + offsetX; 
                    int by = spawnY + offsetY; 
                    float randAngle = (rand() % 360) * M_PI / 180.0;
                    float speed = 2.0f + (rand() % 3);
                    float initialVx = cosf(randAngle) * speed;
                    float initialVy = -sinf(randAngle) * speed;
                    Bubble* bubble = new Bubble(renderer, spawnX + offsetX, spawnY + offsetY, initialVx, initialVy, "assets/bubble.png", 50);
                    bubbles.push_back(bubble);
                }
                playerScore += score::getScoreLevel1(currentLevel);

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
    if (scoreFont) {
        TTF_CloseFont(scoreFont); 
    }
    IMG_Quit();
    SDL_Quit();
}