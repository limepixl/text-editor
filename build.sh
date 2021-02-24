cppflags='-O2 -Wall -Wextra'

if [ ! -f glad.o ]; then
    gcc $cppflags -c thirdparty/glad/src/glad.c -I thirdparty/glad/include
fi

conf=$(sdl2-config --cflags --libs)
tp_includes='-I thirdparty/glad/include -I thirdparty/glm -I thirdparty/freetype/include -I thirdparty/stb'
tp_libs='-ldl -lfreetype'
source='src/*.cpp src/window/*.cpp src/shader/*.cpp src/texture/*.cpp src/asset_load/*.cpp src/event/*.cpp src/editor/*.cpp'

g++ $cppflags $source glad.o -o editor $conf $tp_includes $tp_libs
