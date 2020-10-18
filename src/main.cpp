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

	// Editor layout
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

	// Cursor info
	int cursorX = 0;
	int cursorY = 0;

	float cursorVerts[]
	{
		0.0f, 0.0f, 0.0f,
		fontWidth, 0.0f, 0.0f,
		fontWidth, fontHeight, 0.0f,
		fontWidth, fontHeight, 0.0f,
		0.0f, fontHeight, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	
	GLuint cursorVAO;
	glGenVertexArrays(1, &cursorVAO);
	glBindVertexArray(cursorVAO);

	GLuint cursorVBO;
	glGenBuffers(1, &cursorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), cursorVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// Editor's data
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

	Shader cursorShader = LoadShaderFromFile("res/shader/cursor.vert", "res/shader/cursor.frag");
	glUseProgram(cursorShader.ID);
	glUniformMatrix4fv(cursorShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	// Render loop
	while(true)
	{
		// Handle events
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
			case SDL_QUIT:
				SDL_Quit();
				return 0;
	
				break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_LEFT)
				{
					if(cursorX == 0)
					{
						if(cursorY > 0)
						{
							cursorX = numColls;
							cursorY--;
						}
					}
					else
						cursorX--;
				}
	
				if(e.key.keysym.sym == SDLK_RIGHT)
				{
					if(cursorX == numColls - 1)
					{
						if(cursorY < numRows - 1)
						{
							cursorX = 0;
							cursorY++;
						}
					} 
					else
						cursorX++;
				}
	
				if(e.key.keysym.sym == SDLK_UP)
				{
					if(cursorY > 0)
						cursorY--;
				}
	
				if(e.key.keysym.sym == SDLK_DOWN)
				{
					if(cursorY < numRows - 1)
						cursorY++;
				}
	
				break;
			default:
				break;	
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

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

		glUseProgram(shader.ID);

		// Update buffer contents
		int vertexCount = (int)vertices.size() / 3;

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (int)vertices.size() * sizeof(float), vertices.data());
		if(!vertices.empty())
			vertices.clear();

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (int)uvs.size() * sizeof(float), uvs.data());
		if(!uvs.empty())
			uvs.clear();

		if(vertexCount > 0)
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		// Update cursor's model matrix
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(cursorX * fontWidth, cursorY * fontHeight, 0.0f));
		glUseProgram(cursorShader.ID);
		glUniformMatrix4fv(cursorShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glBindVertexArray(cursorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(display.window);
	}

	// Cleanup
	SDL_Quit();
	return 0;
}