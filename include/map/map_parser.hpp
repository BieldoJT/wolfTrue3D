#ifndef MAP_MAP_PARSER_HPP
# define MAP_MAP_PARSER_HPP

# include "map/map_data.hpp"
# include "map/map_messages.hpp"

# include <istream>
# include <string>
# include <vector>

// Lê a sintaxe do mapa (diretivas e grade). Retorna false se houver erro.
// Regras de conteúdo (spawn, mapa fechado, texturas) ficam em validateMap.
bool parseMap(std::istream& in, MapData& out, std::vector<MapMessage>& messages);
bool loadMapFile(const std::string& path, MapData& out, std::vector<MapMessage>& messages);

#endif
