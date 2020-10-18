#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <cstdio>
#include <cmath>
#include "window/window.hpp"
#include "asset_load/asset_load.hpp"

#include <glm/gtc/matrix_transform.hpp>

unsigned int Texture::NumTexturesLoaded = 0;

int main()
{
	SDL_SetMainReady();

	// Parse the FNT file to get info for each character
	std::vector<Char> chars = ParseFNT("res/font/roboto-mono.fnt");

	// Scale down / up font size
	float scale = 0.5;
	float fontWidth = chars[0].charWidth * scale;
	float fontHeight = chars[0].charHeight * scale;

	int numRows = 25;
	int numColls = 80;
	std::vector<std::string> contentRows;
	contentRows.resize(numRows);

	std::vector<float> uvs, vertices;
	uvs.reserve(numRows * numColls * 12);
	vertices.reserve(numRows * numColls * 18);

	int windowWidth = numColls * fontWidth;
	int windowHeight = numRows * fontHeight;
	Display display("Text Editor", windowWidth, windowHeight);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, numRows * numColls * 18 * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, numRows * numColls * 12 * sizeof(float), uvs.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	Texture atlas = LoadTextureFromBinary("res/font/roboto-mono.bin");

	Shader shader = LoadShaderFromFile("res/shader/basic.vert", "res/shader/basic.frag");
	glUseProgram(shader.ID);
	glUniform1i(shader.uniforms["tex"], atlas.index);

	glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
	glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

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

		// Parse text file as rows of characters
		contentRows.clear();
		contentRows.resize(numRows);
		ParseText("res/textfiles/test1.txt", contentRows);

		for(int i = 0; i < numRows; i++)
		{
			int jmax = std::min((int)contentRows[i].size(), numColls);
			for(int j = 0; j < jmax; j++)
			{
				Char& currentChar = chars[contentRows[i][j] - ' '];
				uvs.insert(uvs.end(), currentChar.uvs.begin(), currentChar.uvs.end());

				float startx = j * fontWidth;
				float starty = i * fontHeight;
				float endx = (j + 1) * fontWidth;
				float endy = (i + 1) * fontHeight;

				std::vector<float> tempVec
				{
					startx, starty, 0.0f, 
					endx, starty, 0.0f,
					endx, endy, 0.0f,
					endx, endy, 0.0f, 
					startx, endy, 0.0f,
					startx, starty, 0.0f
				};
				vertices.insert(vertices.end(), tempVec.begin(), tempVec.end());
			}
		}

		// Update buffer contents
		int vertexCount = (int)vertices.size() / 3;

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (int)vertices.size() * sizeof(float), vertices.data());
		if(!vertices.empty())
			vertices.clear();

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (int)uvs.size() * sizeof(float), uvs.data());
		if(!uvs.empty())
			uvs.clear();

		glClear(GL_COLOR_BUFFER_BIT);

		if(vertexCount > 0)
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		SDL_GL_SwapWindow(display.window);
	}

	// Cleanup
	SDL_Quit();
	return 0;
}