#include "include/sdl.hpp"
#include "include/defines.h"


SDLConfig::SDLConfig()
{
	int a = SDL_Init(SDL_INIT_VIDEO);
	if (a)
	{
		printf("SDL não pôde ser inicializado! SDL_Error: %s\n", SDL_GetError());
		//FAZER A EXPECTIOON DE ERRO
	}
	this->window = SDL_CreateWindow("Hello SDL2", 0, 0, WEIGHT, HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) {
		printf("Janela não pôde ser criada! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		//FAZER A EXPECTIOON DE ERRO
	}
	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	this->texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WEIGHT, HEIGHT
	);
}

SDLConfig::~SDLConfig(void)
{
	SDL_DestroyTexture(this->texture);
	SDL_DestroyRenderer(this->renderer);
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
	}
}
