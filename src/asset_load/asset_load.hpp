#pragma once
#include "../texture/texture.hpp"
#include "../shader/shader.hpp"

Texture LoadTextureFromBinary(const char* path);
Shader LoadShaderFromFile(const char* vertexPath, const char* fragmentPath);