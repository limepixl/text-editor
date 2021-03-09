#include "asset_load.hpp"
#include <cstdio>
#include <cstring>
#include <inttypes.h>

#include <glad/glad.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include FT_FREETYPE_H

struct BlockType1
{
	int16_t fontSize;
	int8_t bitField;
	uint8_t charSet;
	uint16_t stretchH;
	uint8_t aa;
	uint8_t paddingUp;
	uint8_t paddingRight;
	uint8_t paddingDown;
	uint8_t paddingLeft;
	uint8_t spacingHoriz;
	uint8_t spacingVert;
	uint8_t outline;
	char* stringPtr;

	~BlockType1()
	{
		if(stringPtr != nullptr)
			delete[] stringPtr;
	}
};

struct BlockType2
{
	uint16_t lineHeight;
	uint16_t base;
	uint16_t scaleW;
	uint16_t scaleH;
	uint16_t pages;
	int8_t bitField;
	uint8_t alpha;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct BlockType3
{
	char** pageNames;
	int numStrings;

	~BlockType3()
	{
		for(int i = 0; i < numStrings; i++)
			delete[] pageNames[i];
		delete[] pageNames;
	}
};

struct BlockType4
{
	uint32_t id;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	int16_t xoffset;
	int16_t yoffset;
	int16_t xadvance;
	uint8_t page;
	uint8_t channel;
};

std::vector<Char> ParseFNT(const char* path)
{
	FILE* fnt = fopen(path, "rb");
	if(!fnt)
		printf("Failed to load FNT file at path: %s\n", path);
	
	fseek(fnt, 0, SEEK_END);
	long size = ftell(fnt);
	rewind(fnt);

	uint8_t* buffer = new uint8_t[size + 1];
	if((long)fread(buffer, sizeof(uint8_t), size, fnt) > size)
		printf("Insufficient read!\n");
	buffer[size] = '\0';
	fclose(fnt);

	long byteIndex = 0;
	if(memcmp(buffer, "BMF", 3 * sizeof(char)) || buffer[3] != 3)
		printf("Invalid FNT file!\n");

	byteIndex = 4;

	BlockType1 block1{};
	BlockType2 block2{};
	BlockType3 block3{};

	std::vector<Char> chars;

	while(byteIndex < size)
	{
		uint8_t typeIdentifier = buffer[byteIndex++];
		int blockSize;
		memcpy(&blockSize, buffer + byteIndex, 4);
		byteIndex += 4;

		if(typeIdentifier == 1)
		{
			memcpy((void*)&block1, buffer + byteIndex, 14);

			size_t strsize = strlen((const char*)(buffer + (byteIndex += 14)));
			block1.stringPtr = new char[strsize + 1];
			strncpy(block1.stringPtr, (const char*)(buffer + byteIndex), strsize+1);
			block1.stringPtr[strsize] = '\0';

			byteIndex += (long)(strsize + 1);
		}
		else if(typeIdentifier == 2)
		{
			memcpy(&block2, buffer + byteIndex, 15);

			byteIndex += 15;
		}
		else if(typeIdentifier == 3)
		{
			block3.pageNames = new char*[block2.pages];
			
			for(int i = 0; i < block2.pages; i++)
			{
				size_t strsize = strlen((const char*)(buffer + byteIndex));
				block3.pageNames[i] = new char[strsize + 1];
				strncpy(block3.pageNames[i], (const char*)(buffer + byteIndex), strsize+1);
				byteIndex += (long)(strsize + 1);
			}

			block3.numStrings = block2.pages;
		}
		else if(typeIdentifier == 4)
		{
			uint16_t numChars = uint16_t((size - byteIndex) / 20);
			BlockType4* block4 = new BlockType4[numChars];
			for(int i = 0; i < numChars; i++)
			{
				BlockType4& current = block4[i];
				memcpy(&current, buffer + byteIndex, 20);

				float startx = (float) current.x / block2.scaleW;
				float endx = (float) (current.x + current.width) / block2.scaleW;
				float starty = (float) current.y / block2.scaleH;
				float endy = (float) (current.y + current.height) / block2.scaleH;

				std::vector<float> tempUVs
				{
					startx, starty,
					endx, starty,
					endx, endy,
					endx, endy,
					startx, endy,
					startx, starty,
				};
				chars.push_back({(char)current.id, tempUVs, current.width, current.height});

				byteIndex += 20;
			}
			
			delete[] block4;
		}
	}

	delete[] buffer;

	return chars;
}

Texture LoadTextureFromBinary(const char* path)
{
	FILE* binfile = fopen(path, "rb");
	if(binfile == nullptr)
		printf("Failed to load binary file from path: %s\n", path);

	int w, h, c;
	if(fscanf(binfile, "%d %d %d\n", &w, &h, &c) == EOF)
		printf("Insufficient scan!\n");
	
	long startingPos = ftell(binfile);
	fseek(binfile, 0, SEEK_END);
	long size = ftell(binfile);
	fseek(binfile, startingPos, SEEK_SET);

	unsigned char* data = new unsigned char[size+1];
	if((long)fread(data, sizeof(unsigned char), size, binfile) > size)
		printf("Insufficient read!\n");
	data[size] = '\0';
	Texture result(data, w, h, c);

	fclose(binfile);
	delete[] data;
	return result;
}

Shader LoadShaderFromFile(const char* vertexPath, const char* fragmentPath)
{
	/* Vertex shader */
	FILE* vRaw = fopen(vertexPath, "rb");
	if(vRaw == nullptr)
		printf("Failed to read from file at path: %s\n", vertexPath);

	fseek(vRaw, 0, SEEK_END);
	long size = ftell(vRaw);
	rewind(vRaw);

	char* vSource = new char[size + 1];
	if((long)fread(vSource, sizeof(unsigned char), size, vRaw) > size)
		printf("Insufficient read!\n");
	vSource[size] = '\0';

	fclose(vRaw);

	/* Fragment shader */
	FILE* fRaw = fopen(fragmentPath, "rb");
	if(fRaw == nullptr)
		printf("Failed to read from file at path: %s\n", fragmentPath);

	fseek(fRaw, 0, SEEK_END);
	size = ftell(fRaw);
	rewind(fRaw);

	char* fSource = new char[size + 1];
	if((long)fread(fSource, sizeof(unsigned char), size, fRaw) > size)
		printf("Insufficient read!\n");
	fSource[size] = '\0';

	fclose(fRaw);

	Shader result(vSource, fSource);
	delete[] vSource;
	delete[] fSource;

	return result;
}

uint32_t ParseText(const char* path, std::vector<std::string>& contentRows, uint32_t& numRows, uint32_t& numColls)
{
	FILE* textfile = fopen(path, "r");
	if(!textfile)
	{
		printf("Failed to load textfile at path: %s\n", path);
		fclose(textfile);
		exit(-1);
	}

	int numLinesProcessed = 0;

	char line[1025];
	while(fgets(line, sizeof(line), textfile) != nullptr)
	{
		// Remove trailing newline
		// TODO: Fix line-wrapping over 80 characters
		uint32_t length = (uint32_t)strlen(line);
		if(numColls < length)
			numColls = length;

		if(length >= 2 && line[length - 2] == '\r')
			length -= 2;
		else if(line[length - 1] == '\n')
			length--;

		line[length] = '\0';

		std::string tmp(line);
		auto l = tmp.find('\0');
		if(l != std::string::npos)
			tmp.resize(tmp.find('\0'));

		contentRows.push_back(std::string(line));
		numLinesProcessed++;
	}

	if(numRows < (uint32_t)numLinesProcessed)
		numRows = numLinesProcessed;

	fclose(textfile);

	return (int)contentRows.size();
}

FontData ParseFontFT(const char* path, int pointSize)
{
	FontData result;
	result.pointSize = pointSize;

	FT_Library library;

	if(FT_Init_FreeType(&library))
		printf("Could not init FreeType!\n");

	FT_Face face;
	int error = FT_New_Face(library, path, 0, &face);
	if(error == FT_Err_Unknown_File_Format)
		printf("The font file was read successfully but the format of the file is unsupported!\n");
	else if(error)
		printf("The font file was not read! Could be broken!\n");

	if(FT_Set_Char_Size(face, 64 * pointSize, 0, 96, 96))
		printf("Unsupported fixed font size!\n");
	
	result.loadedCharacters.reserve(128);

	// Get max width and height
	int w = 0, h = 0;
	for(unsigned char c = 0; c < 128; c++)
	{
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
			printf("Failed to load char: %c\n", c);

		w += face->glyph->bitmap.width;
		h = std::max((unsigned int)h, face->glyph->bitmap.rows);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	result.fontAtlas = Texture(NULL, w, h, 1);
	glActiveTexture(GL_TEXTURE0 + result.fontAtlas.index);
	glBindTexture(GL_TEXTURE_2D, result.fontAtlas.ID);

	int x = 0;
	for(unsigned char c = 0; c < 128; c++)
	{
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
			printf("Failed to load char: %c\n", c);

		auto* g = face->glyph;
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		FTChar tmpChar
		{
			glm::ivec2(g->bitmap.width, g->bitmap.rows),
			glm::ivec2(g->bitmap_left, g->bitmap_top),
			g->advance.x >> 6,
			(float)x / w
		};
		result.loadedCharacters.push_back(tmpChar);

		x += face->glyph->bitmap.width;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return result;
}