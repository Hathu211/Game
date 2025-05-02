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
    IMG_Init(IMG_INIT_PNG);

	Game Feeding_frezy;
	if (Feeding_frezy.running) {
		Feeding_frezy.run();
	}
	return 0;
}
