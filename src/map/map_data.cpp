#include "map/map_data.hpp"

MapData::MapData()
	: wallTexture(1), floorTexture(1), ceilingTexture(1),
	wallLine(0), floorLine(0), ceilingLine(0),
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

bool isMapGridChar(char cell)
{
	return isMapWallCell(cell) || cell == '.' || cell == ' ' || cell == 'P';
}

int mapWallTextureFor(const MapData& map, char cell)
{
	if (cell >= '1' && cell <= '9' && map.symbolTextures[cell - '1'] != 0)
		return map.symbolTextures[cell - '1'];
	return map.wallTexture;
}
