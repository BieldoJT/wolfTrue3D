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

	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	vec = trans * vec;
	std::cout << vec.x << vec.y << vec.z << std::endl;

	std::cout << "Rodou porra!!" << std::endl;
	return 0;
}
