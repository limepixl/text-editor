#pragma once
#include <vector>
#include <string>

void ProcessKeyboard(union SDL_Event& e, std::vector<std::string>& contentRows, int& cursorX, int& cursorY, int numRows, int numColls);