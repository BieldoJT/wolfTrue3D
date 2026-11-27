// mapcheck: valida mapas sem abrir o jogo. Usa só a biblioteca de formato (src/map/),
// a mesma que o jogo e o futuro editor usam.
//
//   ./mapcheck [--catalog data/textures.txt] [--items data/items.txt] mapa.txt [mais mapas...]
//
// Código de saída: 0 sem erros (avisos podem existir), 1 se algum mapa tiver erro, 2 uso inválido.
#include "map/catalog.hpp"
#include "map/map_data.hpp"
#include "map/map_parser.hpp"
#include "map/map_validator.hpp"

#include <iostream>
#include <set>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
	std::string catalogPath;
	std::string itemsPath;
	std::vector<std::string> maps;
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg == "--catalog" && i + 1 < argc)
			catalogPath = argv[++i];
		else if (arg == "--items" && i + 1 < argc)
			itemsPath = argv[++i];
		else if (arg == "-h" || arg == "--help")
		{
			std::cout << "uso: mapcheck [--catalog data/textures.txt] [--items data/items.txt] mapa.txt [...]" << std::endl;
			return 0;
		}
		else
			maps.push_back(arg);
	}
	if (maps.empty())
	{
		std::cerr << "uso: mapcheck [--catalog data/textures.txt] [--items data/items.txt] mapa.txt [...]" << std::endl;
		return 2;
	}

	std::set<int> textureIds;
	bool hasCatalog = !catalogPath.empty();
	if (hasCatalog)
	{
		std::map<int, std::string> catalog;
		std::vector<MapMessage> messages;
		bool ok = readTextureCatalog(catalogPath, catalog, messages);
		printMapMessages(std::cout, catalogPath, messages);
		if (!ok)
			return 1;
		textureIds = catalogIds(catalog);
	}

	std::map<char, ItemDef> items;
	bool hasItems = !itemsPath.empty();
	if (hasItems)
	{
		std::vector<MapMessage> messages;
		bool ok = readItemCatalog(itemsPath, items, messages);
		printMapMessages(std::cout, itemsPath, messages);
		if (!ok)
			return 1;
	}

	MapCatalogs catalogs;
	if (hasCatalog)
		catalogs.textureIds = &textureIds;
	if (hasItems)
		catalogs.items = &items;

	bool anyError = false;
	for (std::size_t i = 0; i < maps.size(); ++i)
	{
		MapData map;
		std::vector<MapMessage> messages;
		if (loadMapFile(maps[i], map, messages))
			validateMap(map, catalogs, messages);
		printMapMessages(std::cout, maps[i], messages);
		if (messages.empty())
			std::cout << maps[i] << ": ok" << std::endl;
		if (hasMapErrors(messages))
			anyError = true;
	}
	return anyError ? 1 : 0;
}
