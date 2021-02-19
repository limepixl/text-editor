#pragma once
#include <vector>
#include <string>

void ProcessText(union SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int& lastCursorX, int numColls);

void MoveWordLeft(int& cursorX, int& cursorY, std::vector<std::string>& contentRows);
void MoveWordRight(int& cursorX, int& cursorY, std::vector<std::string>& contentRows);

void DeleteWordLeft(int& cursorX, int cursorY, std::vector<std::string>& contentRows);
void DeleteWordRight(int cursorX, int cursorY, std::vector<std::string>& contentRows);

void IncrementX(int& cursorX, int& cursorY, int& lastCursorX, int numColls, std::vector<std::string>& contentRows);
void DecrementX(int& cursorX, int& lastCursorX);
void IncrementY(int& cursorX, int& cursorY, int lastCursorX, std::vector<std::string>& contentRows, int editableRows);
void DecrementY(int& cursorX, int& cursorY, int lastCursorX, std::vector<std::string>& contentRows);