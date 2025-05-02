#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "const.h"
#include <direct.h>  

using namespace std;

int main(int argc, char* argv[]) {
    char* base = SDL_GetBasePath();
    SDL_Log("Base path: %s", base);
    SDL_free(base);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init : " << SDL_GetError() << std::endl;
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Game Feeding_frezy;
    if (Feeding_frezy.running) {
        Feeding_frezy.run();
    }
    IMG_Quit();
    SDL_Quit();
    return 0;
}