#version 330 core
in vec3 ccolor;
out vec4 color;

void main()
{
    color = vec4(ccolor, 0.4);
}