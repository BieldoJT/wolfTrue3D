#include "map/map_parser.hpp"

#include <fstream>
#include <sstream>

namespace
{
	void stripCarriageReturn(std::string& text)
	{
		if (!text.empty() && text[text.size() - 1] == '\r')
			text.erase(text.size() - 1);
	}

	bool isComment(const std::string& line)
	{
		return line.size() >= 2 && line[0] == '/' && line[1] == '/';
	}

	// Diretivas começam com letra minúscula; a grade nunca tem minúsculas.
	bool looksLikeDirective(const std::string& line)
	{
		return !line.empty() && line[0] >= 'a' && line[0] <= 'z';
	}

	std::string directiveName(const std::string& line)
	{
		std::size_t end = line.find_first_of(" \t");
		return line.substr(0, end);
	}

	std::string toString(int value)
	{
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

	// Lê um id de textura (inteiro > 0). Texto depois do id vira aviso, como o parser antigo ignorava.
	bool readTextureId(const std::string& text, int lineNo, int& out, std::vector<MapMessage>& messages)
	{
		std::istringstream iss(text);
		int value = 0;
		if (!(iss >> value) || value <= 0)
			return false;
		std::string rest;
		std::getline(iss, rest);
		if (rest.find_first_not_of(" \t") != std::string::npos)
			addMapWarning(messages, lineNo, "texto extra '" + rest.substr(rest.find_first_not_of(" \t"))
				+ "' ignorado depois do id " + toString(value));
		out = value;
		return true;
	}

	void noteRepeated(std::vector<MapMessage>& messages, int lineNo, int previousLine, const std::string& what)
	{
		if (previousLine > 0)
			addMapWarning(messages, lineNo, "'" + what + "' repetido (linha " + toString(previousLine)
				+ "); vale o último");
	}

	bool parseTextureDirective(const std::string& name, const std::string& args, int lineNo,
		MapData& out, std::vector<MapMessage>& messages)
	{
		if (name == "floor" || name == "ceiling")
		{
			bool isFloor = name == "floor";
			int textureId = 0;
			if (!readTextureId(args, lineNo, textureId, messages))
			{
				addMapError(messages, lineNo, "use '" + name + " <id>' (id inteiro maior que zero)");
				return false;
			}
			noteRepeated(messages, lineNo, isFloor ? out.floorLine : out.ceilingLine, name);
			(isFloor ? out.floorTexture : out.ceilingTexture) = textureId;
			(isFloor ? out.floorLine : out.ceilingLine) = lineNo;
			return true;
		}

		// "wall <id>" define a textura de '#'; "wall <símbolo> <id>" a de '1'-'9'
		std::istringstream iss(args);
		std::string first;
		std::string second;
		iss >> first >> second;
		int textureId = 0;
		if (second.empty())
		{
			if (!readTextureId(first, lineNo, textureId, messages))
			{
				addMapError(messages, lineNo, "use 'wall <id>' ou 'wall <1-9> <id>' (id inteiro maior que zero)");
				return false;
			}
			noteRepeated(messages, lineNo, out.wallLine, "wall");
			out.wallTexture = textureId;
			out.wallLine = lineNo;
			return true;
		}
		if (first.size() != 1 || first[0] < '1' || first[0] > '9'
			|| !readTextureId(args.substr(args.find(first) + 1), lineNo, textureId, messages))
		{
			addMapError(messages, lineNo, "use 'wall <1-9> <id>' (símbolo de 1 a 9, id inteiro maior que zero)");
			return false;
		}
		int index = first[0] - '1';
		noteRepeated(messages, lineNo, out.symbolLines[index], "wall " + first);
		out.symbolTextures[index] = textureId;
		out.symbolLines[index] = lineNo;
		return true;
	}
}

bool parseMap(std::istream& in, MapData& out, std::vector<MapMessage>& messages)
{
	out = MapData();
	bool ok = true;
	std::string line;
	int lineNo = 0;

	while (std::getline(in, line))
	{
		++lineNo;
		stripCarriageReturn(line);
		if (line.empty() || isComment(line))
			continue;

		if (looksLikeDirective(line))
		{
			std::string name = directiveName(line);
			if (name == "wall" || name == "floor" || name == "ceiling")
			{
				if (!parseTextureDirective(name, line.substr(name.size()), lineNo, out, messages))
					ok = false;
			}
			else
				addMapWarning(messages, lineNo, "linha desconhecida '" + name + "' ignorada");
			continue;
		}

		int row = static_cast<int>(out.rows.size());
		for (std::size_t col = 0; col < line.size(); ++col)
		{
			char cell = line[col];
			if (!isMapGridChar(cell))
			{
				addMapError(messages, lineNo, std::string("caractere inválido '") + cell
					+ "' na grade (coluna " + toString(static_cast<int>(col) + 1) + ")");
				ok = false;
			}
			else if (cell == 'P' && out.spawnCol < 0)
			{
				out.spawnCol = static_cast<int>(col);
				out.spawnRow = row;
			}
		}
		out.rows.push_back(line);
		out.rowLines.push_back(lineNo);
	}

	if (out.rows.empty())
	{
		addMapError(messages, 0, "mapa sem grade");
		ok = false;
	}
	return ok;
}

bool loadMapFile(const std::string& path, MapData& out, std::vector<MapMessage>& messages)
{
	std::ifstream file(path.c_str());
	if (!file)
	{
		out = MapData();
		addMapError(messages, 0, "não foi possível abrir o arquivo");
		return false;
	}
	return parseMap(file, out, messages);
}
