#version 400 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D tex;

void main()
{
    color = texture(tex, texCoords);

    if(color.a == 0.0) 
        discard;
}