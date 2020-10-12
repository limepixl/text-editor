#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cstdio>

int main()
{
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Unable to initialize SDL2!\n");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow(
		"Text Editor",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if(window == nullptr)
	{
		printf("Failed to create SDL2 window\n");
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == nullptr)
	{
		printf("Failed to create OpenGL context\n");
		return -1;
	}

	// Init GLAD
	if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
	{
		printf("Failed to init GLAD with SDL2\n");
		return -1;
	}
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Render loop
	while(true)
	{
		// Handle events
		SDL_Event e;
		SDL_PollEvent(&e);
		if(e.type == SDL_QUIT)
		{
			SDL_Quit();
			return 0;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	SDL_Quit();
	return 0;
}