#ifndef GAME_SCENE_HPP
# define GAME_SCENE_HPP

# include "game/entity.hpp"
# include "render/mesh.hpp"

# include <cstddef>
# include <vector>

class Scene
{
	private:
		Mesh cube;
		std::vector<Entity> entities;

	public:
		Scene();

		void reserve(std::size_t count);
		Entity& addEntity();
		std::vector<Entity>& getEntities();
		Mesh& cubeMesh();
		Entity* cameraEntity();
};

#endif
