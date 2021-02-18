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

void MoveWordLeft(int& cursorX, int& cursorY, std::vector<std::string>& contentRows)
{
	std::string& current = contentRows[cursorY];

	for(int i = cursorX - 1; i >= 0; i--)
	{
		if(i <= 0)
		{
			if(cursorY > 0)
			{
				current = contentRows[--cursorY];
				i = (int)current.length();
				continue;
			}
			
			cursorX = 0;
			return;
		}

		if(current[i] == ' ')
		{
			cursorX = i;
			return;
		}
	}
}

void DeleteWordLeft(int& cursorX, int cursorY, std::vector<std::string>& contentRows)
{
	std::string& currentLine = contentRows[cursorY];

	for(int i = cursorX - 1; i >= 0; i--)
	{
		if(i <= 0)
		{
			currentLine.erase(currentLine.begin(), currentLine.begin() + cursorX);
			cursorX = 0;
			return;
		}

		if(currentLine[i] == ' ')
		{
			currentLine.erase(currentLine.begin() + i, currentLine.begin() + cursorX);
			cursorX = i;
			return;
		}
	}
}

void IncrementX(int& cursorX, int& cursorY, int& lastCursorX, int numColls, std::vector<std::string>& contentRows)
{
	if(contentRows[cursorY].empty())
		return;

	if(cursorX < (int)contentRows[cursorY].size() && cursorX < numColls - 1)
	{
		cursorX++;
		lastCursorX = cursorX;
	}
}

void DecrementX(int& cursorX, int& lastCursorX)
{
	if(cursorX > 0)
	{
		cursorX--;
		lastCursorX = cursorX;
	}
}

void IncrementY(int& cursorX, int& cursorY, int lastCursorX, std::vector<std::string>& contentRows, int editableRows)
{
	if(cursorY < editableRows - 1)
		cursorY++;

	// Adjust cursorX to be at the end of the next line
	if(cursorX > (int)contentRows[cursorY].size())
		cursorX = (int)contentRows[cursorY].size();
	else
		cursorX = std::min((int)contentRows[cursorY].size(), lastCursorX);
}

void DecrementY(int& cursorX, int& cursorY, int lastCursorX, std::vector<std::string>& contentRows)
{
	if(cursorY > 0)
		cursorY--;
	
	// Adjust cursorX to be at the end of the previous line
	if(cursorX > (int)contentRows[cursorY].size())
		cursorX = (int)contentRows[cursorY].size();
	else
		cursorX = std::min((int)contentRows[cursorY].size(), lastCursorX);
}