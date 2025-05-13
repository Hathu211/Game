#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include "Game.h"
#include "const.h"
#include "Background.h"
#include "Fish.h"
#include <ctime>
#include <cstdlib>
#include "Score.h"

using namespace std;

Game::Game() {
        //mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "Open success" << Mix_GetError() << endl;
        running = false;
        return;
    }
    Mix_AllocateChannels(16); 
    backgroundMusic = Mix_LoadMUS("mixer/background.mp3"); 
    if (!backgroundMusic) {
        cerr << "Failed to load background music: " << Mix_GetError() << endl;
    }
    levelMusic = Mix_LoadMUS("mixer/level_music.mp3"); 
    if (!levelMusic) {
        cerr << "Failed to load level music: " << Mix_GetError() << endl;
    }
    eatFishSound = Mix_LoadWAV("mixer/eat.wav"); 
    if (!eatFishSound) {
        cerr << "Failed to load eat fish sound: " << Mix_GetError() << endl;
    }
    levelUpSound = Mix_LoadWAV("mixer/level_up.wav");
    if (!levelUpSound) {
        cerr << "Failed to load level up sound: " << Mix_GetError() << endl;
    }
    gameOverSound = Mix_LoadWAV("assets/game_over.wav");
    if (!gameOverSound) {
        cerr << "Failed to load game over sound: " << Mix_GetError() << endl;
    }
    clickSound = Mix_LoadWAV("mixer/click.wav");
    if (!clickSound) {
        cerr << "Failed to load click sound: " << Mix_GetError() << endl;
    }
    Mix_VolumeMusic(64); // Âm lượng nhạc nền (0-128)
    Mix_VolumeChunk(eatFishSound, 80);
    Mix_VolumeChunk(levelUpSound, 80);
    Mix_VolumeChunk(gameOverSound, 80);
    Mix_VolumeChunk(clickSound, 80);
    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1); //vong lap vo han 
    }
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
    gameState = MENU; 
    selectedOption = -1; 
    running = true; 
    currentLevel = 1; 
    playerScore = 0; 
    bossSpawned = false;
    showWarning = false; 
    warningStart = 0; 
    warningLife = 600; //doi 2s + life 1,2s
    pendingShark = false; 
    sharkSpawnTime = 0; 
    pendingSharkX = 0; 
    pendingSharkY = 0;
    pendingSharkW = 0;
    pendingSharkH = 0;
    pendingSharkDirection = 0.0f;
    pendingSharkPointValue = 0;
    pendingSharkImage = "";
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
        //menu
    menu = new Menu(renderer, clickSound, &isMusicOn); 
    if (!menu) {
        cerr << "Failed" << endl; 
        running = false; 
        return; 
    }
        //level Back
    std::vector<std::string> backLevel = { "assets/level1Back.png", "assets/level2Back.png" };
    background = new Background(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, "assets/nen.png");
    background->loadBackLevel(backLevel); 
    overlayActive = true; 
    overlayStart = SDL_GetTicks(); 
    overlayLife = 2800 ; 
    int worldW = background->getWidth();
    int worldH = background->getHeight();
    std::cout << "worldW = " << worldW << ", worldH = " << worldH << std::endl; //in ra terminal kiem tra worldW and worldH co loi khong, = 0; loi 
    fish = new Fish(180, 260, 100, 100, 150, renderer, "assets/fish.png", worldW, worldH);

	FishAI::InitScreenSize(worldW, worldH);
	fishAI.push_back(new FishAI( 0, 100, 80, 82, renderer, "assets/cavang20.png", 0.0f, 3.5f, score::getScoreFishAI(imagePathAI,currentLevel)));
	lastSpawnTime = SDL_GetTicks();
    bossSpawned = false; 
    running = true;
}

void Game::update() {
    Uint32 now = SDL_GetTicks();
    if (now - lastSpawnTime >= spawnInterval) {
        lastSpawnTime = now;
        int w = 80, h = 82;
        int worldH = background->getHeight();
        int worldW = background->getWidth();
        //gioi han phia tren
        const int topLimit = worldH - 550;
        const int bottomEdge = worldH;
        if (worldW <= w || worldH <= h) {
            std::cerr << "Invalid world size: worldW = " << worldW << ", worldH" << worldH << std::endl;
            return;
        }
        //random 2 goc sinh ra ca AI
        int edge = rand() % 2;
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
        //level 1 + level 2
        if (currentLevel == 1) {
            FishAI* newFishAI = new FishAI(x, y, w, h, renderer, "assets/cavang20.png", score::scoreCavang);
            if (newFishAI) {
                fishAI.push_back(newFishAI);
                std::cout << "FishAI spawned, total: " << fishAI.size() << std::endl;
            }
            else {
                std::cerr << "Error: Failed to create FishAI" << std::endl;
            }
        }
        else if (currentLevel == 2) {
            const char* fishImages[] = { "assets/cavang20.png", "assets/bachtuoc50.png", "assets/camapsmall.png", "assets/camapbig.png" };
            int imageIndex = rand() % 4;
            const char* fishImage = fishImages[imageIndex];
            int pointValue = (imageIndex == 2 || imageIndex == 3) ? score::getRandomSharkScore() : score::getScoreFishAI(fishImage, currentLevel);
            if (imageIndex == 2 || imageIndex == 3) {
                w = (pointValue >= 600) ? 230 : 140;
                h = (pointValue >= 600) ? 230 : 140;
                pendingShark = true;
                sharkSpawnTime = now + 1200;
                pendingSharkX = x;
                pendingSharkY = y;
                pendingSharkW = w;
                pendingSharkH = h;
                pendingSharkDirection = directionAngle;
                pendingSharkPointValue = pointValue;
                pendingSharkImage = fishImage;
                showWarning = true;
                warningStart = now;
            }
            else {
                FishAI* newFishAI = new FishAI(x, y, w, h, renderer, fishImage, directionAngle, 3.5f, pointValue);
                if (newFishAI) {
                    fishAI.push_back(newFishAI);
                }
            }
        }
    }
    if (pendingShark && now >= sharkSpawnTime) {
        FishAI* newFishAI = new FishAI(pendingSharkX, pendingSharkY, pendingSharkW, pendingSharkH, renderer, pendingSharkImage.c_str(), pendingSharkDirection, 3.5f, pendingSharkPointValue);
        if (newFishAI) {
            fishAI.push_back(newFishAI);
        }
        pendingShark = false;
    }
    fish->move(isKick);
    background->updateCamera(fish->getRect());
    //su xuat hien cua bossFish
    int worldW = background->getWidth();
    int worldH = background->getHeight();
    const int topLimit = worldH - 550;
    const int bottomEdge = worldH;
    const int bw = 100, bh = 102;
    const float speed = 3.5f;
    int levelScore = (currentLevel == 1 ? 50 : 1000); //chon level de render image
    const char* bossImage = (currentLevel == 1 ? "assets/bachtuoc50.png" : "assets/boss1000.png");
    if (!bossSpawned && playerScore > levelScore) {
        int edge = rand() % 2;
        int bx = (edge == 0 ? 0 : worldW - bw);
        int by = topLimit + rand() % (bottomEdge - topLimit + 1);
        bossFish.push_back(new BossFish(bx, by, bw, bh, speed, renderer, bossImage));
        bossSpawned = true;
    }
    //ve boss va cham 
    SDL_Rect playerRect = fish->getRect();
    SDL_Rect headRect = fish->getHeadRect();
    int pcx = playerRect.x + playerRect.w / 2;
    int pcy = playerRect.y + playerRect.h / 2;
    for (auto it = bossFish.begin(); it != bossFish.end(); ) {
        BossFish* b = *it;
        b->updateBoss(pcx, pcy);
        SDL_Rect bRect = b->getRectBoss();
        SDL_Rect intersection;
        // Nếu head của Player chạm boss
        if (SDL_IntersectRect(&headRect, &bRect, &intersection)) {
            int spawnX = intersection.x + intersection.w / 2;
            int spawnY = intersection.y + intersection.h / 2;
            int numBubbles = 5 + rand() % 5;
            for (int i = 0; i < numBubbles; ++i) {
                int offsetX = rand() % 20 - 10;
                int offsetY = rand() % 20 - 10;
                float angle = (rand() % 360) * M_PI / 180.0f;
                float speed = 2.0f + (rand() % 3);
                bubbles.push_back(new Bubble(renderer, spawnX + offsetX, spawnY + offsetY, cosf(angle) * speed, -sinf(angle) * speed, "assets/bubble.png", 50));
            }
            if (currentLevel == 1 && playerScore >= 100) {
                fish->grow(1.4f);
                playerScore += 100;
                delete b;
                it = bossFish.erase(it);
                if (isMusicOn) Mix_PlayChannel(-1, levelUpSound, 0); //phat am thanh level up
                nextLevel();
                return;
            }
            else if (currentLevel == 2 && playerScore >= 2500) {
                    fish->grow(1.0f);
                    playerScore += 2500;
                    delete b;
                    it = bossFish.erase(it);
                    return;
                }
                else {
                    running = false; // boss mạnh hơn → game over
                    if (isMusicOn) Mix_PlayChannel(-1, gameOverSound, 0); 
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
                continue;
            }
            else {
                SDL_Rect aiRect = (*ai)->getRect();
                SDL_Rect pRect = fish->getCollisionRect();
                SDL_Rect headRect = fish->getHeadRect();
                SDL_Rect intersection;
                if (SDL_IntersectRect(&headRect, &aiRect, &intersection)) {
                    //tao bubble
                    int spawnX = intersection.x + intersection.w / 2;
                    int spawnY = intersection.y + intersection.h / 2;
                    int numBubbles;
                    int bubbleSize;
                    bool isShark = ((*ai)->getImagePath() == "assets/camapsmall.png" || (*ai)->getImagePath() == "assets/camapbig.png");
                    if (isShark) {
                        numBubbles = 20 + (rand() % 6);
                        bubbleSize = 100;
                    }
                    else {
                        numBubbles = 5 + (rand() % 5);
                        bubbleSize = 50;
                    }
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
                    if (isShark) {
                        if ((*ai)->getPointValue() > playerScore) {
                            running = false;
                            Mix_PlayChannel(-1, gameOverSound, 0);
                            //SDL_Delay(900);
                            delete* ai;
                            ai = fishAI.erase(ai);
                            continue;
                        }
                        else {
                            if (currentLevel != 2 || !sharkGrow) {
                                fish->grow(1.4f);
                                sharkGrow = true;
                            }
                            if (isMusicOn) Mix_PlayChannel(-1, eatFishSound, 0);
                        }
                    }
                    else {
                        if (isMusicOn) Mix_PlayChannel(-1, eatFishSound, 0);
                    }
                    playerScore += (*ai)->getPointValue();
                    delete* ai;
                    ai = fishAI.erase(ai);
                }
                else {
                    ++ai;
                }
            }
        }
        for (auto it = bubbles.begin(); it != bubbles.end(); ) {
            if (!(*it)->updateBubble(now)) {
                delete* it;
                it = bubbles.erase(it);
            }
            else {
                ++it;
            }
        }
        if (showWarning && (now - warningStart >= warningLife)) {
            showWarning = false;
        }
    }

void Game::render() {
    if (gameState == MENU) {
        menu->render(selectedOption);
    }
    else if (gameState == PLAYING) {
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
        SDL_Rect scorePlayer = { dr.x + dr.w / 2 - textW / 2, dr.y - textH - 10, textW, textH };
        SDL_FreeSurface(scoreSurface);
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scorePlayer);
        SDL_DestroyTexture(scoreTexture);

        //ve scoreAI + AI
        for (auto ai : fishAI) {
            ai->render(cam);
            SDL_Rect aiRect = ai->getRect();
            SDL_Rect aiScreen = { aiRect.x - cam.x, aiRect.y - cam.y, aiRect.w, aiRect.h };
            SDL_Color white = { 255,255,255,255 };
            string aiScoreText = to_string(ai->getPointValue());
            SDL_Surface* aiSurface = TTF_RenderText_Solid(scoreFont, aiScoreText.c_str(), white);
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
            SDL_Color blue = { 0, 255, 255, 255 };
            std::string text = (currentLevel == 1) ? "100" : "2500";
            SDL_Surface* surf = TTF_RenderText_Solid(scoreFont, text.c_str(), blue);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            int w, h; SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            SDL_Rect r = b->getRectBoss();
            SDL_Rect dst = { r.x - cam.x + r.w / 2 - w / 2,r.y - cam.y - h - 5, w, h };
            SDL_FreeSurface(surf);
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
        }
        for (auto bubble : bubbles) {
            bubble->renderBubble(cam);
        }
        //ve warning
        if (showWarning) {
            SDL_Color red{ 255, 0, 0, 255 };
            string warningText = "WARNING !!!";
            SDL_Surface* warningSurface = TTF_RenderText_Solid(scoreFont, warningText.c_str(), red);
            SDL_Texture* warningTexture = SDL_CreateTextureFromSurface(renderer, warningSurface);
            int textH, textW;
            SDL_QueryTexture(warningTexture, nullptr, nullptr, &textW, &textH);
            int screenW = SCREEN_WIDTH;
            SDL_Rect warningRect = { (screenW - textW) / 2, 20, textW, textH }; 
            SDL_FreeSurface(warningSurface);
            SDL_RenderCopy(renderer, warningTexture, nullptr, &warningRect);
            SDL_DestroyTexture(warningTexture);
        }

        //ve level 
    //background->renderBackLevel(currentLevel); 
        if (overlayActive) {
            Uint32 now = SDL_GetTicks();
            Uint32 e = now - overlayStart;
            if (e < overlayLife) {
                SDL_Texture* ov = background->getoverLays(currentLevel);
                if (ov) {
                    float t = float(e) / overlayLife;
                    t *= 2.0f; //tgian mờ nhanh hơn 
                    if (t > 1.0f) t = 1.0f;
                    Uint8 alpha = Uint8(255 * (1.0f - t));
                    SDL_SetTextureAlphaMod(ov, alpha);
                    int ow = SCREEN_WIDTH / 30;
                    int oh = SCREEN_HEIGHT / 60;
                    SDL_QueryTexture(ov, nullptr, nullptr, &ow, &oh);
                    SDL_Rect dst = { (SCREEN_WIDTH - ow) / 2, (SCREEN_HEIGHT - oh) / 2,ow, oh };
                    SDL_RenderCopy(renderer, ov, nullptr, &dst);
                }
                else {
                    overlayActive = false;
                }
            }
        }
    SDL_RenderPresent(renderer);
    }  
}

void Game::nextLevel() {
    for (auto ai : fishAI) delete ai; 
    for (auto b : bossFish) delete b; 
    for (auto bub : bubbles) delete bub; 
    fishAI.clear(); 
    bossFish.clear(); 
    bubbles.clear(); 
    currentLevel++; 
    bossSpawned = false; 
    overlayActive = true; 
    overlayStart = SDL_GetTicks(); 
    showWarning = false; 
    pendingShark = false; 
    sharkGrow = false;

        //nhac chuyen level
    if (isMusicOn && levelMusic) {
        Mix_HaltMusic(); //dung nhac nen
        Mix_PlayMusic( levelMusic, 0); //phat nhac, channel: phat sound
    }
    int worldW = background->getWidth(); 
    int worldH = background->getHeight(); 
    const int topLimit = worldH - 550; 
    const int bottomEdge = worldH;
    int w = 80, h = 82;
    if (currentLevel == 1) {
        fishAI.push_back(new FishAI(0, topLimit + rand() % (bottomEdge - topLimit - h), w, h, renderer, "assets/cavang20.png", 0.0f, 3.5f, score::getScoreFishAI("assets/cavang20.png", currentLevel)));
    }
    else if (currentLevel == 2) {
        //sinh ca vang
        int edge = rand() % 2;
        int x = (edge == 0 ? 0 : worldW - w);
        int y = topLimit + rand() % (bottomEdge - topLimit - h);
        float directionAngle = (edge == 0 ? 0.0f : M_PI);
        fishAI.push_back(new FishAI(x, y, w, h, renderer, "assets/cavang20.png", directionAngle, 3.5f, score::getScoreFishAI("assets/cavang20.png", currentLevel)));
        //sinh bach tuoc
        edge = rand() % 2;
        x = (edge == 0 ? 0 : worldW - w);
        y = topLimit + rand() % (bottomEdge - topLimit - h);
        directionAngle = (edge == 0 ? 0.0f : M_PI);
        fishAI.push_back(new FishAI(x, y, w, h, renderer, "assets/bachtuoc50.png", directionAngle, 3.5f, score::getScoreFishAI("assets/bachtuoc50.png", currentLevel)));
        //sinh shark
        edge = rand() % 2;
        x = (edge == 0 ? 0 : worldW - w);
        y = topLimit + rand() % (bottomEdge - topLimit - h);
        directionAngle = (edge == 0 ? 0.0f : M_PI);
        int pointValue = score::getRandomSharkScore(); // Định nghĩa pointValue
        w = (pointValue >= 600) ? 230 : 140;
        h = (pointValue >= 600) ? 230 : 140;
        fishAI.push_back(new FishAI(x, y, w, h, renderer, "assets/camapsmall.png", directionAngle, 3.5f, score::getRandomSharkScore()));
        pendingShark = true;
        sharkSpawnTime = SDL_GetTicks() + 2000; // Trì hoãn 2 giây
        pendingSharkX = x;
        pendingSharkY = y;
        pendingSharkW = w - 100 ;
        pendingSharkH = h - 100;
        pendingSharkDirection = directionAngle;
        pendingSharkPointValue = pointValue ;
        pendingSharkImage = "assets/camapsmall.png";
        showWarning = true;
        warningStart = SDL_GetTicks();
    }
}

void Game::handleEvents(SDL_Event& event) {
    if (gameState == MENU) {
        int action = menu->handleEvents(event); 
        switch (action) {
        case 1: //PLAY
            gameState = PLAYING;
            Mix_HaltMusic(); 
            if (isMusicOn && levelMusic) {
                Mix_PlayMusic(levelMusic, -1);
            }
            //selectedOption = -1;
            currentLevel = 1;
            playerScore = 0;
            fishAI.clear();
            bossFish.clear();
            bubbles.clear();
            bossSpawned = false;
            overlayActive = true;
            overlayStart = SDL_GetTicks();
            fish->reset(180, 260, 85, 82);
            break; 
        case 2: 
            isMusicOn = !isMusicOn; 
            if (isMusicOn) {
                Mix_ResumeMusic(); 
            }
            else {
                Mix_HaltMusic(); 
            }
            selectedOption = -1;
            break;
        case 3: // QUIT
            running = false;
            break;
        }
    }
    else if (gameState == PLAYING) {
        SDL_Rect cam = background->getCamera();
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int worldX = event.button.x + cam.x;
            int worldY = event.button.y + cam.y;
            SDL_Rect r = fish->getRect();
            if (worldX >= r.x && worldX <= r.x + r.w && worldY >= r.y && worldY <= r.y + r.h) {
                isKick = true;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            isKick = false;
        }
        else if (event.type == SDL_MOUSEMOTION && isKick) {
            int worldX = event.motion.x + cam.x;
            int worldY = event.motion.y + cam.y;
            fish->setTarget(worldX, worldY);
        }
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
        if (isMusicOn && !Mix_PlayingMusic() && backgroundMusic) {
            Mix_PlayMusic(backgroundMusic, -1);
        }
        /*if (!Mix_PlayingMusic() && backgroundMusic) {
            Mix_PlayMusic(backgroundMusic, -1); 
        }*/
        SDL_Delay(15);
    }
}

Game::~Game() {
    delete menu; 
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
    }
    if (levelMusic) {
        Mix_FreeMusic(levelMusic);
    }
    if (eatFishSound) {
        Mix_FreeChunk(eatFishSound);
    }
    if (levelUpSound) {
        Mix_FreeChunk(levelUpSound);
    }
    if (gameOverSound) {
        Mix_FreeChunk(gameOverSound);
    }
    Mix_CloseAudio();
    Mix_Quit();

    for (auto ai : fishAI) delete ai;
    fishAI.clear();
    if (fishAITex) SDL_DestroyTexture(fishAITex);
    delete fish;
    delete background;
    if (scoreFont) {
        TTF_CloseFont(scoreFont); 
    }
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}