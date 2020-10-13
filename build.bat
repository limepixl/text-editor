@echo off

if not exist glad.o (
    clang -O2 -Wall -c -Wextra -fno-exceptions thirdparty/glad/src/glad.c -I thirdparty/glad/include
)

if not exist stb_image.o (
    clang -O2 -Wall -c -Wextra -fno-exceptions thirdparty/stb_image/stb_image.c -I thirdparty/stb_image
)

clang++ -O2 -Wall -Wextra -fno-exceptions .\glad.o .\stb_image.o .\src\main.cpp .\src\asset_load\*.cpp .\src\shader\*.cpp .\src\texture\*.cpp .\src\window\*.cpp -I .\thirdparty\glad\include\ -I .\thirdparty\stb_image\ -I .\thirdparty\SDL2\include\ -D_CRT_SECURE_NO_WARNINGS -l .\thirdparty\SDL2\lib\SDL2.lib -o editor.exe