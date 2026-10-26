#include "app/app.hpp"

#include <iostream>
#include <exception>
#include <string>

namespace
{
	const char* DEFAULT_MAP = "maps/level1.txt";
}

// Assinatura completa: no Windows o SDL renomeia main para SDL_main.
int main(int argc, char** argv)
{
	try
	{
		const std::string mapPath = (argc > 1) ? argv[1] : DEFAULT_MAP;
		App app(mapPath);
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
