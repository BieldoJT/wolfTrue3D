#ifndef GAME_ENTITY_HPP
# define GAME_ENTITY_HPP

# include "game/components/transform.hpp"
# include "game/components/camera.hpp"
# include "game/components/mesh_renderer.hpp"

struct Entity
{
	bool hasTransform;
	bool hasCamera;
	bool hasMeshRenderer;
	Transform transform;
	Camera camera;
	MeshRenderer meshRenderer;

	Entity()
		: hasTransform(false),
		  hasCamera(false),
		  hasMeshRenderer(false)
	{}
};

#endif
