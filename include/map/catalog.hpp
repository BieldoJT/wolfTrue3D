#ifndef MAP_CATALOG_HPP
# define MAP_CATALOG_HPP

# include "map/map_messages.hpp"

# include <map>
# include <set>
# include <string>
# include <vector>

// Lê data/textures.txt ("id caminho" por linha) sem carregar imagens.
// Retorna false se houver erro.
bool readTextureCatalog(const std::string& path, std::map<int, std::string>& out, std::vector<MapMessage>& messages);

std::set<int> catalogIds(const std::map<int, std::string>& catalog);

#endif
