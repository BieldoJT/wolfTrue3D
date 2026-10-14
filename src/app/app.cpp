#include "app/app.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <stdexcept>

namespace
{
	const char* TEXTURE_CATALOG = "data/textures.txt";
	const char* MAP_PATH = "maps/level1.txt";
	const float MAX_DELTA = 0.05f;
}

App::App()
	: shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl")
{
	this->input.captureMouse(true);
	if (!this->textures.load(TEXTURE_CATALOG))
		throw std::runtime_error("Não foi possível carregar o catálogo de texturas");
	if (!this->mapBuild.build(this->scene, MAP_PATH))
		throw std::runtime_error("Não foi possível carregar o mapa");
}

void App::run()
{
	Uint64 last = SDL_GetPerformanceCounter();
	Uint64 frequency = SDL_GetPerformanceFrequency();

	while (!this->input.quit)
	{
		Uint64 now = SDL_GetPerformanceCounter();
		float dt = static_cast<float>(now - last) / static_cast<float>(frequency);
		last = now;
		if (dt > MAX_DELTA)
			dt = MAX_DELTA;

		this->input.poll();
		this->cameraSystem.update(this->scene, this->input, dt);
		this->renderSystem.draw(this->scene, this->window, this->shader, this->textures);
		this->window.swap();
	}

	this->input.captureMouse(false);
}
