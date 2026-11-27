#include "map/catalog.hpp"

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
