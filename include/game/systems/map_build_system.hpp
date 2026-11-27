#ifndef GAME_MAP_BUILD_SYSTEM_HPP
# define GAME_MAP_BUILD_SYSTEM_HPP

class Scene;
struct MapData;

// Monta a cena (chão, teto, paredes e jogador) a partir de um mapa já lido e validado.
class MapBuildSystem
{
	public:
		void build(Scene& scene, const MapData& map);
};

#endif
