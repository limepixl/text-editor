#pragma once

struct Texture
{
    unsigned int ID;
    unsigned int width, height, index;

    static unsigned int NumTexturesLoaded;

    Texture(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels);
};