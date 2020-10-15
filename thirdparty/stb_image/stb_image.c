#define STB_IMAGE_IMPLEMENTATION

#define STB_NO_BMP
#define STB_NO_PSD
#define STB_NO_TGA
#define STB_NO_GIF
#define STB_NO_PIC
#define STB_NO_PNM

#include "stb_image.h"

// A small program that decodes and outputs 
// the decompressed PNG file as a bin that
// can be easily and efficiently read at
// program start up.

int main()
{
    stbi_set_flip_vertically_on_load(1);

    int w, h, c;
    unsigned char* data = stbi_load("res/font/roboto.png", &w, &h, &c, 0);
    if(data == NULL)
        printf("Failed to load data!\n");
    
    FILE* outfile = fopen("res/font/roboto.bin", "wb");

    fprintf(outfile, "%d %d %d\n", w, h, c);
    fwrite(data, sizeof(unsigned char), w * h * c, outfile);

    fclose(outfile);
    stbi_image_free(data);
    return 0;
}