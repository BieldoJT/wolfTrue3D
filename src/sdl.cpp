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
		WEIGHT,
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
		glClearColor(0,0,255,1);
		glClear(GL_COLOR_BUFFER_BIT);

		//Troca buffers (mostra na tela)
		SDL_GL_SwapWindow(this->window);

		/*
		void* pixels;
		int pitch;
		SDL_LockTexture(texture, nullptr, &pixels, &pitch);
		 // 2) desenhar no buffer
		uint32_t* pix = (uint32_t*)pixels;
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WEIGHT; x++) {
				// exemplo: gradiente
				uint8_t r = (uint8_t)(x * 255 / WEIGHT);
				uint8_t g = (uint8_t)(y * 255 / HEIGHT);
				uint8_t b = 50;
				pix[y * (pitch/4) + x] = (255<<24) | (r<<16) | (g<<8) | (b);
			}
		}
		// 3) unlock (entregar buffer pra SDL)
		SDL_UnlockTexture(texture);
		// 4) renderizar textura na janela
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
		*/
	}
}
