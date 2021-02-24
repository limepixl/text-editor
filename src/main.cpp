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

	int windowWidth = 1280;
	int windowHeight = 720;
	Display display("Text Editor", windowWidth, windowHeight);

	// Parse font from ttf file
	int pointSize = 20;
	FontData fontData = ParseFontFT("res/font/Hack-ttf/Hack-Regular.ttf", pointSize);
	FTChar& tChar = fontData.loadedCharacters['T'];

	float fontWidth = 0.0f;
	float fontHeight = 0.0f;
	for(auto& f : fontData.loadedCharacters)
	{
		if(fontWidth < f.size.x)
			fontWidth = f.size.x;
		if(fontHeight < f.size.y)
			fontHeight = f.size.y;
	}

	// Editor layout
	int numRows = windowHeight / fontHeight;
	int numColls = windowWidth / fontWidth;
	std::vector<std::string> contentRows;

	int actualRows, actualColls;
	ParseText("res/textfiles/test1.txt", contentRows, actualRows, actualColls);
	int editableRows = (int)contentRows.size();

	std::vector<float> uvs, vertices;
	uvs.reserve(numRows * numColls * 12);
	vertices.reserve(numRows * numColls * 18);

	// Cursor info
	int cursorX = 0;
	int cursorY = 0;
	int lastCursorX = 0;

	float cursorVerts[]
	{
		0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0,
		fontWidth, 0.0f, 0.0f, 1.0, 0.0, 0.0,
		fontWidth, fontHeight, 0.0f, 1.0, 0.0, 0.0,
		fontWidth, fontHeight, 0.0f, 1.0, 0.0, 0.0,
		0.0f, fontHeight, 0.0f, 1.0, 0.0, 0.0,
		0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0
	};
	
	GLuint cursorVAO;
	glGenVertexArrays(1, &cursorVAO);
	glBindVertexArray(cursorVAO);

	GLuint cursorVBO[2];
	glGenBuffers(2, cursorVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), cursorVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), cursorVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Line highlight data
	float highlightVerts[]
	{
		0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		(float)windowWidth + 0.01f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		(float)windowWidth + 0.01f, fontHeight, 0.0f, 0.5f, 0.5f, 0.5f,
		(float)windowWidth + 0.01f, fontHeight, 0.0f, 0.5f, 0.5f, 0.5f,
		0.0f, fontHeight, 0.0f, 0.5f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
	};

	GLuint highlightVAO;
	glGenVertexArrays(1, &highlightVAO);
	glBindVertexArray(highlightVAO);

	GLuint highlightVBO[2];
	glGenBuffers(2, highlightVBO);

	glBindBuffer(GL_ARRAY_BUFFER, highlightVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), highlightVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, highlightVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), highlightVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Editor's data
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, numRows * numColls * 6 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, numRows * numColls * 6 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	Shader shader = LoadShaderFromFile("res/shader/glyph.vert", "res/shader/glyph.frag");
	glUseProgram(shader.ID);

	glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
	glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	Shader cursorShader = LoadShaderFromFile("res/shader/cursor.vert", "res/shader/cursor.frag");
	glUseProgram(cursorShader.ID);
	glUniformMatrix4fv(cursorShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	// Used for FPS
	Uint64 start, end;
	double targetFPS = 1000.0f / 60.0f;

	SDL_StartTextInput();

	// Scrolling
	float scroll = 0.0f;
	float scrollSensitivity = fontHeight;
	bool shouldConstrict = false;

	// Which rows to render after scrolling
	int startRow = (int)(scroll / fontHeight);
	int endRow = std::min(numRows + startRow + 1, editableRows);

	// Render loop
	while(true)
	{
		// Start counting ms
		start = SDL_GetPerformanceCounter();

		int numKeys;
		const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);

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
			else if(e.type == SDL_MOUSEWHEEL)
			{
				// Scrolling down is positive, scrolling up is negative
				scroll -= scrollSensitivity * e.wheel.y;

				// Shouldn't scroll above first line
				scroll = std::max(scroll, 0.0f);
			}
			else if(e.type == SDL_MOUSEBUTTONDOWN)
			{
				if(e.button.button == SDL_BUTTON_LEFT)
				{
					cursorY = (e.button.y + scroll) / fontHeight;
					cursorY = std::min(std::max(cursorY, 0), editableRows-1);

					std::string& newLine = contentRows[cursorY];
					cursorX = e.button.x / fontWidth;
					cursorX = std::min(std::max(cursorX, 0), (int)newLine.size());
					lastCursorX = cursorX;
				}
			}
			else if(e.type == SDL_KEYDOWN)
			{
				SDL_Keycode& code = e.key.keysym.sym;

				bool ctrlDown = (keyboardState[SDL_SCANCODE_LCTRL] == 1 || keyboardState[SDL_SCANCODE_RCTRL]);

				// Backspace
				if(code == SDLK_BACKSPACE)
				{
					// If there are characters to be deleted
					if((int)contentRows[cursorY].size() > 0 && cursorX > 0)
					{
						if(ctrlDown)
							DeleteWordLeft(cursorX, cursorY, contentRows);
						else
						{
							contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX - 1);
							cursorX--;
						}

						lastCursorX = cursorX;

						shouldConstrict = true;
					}
					else if(cursorX == 0 && cursorY > 0) // There isn't a character in the line, so go to the previous line
					{
						DecrementY(cursorX, cursorY, (int)contentRows[cursorY-1].size(), contentRows);
						contentRows[cursorY].append(contentRows[cursorY + 1]);
						contentRows.erase(contentRows.begin() + cursorY + 1);
						lastCursorX = cursorX;
						editableRows--;
					}
				}
				else if(code == SDLK_LEFT)
				{
					if(cursorX == 0 && cursorY > 0)
					{
						DecrementY(cursorX, cursorY, (int)contentRows[cursorY-1].size(), contentRows);
						lastCursorX = cursorX;
					}
					else if(ctrlDown)
					{
						MoveWordLeft(cursorX, cursorY, contentRows);
						lastCursorX = cursorX;
					}
					else
						DecrementX(cursorX, lastCursorX);

					shouldConstrict = true;
				}
				else if(code == SDLK_RIGHT)
				{
					if(cursorX >= (int)contentRows[cursorY].size() - 1 && cursorY < editableRows - 1)
					{
						IncrementY(cursorX, cursorY, 0, contentRows, editableRows);
						cursorX = 0;
						lastCursorX = 0;
					}
					else if(ctrlDown)
					{
						MoveWordRight(cursorX, cursorY, contentRows);
						lastCursorX = cursorX;
					}
					else
						IncrementX(cursorX, cursorY, lastCursorX, numColls, contentRows);

					shouldConstrict = true;
				}
				else if(code == SDLK_UP)
				{
					DecrementY(cursorX, cursorY, lastCursorX, contentRows);
					shouldConstrict = true;
				}
				else if(code == SDLK_DOWN)
				{
					IncrementY(cursorX, cursorY, lastCursorX, contentRows, editableRows);
					shouldConstrict = true;
				}
				else if(code == SDLK_RETURN)
				{
					std::string leftover(contentRows[cursorY].begin() + cursorX, contentRows[cursorY].end());
					contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX, contentRows[cursorY].end());
					if(cursorY == editableRows - 1)
					{
						if((int)contentRows.size() == editableRows)
							contentRows.push_back(leftover);
						else
							contentRows[cursorY+1] = leftover;
					}
					else
						contentRows.insert(contentRows.begin() + cursorY + 1, leftover);

					cursorX = 0;
					lastCursorX = 0;
					cursorY++;
					editableRows++;

					shouldConstrict = true;
				}
				else if(code == SDLK_DELETE)
				{
					// There is content to be deleted
					if((int)contentRows[cursorY].size() > 0 && cursorX < (int)contentRows[cursorY].size())
					{
						if(ctrlDown)
							DeleteWordRight(cursorX, cursorY, contentRows);
						else
							contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX);
					}
					else if(cursorY + 1 < editableRows) // Delete newline char and append next line to current
					{
						contentRows[cursorY].append(contentRows[cursorY+1]);
						contentRows.erase(contentRows.begin() + cursorY + 1);
						editableRows--;
					}

					shouldConstrict = true;
				}
				else if(code == SDLK_HOME)
					cursorX = 0;
				else if(code == SDLK_END)
					cursorX = (int)contentRows[cursorY].size();

				// TODO: copy/paste
			}
			else if(e.type == SDL_TEXTINPUT)
				ProcessText(e, contentRows, cursorX, cursorY, lastCursorX, numColls);
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		if(shouldConstrict)
		{
			startRow = (int)(scroll / fontHeight);
			endRow = std::min(numRows + startRow + 1, editableRows);

			if(cursorY >= endRow)
				scroll += (cursorY - endRow + 1) * scrollSensitivity;
			else if(cursorY < startRow)
				scroll -= (startRow - cursorY) * scrollSensitivity;

			shouldConstrict = false;
		}

		startRow = (int)(scroll / fontHeight);
		endRow = std::min(numRows + startRow + 1, editableRows);

		// Use editor shader and bind editor VAO
		glBindVertexArray(VAO);
		glUseProgram(shader.ID);
		int loc = shader.uniforms["tex"];
		glUniform1i(loc, fontData.fontAtlas.index);

		for(int i = std::max(startRow - 1, 0); i < endRow; i++)
		{
			int jmax = std::min((int)contentRows[i].size(), numColls);
			std::string& currentRow = contentRows[i];

			float y = i * fontHeight + pointSize / 5.0f;

			for(int j = 0; j < jmax; j++)
			{
				float x = j * fontWidth;

				FTChar& currentChar = fontData.loadedCharacters[currentRow[j]];

				float startx = x + currentChar.bearing.x;
				float starty = y + tChar.size.y - currentChar.bearing.y;
				float width = currentChar.size.x;
				float height = currentChar.size.y;

				std::vector<float> tempVerts
				{
					startx, starty,
					startx + width, starty,
					startx + width, starty + height,
					startx + width, starty + height,
					startx, starty + height,
					startx, starty,
				};

				float ux = currentChar.tx;
				float uxend = ux + (float)currentChar.size.x / fontData.fontAtlas.width;
				float uyend = (float)currentChar.size.y / fontData.fontAtlas.height;
				std::vector<float> tempUVs
				{
					ux, 0.0f,
					uxend, 0.0f,
					uxend, uyend,
					uxend, uyend,
					ux, uyend,
					ux, 0.0f
				};

				vertices.insert(vertices.end(), tempVerts.begin(), tempVerts.end());
				uvs.insert(uvs.end(), tempUVs.begin(), tempUVs.end());
			}
		}

		int vertexCount = (int)vertices.size() / 2;

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(float), uvs.data());

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		if(!vertices.empty())
			vertices.clear();

		if(!uvs.empty())
			uvs.clear();

		projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight + scroll, 0.0f + scroll);
		glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

		// Update cursor's model matrix
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(cursorX * fontWidth, cursorY * fontHeight, 0.0f));
		glUseProgram(cursorShader.ID);
		glUniformMatrix4fv(cursorShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(cursorShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

		// Draw cursor
		glBindVertexArray(cursorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Update highlight's model matrix
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, cursorY * fontHeight, 0.0f));
		glUniformMatrix4fv(cursorShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		// Draw highlight
		glBindVertexArray(highlightVAO);
		glDrawArrays(GL_LINE_STRIP, 0, 6);

		glFinish();
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