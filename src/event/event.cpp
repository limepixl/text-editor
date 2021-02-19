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

// Returns the index of the first position
// of character c in string str.
inline int find_first(std::string& str, char c, int from)
{
	int sz = (int)str.size();
	if(from > sz || from < 0)
		return sz;
	
	for(int i = from; i < sz; i++)
	{
		if(str[i] == c)
			return i;
	}

	return sz;
}

// Returns the index of the last position 
// of character c in string str.
inline int find_last(std::string& str, char c, int from)
{
	if(from > (int)str.size() || from < 0)
		return -1;

	for(int i = from; i >= 0; i--)
	{
		if(str[i] == c)
			return i;
	}

	return -1;
}

// Returns the index of the first position
// of a character not equal to c in string str.
inline int find_first_not(std::string& str, char c, int from)
{
	int sz = (int)str.size();
	if(from > sz || from < 0)
		return sz;

	for(int i = from; i < sz; i++)
	{
		if(str[i] != c)
			return i-1;
	}

	return sz;
}

// Returns the index of the last position
// of a character not equal to c in string str.
inline int find_last_not(std::string& str, char c, int from)
{
	if(from > (int)str.size() || from < 0)
		return -1;

	for(int i = from; i >= 0; i--)
	{
		if(str[i] != c)
			return i+1;
	}

	return -1;
}

void MoveWordLeft(int& cursorX, int& cursorY, std::vector<std::string>& contentRows)
{
	std::string current = contentRows[cursorY];

	// Go to beginning of current word
	if(current[cursorX] != ' ')
		cursorX = std::max(0, find_last(current, ' ', cursorX));
	// Go in front of previous word
	else if(current[cursorX] == ' ' && current[cursorX - 1] != ' ')
		cursorX = std::max(0, find_last(current, ' ', cursorX - 1));
	// skip all spaces and go after previous word
	else
		cursorX = std::max(0, find_last_not(current, ' ', cursorX));
}

void MoveWordRight(int& cursorX, int& cursorY, std::vector<std::string>& contentRows)
{
	std::string current = contentRows[cursorY];
	int size = (int)current.size();

	// Go to end of current word
	if(current[cursorX] != ' ')
		cursorX = std::min(size, find_first(current, ' ', cursorX));
	// Go to end of next word
	else if(current[cursorX] == ' ' && current[cursorX + 1] != ' ')
		cursorX = std::min(size, find_first(current, ' ', cursorX + 1));
	// skip all spaces and go before next word
	else
		cursorX = std::min(size, find_first_not(current, ' ', cursorX));
}

void DeleteWordLeft(int& cursorX, int cursorY, std::vector<std::string>& contentRows)
{
	std::string& current = contentRows[cursorY];
	int lastCursorX = cursorX;

	// Go to beginning of current word
	if(current[cursorX] != ' ')
	{
		// Remove all whitespace
		if(cursorX >= 1 && current[cursorX-1] == ' ')
			cursorX = std::max(0, find_last_not(current, ' ', cursorX - 1));
		// Remove up until whitespace
		else
			cursorX = std::max(0, find_last(current, ' ', cursorX));
	}
	// Go in front of previous word
	else if(current[cursorX] == ' ' && current[cursorX - 1] != ' ')
		cursorX = std::max(0, find_last(current, ' ', cursorX - 1));
	// skip all spaces and go after previous word
	else
		cursorX = std::max(0, find_last_not(current, ' ', cursorX));

	if(lastCursorX - cursorX != 0)
		current.erase(current.begin() + cursorX, current.begin() + lastCursorX);
}

void DeleteWordRight(int cursorX, int cursorY, std::vector<std::string>& contentRows)
{
	std::string& current = contentRows[cursorY];
	int size = (int)current.size();

	int lastCursorX = cursorX;

	// Go to end of current word
	if(current[cursorX] != ' ')
	{
		// Remove all whitespace
		if(cursorX < size - 1 && current[cursorX + 1] == ' ')
			cursorX = std::min(size, find_first_not(current, ' ', cursorX + 1));
		// Remove up until whitespace
		else
			cursorX = std::min(size, find_first(current, ' ', cursorX));
	}
	// Go to end of next word
	else if(current[cursorX] == ' ' && current[cursorX + 1] != ' ')
		cursorX = std::min(size, find_first(current, ' ', cursorX + 1));
	// skip all spaces and go before next word
	else
		cursorX = std::min(size, find_first_not(current, ' ', cursorX));

	if(cursorX - lastCursorX != 0)
		current.erase(current.begin() + lastCursorX, current.begin() + cursorX + 1);
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