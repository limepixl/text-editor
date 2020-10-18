#include "event.hpp"
#include <SDL2/SDL.h>

inline void IncrementX(int& cursorX, int& cursorY, int numColls, std::vector<std::string>& contentRows)
{
	if(contentRows[cursorY].empty())
		return;

	if(cursorX < (int)contentRows[cursorY].size() && cursorX < numColls - 1)
		cursorX++;
}

inline void DecrementX(int& cursorX)
{
	if(cursorX > 0)
		cursorX--;
}

inline void IncrementY(int& cursorX, int& cursorY, int numRows, std::vector<std::string>& contentRows)
{
	if(cursorY < numRows - 1)
		cursorY++;

	// Adjust cursorX to be at the end of the next line
	if(cursorX > (int)contentRows[cursorY].size())
		cursorX = (int)contentRows[cursorY].size();
}

inline void DecrementY(int& cursorX, int& cursorY, std::vector<std::string>& contentRows)
{
	if(cursorY > 0)
		cursorY--;
	
	// Adjust cursorX to be at the end of the previous line
	if(cursorX > (int)contentRows[cursorY].size())
		cursorX = (int)contentRows[cursorY].size();
}

void ProcessKeyboard(SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int numRows, int numColls)
{
	SDL_Keycode& keycode = e.key.keysym.sym;
	SDL_Scancode& scancode = e.key.keysym.scancode;
	if(keycode == SDLK_LEFT)
		DecrementX(cursorX);

	if(keycode == SDLK_RIGHT)
		IncrementX(cursorX, cursorY, numColls, contentRows);

	if(keycode == SDLK_UP)
		DecrementY(cursorX, cursorY, contentRows);

	if(keycode == SDLK_DOWN)
		IncrementY(cursorX, cursorY, numRows, contentRows);

	if(keycode >= SDLK_SPACE && keycode <= SDLK_AT)
	{
		std::string& currentString = contentRows[cursorY];
		if((int)currentString.size() < numColls)
			currentString.insert(currentString.begin() + cursorX, (char)keycode);
		IncrementX(cursorX, cursorY, numColls, contentRows);
	}

	// TODO: scancodes for last few characters
}