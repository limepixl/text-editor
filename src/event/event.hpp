#pragma once
#include <vector>
#include <string>

void ProcessKeyboard(std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int numRows, int numColls);
void ProcessText(union SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int numRows, int numColls);

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