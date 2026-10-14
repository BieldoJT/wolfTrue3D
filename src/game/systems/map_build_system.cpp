#include "game/systems/map_build_system.hpp"
#include "game/scene.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace
{
	const float CELL = 1.0f;

	void stripCarriageReturn(std::string& text)
	{
		if (!text.empty() && text[text.size() - 1] == '\r')
			text.erase(text.size() - 1);
	}

	bool isComment(const std::string& line)
	{
		return line.size() >= 2 && line[0] == '/' && line[1] == '/';
	}

	bool isWallDirective(const std::string& line)
	{
		return line.size() >= 4
			&& line.compare(0, 4, "wall") == 0
			&& (line.size() == 4 || line[4] == ' ' || line[4] == '\t');
	}

	bool isGridChar(char cell)
	{
		return cell == '#' || cell == '.' || cell == ' ' || cell == 'P';
	}
}

bool MapBuildSystem::build(Scene& scene, const std::string& mapPath)
{
	std::ifstream file(mapPath.c_str());
	if (!file)
	{
		std::cerr << "Falha ao abrir mapa: " << mapPath << std::endl;
		return false;
	}

	int wallTextureId = 1;
	bool sawWallDirective = false;
	std::vector<std::string> rows;
	std::string line;
	int lineNo = 0;

	while (std::getline(file, line))
	{
		++lineNo;
		stripCarriageReturn(line);
		if (line.empty() || isComment(line))
			continue;
		if (isWallDirective(line))
		{
			std::istringstream iss(line.substr(4));
			int textureId = 0;
			if (!(iss >> textureId))
			{
				std::cerr << "Textura de parede inválida em " << mapPath << ":" << lineNo << std::endl;
				return false;
			}
			wallTextureId = textureId;
			sawWallDirective = true;
			continue;
		}

		for (std::size_t i = 0; i < line.size(); ++i)
		{
			if (!isGridChar(line[i]))
			{
				std::cerr << "Caractere inválido '" << line[i] << "' em "
					<< mapPath << ":" << lineNo << std::endl;
				return false;
			}
		}
		rows.push_back(line);
	}

	if (rows.empty())
	{
		std::cerr << "Mapa sem grade: " << mapPath << std::endl;
		return false;
	}
	if (!sawWallDirective)
		std::cout << "Mapa sem linha 'wall <id>'; usando textura " << wallTextureId << std::endl;

	std::size_t cellCount = 0;
	for (std::size_t r = 0; r < rows.size(); ++r)
		cellCount += rows[r].size();
	scene.reserve(cellCount + 1);

	bool foundSpawn = false;
	float spawnX = 0.0f;
	float spawnZ = 0.0f;

	for (std::size_t row = 0; row < rows.size(); ++row)
	{
		const std::string& cells = rows[row];
		for (std::size_t col = 0; col < cells.size(); ++col)
		{
			char cell = cells[col];
			float x = static_cast<float>(col) * CELL;
			float z = static_cast<float>(row) * CELL;

			if (cell == '#')
			{
				Entity& wall = scene.addEntity();
				wall.hasTransform = true;
				wall.transform.position = glm::vec3(x, 0.0f, z);
				wall.transform.scale = glm::vec3(1.0f);
				wall.hasMeshRenderer = true;
				wall.meshRenderer.mesh = &scene.cubeMesh();
				wall.meshRenderer.textureId = wallTextureId;
			}
			else if (cell == 'P')
			{
				if (foundSpawn)
				{
					std::cerr << "Spawn extra ignorado em " << mapPath << std::endl;
					continue;
				}
				foundSpawn = true;
				spawnX = x;
				spawnZ = z;
			}
		}
	}

	if (!foundSpawn)
	{
		std::cerr << "Mapa sem spawn (P): " << mapPath << std::endl;
		return false;
	}

	Entity& player = scene.addEntity();
	player.hasTransform = true;
	player.transform.position = glm::vec3(spawnX, 0.0f, spawnZ);
	player.hasCamera = true;
	return true;
}
