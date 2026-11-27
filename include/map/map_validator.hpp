#ifndef MAP_MAP_VALIDATOR_HPP
# define MAP_MAP_VALIDATOR_HPP

# include "map/catalog.hpp"
# include "map/map_data.hpp"
# include "map/map_messages.hpp"

# include <map>
# include <set>
# include <vector>

// Catálogos usados na validação. Ponteiro nulo = aquele catálogo não é conferido.
struct MapCatalogs
{
	const std::set<int>* textureIds;
	const std::map<char, ItemDef>* items;

	MapCatalogs();
};

// Regras de conteúdo do mapa (ver docs/03-formato-de-mapas.md). Retorna false se houver erro.
bool validateMap(const MapData& map, const MapCatalogs& catalogs, std::vector<MapMessage>& messages);

#endif
