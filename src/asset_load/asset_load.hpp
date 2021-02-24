#pragma once
#include "../texture/texture.hpp"
#include "../shader/shader.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct FTChar
{
    glm::ivec2 size;
    glm::ivec2 bearing;
    int64_t advanceX;
    float tx; // x offset in uvs
};

struct Char
{
	char c;
	std::vector<float> uvs;
    int charWidth;
    int charHeight;
};

struct FontData
{
    int pointSize;
    Texture fontAtlas;
    std::vector<FTChar> loadedCharacters;
};

std::vector<Char> ParseFNT(const char* path);
Texture LoadTextureFromBinary(const char* path);
Shader LoadShaderFromFile(const char* vertexPath, const char* fragmentPath);
uint32_t ParseText(const char* path, std::vector<std::string>& contentRows, uint32_t& numRows, uint32_t& numColls);

FontData ParseFontFT(const char* path, int pointSize);