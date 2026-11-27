#include "game/systems/interaction_system.hpp"
#include "game/scene.hpp"
#include "map/map_data.hpp"
#include "platform/input.hpp"

#include <cmath>
#include <glm/glm.hpp>

namespace
{
	const float REACH = 1.2f;	// alcance a partir do centro do jogador, em células
	const float STEP = 0.05f;

	Entity* doorAt(Scene& scene, int col, int row)
	{
		std::vector<Entity>& entities = scene.getEntities();
		for (std::size_t i = 0; i < entities.size(); ++i)
			if (entities[i].hasDoor && entities[i].door.col == col && entities[i].door.row == row)
				return &entities[i];
		return 0;
	}
}

void InteractionSystem::update(Scene& scene, const Input& input)
{
	if (!input.use)
		return;
	Entity* player = scene.cameraEntity();
	if (!player || !player->hasTransform)
		return;

	MapGrid& grid = scene.grid();
	float yaw = glm::radians(player->camera.yaw);
	glm::vec3 dir(std::cos(yaw), 0.0f, std::sin(yaw));
	glm::vec3 origin = player->transform.position;
	int startCol = grid.cellAt(origin.x);
	int startRow = grid.cellAt(origin.z);

	// Anda pelo raio do olhar até a primeira célula que não é chão.
	for (float t = STEP; t <= REACH * grid.cellSize; t += STEP)
	{
		glm::vec3 p = origin + dir * t;
		int col = grid.cellAt(p.x);
		int row = grid.cellAt(p.z);
		if (col == startCol && row == startRow)
			continue;
		char cell = grid.cell(col, row);

		if (isMapExitCell(cell))
		{
			scene.events().exitRequested = true;
			return;
		}
		if (isMapDoorCell(cell))
		{
			Entity* entity = doorAt(scene, col, row);
			if (!entity)
				return;
			Door& door = entity->door;
			if (door.locked)
			{
				if (!player->hasPlayerStats || !player->playerStats.hasKey)
				{
					scene.events().show("PORTA TRANCADA: PRECISA DA CHAVE");
					return;
				}
				door.locked = false;
				scene.events().show("PORTA DESTRANCADA");
			}
			if (door.state == Door::Closed || door.state == Door::Closing)
				door.state = Door::Opening;
			else if (door.state == Door::Open)
				door.timer = 0.0f;
			return;
		}
		if (cell == '\0' || isMapSolidCell(cell))
			return;
	}
}
