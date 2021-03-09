@REM set "cppflags=-O0 -g3 -Wall -Wextra -D_CRT_SECURE_NO_WARNINGS"
@REM set "tp_includes=-I ./thirdparty/glad/include -I ./thirdparty/SDL2/include -I ./thirdparty/glm -I ./thirdparty/freetype/include"

@REM clang %cppflags% -c thirdparty/glad/src/glad.c -I thirdparty/glad/include

@REM clang++ %cppflags% .\glad.o .\src\main.cpp .\src\asset_load\*.cpp .\src\shader\*.cpp .\src\texture\*.cpp .\src\window\*.cpp .\src\event\*.cpp .\src\editor\*.cpp %tp_includes% -l .\thirdparty\freetype\lib\freetype.lib -l .\thirdparty\SDL2\lib\SDL2.lib -o editor.exe

@echo off

set "cppflags=/O2 /W4 /EHsc /D_CRT_SECURE_NO_WARNINGS"
set "tp_includes=-I ./thirdparty/glad/include -I ./thirdparty/SDL2/include -I ./thirdparty/glm -I ./thirdparty/freetype/include"

cl %cppflags% -c thirdparty/glad/src/glad.c -I thirdparty/glad/include
cl %cppflags% .\glad.obj .\src\main.cpp .\src\asset_load\*.cpp .\src\shader\*.cpp .\src\texture\*.cpp .\src\window\*.cpp .\src\event\*.cpp .\src\editor\*.cpp %tp_includes% /link .\thirdparty\freetype\lib\freetype.lib .\thirdparty\SDL2\lib\SDL2.lib /out:editor.exe