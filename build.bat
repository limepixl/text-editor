@echo off

set "cppflags=-O2 -Wall -Wextra -fno-exceptions -D_CRT_SECURE_NO_WARNINGS"

if not exist glad.o (
	clang %cppflags% -c thirdparty/glad/src/glad.c -I thirdparty/glad/include
)

if not exist stb_image.o (
	clang %cppflags% -c thirdparty/stb_image/stb_image.c -I thirdparty/stb_image
)

set "tp_includes=-I ./thirdparty/glad/include -I ./thirdparty/stb_image -I ./thirdparty/SDL2/include"

clang++ %cppflags% .\glad.o .\stb_image.o .\src\main.cpp .\src\asset_load\*.cpp .\src\shader\*.cpp .\src\texture\*.cpp .\src\window\*.cpp %tp_includes% -l .\thirdparty\SDL2\lib\SDL2.lib -o editor.exe
