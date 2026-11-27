#include "map/map_validator.hpp"

#include <fstream>
#include <sstream>
#include <utility>

namespace
{
	std::string toString(int value)
	{
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

	// Coordenadas no texto seguem "coluna linha" (1-based, como num editor de texto).
	std::string cellName(int col, int row)
	{
		return "coluna " + toString(col + 1) + ", linha " + toString(row + 1) + " da grade";
	}

	bool insideRow(const MapData& map, int col, int row)
	{
		return row >= 0 && row < static_cast<int>(map.rows.size())
			&& col >= 0 && col < static_cast<int>(map.rows[static_cast<std::size_t>(row)].size());
	}

	void checkSpawns(const MapData& map, std::vector<MapMessage>& messages)
	{
		bool first = true;
		for (std::size_t row = 0; row < map.rows.size(); ++row)
		{
			const std::string& cells = map.rows[row];
			for (std::size_t col = 0; col < cells.size(); ++col)
			{
				if (cells[col] != 'P')
					continue;
				if (!first)
					addMapWarning(messages, map.rowLines[row], "spawn extra (P) ignorado na "
						+ cellName(static_cast<int>(col), static_cast<int>(row)));
				first = false;
			}
		}
		if (map.spawnCol < 0)
			addMapError(messages, 0, "mapa sem spawn (P)");
	}

	void checkRectangular(const MapData& map, std::vector<MapMessage>& messages)
	{
		if (map.rows.empty())
			return;
		std::size_t width = map.rows[0].size();
		for (std::size_t row = 1; row < map.rows.size(); ++row)
		{
			if (map.rows[row].size() != width)
			{
				addMapWarning(messages, map.rowLines[row], "grade não retangular: esta linha tem "
					+ toString(static_cast<int>(map.rows[row].size())) + " colunas, a primeira tem "
					+ toString(static_cast<int>(width)));
				return;
			}
		}
	}

	// Flood fill a partir do spawn (portas e itens deixam passar; paredes e 'X' não).
	// Avisa se uma célula andável encosta fora da grade (mapa aberto) e diz se algum 'X' é alcançável.
	bool checkClosed(const MapData& map, std::vector<MapMessage>& messages)
	{
		if (map.spawnCol < 0)
			return true;
		std::vector<std::vector<bool> > seen(map.rows.size());
		for (std::size_t row = 0; row < map.rows.size(); ++row)
			seen[row].assign(map.rows[row].size(), false);

		std::vector<std::pair<int, int> > stack;
		stack.push_back(std::make_pair(map.spawnCol, map.spawnRow));
		seen[static_cast<std::size_t>(map.spawnRow)][static_cast<std::size_t>(map.spawnCol)] = true;
		const int dc[4] = {1, -1, 0, 0};
		const int dr[4] = {0, 0, 1, -1};
		bool warnedOpen = false;
		bool exitReached = false;

		while (!stack.empty())
		{
			int col = stack.back().first;
			int row = stack.back().second;
			stack.pop_back();
			for (int i = 0; i < 4; ++i)
			{
				int nc = col + dc[i];
				int nr = row + dr[i];
				if (!insideRow(map, nc, nr))
				{
					if (!warnedOpen)
						addMapWarning(messages, map.rowLines[static_cast<std::size_t>(row)],
							"mapa aberto: dá para sair da grade pela " + cellName(col, row)
							+ " (feche com parede)");
					warnedOpen = true;
					continue;
				}
				std::size_t r = static_cast<std::size_t>(nr);
				std::size_t c = static_cast<std::size_t>(nc);
				char cell = map.rows[r][c];
				if (isMapExitCell(cell))
					exitReached = true;
				if (seen[r][c] || isMapSolidCell(cell))
					continue;
				seen[r][c] = true;
				stack.push_back(std::make_pair(nc, nr));
			}
		}
		return exitReached;
	}

	// Primeira linha do arquivo onde a célula aparece (0 = não aparece).
	int firstLineOf(const MapData& map, char symbol)
	{
		for (std::size_t row = 0; row < map.rows.size(); ++row)
			if (map.rows[row].find(symbol) != std::string::npos)
				return map.rowLines[row];
		return 0;
	}

	void checkDoors(const MapData& map, std::vector<MapMessage>& messages)
	{
		for (std::size_t row = 0; row < map.rows.size(); ++row)
		{
			const std::string& cells = map.rows[row];
			for (std::size_t col = 0; col < cells.size(); ++col)
			{
				if (isMapDoorCell(cells[col])
					&& mapDoorAxis(map, static_cast<int>(col), static_cast<int>(row)) == DoorAxisNone)
					addMapWarning(messages, map.rowLines[row], std::string("porta '") + cells[col]
						+ "' sem paredes em dois lados opostos na "
						+ cellName(static_cast<int>(col), static_cast<int>(row)));
			}
		}
	}

	// Diretiva de textura opcional ('door', 'locked', 'exit'): usada sem linha, ou linha sem uso.
	void checkObjectDirective(const MapData& map, char symbol, int directiveLine, const std::string& name,
		const std::string& fallback, std::vector<MapMessage>& messages)
	{
		int used = firstLineOf(map, symbol);
		if (used > 0 && directiveLine == 0)
			addMapWarning(messages, used, std::string("'") + symbol + "' sem linha '" + name + " <id>'; usando "
				+ fallback);
		if (used == 0 && directiveLine > 0)
			addMapWarning(messages, directiveLine, "'" + name + "' definido mas '" + std::string(1, symbol)
				+ "' não aparece na grade");
	}

	void checkExit(const MapData& map, bool exitReached, std::vector<MapMessage>& messages)
	{
		int exitLine = firstLineOf(map, 'X');
		if (exitLine > 0 && !exitReached && map.spawnCol >= 0)
			addMapWarning(messages, exitLine, "nenhum interruptor de saída 'X' é alcançável a partir do spawn");
		if (map.nextLine > 0 && exitLine == 0)
			addMapWarning(messages, map.nextLine, "'next' definido mas o mapa não tem saída 'X'");
		if (map.nextLine > 0)
		{
			std::ifstream next(map.nextMapPath.c_str());
			if (!next)
				addMapWarning(messages, map.nextLine, "próximo mapa '" + map.nextMapPath + "' não encontrado");
		}
	}

	void checkItems(const MapData& map, const MapCatalogs& catalogs, std::vector<MapMessage>& messages)
	{
		bool seen[26] = {false};
		bool hasKey = false;
		for (std::size_t row = 0; row < map.rows.size(); ++row)
		{
			const std::string& cells = map.rows[row];
			for (std::size_t col = 0; col < cells.size(); ++col)
			{
				char cell = cells[col];
				if (!isMapItemCell(cell) || seen[cell - 'A'])
					continue;
				seen[cell - 'A'] = true;
				if (!catalogs.items)
					continue;
				std::map<char, ItemDef>::const_iterator it = catalogs.items->find(cell);
				if (it == catalogs.items->end())
				{
					addMapWarning(messages, map.rowLines[row], std::string("item '") + cell
						+ "' não existe no catálogo de itens; a célula fica vazia");
					continue;
				}
				if (it->second.effect == ItemDef::Key)
					hasKey = true;
				if (catalogs.textureIds && catalogs.textureIds->find(it->second.textureId) == catalogs.textureIds->end())
					addMapWarning(messages, map.rowLines[row], "textura " + toString(it->second.textureId)
						+ " do item '" + it->second.name + "' não existe no catálogo de texturas");
			}
		}
		int locked = firstLineOf(map, 'L');
		if (catalogs.items && locked > 0 && !hasKey)
			addMapWarning(messages, locked, "porta trancada 'L' sem nenhuma chave no mapa");
	}

	void checkSymbols(const MapData& map, std::vector<MapMessage>& messages)
	{
		bool used[10] = {false, false, false, false, false, false, false, false, false, false};
		int firstLine[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		for (std::size_t row = 0; row < map.rows.size(); ++row)
		{
			const std::string& cells = map.rows[row];
			for (std::size_t col = 0; col < cells.size(); ++col)
			{
				char cell = cells[col];
				int index = -1;
				if (cell == '#')
					index = 9;
				else if (cell >= '1' && cell <= '9')
					index = cell - '1';
				if (index >= 0 && !used[index])
				{
					used[index] = true;
					firstLine[index] = map.rowLines[row];
				}
			}
		}

		for (int i = 0; i < 9; ++i)
		{
			std::string symbol(1, static_cast<char>('1' + i));
			if (used[i] && map.symbolTextures[i] == 0)
				addMapWarning(messages, firstLine[i], "símbolo '" + symbol + "' sem linha 'wall " + symbol
					+ " <id>'; usando a textura de '#' (" + toString(map.wallTexture) + ")");
			if (!used[i] && map.symbolLines[i] > 0)
				addMapWarning(messages, map.symbolLines[i], "'wall " + symbol + "' definido mas o símbolo '"
					+ symbol + "' não aparece na grade");
		}
		if (used[9] && map.wallLine == 0)
			addMapWarning(messages, 0, "mapa sem linha 'wall <id>'; '#' usa a textura "
				+ toString(map.wallTexture));
	}

	void checkTextureId(const std::set<int>& known, int textureId, int line, const std::string& what,
		std::vector<MapMessage>& messages)
	{
		if (known.find(textureId) == known.end())
			addMapWarning(messages, line, "textura " + toString(textureId) + " de " + what
				+ " não existe no catálogo de texturas");
	}

	void checkTextures(const MapData& map, const std::set<int>& known, std::vector<MapMessage>& messages)
	{
		checkTextureId(known, map.wallTexture, map.wallLine, "'wall'", messages);
		for (int i = 0; i < 9; ++i)
		{
			if (map.symbolLines[i] > 0)
				checkTextureId(known, map.symbolTextures[i], map.symbolLines[i],
					"'wall " + std::string(1, static_cast<char>('1' + i)) + "'", messages);
		}
		checkTextureId(known, map.floorTexture, map.floorLine, "'floor'", messages);
		checkTextureId(known, map.ceilingTexture, map.ceilingLine, "'ceiling'", messages);
		if (map.doorLine > 0)
			checkTextureId(known, map.doorTexture, map.doorLine, "'door'", messages);
		if (map.lockedLine > 0)
			checkTextureId(known, map.lockedTexture, map.lockedLine, "'locked'", messages);
		if (map.exitLine > 0)
			checkTextureId(known, map.exitTexture, map.exitLine, "'exit'", messages);
	}
}

MapCatalogs::MapCatalogs()
	: textureIds(0), items(0)
{}

bool validateMap(const MapData& map, const MapCatalogs& catalogs, std::vector<MapMessage>& messages)
{
	std::size_t before = messages.size();
	checkSpawns(map, messages);
	checkRectangular(map, messages);
	bool exitReached = checkClosed(map, messages);
	checkSymbols(map, messages);
	checkDoors(map, messages);
	checkObjectDirective(map, 'D', map.doorLine, "door", "a textura de '#'", messages);
	checkObjectDirective(map, 'L', map.lockedLine, "locked",
		map.doorLine > 0 ? "a textura de 'door'" : "a textura de '#'", messages);
	checkObjectDirective(map, 'X', map.exitLine, "exit", "a textura de '#'", messages);
	checkExit(map, exitReached, messages);
	checkItems(map, catalogs, messages);
	if (catalogs.textureIds)
		checkTextures(map, *catalogs.textureIds, messages);

	for (std::size_t i = before; i < messages.size(); ++i)
		if (messages[i].level == MapMessage::Error)
			return false;
	return true;
}
