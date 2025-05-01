#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "const.h"

using namespace std;

int main(int argc, char* argv[]) {
	Game Feeding_frezy;
	if (Feeding_frezy.running) {
		Feeding_frezy.run();
	}
	return 0;
}
