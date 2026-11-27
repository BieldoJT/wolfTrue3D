#include "map/catalog.hpp"
#include "map/map_data.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

namespace
{
	void trim(std::string& text)
	{
		while (!text.empty() && std::isspace(static_cast<unsigned char>(text[text.size() - 1])))
			text.erase(text.size() - 1);
		std::size_t start = 0;
		while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
			++start;
		if (start > 0)
			text.erase(0, start);
	}
}

bool readTextureCatalog(const std::string& path, std::map<int, std::string>& out, std::vector<MapMessage>& messages)
{
	out.clear();
	std::ifstream file(path.c_str());
	if (!file)
	{
		addMapError(messages, 0, "não foi possível abrir o catálogo de texturas");
		return false;
	}

	bool ok = true;
	std::string line;
	int lineNo = 0;
	while (std::getline(file, line))
	{
		++lineNo;
		trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		if (line.size() >= 2 && line[0] == '/' && line[1] == '/')
			continue;

		std::istringstream iss(line);
		int textureId = 0;
		if (!(iss >> textureId) || textureId <= 0)
		{
			addMapError(messages, lineNo, "id de textura inválido (use 'id caminho', id inteiro maior que zero)");
			ok = false;
			continue;
		}

		std::string imagePath;
		std::getline(iss, imagePath);
		trim(imagePath);
		if (imagePath.empty())
		{
			addMapError(messages, lineNo, "caminho vazio para a textura");
			ok = false;
			continue;
		}
		if (out.find(textureId) != out.end())
		{
			std::ostringstream oss;
			oss << "id de textura repetido: " << textureId;
			addMapError(messages, lineNo, oss.str());
			ok = false;
			continue;
		}
		out[textureId] = imagePath;
	}

	if (out.empty() && ok)
	{
		addMapError(messages, 0, "catálogo de texturas vazio");
		ok = false;
	}
	return ok;
}

std::set<int> catalogIds(const std::map<int, std::string>& catalog)
{
	std::set<int> ids;
	for (std::map<int, std::string>::const_iterator it = catalog.begin(); it != catalog.end(); ++it)
		ids.insert(it->first);
	return ids;
}

ItemDef::ItemDef()
	: symbol('\0'), textureId(0), effect(Ammo), amount(0)
{}

namespace
{
	bool readEffect(const std::string& text, ItemDef::Effect& out)
	{
		if (text == "ammo")
			out = ItemDef::Ammo;
		else if (text == "health")
			out = ItemDef::Health;
		else if (text == "key")
			out = ItemDef::Key;
		else
			return false;
		return true;
	}
}

bool readItemCatalog(const std::string& path, std::map<char, ItemDef>& out, std::vector<MapMessage>& messages)
{
	out.clear();
	std::ifstream file(path.c_str());
	if (!file)
	{
		addMapError(messages, 0, "não foi possível abrir o catálogo de itens");
		return false;
	}

	bool ok = true;
	std::string line;
	int lineNo = 0;
	while (std::getline(file, line))
	{
		++lineNo;
		trim(line);
		if (line.empty() || (line.size() >= 2 && line[0] == '/' && line[1] == '/'))
			continue;

		std::istringstream iss(line);
		std::string symbol;
		std::string effect;
		ItemDef item;
		if (!(iss >> symbol >> item.name >> item.textureId >> effect >> item.amount))
		{
			addMapError(messages, lineNo, "use 'símbolo nome textura efeito quantidade'");
			ok = false;
			continue;
		}
		if (symbol.size() != 1 || !isMapItemCell(symbol[0]))
		{
			addMapError(messages, lineNo, "símbolo de item deve ser uma letra maiúscula (exceto P, D, L e X)");
			ok = false;
			continue;
		}
		if (!readEffect(effect, item.effect))
		{
			addMapError(messages, lineNo, "efeito '" + effect + "' desconhecido (use ammo, health ou key)");
			ok = false;
			continue;
		}
		if (item.textureId <= 0 || item.amount <= 0)
		{
			addMapError(messages, lineNo, "textura e quantidade devem ser inteiros maiores que zero");
			ok = false;
			continue;
		}
		item.symbol = symbol[0];
		if (out.find(item.symbol) != out.end())
		{
			addMapError(messages, lineNo, "símbolo de item repetido: " + symbol);
			ok = false;
			continue;
		}
		out[item.symbol] = item;
	}
	return ok;
}
