#!/bin/bash

clang -O2 -Wall -Wextra -c thirdparty/glad/src/glad.c -I thirdparty/glad/include thirdparty/stb_image/stb_image.c -I thirdparty/stb_image

conf=$(sdl2-config --cflags --libs)
tp_includes='-I thirdparty/glad/include -I thirdparty/stb_image'
tp_libs='-ldl'
source='src/*.cpp src/window/*.cpp src/shader/*.cpp src/texture/*.cpp src/asset_load/*.cpp'
clang++ -O2 -Wall -Wextra $source glad.o stb_image.o -o editor $conf $tp_includes $tp_libs