#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <cstdio>
#include <cmath>
#include "window/window.hpp"
#include "asset_load/asset_load.hpp"
#include "event/event.hpp"

#include <glm/gtc/matrix_transform.hpp>

unsigned int Texture::NumTexturesLoaded = 0;

int main()
{
	SDL_SetMainReady();

	// Parse the FNT file to get info for each character
	std::vector<Char> chars = ParseFNT("res/font/roboto-mono.fnt");

	// Scale down / up font size
	float scale = 0.5f;
	float fontWidth = chars[0].charWidth * scale;
	float fontHeight = chars[0].charHeight * scale;

	int windowWidth = 1280;
	int windowHeight = 720;
	Display display("Text Editor", windowWidth, windowHeight);

	// Editor layout
	int numRows = windowHeight / fontHeight;
	int numColls = windowWidth / fontWidth;
	std::vector<std::string> contentRows;
	contentRows.resize(numRows);

	std::vector<float> uvs, vertices;
	uvs.reserve(numRows * numColls * 12);
	vertices.reserve(numRows * numColls * 18);

	// Cursor info
	int cursorX = 0;
	int cursorY = 0;
	int lastCursorX = 0;

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

	// Used for FPS
	Uint64 start, end;
	double targetFPS = 1000.0f / 60.0f /*change this*/;

	SDL_StartTextInput();

	// Render loop
	while(true)
	{
		// Start counting ms
		start = SDL_GetPerformanceCounter();

		// Handle events
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
			else if(e.type == SDL_WINDOWEVENT)
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					display.width = e.window.data1;
					display.height = e.window.data2;

					// Update viewport and projection matrix
					glViewport(0, 0, display.width, display.height);
					projection = glm::ortho(0.0f, (float)display.width, (float)display.height, 0.0f);
					glUseProgram(shader.ID);
					glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
					glUseProgram(cursorShader.ID);
					glUniformMatrix4fv(cursorShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

					numRows = display.height / fontHeight;
					numColls = display.width / fontWidth;

					// Update content vector
					contentRows.resize(numRows);
				}
			}
			else if(e.type == SDL_KEYDOWN)
			{
				SDL_Keycode& code = e.key.keysym.sym;

				// Backspace
				if(code == SDLK_BACKSPACE)
				{
					// If there are characters to be deleted
					if((int)contentRows[cursorY].size() > 0 && cursorX > 0)
					{
						contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX - 1);
						cursorX--;
					}
					else if((int)contentRows[cursorY].size() == 0) // There isn't a character in the line, so go to the previous line
					{

					}
				}
				else if(code == SDLK_LEFT)
					DecrementX(cursorX, lastCursorX);
				else if(code == SDLK_RIGHT)
					IncrementX(cursorX, cursorY, lastCursorX, numColls, contentRows);
				else if(code == SDLK_UP)
					DecrementY(cursorX, cursorY, lastCursorX, contentRows);
				else if(code == SDLK_DOWN || code == SDLK_RETURN)
					IncrementY(cursorX, cursorY, lastCursorX, numRows, contentRows);
				else if(code == SDLK_DELETE && (int)contentRows[cursorY].size() > 0 && cursorX < (int)contentRows[cursorY].size())
					contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX);
				else if(code == SDLK_HOME)
					cursorX = 0;
				else if(code == SDLK_END)
					cursorX = (int)contentRows[cursorY].size();

				// TODO: copy/paste
			}
			else if(e.type == SDL_TEXTINPUT)
				ProcessText(e, contentRows, cursorX, cursorY, lastCursorX, numRows, numColls);
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

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

		glFinish();
		SDL_GL_SwapWindow(display.window);

		// End counting ms and calculate elapsed ms
		end = SDL_GetPerformanceCounter();
		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		printf("Elapsed ms: %.2f\nFPS: %.2f\n", elapsed, 1000.0 / elapsed);
		if(elapsed < targetFPS)
			SDL_Delay(floor(targetFPS - elapsed));
	}

	// Cleanup
	SDL_StopTextInput();
	SDL_Quit();
	return 0;
}