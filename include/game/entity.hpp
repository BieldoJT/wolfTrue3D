#ifndef GAME_ENTITY_HPP
# define GAME_ENTITY_HPP

# include "game/components/transform.hpp"
# include "game/components/camera.hpp"
# include "game/components/mesh_renderer.hpp"
# include "game/components/collider.hpp"

struct Entity
{
	bool hasTransform;
	bool hasCamera;
	bool hasMeshRenderer;
	bool hasCollider;
	Transform transform;
	Camera camera;
	MeshRenderer meshRenderer;
	Collider collider;

	Entity()
		: hasTransform(false),
		  hasCamera(false),
		  hasMeshRenderer(false),
		  hasCollider(false)
	{}
};

#endif
