#!/bin/bash

clang -O -Wall -Wextra -c thirdparty/glad/src/glad.c -I thirdparty/glad/include

conf=$(sdl2-config --cflags --libs)
tp_includes='-I thirdparty/glad/include'
tp_libs='-ldl'
clang++ -O2 -Wall -Wextra src/*.cpp glad.o -o editor $conf $tp_includes $tp_libs