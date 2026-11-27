#include "game/systems/map_build_system.hpp"
#include "game/map_grid.hpp"
#include "game/scene.hpp"
#include "map/map_data.hpp"

namespace
{
	const float CELL = 1.0f;
	const float DOOR_THICKNESS = 0.1f;
	const float ITEM_SIZE = 0.5f;

	Entity& addBlock(Scene& scene, float x, float z, int textureId)
	{
		Entity& block = scene.addEntity();
		block.hasTransform = true;
		block.transform.position = glm::vec3(x, 0.0f, z);
		block.transform.scale = glm::vec3(1.0f);
		block.hasMeshRenderer = true;
		block.meshRenderer.mesh = &scene.cubeMesh();
		block.meshRenderer.textureId = textureId;
		return block;
	}
}

void MapBuildSystem::build(Scene& scene, const MapData& map, const std::map<char, ItemDef>& items,
	const PlayerStats& stats)
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
			float x = static_cast<float>(col) * CELL;
			float z = static_cast<float>(row) * CELL;

			if (isMapSolidCell(cell))
				addBlock(scene, x, z, mapWallTextureFor(map, cell));
			else if (isMapDoorCell(cell))
			{
				// Folha fina no meio da célula, atravessando o corredor.
				Entity& door = addBlock(scene, x, z, mapWallTextureFor(map, cell));
				door.hasDoor = true;
				door.door.col = static_cast<int>(col);
				door.door.row = static_cast<int>(row);
				door.door.locked = cell == 'L';
				door.door.alongX = mapDoorAxis(map, static_cast<int>(col), static_cast<int>(row)) != DoorAxisZ;
				door.door.closedPosition = door.transform.position;
				door.transform.scale = door.door.alongX
					? glm::vec3(1.0f, 1.0f, DOOR_THICKNESS)
					: glm::vec3(DOOR_THICKNESS, 1.0f, 1.0f);
			}
			else if (isMapItemCell(cell))
			{
				std::map<char, ItemDef>::const_iterator it = items.find(cell);
				if (it == items.end())
					continue;	// o validador já avisou
				Entity& item = scene.addEntity();
				item.hasTransform = true;
				item.transform.position = glm::vec3(x, -0.5f + ITEM_SIZE * 0.5f, z);
				item.transform.scale = glm::vec3(ITEM_SIZE);
				item.hasMeshRenderer = true;
				item.meshRenderer.mesh = &scene.quadMesh();
				item.meshRenderer.textureId = it->second.textureId;
				item.meshRenderer.billboard = true;
				item.hasPickup = true;
				item.pickup.item = it->second;
			}
		}
	}

	Entity& player = scene.addEntity();
	player.hasTransform = true;
	player.transform.position = glm::vec3(
		static_cast<float>(map.spawnCol) * CELL, 0.0f, static_cast<float>(map.spawnRow) * CELL);
	player.hasCamera = true;
	player.hasCollider = true;
	player.hasPlayerStats = true;
	player.playerStats = stats;
}
