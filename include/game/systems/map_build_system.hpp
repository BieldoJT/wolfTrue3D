#ifndef GAME_MAP_BUILD_SYSTEM_HPP
# define GAME_MAP_BUILD_SYSTEM_HPP

# include "map/catalog.hpp"

# include <map>

class Scene;
struct MapData;
struct PlayerStats;

// Monta a cena (chão, teto, paredes, portas, saída, itens e jogador) a partir de um mapa já lido e validado.
class MapBuildSystem
{
	public:
		void build(Scene& scene, const MapData& map, const std::map<char, ItemDef>& items,
			const PlayerStats& stats);
};

#endif
