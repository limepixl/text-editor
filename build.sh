#!/bin/bash

cppflags='-O2 -Wall -Wextra -fno-exceptions'

if [ ! -f glad.o ]; then
    clang $cppflags -c thirdparty/glad/src/glad.c -I thirdparty/glad/include
fi

if [ ! -f stb_image.o ]; then
    clang $cppflags -c thirdparty/stb_image/stb_image.c -I thirdparty/stb_image
fi

conf=$(sdl2-config --cflags --libs)
tp_includes='-I thirdparty/glad/include -I thirdparty/stb_image'
tp_libs='-ldl'
source='src/*.cpp src/window/*.cpp src/shader/*.cpp src/texture/*.cpp src/asset_load/*.cpp'

clang++ $cppflags $source glad.o stb_image.o -o editor $conf $tp_includes $tp_libs
