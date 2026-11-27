#include "game/systems/door_system.hpp"
#include "game/scene.hpp"

#include <utility>

namespace
{
	const float OPEN_SPEED = 1.25f;		// fração da porta por segundo (~0.8 s para abrir)
	const float STAY_OPEN = 4.0f;		// segundos aberta antes de tentar fechar
	const float PASSABLE_AT = 0.9f;		// a partir daqui o jogador passa
	const float SLIDE = 0.95f;			// quanto a folha entra na parede ao abrir
}

void DoorSystem::update(Scene& scene, float dt)
{
	MapGrid& grid = scene.grid();
	Entity* player = scene.cameraEntity();
	std::vector<Entity>& entities = scene.getEntities();

	for (std::size_t i = 0; i < entities.size(); ++i)
	{
		Entity& entity = entities[i];
		if (!entity.hasDoor)
			continue;
		Door& door = entity.door;

		if (door.state == Door::Opening)
		{
			door.openAmount += OPEN_SPEED * dt;
			if (door.openAmount >= 1.0f)
			{
				door.openAmount = 1.0f;
				door.state = Door::Open;
				door.timer = 0.0f;
			}
		}
		else if (door.state == Door::Open)
		{
			door.timer += dt;
			bool blocked = player && player->hasTransform && grid.overlapsCell(
				player->transform.position.x, player->transform.position.z,
				player->hasCollider ? player->collider.radius : 0.0f, door.col, door.row);
			if (door.timer >= STAY_OPEN && !blocked)
				door.state = Door::Closing;
		}
		else if (door.state == Door::Closing)
		{
			// Não fecha em cima do jogador: volta a abrir.
			bool blocked = player && player->hasTransform && grid.overlapsCell(
				player->transform.position.x, player->transform.position.z,
				player->hasCollider ? player->collider.radius : 0.0f, door.col, door.row);
			if (blocked)
				door.state = Door::Opening;
			else
			{
				door.openAmount -= OPEN_SPEED * dt;
				if (door.openAmount <= 0.0f)
				{
					door.openAmount = 0.0f;
					door.state = Door::Closed;
				}
			}
		}

		float offset = door.openAmount * SLIDE * grid.cellSize;
		entity.transform.position = door.closedPosition
			+ (door.alongX ? glm::vec3(offset, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, offset));

		std::pair<int, int> key(door.col, door.row);
		if (door.openAmount >= PASSABLE_AT)
			grid.openDoors.insert(key);
		else
			grid.openDoors.erase(key);
	}
}
