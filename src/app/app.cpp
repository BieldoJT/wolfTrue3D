#include "app/app.hpp"
#include "map/map_data.hpp"
#include "map/map_parser.hpp"
#include "map/map_validator.hpp"

#include <SDL.h>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

namespace
{
	const char* TEXTURE_CATALOG = "data/textures.txt";
	const char* ITEM_CATALOG = "data/items.txt";
	const float MAX_DELTA = 0.05f;
}

App::App(const std::string& mapPath)
	: shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl"),
	  firstMapPath(mapPath),
	  gameOver(false)
{
	this->input.attachWindow(this->window.sdlWindow());
	this->input.captureMouse(true);
	if (!this->textures.load(TEXTURE_CATALOG))
		throw std::runtime_error("Não foi possível carregar o catálogo de texturas");

	std::vector<MapMessage> messages;
	bool itemsOk = readItemCatalog(ITEM_CATALOG, this->items, messages);
	printMapMessages(std::cerr, ITEM_CATALOG, messages);
	if (!itemsOk)
		throw std::runtime_error("Não foi possível carregar o catálogo de itens");

	if (!this->loadLevel(mapPath, PlayerStats()))
		throw std::runtime_error("Não foi possível carregar o mapa: " + mapPath);
}

bool App::loadLevel(const std::string& mapPath, const PlayerStats& stats)
{
	MapData map;
	std::vector<MapMessage> messages;
	std::set<int> textureIds = this->textures.ids();
	MapCatalogs catalogs;
	catalogs.textureIds = &textureIds;
	catalogs.items = &this->items;
	if (loadMapFile(mapPath, map, messages))
		validateMap(map, catalogs, messages);
	printMapMessages(std::cerr, mapPath, messages);
	if (hasMapErrors(messages))
		return false;

	PlayerStats carried = stats;
	carried.hasKey = false;
	float sensitivity = -1.0f;
	if (Entity* old = this->scene.cameraEntity())
		sensitivity = old->camera.mouseSensitivity;

	this->scene.clear();
	this->mapBuild.build(this->scene, map, this->items, carried);
	if (sensitivity > 0.0f)
		this->scene.cameraEntity()->camera.mouseSensitivity = sensitivity;
	this->nextMapPath = map.nextMapPath;
	std::cout << "Mapa carregado: " << mapPath << std::endl;
	return true;
}

void App::finishLevel()
{
	Entity* player = this->scene.cameraEntity();
	PlayerStats stats = (player && player->hasPlayerStats) ? player->playerStats : PlayerStats();
	if (this->nextMapPath.empty())
	{
		this->gameOver = true;
		return;
	}
	std::string next = this->nextMapPath;
	if (!this->loadLevel(next, stats))
	{
		std::cerr << "Não foi possível carregar o próximo mapa: " << next << std::endl;
		this->gameOver = true;
	}
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

		if (this->gameOver)
		{
			if (this->input.confirm && this->loadLevel(this->firstMapPath, PlayerStats()))
				this->gameOver = false;
			this->hud.drawGameOver(this->window);
			this->window.swap();
			continue;
		}

		Entity* cameraEntity = this->scene.cameraEntity();
		if (cameraEntity)
			this->sensitivityMenu.update(this->input, this->window,
				cameraEntity->camera.mouseSensitivity);
		this->cameraSystem.update(this->scene, this->input, dt);
		this->interactionSystem.update(this->scene, this->input);
		this->doorSystem.update(this->scene, dt);
		this->pickupSystem.update(this->scene);

		GameEvents& events = this->scene.events();
		if (events.messageTimer > 0.0f)
			events.messageTimer -= dt;
		if (events.exitRequested)
		{
			this->finishLevel();
			continue;
		}

		this->renderSystem.draw(this->scene, this->window, this->shader, this->textures);
		cameraEntity = this->scene.cameraEntity();
		if (cameraEntity && cameraEntity->hasPlayerStats)
			this->hud.draw(this->window, cameraEntity->playerStats, events);
		if (cameraEntity && !this->input.mouseCaptured)
			this->sensitivityMenu.draw(this->window, cameraEntity->camera.mouseSensitivity);
		this->window.swap();
	}

	this->input.captureMouse(false);
}
