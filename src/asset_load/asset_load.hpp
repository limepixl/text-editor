#pragma once
#include "../texture/texture.hpp"
#include "../shader/shader.hpp"

Texture LoadTextureFromFile(const char* path);
Shader LoadShaderFromFile(const char* vertexPath, const char* fragmentPath);