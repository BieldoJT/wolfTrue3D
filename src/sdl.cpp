#include "include/sdl.hpp"
#include "include/defines.h"
#include <include/glad/glad.h>
#include "include/shader.hpp"


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

	Shader shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
  float vertices[] = {
		// positions		// colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
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
		shader.use();

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES,0,3);
        // glBindVertexArray(0); // no need to unbind it every time


		//Troca buffers (mostra na tela)
		SDL_GL_SwapWindow(this->window);

	}
}
