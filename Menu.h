#pragma once
#include <SDL.h>
#include <SDL_mixer.h>

class Menu {
public:
	enum MenuState { MAIN };
	Menu(SDL_Renderer* renderer, Mix_Chunk* clickSound = nullptr, bool* isMusicOn = nullptr ); 
	~Menu(); 
	void render (int selectedOption); 
	int handleEvents(SDL_Event& e);
	bool* isMusicOn; 

private:
	SDL_Renderer* renderer; 
	SDL_Texture* menuTexture; 
	Mix_Chunk* clickSound; 
	int selectedOption; 
	SDL_Rect optionRect[3];
};
