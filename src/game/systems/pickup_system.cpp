#include "game/systems/pickup_system.hpp"
#include "game/scene.hpp"

#include <sstream>

namespace
{
	const float PICKUP_RADIUS = 0.5f;

	std::string gainText(int amount, const char* what)
	{
		std::ostringstream oss;
		oss << "+" << amount << " " << what;
		return oss.str();
	}
}

void PickupSystem::update(Scene& scene)
{
	Entity* player = scene.cameraEntity();
	if (!player || !player->hasTransform || !player->hasPlayerStats)
		return;
	PlayerStats& stats = player->playerStats;
	glm::vec3 p = player->transform.position;
	std::vector<Entity>& entities = scene.getEntities();

	for (std::size_t i = 0; i < entities.size(); ++i)
	{
		Entity& entity = entities[i];
		if (!entity.hasPickup || entity.pickup.collected || !entity.hasTransform)
			continue;
		float dx = entity.transform.position.x - p.x;
		float dz = entity.transform.position.z - p.z;
		if (dx * dx + dz * dz > PICKUP_RADIUS * PICKUP_RADIUS)
			continue;

		const ItemDef& item = entity.pickup.item;
		// Como no Wolfenstein: vida/munição cheias deixam o item no chão.
		if (item.effect == ItemDef::Health)
		{
			if (stats.health >= PlayerStats::MAX_HEALTH)
				continue;
			int before = stats.health;
			stats.health += item.amount;
			if (stats.health > PlayerStats::MAX_HEALTH)
				stats.health = PlayerStats::MAX_HEALTH;
			scene.events().show(gainText(stats.health - before, "VIDA"));
		}
		else if (item.effect == ItemDef::Ammo)
		{
			if (stats.ammo >= PlayerStats::MAX_AMMO)
				continue;
			int before = stats.ammo;
			stats.ammo += item.amount;
			if (stats.ammo > PlayerStats::MAX_AMMO)
				stats.ammo = PlayerStats::MAX_AMMO;
			scene.events().show(gainText(stats.ammo - before, "MUNICAO"));
		}
		else
		{
			stats.hasKey = true;
			scene.events().show("PEGOU A CHAVE");
		}
		entity.pickup.collected = true;
	}
}
