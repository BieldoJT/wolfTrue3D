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

// Item coletável de data/items.txt: "símbolo nome textura efeito quantidade".
struct ItemDef
{
	enum Effect
	{
		Ammo,
		Health,
		Key
	};

	char symbol;		// letra maiúscula usada na grade
	std::string name;
	int textureId;		// sprite (id de data/textures.txt)
	Effect effect;
	int amount;

	ItemDef();
};

// Lê data/items.txt. Retorna false se houver erro.
bool readItemCatalog(const std::string& path, std::map<char, ItemDef>& out, std::vector<MapMessage>& messages);

#endif
