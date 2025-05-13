#include "Menu.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include "const.h"

using namespace std; 

Menu::Menu(SDL_Renderer* renderer, Mix_Chunk* clickSound, bool* isMusicOn) :
	renderer(renderer), clickSound(clickSound), isMusicOn(isMusicOn), selectedOption(-1) {
	menuTexture = IMG_LoadTexture(renderer, "assets/menu.png");
	if (!menuTexture) {
		cerr << "Falied to load menu" << IMG_GetError() << endl;
	}
	optionRect[0] = { 300, 420, 200, 65 }; // PLAY
	optionRect[1] = { 300, 486, 200, 65}; // MUSIC
	optionRect[2] = { 300, 546, 200, 65 }; // QUIT

}
Menu::~Menu() {
	if (menuTexture) {
		SDL_DestroyTexture(menuTexture); 
	}
}
void Menu::render(int ) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255); 
	SDL_RenderClear(renderer); 
	if (menuTexture) {
		SDL_RenderCopy(renderer, menuTexture, nullptr, nullptr); 
	}
	if (this->selectedOption >= 0 && selectedOption < 3) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 10); // Hiệu ứng sáng vàng
		SDL_RenderFillRect(renderer, &optionRect[selectedOption]);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	}
	SDL_RenderPresent(renderer); 
}
int Menu::handleEvents(SDL_Event& event) {
	this->selectedOption = -1; 
	if (event.type == SDL_QUIT) {
		return 3;
	}
	else if (event.type == SDL_MOUSEMOTION) {
		int x = event.motion.x; 
		int y = event.motion.y; 
		for (int i = 0; i < 3; i++) {
			if (x >= optionRect[i].x && x <= optionRect[i].x + optionRect[i].w && y >= optionRect[i].y && y <= optionRect[i].y + optionRect[i].h) {
				this->selectedOption = i; 
				break; 
			}
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		int x = event.button.x;
		int y = event.button.y;

		for (int i = 0; i < 3; i++) {
			if (x >= optionRect[i].x && x <= optionRect[i].x + optionRect[i].w &&
				y >= optionRect[i].y && y <= optionRect[i].y + optionRect[i].h) {
				this->selectedOption = i;
				if (clickSound) Mix_PlayChannel(-1, clickSound, 0);
				if (i == 0) return 1; // PLAY
				if (i == 1) return 2; // MUSIC
				if (i == 2) return 3; // QUIT
			}
		}
	}
	return 0;
}