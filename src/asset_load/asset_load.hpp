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

std::vector<Char> ParseFNT(const char* path);
Texture LoadTextureFromBinary(const char* path);
Shader LoadShaderFromFile(const char* vertexPath, const char* fragmentPath);
void ParseText(const char* path, std::vector<std::string>& contentRows, int& numRows, int& numColls);

Texture ParseFontFT(const char* path, int pointSize, std::vector<FTChar>& loadedCharacters);