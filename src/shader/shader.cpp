#include "shader.hpp"
#include <glad/glad.h>

Shader::Shader(const char* vSource, const char* fSource)
{
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, nullptr);
	glCompileShader(vertex);

	GLint compiled;
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);
	if(compiled != GL_TRUE)
	{
	    GLsizei log_length = 0;
	    GLchar message[1024];
	    glGetShaderInfoLog(vertex, 1024, &log_length, message);
	    printf("VERTEX COMPILE ERROR:\n%s\n", message);
	}

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, nullptr);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &compiled);
	if(compiled != GL_TRUE)
	{
	    GLsizei log_length = 0;
	    GLchar message[1024];
	    glGetShaderInfoLog(fragment, 1024, &log_length, message);
	    printf("FRAGMENT COMPILE ERROR:\n%s\n", message);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

    // Get number and names of active uniforms
    int numUniforms;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numUniforms);

    GLchar name[256];
    GLsizei length;
    GLint size;
    GLenum type;
    for(int i = 0; i < numUniforms; i++)
    {
        glGetActiveUniform(ID, i, sizeof(name), &length, &size, &type, name);
        uniforms[std::string(name)] = glGetUniformLocation(ID, name);
    }

    // Clean up
    glDetachShader(ID, vertex);
    glDetachShader(ID, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}