#ifndef SDL_HPP
# define SDL_HPP

# include <SDL2/SDL.h>
# include <iostream>
# include <string>
# include <exception>



class SDLConfig
{
	private:
		SDL_Window *window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
		SDL_GLContext glContext;

	public:
	//construtor e destrutor
		SDLConfig(void); // construtor, incia o servidor, janela e imagem do sdl.
		~SDLConfig(void); // destrutor, destroi servidor, janela e imagem.

		//renderiza a imagem e joga na tela
		void render();

		//getters
		SDL_Window *getWindow();
		SDL_Renderer* getRenderer();
		SDL_Texture* getTexture();

	//execptions
		class ErroNoSDL : public std::exception{
			virtual const char* what() const throw();
		};



};

#endif
