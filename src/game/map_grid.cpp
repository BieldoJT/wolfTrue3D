#include "game/map_grid.hpp"
#include "map/map_data.hpp"

#include <cmath>

MapGrid::MapGrid()
	: cellSize(1.0f)
{}

bool MapGrid::isWallCell(char cell)
{
	return isMapWallCell(cell);
}

int MapGrid::cellAt(float position) const
{
	return static_cast<int>(std::floor((position / this->cellSize) + 0.5f));
}

char MapGrid::cell(int col, int row) const
{
	if (row < 0 || row >= static_cast<int>(this->rows.size()))
		return '\0';
	const std::string& cells = this->rows[static_cast<std::size_t>(row)];
	if (col < 0 || col >= static_cast<int>(cells.size()))
		return '\0';
	return cells[static_cast<std::size_t>(col)];
}

bool MapGrid::isSolid(int col, int row) const
{
	char c = this->cell(col, row);
	if (c == '\0' || isMapSolidCell(c))
		return true;
	if (isMapDoorCell(c))
		return this->openDoors.find(std::make_pair(col, row)) == this->openDoors.end();
	return false;
}

bool MapGrid::overlapsWall(float x, float z, float radius) const
{
	if (this->cellSize <= 0.0f)
		return true;
	if (radius < 0.0f)
		radius = 0.0f;

	int col0 = this->cellAt(x - radius);
	int col1 = this->cellAt(x + radius);
	int row0 = this->cellAt(z - radius);
	int row1 = this->cellAt(z + radius);

	for (int row = row0; row <= row1; ++row)
		for (int col = col0; col <= col1; ++col)
			if (this->isSolid(col, row))
				return true;
	return false;
}

bool MapGrid::overlapsCell(float x, float z, float radius, int col, int row) const
{
	return col >= this->cellAt(x - radius) && col <= this->cellAt(x + radius)
		&& row >= this->cellAt(z - radius) && row <= this->cellAt(z + radius);
}
