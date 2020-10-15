#include "asset_load.hpp"
#include <cstdio>

Texture LoadTextureFromBinary(const char* path)
{
	FILE* binfile = fopen(path, "rb");
	if(binfile == nullptr)
		printf("Failed to load binary file from path: %s\n", path);

	int w, h, c;
	fscanf(binfile, "%d %d %d\n", &w, &h, &c);
	
	fseek(binfile, 0, SEEK_END);
	long size = ftell(binfile);
	rewind(binfile);

	unsigned char* data = new unsigned char[size+1];
	fread(data, sizeof(unsigned char), size, binfile);
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