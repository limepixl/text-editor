#!/bin/bash

clang -O2 -Wall -Wextra -c thirdparty/glad/src/glad.c -I thirdparty/glad/include thirdparty/stb_image/stb_image.c -I thirdparty/stb_image

conf=$(sdl2-config --cflags --libs)
tp_includes='-I thirdparty/glad/include -I thirdparty/stb_image'
tp_libs='-ldl'
clang++ -O2 -Wall -Wextra src/*.cpp glad.o stb_image.o -o editor $conf $tp_includes $tp_libs