#pragma once
#include "../texture/texture.hpp"
#include "../shader/shader.hpp"
#include <vector>

struct Char
{
	char c;
	std::vector<float> uvs;
    int charSize;
    int charHeight;
};

std::vector<Char> ParseFNT(const char* path);
Texture LoadTextureFromBinary(const char* path);
Shader LoadShaderFromFile(const char* vertexPath, const char* fragmentPath);