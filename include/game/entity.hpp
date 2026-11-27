#ifndef GAME_ENTITY_HPP
# define GAME_ENTITY_HPP

# include "game/components/transform.hpp"
# include "game/components/camera.hpp"
# include "game/components/mesh_renderer.hpp"
# include "game/components/collider.hpp"
# include "game/components/door.hpp"
# include "game/components/pickup.hpp"
# include "game/components/player_stats.hpp"

struct Entity
{
	bool hasTransform;
	bool hasCamera;
	bool hasMeshRenderer;
	bool hasCollider;
	bool hasDoor;
	bool hasPickup;
	bool hasPlayerStats;
	Transform transform;
	Camera camera;
	MeshRenderer meshRenderer;
	Collider collider;
	Door door;
	Pickup pickup;
	PlayerStats playerStats;

	Entity()
		: hasTransform(false),
		  hasCamera(false),
		  hasMeshRenderer(false),
		  hasCollider(false),
		  hasDoor(false),
		  hasPickup(false),
		  hasPlayerStats(false)
	{}
};

#endif
