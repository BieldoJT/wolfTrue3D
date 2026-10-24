#include "game/systems/map_build_system.hpp"
#include "game/map_grid.hpp"
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

	bool isDirective(const std::string& line, const char* name)
	{
		std::size_t len = std::string(name).size();
		return line.size() >= len
			&& line.compare(0, len, name) == 0
			&& (line.size() == len || line[len] == ' ' || line[len] == '\t');
	}

	bool isGridChar(char cell)
	{
		return MapGrid::isWallCell(cell) || cell == '.' || cell == ' ' || cell == 'P';
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
	int symbolTextures[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	int floorTextureId = 1;
	int ceilingTextureId = 1;
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
		if (isDirective(line, "floor") || isDirective(line, "ceiling"))
		{
			bool isFloor = isDirective(line, "floor");
			std::istringstream iss(line.substr(isFloor ? 5 : 7));
			int textureId = 0;
			if (!(iss >> textureId))
			{
				std::cerr << "Textura de " << (isFloor ? "chão" : "teto")
					<< " inválida em " << mapPath << ":" << lineNo << std::endl;
				return false;
			}
			(isFloor ? floorTextureId : ceilingTextureId) = textureId;
			continue;
		}
		if (isDirective(line, "wall"))
		{
			// "wall <id>" define a textura de '#'; "wall <símbolo> <id>" a de '1'-'9'
			std::istringstream iss(line.substr(4));
			std::string first;
			std::string second;
			iss >> first >> second;
			std::istringstream firstNum(first);
			std::istringstream secondNum(second);
			int textureId = 0;
			if (second.empty())
			{
				if (!(firstNum >> textureId))
				{
					std::cerr << "Textura de parede inválida em " << mapPath << ":" << lineNo << std::endl;
					return false;
				}
				wallTextureId = textureId;
				sawWallDirective = true;
			}
			else
			{
				if (first.size() != 1 || first[0] < '1' || first[0] > '9' || !(secondNum >> textureId))
				{
					std::cerr << "Símbolo de parede inválido (use 'wall <1-9> <id>') em "
						<< mapPath << ":" << lineNo << std::endl;
					return false;
				}
				symbolTextures[first[0] - '1'] = textureId;
			}
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

	scene.grid().cellSize = CELL;
	scene.grid().rows = rows;
	if (!sawWallDirective)
		std::cout << "Mapa sem linha 'wall <id>'; usando textura " << wallTextureId << std::endl;

	std::size_t cellCount = 0;
	for (std::size_t r = 0; r < rows.size(); ++r)
		cellCount += rows[r].size();
	scene.reserve(cellCount + 3);

	std::size_t maxCols = 0;
	for (std::size_t r = 0; r < rows.size(); ++r)
		if (rows[r].size() > maxCols)
			maxCols = rows[r].size();

	// Uma textura por célula: o quad cobre a grade inteira e repete o tile.
	float cols = static_cast<float>(maxCols);
	float depth = static_cast<float>(rows.size());
	Mesh::uploadPlane(scene.planeMesh(), cols, depth);
	for (int i = 0; i < 2; ++i)
	{
		Entity& plane = scene.addEntity();
		plane.hasTransform = true;
		plane.transform.position = glm::vec3(
			(cols - 1.0f) * 0.5f * CELL,
			i == 0 ? -0.5f : 0.5f,
			(depth - 1.0f) * 0.5f * CELL);
		plane.transform.scale = glm::vec3(cols * CELL, 1.0f, depth * CELL);
		plane.hasMeshRenderer = true;
		plane.meshRenderer.mesh = &scene.planeMesh();
		plane.meshRenderer.textureId = i == 0 ? floorTextureId : ceilingTextureId;
	}

	bool warnedSymbol[9] = {false, false, false, false, false, false, false, false, false};
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

			if (MapGrid::isWallCell(cell))
			{
				int textureId = wallTextureId;
				if (cell != '#')
				{
					textureId = symbolTextures[cell - '1'];
					if (textureId == 0)
					{
						if (!warnedSymbol[cell - '1'])
							std::cerr << "Símbolo '" << cell << "' sem linha 'wall " << cell
								<< " <id>' em " << mapPath << "; usando textura " << wallTextureId << std::endl;
						warnedSymbol[cell - '1'] = true;
						textureId = wallTextureId;
					}
				}
				Entity& wall = scene.addEntity();
				wall.hasTransform = true;
				wall.transform.position = glm::vec3(x, 0.0f, z);
				wall.transform.scale = glm::vec3(1.0f);
				wall.hasMeshRenderer = true;
				wall.meshRenderer.mesh = &scene.cubeMesh();
				wall.meshRenderer.textureId = textureId;
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
	player.hasCollider = true;
	return true;
}
