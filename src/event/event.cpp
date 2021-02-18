#include "event.hpp"
#include <SDL2/SDL.h>

void ProcessText(SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int& lastCursorX, int numColls)
{
	std::string& current = contentRows[cursorY];
	const char* text = e.text.text;
	std::string strText(text);

	if(cursorX == (int)current.size())
		current += text;
	else
		current = std::string(current.begin(), current.begin() + cursorX) +
		          strText + std::string(current.begin() + cursorX, current.end());

	if((int)current.size() > numColls)
		current.pop_back();
	
	IncrementX(cursorX, cursorY, lastCursorX, numColls, contentRows);
}

void MoveWordLeft(int& cursorX, int& cursorY, std::vector<std::string>& contentRows)
{
	std::string current = contentRows[cursorY];

	for(int i = cursorX - 1; i >= -1; i--)
	{
		if(i == 0)
		{
			cursorX = 0;
			return;
		}

		if(i < 0)
		{
			if(cursorY > 0)
			{
				current = contentRows[--cursorY];
				cursorX = (int)current.length();
				return;
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

void MoveWordRight(int& cursorX, int& cursorY, std::vector<std::string>& contentRows, int editableRows)
{
	std::string current = contentRows[cursorY];

	for(int i = cursorX + 1; i <= (int)current.size() + 1; i++)
	{
		if(i == (int)current.size() - 1)
		{
			cursorX = (int)current.size() - 1;
			return;
		}

		if(i >= (int)current.size())
		{
			if(cursorY < editableRows - 1)
			{
				current = contentRows[++cursorY];
				cursorX = 0;
				return;
			}

			cursorX = (int)current.size() - 1;
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

void DeleteWordRight(int cursorX, int cursorY, std::vector<std::string>& contentRows)
{
	std::string& currentLine = contentRows[cursorY];

	for(int i = cursorX + 1; i <= (int)currentLine.size() + 1; i++)
	{
		if(i == (int)currentLine.size())
		{
			currentLine.erase(currentLine.begin() + cursorX, currentLine.end());
			return;
		}

		if(currentLine[i] == ' ')
		{
			currentLine.erase(currentLine.begin() + cursorX, currentLine.begin() + i);
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