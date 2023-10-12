#include <iostream>
#include <glad/glad.h>
#include <SDL.h>

int main(int agrc, char **agrv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "Failed to create SDL Window" << std::endl;
		SDL_Quit();
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		SDL_Quit();
		return -2;
	}

	float vertices[] = {
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right FRONT 0 
		0.5f,  0.75f, -0.5, 1.0f, 0.0f, 0.0f, // top right BACK 1 
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right Front 2
		0.5f, -0.25f, -0.5f,0.0f, 1.0f, 0.0f, // bottom right Back 3 
	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// bottom left Front 4
	   -0.5f, -0.25f, -0.5f,0.0f, 0.0f, 1.0f,// bottom left Back 5
	   -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,// top left Front 6
	   -0.5f,  0.75f, -0.5f,1.0f, 1.0f, 0.0f,// top left Back 7
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 2, 6,   // first triangle
		2, 4, 6,    // second triangle

		0, 1, 2,    // third triangle
		2, 1, 3,    // fourd triangle

		6, 7, 4,    // fifth triangle
		4, 7, 5,    // six triangle

		1, 3, 7,   // first triangle
		3, 5, 7,   // second triangle

		0, 1, 7,   // first triangle
		7, 0, 1,    // second triangle

		4, 5, 3,   // first triangle
		3, 2, 2    // second triangle
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);

	//Generate 1 buffer for triangle (if we want more that 1 buffer we need to assign a vector to the pointer reference)
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLuint vao;
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const char* vertexShaderSource = R"glsl(
		#version 330 core

		in vec3 position;
		in vec3 color;
		out vec3 Color;

		void main()
		{
			Color = color;
			gl_Position = vec4(position, 1.0f);
		}
	)glsl";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	const char* fragmentShaderSource = R"glsl(
		#version 330 core

		in vec3 Color;
		out vec4 outColor;	

		void main()
		{
			outColor = vec4(Color, 1.0f);
		}
	)glsl";
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Attrib the colors to the position of each vertex
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	GLint colorAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	SDL_Event windowEvent;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
		}


		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

