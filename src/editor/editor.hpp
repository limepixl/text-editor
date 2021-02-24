#pragma once
#include <inttypes.h>
#include <glm/glm.hpp>
#include "../asset_load/asset_load.hpp"


struct Editor
{
	// Window data
	uint32_t windowWidth, windowHeight;
	
	// Editor data
	uint32_t numRows, numColls;
	uint32_t actualRows, actualColls;
	uint32_t editableRows;

	// Editor OpenGL data
	std::vector<float> vertices, uvs;
	uint32_t VAO, *VBOs;

	// Editor content
	std::vector<std::string> contentRows;
	int32_t startRow, endRow;

	// Font data
	FontData fontData;
	FTChar tChar;
	float fontWidth = 0.0f, fontHeight = 0.0f;

	// Cursor data
	int cursorX = 0, cursorY = 0;
	int lastCursorX = 0;

	// Cursor OpenGL data
	uint32_t cursorVAO, *cursorVBOs;
	uint32_t highlightVAO, *highlightVBOs;

	// Scroll data
	float scroll = 0.0f;
	bool shouldConstrict = false;

	// Shaders and matrices
	struct Shader shader, cursorShader;
	glm::mat4 projection;

	Editor(struct Display& display, struct FontData& fontData);
	~Editor();

	void ProcessEvents();
	void Draw();
};

Editor CreateEditor(struct Display& display, struct FontData& fontData);

