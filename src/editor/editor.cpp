#include "editor.hpp"
#include "../window/window.hpp"
#include "../event/event.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL2/SDL.h>

Editor::Editor(Display &display, FontData &fontData)
{
	this->fontData = fontData;
	for (auto &f : fontData.loadedCharacters)
	{
		if (fontWidth < f.size.x)
			fontWidth = f.size.x;
		if (fontHeight < f.size.y)
			fontHeight = f.size.y;
	}

	windowWidth = display.width;
	windowHeight = display.height;

	numRows = display.height / fontHeight;
	numColls = display.width / fontWidth;

	vertices.reserve(numRows * numColls * 12);
	uvs.reserve(numRows * numColls * 18);

	editableRows = ParseText("res/textfiles/test1.txt", contentRows, actualRows, actualColls);
	tChar = fontData.loadedCharacters['T'];

	// Cursor OpenGL setup
	float cursorVerts[]
	{
		0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0,
		fontWidth, 0.0f, 0.0f, 1.0, 0.0, 0.0,
		fontWidth, fontHeight, 0.0f, 1.0, 0.0, 0.0,
		fontWidth, fontHeight, 0.0f, 1.0, 0.0, 0.0,
		0.0f, fontHeight, 0.0f, 1.0, 0.0, 0.0,
		0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0
	};

	glGenVertexArrays(1, &cursorVAO);
	glBindVertexArray(cursorVAO);

	cursorVBOs = new GLuint[2];
	glGenBuffers(2, cursorVBOs);

	glBindBuffer(GL_ARRAY_BUFFER, cursorVBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), cursorVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, cursorVBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), cursorVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

	// Line highlight OpenGL setup
	float highlightVerts[]
	{
		0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		(float)display.width + 0.01f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
		(float)display.width + 0.01f, fontHeight, 0.0f, 0.5f, 0.5f, 0.5f,
		(float)display.width + 0.01f, fontHeight, 0.0f, 0.5f, 0.5f, 0.5f,
		0.0f, fontHeight, 0.0f, 0.5f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
	};

	glGenVertexArrays(1, &highlightVAO);
	glBindVertexArray(highlightVAO);

	highlightVBOs = new GLuint[2];
	glGenBuffers(2, highlightVBOs);

	glBindBuffer(GL_ARRAY_BUFFER, highlightVBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), highlightVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, highlightVBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), highlightVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

	// Editor area OpenGL setup
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VBOs = new GLuint[2];
	glGenBuffers(2, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, numRows * numColls * 6 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, numRows * numColls * 6 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	startRow = (int)(scroll / fontHeight);
	endRow = std::min(numRows + startRow + 1, editableRows);

	shader = LoadShaderFromFile("res/shader/glyph.vert", "res/shader/glyph.frag");
	glUseProgram(shader.ID);

	projection = glm::ortho(0.0f, (float)display.width, (float)display.height, 0.0f);
	glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	cursorShader = LoadShaderFromFile("res/shader/cursor.vert", "res/shader/cursor.frag");
	glUseProgram(cursorShader.ID);
	glUniformMatrix4fv(cursorShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
}

Editor::~Editor()
{
	delete[] VBOs;
	delete[] cursorVBOs;
	delete[] highlightVBOs;
}

void Editor::ProcessEvents()
{
	int numKeys;
	const Uint8 *keyboardState = SDL_GetKeyboardState(&numKeys);

	// Handle events
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		/*
		if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
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
		*/
		if(e.type == SDL_QUIT)
		{
			SDL_Quit();
			exit(-1);
		}
		if (e.type == SDL_MOUSEWHEEL)
		{
			// Scrolling down is positive, scrolling up is negative
			scroll -= fontHeight * e.wheel.y;

			// Shouldn't scroll above first line
			scroll = std::max(scroll, 0.0f);
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				cursorY = (e.button.y + scroll) / fontHeight;
				cursorY = std::min(std::max(cursorY, 0), (int)editableRows - 1);

				std::string &newLine = contentRows[cursorY];
				cursorX = e.button.x / fontWidth;
				cursorX = std::min(std::max(cursorX, 0), (int)newLine.size());
				lastCursorX = cursorX;
			}
		}
		else if (e.type == SDL_KEYDOWN)
		{
			SDL_Keycode &code = e.key.keysym.sym;

			bool ctrlDown = (keyboardState[SDL_SCANCODE_LCTRL] == 1 || keyboardState[SDL_SCANCODE_RCTRL]);

			// Backspace
			if (code == SDLK_BACKSPACE)
			{
				// If there are characters to be deleted
				if ((int)contentRows[cursorY].size() > 0 && cursorX > 0)
				{
					if (ctrlDown)
						DeleteWordLeft(cursorX, cursorY, contentRows);
					else
					{
						contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX - 1);
						cursorX--;
					}

					lastCursorX = cursorX;

					shouldConstrict = true;
				}
				else if (cursorX == 0 && cursorY > 0) // There isn't a character in the line, so go to the previous line
				{
					DecrementY(cursorX, cursorY, (int)contentRows[cursorY - 1].size(), contentRows);
					contentRows[cursorY].append(contentRows[cursorY + 1]);
					contentRows.erase(contentRows.begin() + cursorY + 1);
					lastCursorX = cursorX;
					editableRows--;
				}
			}
			else if (code == SDLK_LEFT)
			{
				if (cursorX == 0 && cursorY > 0)
				{
					DecrementY(cursorX, cursorY, (int)contentRows[cursorY - 1].size(), contentRows);
					lastCursorX = cursorX;
				}
				else if (ctrlDown)
				{
					MoveWordLeft(cursorX, cursorY, contentRows);
					lastCursorX = cursorX;
				}
				else
					DecrementX(cursorX, lastCursorX);

				shouldConstrict = true;
			}
			else if (code == SDLK_RIGHT)
			{
				if (cursorX >= (int)contentRows[cursorY].size() - 1 && cursorY < (int)editableRows - 1)
				{
					IncrementY(cursorX, cursorY, 0, contentRows, editableRows);
					cursorX = 0;
					lastCursorX = 0;
				}
				else if (ctrlDown)
				{
					MoveWordRight(cursorX, cursorY, contentRows);
					lastCursorX = cursorX;
				}
				else
					IncrementX(cursorX, cursorY, lastCursorX, numColls, contentRows);

				shouldConstrict = true;
			}
			else if (code == SDLK_UP)
			{
				DecrementY(cursorX, cursorY, lastCursorX, contentRows);
				shouldConstrict = true;
			}
			else if (code == SDLK_DOWN)
			{
				IncrementY(cursorX, cursorY, lastCursorX, contentRows, editableRows);
				shouldConstrict = true;
			}
			else if (code == SDLK_RETURN)
			{
				std::string leftover(contentRows[cursorY].begin() + cursorX, contentRows[cursorY].end());
				contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX, contentRows[cursorY].end());
				if (cursorY == (int)editableRows - 1)
				{
					if ((uint32_t)contentRows.size() == editableRows)
						contentRows.push_back(leftover);
					else
						contentRows[cursorY + 1] = leftover;
				}
				else
					contentRows.insert(contentRows.begin() + cursorY + 1, leftover);

				cursorX = 0;
				lastCursorX = 0;
				cursorY++;
				editableRows++;

				shouldConstrict = true;
			}
			else if (code == SDLK_DELETE)
			{
				// There is content to be deleted
				if ((int)contentRows[cursorY].size() > 0 && cursorX < (int)contentRows[cursorY].size())
				{
					if (ctrlDown)
						DeleteWordRight(cursorX, cursorY, contentRows);
					else
						contentRows[cursorY].erase(contentRows[cursorY].begin() + cursorX);
				}
				else if (cursorY + 1 < (int)editableRows) // Delete newline char and append next line to current
				{
					contentRows[cursorY].append(contentRows[cursorY + 1]);
					contentRows.erase(contentRows.begin() + cursorY + 1);
					editableRows--;
				}

				shouldConstrict = true;
			}
			else if (code == SDLK_HOME)
				cursorX = 0;
			else if (code == SDLK_END)
				cursorX = (int)contentRows[cursorY].size();
			else if (code == SDLK_s && ctrlDown)
			{
				if(!copyBuffer.empty())
				{
					int lastPos = contentRows[0].find_last_of(' ') + 1;
					std::string fileName = std::string(contentRows[0].begin() + lastPos, contentRows[0].end());

					contentRows = copyBuffer;
					copyBuffer.clear();
					
					cursorX = copyX;
					cursorY = copyY;
					lastCursorX = cursorX;
					editableRows = (int)contentRows.size();

					FILE* output = fopen(fileName.c_str(), "wb");

					for(uint32_t i = 0; i < editableRows; i++)
						fprintf(output, "%s\n", contentRows[i].c_str());

					fclose(output);
				}
				else
				{
					copyBuffer = contentRows;
					contentRows.clear();
					contentRows.push_back("File name: name.txt");

					copyX = cursorX;
					copyY = cursorY;

					editableRows = 1;
					cursorX = 0;
					cursorY = 0;
					lastCursorX = 0;
				}
			}
		}
		else if (e.type == SDL_TEXTINPUT)
			ProcessText(e, contentRows, cursorX, cursorY, lastCursorX, numColls);
	}

	if(shouldConstrict)
	{
		startRow = (int)(scroll / fontHeight);
		endRow = std::min(numRows + startRow + 1, editableRows);

		if(cursorY >= endRow)
			scroll += (cursorY - endRow + 1) * fontHeight;
		else if(cursorY < startRow)
			scroll -= (startRow - cursorY) * fontHeight;

		shouldConstrict = false;
	}

	startRow = (int)(scroll / fontHeight);
	endRow = std::min(numRows + startRow + 1, editableRows);
}

void Editor::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Use editor shader and bind editor VAO
	glBindVertexArray(VAO);
	glUseProgram(shader.ID);
	int loc = shader.uniforms["tex"];
	glUniform1i(loc, fontData.fontAtlas.index);

	for (int i = std::max(startRow - 1, 0); i < endRow; i++)
	{
		int jmax = std::min((uint32_t)contentRows[i].size(), numColls);
		std::string &currentRow = contentRows[i];

		float y = i * fontHeight + fontData.pointSize / 5.0f;

		for (int j = 0; j < jmax; j++)
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
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(float), uvs.data());

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	if (!vertices.empty())
		vertices.clear();

	if (!uvs.empty())
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
}