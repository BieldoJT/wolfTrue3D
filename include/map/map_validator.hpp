#ifndef MAP_MAP_VALIDATOR_HPP
# define MAP_MAP_VALIDATOR_HPP

# include "map/map_data.hpp"
# include "map/map_messages.hpp"

# include <set>
# include <vector>

// Regras de conteúdo do mapa (ver docs/03-formato-de-mapas.md).
// knownTextureIds pode ser nulo: aí os ids de textura não são conferidos.
// Retorna false se houver erro.
bool validateMap(const MapData& map, const std::set<int>* knownTextureIds, std::vector<MapMessage>& messages);

#endif
