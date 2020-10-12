#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cstdio>

#include <stb_image.h>

int main()
{
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Unable to initialize SDL2!\n");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow(
		"Text Editor",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if(window == nullptr)
	{
		printf("Failed to create SDL2 window\n");
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == nullptr)
	{
		printf("Failed to create OpenGL context\n");
		return -1;
	}

	// Init GLAD
	if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
	{
		printf("Failed to init GLAD with SDL2\n");
		return -1;
	}
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, 640, 480);

	float vertices[]
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};

	float uvs[]
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	GLuint texture;
	glGenTextures(1, &texture);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h, c;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load("res/font/roboto.png", &w, &h, &c, 0);
	if(data == nullptr)
	{
		printf("Failed to load font atlas!\n");
		return -1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	const char* vSource =
	"#version 400 core\n"
	"layout(location = 0) in vec3 aPos;\n"
	"layout(location = 1) in vec2 aTexCoords;\n"
	"out vec2 texCoords;\n"
	"void main() {\n"
	"texCoords = aTexCoords;\n"
	"gl_Position = vec4(aPos, 1.0);}\n";

	const char* fSource =
	"#version 400 core\n"
	"in vec2 texCoords;\n"
	"out vec4 color;\n"
	"uniform sampler2D tex;\n"
	"void main() {"
	"color = texture(tex, texCoords);\n"
	"if(color.a == 0.0) discard;}\n";

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

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	// Render loop
	while(true)
	{
		// Handle events
		SDL_Event e;
		SDL_PollEvent(&e);
		if(e.type == SDL_QUIT)
		{
			SDL_Quit();
			return 0;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	SDL_Quit();
	return 0;
}