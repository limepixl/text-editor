#include "asset_load.hpp"
#include <cstdio>
#include <stb_image.h>

Texture LoadTextureFromFile(const char* path)
{
	stbi_set_flip_vertically_on_load(1);

    int w, h, c;
	unsigned char* data = stbi_load(path, &w, &h, &c, 0);
	if(data == nullptr)
		printf("Failed to load texture form path: %s\n", path);

	Texture result(data, w, h, c);
	stbi_image_free(data);

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
    fread(vSource, sizeof(unsigned char), size, vRaw);
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
    fread(fSource, sizeof(unsigned char), size, fRaw);
    fSource[size] = '\0';

    fclose(fRaw);

    Shader result(vSource, fSource);
    delete[] vSource;
    delete[] fSource;

    return result;
}