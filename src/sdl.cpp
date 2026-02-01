#include "include/sdl.hpp"
#include "include/defines.h"
#include <include/glad/glad.h>


SDLConfig::SDLConfig()
{
	//iniciando o SDL
	int init = SDL_Init(SDL_INIT_VIDEO);
	if (init)
	{
		printf("SDL não pôde ser inicializado! SDL_Error: %s\n", SDL_GetError());
		throw ErroNoSDL();
	}

	// Pedindo o contexto ao OpenGL 3.3 Core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// criando a janela com a flag SDL_WINDOW_OPENGL, para permitir usar openGL
	this->window = SDL_CreateWindow("Hello SDL2",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDGHT,
		HEIGHT,
		SDL_WINDOW_OPENGL); // essa flag que permite usar o OpenGl
	if (!window) {
		printf("Janela não pôde ser criada! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		throw ErroNoSDL();
	}

	//Criando contexto OpenGL
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	//Carregar funções OpenGL (GLAD)
	if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) //essa função carrega as funções do OpenGl
	{
		printf("Erro ao inicializar o GLAD! SDL_Error: %s\n", SDL_GetError());
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(this->window);
		throw ErroNoSDL();
	}
	glViewport(0,0,WIDGHT, HEIGHT);

	printf("OpenGL: %s\n", glGetString(GL_VERSION));

}

SDLConfig::~SDLConfig(void)
{
	//SDL_DestroyTexture(this->texture);
	//SDL_DestroyRenderer(this->renderer);
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(this->window);
	SDL_Quit();
	//fazer algo pra terminar
}

//GETTERS
SDL_Window *SDLConfig::getWindow()
{
	return this->window;
}

SDL_Renderer *SDLConfig::getRenderer()
{
	return this->renderer;
}

SDL_Texture *SDLConfig::getTexture()
{
	return this->texture;
}

//EXECPTION
const char* SDLConfig::ErroNoSDL::what() const throw()
{
	return "Falha na Criação do Servidor ou Criação da imagem";
}


//FUNCTIONS
void SDLConfig::render()
{
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"

		"out vec4 vertexColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"   vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
		"}\0";
	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"

		"uniform vec4 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = ourColor;\n"
		"}\n\0";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	bool loop = true;
	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					loop = false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_w:
							printf("PRA CIMA\n");
							break;
						case SDLK_s:
							printf("pra baixo\n");
							break;
						case SDLK_d:
							printf("direita\n");
							break;
						case SDLK_a:
							printf("esquerda\n");
							break;
						case SDLK_ESCAPE:
							printf("FECHEI\n");
							loop = false;
							break;
					}

				//outros cases aqui
			}
		}
		//Limpa a tela comuma cor(AQUI JA USA GPU)
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ativando o shader
		glUseProgram(shaderProgram);

		float timeValue = (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
		float greenValue = sin(timeValue) / 1.0f + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES,0,3);
        // glBindVertexArray(0); // no need to unbind it every time


		//Troca buffers (mostra na tela)
		SDL_GL_SwapWindow(this->window);

	}
}
