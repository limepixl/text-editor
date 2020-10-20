#include "window.hpp"
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cstdio>

Display::Display(const char* title, unsigned int width, unsigned int height)
    : width(width), height(height)
{
    if(SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Unable to initialize SDL2!\n");
		SDL_Quit();
	}

	window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		width,
		height,
		SDL_WINDOW_OPENGL
	);

	if(window == nullptr)
	{
		printf("Failed to create SDL2 window\n");
		SDL_Quit();
	}

    // Setup OpenGL context settings
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == nullptr)
	{
		printf("Failed to create OpenGL context\n");
		SDL_Quit();
	}

	// Init GLAD
	if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
	{
		printf("Failed to init GLAD with SDL2\n");
		SDL_Quit();
	}
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(SDL_GL_SetSwapInterval(1))
		printf("Failed to set swap interval to 1.\n%s", SDL_GetError());
}