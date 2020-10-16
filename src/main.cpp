#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <cstdio>
#include "window/window.hpp"
#include "asset_load/asset_load.hpp"

unsigned int Texture::NumTexturesLoaded = 0;

int main()
{
	SDL_SetMainReady();

	Display display("Text Editor", 500, 500);

	std::vector<Char> chars = ParseFNT("res/font/roboto-mono.fnt");
	Char& currentChar = chars['a' - chars[0].c];

	float vertices[]
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, (int)currentChar.uvs.size() * sizeof(float), currentChar.uvs.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	Texture atlas = LoadTextureFromBinary("res/font/roboto-mono.bin");

	Shader shader = LoadShaderFromFile("res/shader/basic.vert", "res/shader/basic.frag");
	glUseProgram(shader.ID);
	glUniform1i(shader.uniforms["tex"], atlas.index);

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

		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(display.window);
	}

	// Cleanup
	SDL_Quit();
	return 0;
}