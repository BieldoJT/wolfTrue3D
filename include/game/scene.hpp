#ifndef GAME_SCENE_HPP
# define GAME_SCENE_HPP

# include "game/entity.hpp"
# include "game/game_events.hpp"
# include "game/map_grid.hpp"
# include "render/mesh.hpp"

# include <cstddef>
# include <vector>

class Scene
{
	private:
		Mesh cube;
		Mesh plane;
		Mesh quad;
		std::vector<Entity> entities;
		MapGrid mapGrid;
		GameEvents gameEvents;

	public:
		Scene();

		// Esvazia entidades, grade e eventos (troca de mapa). As malhas continuam.
		void clear();
		void reserve(std::size_t count);
		Entity& addEntity();
		std::vector<Entity>& getEntities();
		Mesh& cubeMesh();
		Mesh& planeMesh();
		Mesh& quadMesh();
		MapGrid& grid();
		Entity* cameraEntity();
		GameEvents& events();
};

#endif
