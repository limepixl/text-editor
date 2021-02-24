#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <cstdio>
#include "window/window.hpp"
#include "editor/editor.hpp"

unsigned int Texture::NumTexturesLoaded = 0;

int main()
{
	SDL_SetMainReady();

	int windowWidth = 1280;
	int windowHeight = 720;
	Display display("Text Editor", windowWidth, windowHeight);

	// Parse font from ttf file
	int pointSize = 25;
	FontData fontData = ParseFontFT("res/font/Hack-ttf/Hack-Regular.ttf", pointSize);
	Editor editor(display, fontData);

	// Used for FPS
	Uint64 start, end;
	double targetFPS = 1000.0f / 60.0f;

	SDL_StartTextInput();

	// Render loop
	while(true)
	{
		// Start counting ms
		start = SDL_GetPerformanceCounter();

		editor.ProcessEvents();
		editor.Draw();

		SDL_GL_SwapWindow(display.window);

		// End counting ms and calculate elapsed ms
		end = SDL_GetPerformanceCounter();

		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		//printf("Elapsed ms: %.2f\n", elapsed);
		if(elapsed < targetFPS)
		{
			//printf("Delaying for: %d\n", (Uint32)floor(targetFPS - elapsed));
			SDL_Delay((Uint32)floor(targetFPS - elapsed));
		}
		end = SDL_GetPerformanceCounter();
		elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		//printf("FPS: %.2f\n", 1000.0 / elapsed);
	}

	// Cleanup
	SDL_StopTextInput();
	SDL_Quit();
	return 0;
}