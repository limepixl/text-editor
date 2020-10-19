#include "event.hpp"
#include <SDL2/SDL.h>

void ProcessKeyboard(std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int numRows, int numColls)
{
	

	int numKeys = 0;
	const Uint8* state = SDL_GetKeyboardState(&numKeys);
	if(state == nullptr)
		return;

	if(state[SDL_SCANCODE_LEFT])
		DecrementX(cursorX);
	else if(state[SDL_SCANCODE_RIGHT])
		IncrementX(cursorX, cursorY, numColls, contentRows);
	else if(state[SDL_SCANCODE_UP])
		DecrementY(cursorX, cursorY, contentRows);
	else if(state[SDL_SCANCODE_DOWN])
		IncrementY(cursorX, cursorY, numRows, contentRows);

	

/*
	if(keycode >= SDLK_SPACE && keycode <= SDLK_AT)
	{
		std::string& currentString = contentRows[cursorY];
		currentString.insert(currentString.begin() + cursorX, (char)keycode);
		if((int)currentString.size() > numColls)
			currentString.pop_back();
		IncrementX(cursorX, cursorY, numColls, contentRows);
	}
*/
	// TODO: scancodes for last few characters
}

void ProcessText(SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int numRows, int numColls)
{
	std::string& current = contentRows[cursorY];
	current += e.text.text;
	if((int)current.size() > numColls)
		current.erase(current.begin() + numColls + 1, current.end());
	
	IncrementX(cursorX, cursorY, numColls, contentRows);
}