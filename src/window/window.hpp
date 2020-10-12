#pragma once

struct SDL_Window;

struct Display
{
    unsigned int width, height;
    SDL_Window* window;

    Display(const char* title, unsigned int width, unsigned int height);
};