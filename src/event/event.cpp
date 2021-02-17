#include "event.hpp"
#include <SDL2/SDL.h>

void ProcessText(SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int& lastCursorX, int numColls)
{
	std::string& current = contentRows[cursorY];
	current += e.text.text;
	if((int)current.size() > numColls)
		current.pop_back();
	
	IncrementX(cursorX, cursorY, lastCursorX, numColls, contentRows);
}