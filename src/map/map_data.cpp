#include "map/map_data.hpp"

MapData::MapData()
	: wallTexture(1), floorTexture(1), ceilingTexture(1),
	doorTexture(0), lockedTexture(0), exitTexture(0),
	wallLine(0), floorLine(0), ceilingLine(0),
	doorLine(0), lockedLine(0), exitLine(0), nextLine(0),
	spawnCol(-1), spawnRow(-1)
{
	for (int i = 0; i < 9; ++i)
	{
		this->symbolTextures[i] = 0;
		this->symbolLines[i] = 0;
	}
}

bool isMapWallCell(char cell)
{
	return cell == '#' || (cell >= '1' && cell <= '9');
}

bool isMapExitCell(char cell)
{
	return cell == 'X';
}

bool isMapSolidCell(char cell)
{
	return isMapWallCell(cell) || isMapExitCell(cell);
}

bool isMapDoorCell(char cell)
{
	return cell == 'D' || cell == 'L';
}

bool isMapItemCell(char cell)
{
	return cell >= 'A' && cell <= 'Z' && cell != 'P' && !isMapDoorCell(cell) && !isMapExitCell(cell);
}

bool isMapGridChar(char cell)
{
	return isMapWallCell(cell) || cell == '.' || cell == ' ' || (cell >= 'A' && cell <= 'Z');
}

char mapCellAt(const MapData& map, int col, int row)
{
	if (row < 0 || row >= static_cast<int>(map.rows.size()))
		return '\0';
	const std::string& cells = map.rows[static_cast<std::size_t>(row)];
	if (col < 0 || col >= static_cast<int>(cells.size()))
		return '\0';
	return cells[static_cast<std::size_t>(col)];
}

int mapWallTextureFor(const MapData& map, char cell)
{
	if (cell >= '1' && cell <= '9' && map.symbolTextures[cell - '1'] != 0)
		return map.symbolTextures[cell - '1'];
	if (cell == 'L' && map.lockedTexture != 0)
		return map.lockedTexture;
	if ((cell == 'D' || cell == 'L') && map.doorTexture != 0)
		return map.doorTexture;
	if (cell == 'X' && map.exitTexture != 0)
		return map.exitTexture;
	return map.wallTexture;
}

MapDoorAxis mapDoorAxis(const MapData& map, int col, int row)
{
	if (isMapSolidCell(mapCellAt(map, col - 1, row)) && isMapSolidCell(mapCellAt(map, col + 1, row)))
		return DoorAxisX;
	if (isMapSolidCell(mapCellAt(map, col, row - 1)) && isMapSolidCell(mapCellAt(map, col, row + 1)))
		return DoorAxisZ;
	return DoorAxisNone;
}
