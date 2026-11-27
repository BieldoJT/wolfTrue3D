#include "app/app.hpp"
#include "map/map_data.hpp"
#include "map/map_parser.hpp"
#include "map/map_validator.hpp"

#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

namespace
{
	const char* TEXTURE_CATALOG = "data/textures.txt";
	const char* ITEM_CATALOG = "data/items.txt";
	const char* SETTINGS_FILE = "settings.txt";
	const float MAX_DELTA = 0.05f;
}

App::App(const std::string& mapPath)
	: settings(loadSettings(SETTINGS_FILE)),
	  window(settings.resolution, settings.mode),
	  shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl"),
	  firstMapPath(mapPath),
	  menuOpen(false),
	  gameOver(false)
{
	this->settings.mode = this->window.displayMode();	// o modo salvo pode ter sido recusado
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
	this->scene.clear();
	this->mapBuild.build(this->scene, map, this->items, carried);
	this->scene.cameraEntity()->camera.mouseSensitivity = this->settings.mouseSensitivity;
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

void App::updateMenu()
{
	bool open = !this->input.mouseCaptured;
	if (open && !this->menuOpen)
		this->optionsMenu.open(this->window, this->settings);
	if (!open && this->menuOpen)
		saveSettings(SETTINGS_FILE, this->settings);
	this->menuOpen = open;
	if (!open)
		return;

	OptionsMenu::Action action = this->optionsMenu.update(this->input, this->window, this->settings);
	if (action == OptionsMenu::Close)
	{
		this->input.captureMouse(true);
		this->menuOpen = false;
		saveSettings(SETTINGS_FILE, this->settings);
	}
	else if (action == OptionsMenu::Save)
		saveSettings(SETTINGS_FILE, this->settings);

	if (Entity* player = this->scene.cameraEntity())
		player->camera.mouseSensitivity = this->settings.mouseSensitivity;
}

void App::drawMenu()
{
	if (this->menuOpen)
		this->optionsMenu.draw(this->window, this->settings);
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

		this->updateMenu();

		if (this->gameOver)
		{
			if (!this->menuOpen && this->input.confirm && this->loadLevel(this->firstMapPath, PlayerStats()))
				this->gameOver = false;
			glViewport(0, 0, this->window.width(), this->window.height());
			this->hud.drawGameOver(this->window);
			this->drawMenu();
			this->window.swap();
			continue;
		}

		// Com o menu aberto o jogador fica parado (portas e itens seguem normais).
		if (!this->menuOpen)
		{
			this->cameraSystem.update(this->scene, this->input, dt);
			this->interactionSystem.update(this->scene, this->input);
		}
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
		Entity* cameraEntity = this->scene.cameraEntity();
		if (cameraEntity && cameraEntity->hasPlayerStats)
			this->hud.draw(this->window, cameraEntity->playerStats, events);
		this->drawMenu();
		this->window.swap();
	}

	this->input.captureMouse(false);
}
