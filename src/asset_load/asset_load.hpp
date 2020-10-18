#pragma once
#include "../texture/texture.hpp"
#include "../shader/shader.hpp"
#include <vector>
#include <string>

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
void ParseText(const char* path, std::vector<std::string>& contentRows);