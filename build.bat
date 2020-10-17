@echo off

set "cppflags=-O2 -Wall -Wextra -D_CRT_SECURE_NO_WARNINGS"
set "tp_includes=-I ./thirdparty/glad/include -I ./thirdparty/SDL2/include -I ./thirdparty/glm"

clang %cppflags% -c thirdparty/glad/src/glad.c -I thirdparty/glad/include

clang++ %cppflags% .\glad.o .\src\main.cpp .\src\asset_load\*.cpp .\src\shader\*.cpp .\src\texture\*.cpp .\src\window\*.cpp %tp_includes% -l .\thirdparty\SDL2\lib\SDL2.lib -o editor.exe
