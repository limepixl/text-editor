cppflags='-g3 -Wall -Wextra'

if [ ! -f glad.o ]; then
    gcc $cppflags -c thirdparty/glad/src/glad.c -I thirdparty/glad/include
fi

conf=$(sdl2-config --cflags --libs)
tp_includes='-I thirdparty/glad/include -I thirdparty/glm'
tp_libs='-ldl'
source='src/*.cpp src/window/*.cpp src/shader/*.cpp src/texture/*.cpp src/asset_load/*.cpp src/event/*.cpp'

g++ $cppflags $source glad.o -o editor $conf $tp_includes $tp_libs
