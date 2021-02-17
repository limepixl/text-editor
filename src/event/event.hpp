#pragma once
#include <vector>
#include <string>

void ProcessText(union SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int& lastCursorX, int numRows, int numColls);

inline void IncrementX(int& cursorX, int& cursorY, int& lastCursorX,  int numColls, std::vector<std::string>& contentRows)
{
	if(contentRows[cursorY].empty())
		return;

	if(cursorX < (int)contentRows[cursorY].size() && cursorX < numColls - 1)
	{
		cursorX++;
		lastCursorX = cursorX;
	}
}

inline void DecrementX(int& cursorX, int& lastCursorX)
{
	if(cursorX > 0)
	{
		cursorX--;
		lastCursorX = cursorX;
	}
}

inline void IncrementY(int& cursorX, int& cursorY, int lastCursorX, int numRows, std::vector<std::string>& contentRows)
{
	if(cursorY < numRows - 1)
		cursorY++;

	// Adjust cursorX to be at the end of the next line
	if(cursorX > (int)contentRows[cursorY].size())
		cursorX = (int)contentRows[cursorY].size();
	else
		cursorX = std::min((int)contentRows[cursorY].size(), lastCursorX);
}

inline void DecrementY(int& cursorX, int& cursorY, int lastCursorX, std::vector<std::string>& contentRows)
{
	if(cursorY > 0)
		cursorY--;
	
	// Adjust cursorX to be at the end of the previous line
	if(cursorX > (int)contentRows[cursorY].size())
		cursorX = (int)contentRows[cursorY].size();
	else
		cursorX = std::min((int)contentRows[cursorY].size(), lastCursorX);
}