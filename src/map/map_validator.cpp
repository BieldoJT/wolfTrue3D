#include "map/map_validator.hpp"

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

	// Flood fill a partir do spawn: se uma célula andável encosta fora da grade, o mapa está aberto.
	void checkClosed(const MapData& map, std::vector<MapMessage>& messages)
	{
		if (map.spawnCol < 0)
			return;
		std::vector<std::vector<bool> > seen(map.rows.size());
		for (std::size_t row = 0; row < map.rows.size(); ++row)
			seen[row].assign(map.rows[row].size(), false);

		std::vector<std::pair<int, int> > stack;
		stack.push_back(std::make_pair(map.spawnCol, map.spawnRow));
		seen[static_cast<std::size_t>(map.spawnRow)][static_cast<std::size_t>(map.spawnCol)] = true;
		const int dc[4] = {1, -1, 0, 0};
		const int dr[4] = {0, 0, 1, -1};

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
					addMapWarning(messages, map.rowLines[static_cast<std::size_t>(row)],
						"mapa aberto: dá para sair da grade pela " + cellName(col, row)
						+ " (feche com parede)");
					return;
				}
				std::size_t r = static_cast<std::size_t>(nr);
				std::size_t c = static_cast<std::size_t>(nc);
				if (seen[r][c] || isMapWallCell(map.rows[r][c]))
					continue;
				seen[r][c] = true;
				stack.push_back(std::make_pair(nc, nr));
			}
		}
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
	}
}

bool validateMap(const MapData& map, const std::set<int>* knownTextureIds, std::vector<MapMessage>& messages)
{
	std::size_t before = messages.size();
	checkSpawns(map, messages);
	checkRectangular(map, messages);
	checkClosed(map, messages);
	checkSymbols(map, messages);
	if (knownTextureIds)
		checkTextures(map, *knownTextureIds, messages);

	for (std::size_t i = before; i < messages.size(); ++i)
		if (messages[i].level == MapMessage::Error)
			return false;
	return true;
}
