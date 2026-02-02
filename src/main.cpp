#include <iostream>
#include <SDL2/SDL.h>
#include <include/glad/glad.h>
#include "defines.h"
#include "include/sdl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
	//instancio a classe SDL
	SDLConfig *SDL = new SDLConfig;

	//Metodo da classe que renderiza e coloca na tela
	SDL->render();

	//destrutor
	delete SDL;

	std::cout << "Rodou porra!!" << std::endl;
	return 0;
}
