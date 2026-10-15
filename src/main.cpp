#include "app/app.hpp"

#include <iostream>
#include <exception>

// Assinatura completa: no Windows o SDL renomeia main para SDL_main.
int main(int, char**)
{
	try
	{
		App app;
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
