#include "game/systems/map_build_system.hpp"
#include "game/map_grid.hpp"
#include "game/scene.hpp"
#include "map/map_data.hpp"

namespace
{
	const float CELL = 1.0f;
}

void MapBuildSystem::build(Scene& scene, const MapData& map)
{
	const std::vector<std::string>& rows = map.rows;
	scene.grid().cellSize = CELL;
	scene.grid().rows = rows;

	std::size_t cellCount = 0;
	std::size_t maxCols = 0;
	for (std::size_t r = 0; r < rows.size(); ++r)
	{
		cellCount += rows[r].size();
		if (rows[r].size() > maxCols)
			maxCols = rows[r].size();
	}
	scene.reserve(cellCount + 3);

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
		plane.meshRenderer.textureId = i == 0 ? map.floorTexture : map.ceilingTexture;
	}

	for (std::size_t row = 0; row < rows.size(); ++row)
	{
		const std::string& cells = rows[row];
		for (std::size_t col = 0; col < cells.size(); ++col)
		{
			char cell = cells[col];
			if (!isMapWallCell(cell))
				continue;
			Entity& wall = scene.addEntity();
			wall.hasTransform = true;
			wall.transform.position = glm::vec3(static_cast<float>(col) * CELL, 0.0f, static_cast<float>(row) * CELL);
			wall.transform.scale = glm::vec3(1.0f);
			wall.hasMeshRenderer = true;
			wall.meshRenderer.mesh = &scene.cubeMesh();
			wall.meshRenderer.textureId = mapWallTextureFor(map, cell);
		}
	}

	Entity& player = scene.addEntity();
	player.hasTransform = true;
	player.transform.position = glm::vec3(
		static_cast<float>(map.spawnCol) * CELL, 0.0f, static_cast<float>(map.spawnRow) * CELL);
	player.hasCamera = true;
	player.hasCollider = true;
}
